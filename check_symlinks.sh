#!/bin/bash

SRC="$1"
DST="$2"

if [ -z "$SRC" ] || [ -z "$DST" ]; then
  echo "Usage: $0 <src_dir> <dst_dir>"
  exit 1
fi

echo "Comparing symlinks between $SRC and $DST"

find "$SRC" -type l | while read src_link; do
  # Get relative path
  rel_path="${src_link#$SRC/}"
  dst_link="$DST/$rel_path"

  if [ ! -L "$dst_link" ]; then
    echo "❌ $rel_path is missing in $DST"
    continue
  fi

  src_target=$(readlink "$src_link")
  dst_target=$(readlink "$dst_link")

  if [ "$src_target" == "$dst_target" ]; then
    echo "✅ $rel_path -> $src_target"
  else
    echo "⚠️  $rel_path differs:"
    echo "   SRC: $src_target"
    echo "   DST: $dst_target"
  fi
done
