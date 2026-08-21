/*
 * Shared streak leaderboard for the OS course guide.
 *
 *   GET  /api/leaderboard            -> { ok, top: [{name, streak, at}] }   (top 5)
 *   GET  /api/leaderboard?start=1    -> { ok, token }                        (run token)
 *   POST /api/leaderboard            -> { ok, rank, top }                    body: {name, streak, token}
 *
 * Storage is an Upstash Redis sorted set reached over its REST API, so the
 * function has no npm dependencies. With no store configured every request
 * answers 503 {error:"store"} and the page falls back to a per-device board.
 *
 * CommonJS on purpose: no package.json, so Vercel treats the project as
 * static files plus zero-config Node functions and never runs a build.
 */
const crypto = require('node:crypto');

const NAME_MAX = 18;
const STREAK_MAX = 5000;
const KEEP = 50;              /* entries retained in the store */
const TOP = 5;                /* entries returned to the page */
const MIN_MS_PER_ANSWER = 1200;
const TOKEN_TTL_MS = 12 * 60 * 60 * 1000;
const RATE_MAX = 30;
const RATE_WINDOW_S = 3600;
const ZKEY = 'oslb:z';

function creds() {
  const url = process.env.KV_REST_API_URL || process.env.UPSTASH_REDIS_REST_URL || '';
  const token = process.env.KV_REST_API_TOKEN || process.env.UPSTASH_REDIS_REST_TOKEN || '';
  return url && token ? { url: url.replace(/\/+$/, ''), token } : null;
}

async function redis(commands) {
  const c = creds();
  if (!c) throw Object.assign(new Error('no store configured'), { code: 'store' });
  const r = await fetch(c.url + '/pipeline', {
    method: 'POST',
    headers: { authorization: 'Bearer ' + c.token, 'content-type': 'application/json' },
    body: JSON.stringify(commands),
  });
  if (!r.ok) throw Object.assign(new Error('store http ' + r.status), { code: 'store' });
  const out = await r.json();
  if (!Array.isArray(out)) throw Object.assign(new Error('store shape'), { code: 'store' });
  return out.map((x) => {
    if (x && x.error) throw Object.assign(new Error(x.error), { code: 'store' });
    return x ? x.result : null;
  });
}

/* A run token is issued when a round starts and checked on submit, so a
   streak of N has to have taken at least N * MIN_MS_PER_ANSWER to play. */
const secret = () =>
  process.env.LB_SECRET || process.env.KV_REST_API_TOKEN || process.env.UPSTASH_REDIS_REST_TOKEN || 'os-guide-unsigned';

function sign(v) {
  return crypto.createHmac('sha256', secret()).update(v).digest('base64url').slice(0, 22);
}
function issueToken() {
  const iat = Date.now().toString(36);
  return iat + '.' + sign(iat);
}
function tokenIssuedAt(t) {
  if (typeof t !== 'string') return null;
  const dot = t.indexOf('.');
  if (dot < 1) return null;
  const iat = t.slice(0, dot);
  const sig = t.slice(dot + 1);
  const expected = sign(iat);
  if (sig.length !== expected.length) return null;
  if (!crypto.timingSafeEqual(Buffer.from(sig), Buffer.from(expected))) return null;
  const ms = parseInt(iat, 36);
  return Number.isFinite(ms) ? ms : null;
}

function cleanName(v) {
  if (typeof v !== 'string') return '';
  return v
    .normalize('NFC')
    .replace(/\p{C}/gu, ' ')
    .replace(/[<>]/g, ' ')
    .replace(/\s+/g, ' ')
    .trim()
    .slice(0, NAME_MAX);
}

function parseTop(flat) {
  const out = [];
  if (!Array.isArray(flat)) return out;
  for (let i = 0; i + 1 < flat.length; i += 2) {
    let e;
    try {
      e = JSON.parse(flat[i]);
    } catch (_) {
      continue;
    }
    out.push({
      name: e && typeof e.n === 'string' ? e.n : '?',
      streak: Number(flat[i + 1]) || 0,
      at: Number(e && e.at) || 0,
    });
  }
  return out;
}

function readBody(req) {
  if (req.body && typeof req.body === 'object') return req.body;
  if (typeof req.body === 'string') {
    try {
      return JSON.parse(req.body);
    } catch (_) {
      return {};
    }
  }
  return {};
}

module.exports = async function handler(req, res) {
  res.setHeader('Cache-Control', 'no-store');
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Headers', 'content-type');
  res.setHeader('Access-Control-Allow-Methods', 'GET,POST,OPTIONS');
  if (req.method === 'OPTIONS') return res.status(204).end();

  try {
    if (req.method === 'GET') {
      const url = new URL(req.url, 'http://localhost');
      if (url.searchParams.has('start')) return res.status(200).json({ ok: true, token: issueToken() });
      const [flat] = await redis([['ZRANGE', ZKEY, 0, TOP - 1, 'REV', 'WITHSCORES']]);
      return res.status(200).json({ ok: true, top: parseTop(flat) });
    }

    if (req.method === 'POST') {
      const body = readBody(req);

      const name = cleanName(body.name);
      if (!name) return res.status(400).json({ ok: false, error: 'name' });

      const streak = Math.floor(Number(body.streak));
      if (!Number.isFinite(streak) || streak < 1 || streak > STREAK_MAX)
        return res.status(400).json({ ok: false, error: 'streak' });

      const iat = tokenIssuedAt(body.token);
      if (iat === null) return res.status(400).json({ ok: false, error: 'token' });
      const elapsed = Date.now() - iat;
      if (elapsed < 0 || elapsed > TOKEN_TTL_MS) return res.status(400).json({ ok: false, error: 'token' });
      /* two answers of grace, so a genuinely quick short streak is not rejected */
      if (elapsed < Math.max(0, streak - 2) * MIN_MS_PER_ANSWER)
        return res.status(400).json({ ok: false, error: 'fast' });

      const ip = String(req.headers['x-forwarded-for'] || '').split(',')[0].trim() || 'anon';
      const rkey = 'oslb:rl:' + crypto.createHash('sha256').update(ip).digest('hex').slice(0, 16);
      const [hits] = await redis([['INCR', rkey]]);
      if (Number(hits) === 1) await redis([['EXPIRE', rkey, RATE_WINDOW_S]]);
      if (Number(hits) > RATE_MAX) return res.status(429).json({ ok: false, error: 'rate' });

      const member = JSON.stringify({ n: name, at: Date.now(), r: crypto.randomBytes(4).toString('hex') });
      await redis([
        ['ZADD', ZKEY, streak, member],
        ['ZREMRANGEBYRANK', ZKEY, 0, -(KEEP + 1)],
      ]);
      const [rank, flat] = await redis([
        ['ZREVRANK', ZKEY, member],
        ['ZRANGE', ZKEY, 0, TOP - 1, 'REV', 'WITHSCORES'],
      ]);
      return res.status(200).json({
        ok: true,
        rank: rank === null || rank === undefined ? 0 : Number(rank) + 1,
        top: parseTop(flat),
      });
    }

    return res.status(405).json({ ok: false, error: 'method' });
  } catch (e) {
    const code = (e && e.code) || 'server';
    return res.status(code === 'store' ? 503 : 500).json({ ok: false, error: code });
  }
};
