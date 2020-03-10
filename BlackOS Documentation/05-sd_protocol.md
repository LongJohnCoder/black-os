# SD Protocol

This is intented to be a short summary of the most important concepts.

# Packet format

There exists three different packet formats in the SD card protocol

- Command
- Response
- Data

Command packets are tokens that starts an operation. A command token is 48-bits, starting with a start bit and ending with a stop bit. Is also includes a 7-bit CRC checksum at the end. Response packets are sent from the SD card to the host as a responce to the previous command. Its length is either 48-bits or 136-bits. Data on the command line are transmitted MSB first. Finally we have the data packets, whcih can be either usual data (8-bits) or wide data (SD register). Wide data is transmitted MSB first and usual data is transmitted LSByte first and MSBit first.

# Registers

- CID - card ID number
- RCA - relative card address; suggested by the card, and approved by the host
- CSD - card spesific data; contains info about the cards operation conditions
- SCR - SD configuration register; contains info about the SD cards special features
- OCR - operating condition register
- SSD - SD status; contains info about the SD cards proprietary features
- CSR - card status

