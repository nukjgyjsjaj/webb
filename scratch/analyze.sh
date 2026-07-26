#!/bin/bash
OUT="/workspace/aa181bae-c121-45b8-8ced-dfeacd850e95/sessions/agent_c5943ff5-8210-4f99-ad2f-7afb9c0da518/scratch/output.log"
if [ -f "$OUT" ]; then
  COUNT=$(wc -l < "$OUT")
  LAST=$(tail -n 1 "$OUT")
  echo "[$(date -Iseconds)] Analysis: total_lines=$COUNT last_entry=\"$LAST\""
else
  echo "[$(date -Iseconds)] Analysis: output not found"
fi
