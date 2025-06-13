#!/bin/bash

while  true; do
    if [ -t 1 ]; then
        echo "Starting IRC server..."
        ./ircserv 6667 Jesuispasla@ ./ssl/cert.pem ./ssl/privkey.pem
        break
    else
        echo "Waiting for terminal to be available..."
        sleep 5
    fi
done
