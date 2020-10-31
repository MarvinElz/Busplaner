#!/bin/bash
echo "Service kopieren."
sudo cp Busplaner.service /etc/systemd/system/Busplaner.service

echo "Rechte setzen."
sudo chown root:root /etc/systemd/system/Busplaner.service
