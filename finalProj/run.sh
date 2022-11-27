#!/usr/bin/bash

if [[ ! -f sender || ! -f receiver  ]]; then 
    make; 
    make clean;
fi

./receiver &
./sender localhost
