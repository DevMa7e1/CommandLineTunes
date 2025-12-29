#!/bin/bash
if [[ $EUID -ne 0 ]]; then
    echo "Please run this script as root."
    exit 100
else
apt install libc6 libstdc++6 libgcc-s1 wget
wget https://github.com/DevMa7e1/CommandLineTunes/releases/download/v1.1.0/CLTunes.Linux-x86_64
mv CLTunes.Linux-x86_64 /usr/local/bin/cltunes
chmod +x /usr/local/bin/cltunes
echo "CommandLineTunes was succesfully installed! To run it, just run the cltunes command in a terminal."
fi