#!/bin/bash

if [ -z "$IRC_PASSWORD" ]; then
    echo "Error: IRC_PASSWORD environment variable is not set."
    exit 1
fi

if [ -z "$ENABLE_SSL" ]; then
    echo "Error: ENABLE_SSL environment variable is not set."
    exit 1
fi

if [ "$ENABLE_SSL" = "true" ]; then
    echo "Starting IRC server with SSL..."
    ./ircserv 6667 $IRC_PASSWORD ./ssl/cert.pem ./ssl/privkey.pem
else
    echo "Starting IRC server without SSL..."
    ./ircserv 6667 $IRC_PASSWORD
fi
