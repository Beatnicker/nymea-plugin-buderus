# Generate key 

To generate the key, enter the password that can be found on the KM200 and the user password that was assigned during the initial setup. 

```
./key-generator.sh examplepasswort privatepasswort
```

# Read Endpoints 

To run the script, *jp* is still needed to format the JSON response 
```
sudo apt install jq
```

Before endpoints can be read out, the key must be added to the script. 

Example:
```
./read_data.sh recordings/heatSources
```

