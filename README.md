# Arduino/ESP8266 SSD1322 Library
## For Newhaven NHD-3.12-25664UCY2 OLED Display

The [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) introduces graphics primitives (points, lines, circles, etc.). This library add support for the [Newhaven NHD-3.12-25664UCY2](http://www.newhavendisplay.com/nhd31225664ucy2-p-3537.html) 256x64x16 Yellow OLED Display.  The driver supports 4-wire SPI mode.

This repository was forked from [ESP8266_SSD1322](https://github.com/winneymj/ESP8266_SSD1322) that in turn was based on [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306).

![image](media/arduino101_oled_resize.jpg)

Changes compared to [ESP8266_SSD1322](https://github.com/winneymj/ESP8266_SSD1322):

* Add support for the Arduino 101 (native 3.3 Volt, base on Intel Curie)
* Along the way, I did some minor touch ups:
  * add support for !RESET connected to hardware reset instead of GPIO pin (saves me 1 pin)
  * remove unused variables (`rowTerminated`, `offScreen`, `color`)
  * suppress compiler warning strict-aliasing when accessing fonts
  * remove unused pointer value access e.g. `string++` instead of `string++`
  * add test for `flash_address` unassigned
  * add headers to files indicating support for SSD1322.

Tested and passed on Arduino 101. I like this display, and being able to use the Adafruit GFX primitives is even better!

Details in

* [data sheet](http://www.newhavendisplay.com/specs/NHD-3.12-25664UCY2.pdf)
* [app note](http://www.newhavendisplay.com/app_notes/SSD1322.pdf)


