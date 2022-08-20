#!/bin/bash

GatewayPassword=$1
PrivatePassword=$2 # Make sure to properly escape special characters for bash if necessary
Magic=$(printf \\x86\\x78\\x45\\xe9\\x7c\\x4e\\x29\\xdc\\xe5\\x22\\xb9\\xa7\\xd3\\xa3\\xe0\\x7b\\x15\\x2b\\xff\\xad\\xdd\\xbe\\xd7\\xf5\\xff\\xd8\\x42\\xe9\\x89\\x5a\\xd1\\xe4)
Part1=$(echo -n "$GatewayPassword$Magic" | md5sum | cut -c-32)
Part2=$(echo -n "$Magic$PrivatePassword" | md5sum | cut -c-32)
Key="$Part1$Part2"
echo $Key
