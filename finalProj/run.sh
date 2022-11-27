#!/usr/bin/bash

if [[ ! -f sender || ! -f receiver  ]]; then 
    make; 
    make clean;
fi

echo "======== rdt 1.0 ========="
echo "'normal' execution."
echo "receiver spawned first, runs in background;"
echo "sender spawned second, runs in foreground"
echo "output in file normal.out"

echo "=========================="

./normal.sh > normal.out

echo "=========================="
echo "'sender first' execution"
echo "sender spawns first, before receiver is ready"
echo "wait 1s, then spawn receiver"
echo "output in file sender_first.out"
echo "=========================="

./sender_first.sh > sender_first.out
