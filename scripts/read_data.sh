#!/bin/bash
IPAdress=192.168.0.57
Key=test
curl -A TeleHeater http://$IPAdress/$1 -s | grep .. | base64 --decode | openssl enc -aes-256-ecb -d -nopad -K $Key | jq
