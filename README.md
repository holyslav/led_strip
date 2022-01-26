# LED Strip
LED strip project with wifi control. Wifi login and password are set in menuconfig. Able to display the color management page by ip. Generates a PWM signal on pins 14 - Red color, 12 - Blue color, 13 - Green color. Auto-generation of a binary image of a partition based on a folder with data, while the size for the image is automatically taken from partitions.csv. To work, you need either my fork of the SDK, https://github.com/holyslav/ESP8266_RTOS_SDK, or if the pull request is accepted, then you can take the official version.

## How to use example
```
Write your code in the main folder, add your spiffs files to the data folder. Call `script/flash.sh` and your project, along with the files from the data folder, will be loaded into esp memory.
You can also upload the program and storage separately via scripts/flash-app.sh and scripts/flash-storage.sh, respectively.
To build frontend run ./scripts/build-front.sh.
```

### Software required
```
- HolySlav for SDK (https://github.com/holyslav/ESP8266_RTOS_SDK)
- Node.js
```

### Hardware required
```
ESP8266
```

### Build and flash
Build the project and flash it to the board, then run monitor tool to view serial output:
```
./scripts/build-front.sh (if needed) 
./scripts/flash.sh && ./scripts/monitor.sh
```

(To exit the serial monitor, type ``Ctrl-]``.)

## Example output
```
I (43) boot: ESP-IDF v3.4-53-g83517ba1-dirty 2nd stage bootloader
I (44) qio_mode: Enabling default flash chip QIO
I (52) boot: SPI Speed      : 40MHz
I (58) boot: SPI Mode       : QIO
I (64) boot: SPI Flash Size : 4MB
I (70) boot: Partition Table:
I (76) boot: ## Label            Usage          Type ST Offset   Length
I (87) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (98) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (109) boot:  2 factory          factory app      00 00 00010000 00100000
I (121) boot:  3 storage          Unknown data     01 82 00110000 00200000
I (132) boot: End of partition table
I (206) boot: Loaded app from partition at offset 0x10000
I (236) led_strip: app_main
I (237) led_strip: Initializing SPIFFS. max_files: 17
I (334) led_strip: Partition size: total: 1920401, used: 1239689
I (336) led_strip: Reading file
I (366) led_strip: Read from file: '<!DOCTYPE html><html lang=en><head><meta charset=utf-8><meta ht'
I (455) phy_init: phy ver: 1166_0
I (464) example_connect: Connecting to MySweetHome...
I (2747) wifi:state: 0 -> 2 (b0)
I (2749) wifi:state: 2 -> 3 (0)
I (2751) wifi:state: 3 -> 5 (10)
I (3040) wifi:connected with MySweetHome, aid = 2, channel 11, HT20, bssid = ec:43:f6:d2:75:56
I (4253) example_connect: Connected to MySweetHome: 255.255.255.0, gw: 192.168.0.1
I (4257) example_connect: IPv4 address: 192.168.0.77
I (4266) led_strip: Starting HTTP Server
I (4278) gpio: GPIO[12]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (4287) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (4302) gpio: GPIO[14]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (4316) pwm: --- PWM v3.4

```

To erase the contents of all partition, run `scripts/erase.sh`.
