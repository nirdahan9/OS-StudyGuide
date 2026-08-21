#!/usr/bin/env python3
"""Copy the study guide into the deploy folder as index.html.

The guide itself is edited in ../os-course-guide.html; this just mirrors it so
the deployed site is never edited by hand and can't drift from the source.

    python3 deploy/sync.py
"""
import pathlib
import shutil
import sys

here = pathlib.Path(__file__).resolve().parent
src = here.parent / "os-course-guide.html"
dst = here / "index.html"

if not src.exists():
    sys.exit(f"missing source: {src}")

shutil.copyfile(src, dst)
print(f"{src.name} -> {dst.relative_to(here.parent)}  ({dst.stat().st_size:,} bytes)")
