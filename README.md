# lisa-donations
Enabling donations through RFID/NFC tags on stations placed throughout the house.

## Basic functionality
RFID/NFC readers are placed at points of interests to enable donations. The stations have a fixed donation amount associated with them.

Swiping the card will transmit the tag id to the server which processes the donation request.
The server returns the tag owners name and the resulting balance.

## Server 
The server will be placed in the local network to simplify encryption and security requirements.
It will communication with the external server through a secure SSL connection.

### Hardware
The target hardware is a RaspberryPi or similar

## Clients
### Hardware
The target hardware is a ESP8266
