#!/bin/bash

full_deploy=false
host_address=false
username=false

while getopts "fh:u:" opt; do
    case "$opt" in
    f)
        full_deploy=true
        ;;
    h)  host_address=$OPTARG
        ;;
    u)  username=$OPTARG
        ;;
    esac
done

if [ "$host_address" = false ] || [ "$username" = false ]; then
    echo 'Host address and username must be supplyed.'
    exit 0
fi

scp *.cl            $username@$host_address:~/DEM
scp bin/DEM         $username@$host_address:~/DEM/bin/


if [ "$full_deploy" = true ]; then

scp DEM.ini         $username@$host_address:~/DEM/bin/

ssh -tt $username@$host_address << 'ENDSSH'
rm ~/DEM/libs/*
exit
ENDSSH

scp deploy_libs/* $username@$host_address:~/DEM/libs
    
ssh -tt $username@$host_address << 'ENDSSH'
sudo ldconfig
exit
ENDSSH

fi

