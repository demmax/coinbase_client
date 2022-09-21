#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    echo "Usage: docker run ... [SYMBOL]"
    exit 1
fi

symbol=$1

echo $symbol

/build/bin/coinbase_client 127.0.0.1:55556 $symbol &
python3 -u /consumer/consumer.py 127.0.0.1:55556
