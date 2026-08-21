# Deploying the OS course guide with a shared leaderboard

The guide is a single static HTML file plus one serverless function that stores
competition streaks. No build step, no npm dependencies.

```
deploy/
  index.html            generated — mirror of ../os-course-guide.html (run sync.py)
  api/leaderboard.js    the shared leaderboard (Node function, Upstash Redis over REST)
  vercel.json           cache headers
  sync.py               regenerates index.html from the source guide
```

The page always works without the backend: if `/api/leaderboard` is unreachable
it falls back to a leaderboard stored in that browser only, and says so in the
panel ("מקומי בלבד" / "this device only"). Nothing else changes.

## One-time setup

**1. Create the store.** In the Vercel dashboard → your project → **Storage** →
**Upstash for Redis** (Marketplace) → create a free database and connect it to
the project. That injects `KV_REST_API_URL` and `KV_REST_API_TOKEN` into the
project's environment automatically — those are the names the function reads.
`UPSTASH_REDIS_REST_URL` / `UPSTASH_REDIS_REST_TOKEN` also work.

**2. Add a signing secret.** Project → Settings → Environment Variables:

| Name        | Value                          |
| ----------- | ------------------------------ |
| `LB_SECRET` | any long random string         |

Generate one with `node -e "console.log(require('crypto').randomBytes(32).toString('base64url'))"`.
It signs the per-round token; without it the function falls back to signing with
the Redis token, which still works but rotates whenever the database is recreated.

## Deploy

```sh
cd "Course-Map-Site/deploy"
python3 sync.py             # refresh index.html from the source guide
npx vercel login            # once, opens a browser
npx vercel --prod
```

Every later update is `python3 sync.py && npx vercel --prod`.

## API

| Request                        | Response                                    |
| ------------------------------ | ------------------------------------------- |
| `GET /api/leaderboard`         | `{ok:true, top:[{name,streak,at}]}` (top 5) |
| `GET /api/leaderboard?start=1` | `{ok:true, token}` — issued when a round starts |
| `POST /api/leaderboard`        | `{name, streak, token}` → `{ok:true, rank, top}` |

Error responses are `{ok:false, error:"<code>"}` with codes `name`, `streak`,
`token`, `fast`, `rate`, `store`, `method`, `server`. The page maps each to a
message in both languages.

## What stops fake scores

- A round token is issued at round start and HMAC-signed; a submission must
  carry a valid, unexpired one.
- A streak of N is rejected unless the round has been running for at least
  `(N−2) × 1.2s` — enough to make "submit a streak of 900 instantly" fail, with
  two answers of grace so a genuinely fast short streak still counts.
- 30 submissions per IP per hour.
- Names are stripped of control characters and `<>`, collapsed, and capped at 18
  characters; the page renders them with `textContent`, never as HTML.
- The store keeps the top 50 entries and serves the top 5.

This raises the cost of cheating but does not eliminate it: the quiz runs in the
browser, so someone willing to script the page can still pace fake answers and
submit a real-looking run. For a study aid that's the right trade — anything
stronger means moving question checking to the server.

## Data

One Redis sorted set, `oslb:z`: score = streak, member =
`{"n":name,"at":epoch_ms,"r":random}`. Rate-limit counters are `oslb:rl:<hash>`
with a 1-hour TTL. To reset the board: `ZREMRANGEBYRANK oslb:z 0 -1` from the
Upstash console.
