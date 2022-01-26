# SPIFFS example

Modified spiffs example from ESP-RTOS-SDK. Added auto-generation of a binary image of a partition based on a folder with data, while the size for the image is automatically taken from partitions.csv. To work, you need either my fork of the SDK, https://github.com/holyslav/ESP8266_RTOS_SDK, or if the pull request is accepted, then you can take the official version.

## How to use example
```
Write your code in the main folder, add your spiffs files to the data folder. Call `script/flash.sh` and your project, along with the files from the data folder, will be loaded into esp memory.
You can also upload the program and storage separately via scripts/flash-app.sh and scripts/flash-storage.sh, respectively
```

### Hardware required
```
ESP8266
```

### Build and flash
Build the project and flash it to the board, then run monitor tool to view serial output:
```
scripts/flash.sh && scripts/monitor.sh
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
I (233) dir2spiffs: Initializing SPIFFS. max_files: 17
I (328) example: Partition size: total: 1920401, used: 502
I (329) dir2spiffs: Reading file
I (332) dir2spiffs: Read from file: 'Hello world!'
I (335) dir2spiffs: SPIFFS unmounted
```

To erase the contents of all partition, run `scripts/erase.sh`.
