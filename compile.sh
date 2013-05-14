#!/usr/bin/env bash
make clean ; make all 2>&1 | sed -e "s/error:/$(tput setab 1 ; tput bold)error:$(tput sgr0)/g" -e "s/warning:/$(tput setab 4 ; tput bold)warning:$(tput sgr0)/g" 
