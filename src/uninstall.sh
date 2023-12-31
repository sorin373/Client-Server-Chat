#!/bin/bash

CURRENT_DIR=$(basename "$PWD")

if [ "$CURRENT_DIR" = "httpServer" ]; then
    cd ..
fi

sudo rm -r httpServer