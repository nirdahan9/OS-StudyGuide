#!/usr/bin/env python3
"""Regenerate os-course-guide.body.html from os-course-guide.html.

The body file is exactly the <title> line followed by everything between
<body> and </body></html>, so it can be embedded (that is the form published
to the artifact). Never edit it by hand.
"""
import pathlib
import re
import sys

here = pathlib.Path(__file__).resolve().parent
src = here / "os-course-guide.html"
dst = here / "os-course-guide.body.html"

h = src.read_text(encoding="utf-8")

title = re.search(r"<title>.*?</title>", h, re.S)
if not title:
    sys.exit("no <title> found")

start = h.find("<body>")
suffix = "\n</body></html>"
if start < 0 or not h.endswith(suffix):
    sys.exit("unexpected document shape")

core = h[start + len("<body>") : -len(suffix)]
dst.write_text(title.group(0) + core, encoding="utf-8")
print(f"{dst.name}: {dst.stat().st_size:,} bytes")
