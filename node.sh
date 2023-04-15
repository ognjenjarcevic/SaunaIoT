#! /usr/bin/bash
make node_clean
make node
cd nodeOut
./node $1
cd ..