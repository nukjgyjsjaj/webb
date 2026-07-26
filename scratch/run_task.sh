#!/bin/bash
mkdir -p /workspace/aa181bae-c121-45b8-8ced-dfeacd850e95/sessions/agent_c5943ff5-8210-4f99-ad2f-7afb9c0da518/scratch
OUT="/workspace/aa181bae-c121-45b8-8ced-dfeacd850e95/sessions/agent_c5943ff5-8210-4f99-ad2f-7afb9c0da518/scratch/output.log"
echo "[$(date -Iseconds)] Started long-running task" > "$OUT"
while true; do
  echo "[$(date -Iseconds)] Heartbeat" >> "$OUT"
  sleep 10
done
