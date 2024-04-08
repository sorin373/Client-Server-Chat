#!/bin/bash

# Update package lists and upgrade installed packages
sudo apt-get update && sudo apt-get upgrade

# Install cmake via snap
sudo snap install cmake --classic

# Install libmysqlcppconn-dev
sudo apt-get install libmysqlcppconn-dev