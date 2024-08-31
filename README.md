# FlexyPixel

Firmware for embedded system of modular LED panels. The system includes a microcontroller stm32f103, bluetooth module HC-06, flash module hw-234 and led panels (based on WS2812B).
Control is via a mobile application on android, although the system can autonomously display downloaded images and animations. The firmware can be updated via a custom bootloader and application on a PC.


![изображение](https://github.com/user-attachments/assets/6e6d66ee-efe2-4804-a331-550d5400bed1)

## Launch
* ```make``` - building a production version of the code (does not contain tests);
* ```make -f MakefileTest.mk``` - building a test version.

## Structure

The entry point to the program (not taking into account the reset interrupt) is [here](https://github.com/MatveyMelnikov/FlexyPixel/blob/master/Core/Src/main.c). All tests are located [here](https://github.com/MatveyMelnikov/FlexyPixel/tree/master/Tests). Our code, not directly related to STM, can be found [here](https://github.com/MatveyMelnikov/FlexyPixel/tree/master/External).

## Links
* [FlexyPixel](https://github.com/alexp0111/FlexyPixel) - mobile client for embedded LED-panel system;
* [Bootloader](https://github.com/MatveyMelnikov/Bootloader) - a bootloader that allows you to load a user program from a specified address;
* [FirmwareHandler](https://github.com/MatveyMelnikov/FirmwareHandler) - PC program for updating system firmware.

## License

[Licensed via CC BY-NC-ND 4.0](https://creativecommons.org/licenses/by-nc-nd/4.0/)
