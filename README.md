# Arduino_SomfyRTS

Arduino remote for Somfy RTS blinds, with I2C interface.

Based on https://www.romainpiquard.fr/article-133-controler-ses-volets-somfy-avec-un-arduino.php

With I2C connection on pins A4,A5 (for openHAB control with [Arduino_MQTT_I2C](https://github.com/richardbounds/Arduino_MQTT_I2C))

Working with:
- Arduino Nano v3 (AZDelivery clones didn't work, but [this one](https://www.ebay.co.uk/itm/Arduino-Nano-Mini-Pro-R3-compatible-board-with-USB-lead-ATmega328-UK-Supplier/133519577527) did)
- 433.92 MHz RF Transmitter, from [Amazon](https://www.amazon.fr/gp/product/B016V18KZ8/ref=as_li_tl?ie=UTF8&camp=1642&creative=6746&creativeASIN=B016V18KZ8&linkCode=as2&tag=deejayfool00-21&linkId=95864c4be00b304a0403dffcc16ed480), connected to 5v and pin D5.
- 433.42 MHz Resonator (to replace resonator on the transmitter) from [eBay](https://www.ebay.fr/itm/10PCS-433-42M-433-42MHz-R433-F433-SAW-Resonator-Crystals-TO-39-/331637441887)
- 433MHz Antenna, from [Amazon](https://www.amazon.fr/gp/product/B00SO651VU/ref=as_li_tl?ie=UTF8&camp=1642&creative=6746&creativeASIN=B00SO651VU&linkCode=as2&tag=deejayfool00-21&linkId=216ed68eb274104b5d08b943566c9e0a)