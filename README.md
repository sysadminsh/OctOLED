# OctOLED - remote mini display for OctoPrint
OctOLED is remote (WiFi on ESP8266) mini display (SSD1306 128x64 OLED) showing the status of OctoPrint. The motivation of the project was that the printer is behind me and I do not always have space on the screen to have an open card with the OctoPrint website to see the print progress. The solution works perfectly ingeniously, allowing easy access to the current progress bar and printer temperature status. I have designed a quite well-fitting casing for the whole which (as it seems to me) should not resemble a mare. To achieve this goal in the upper part I placed the display itself and in the stand ESP8266, leaving a cable tunnel between them. 

## Required Parts
-   1x  [ESP8266 (I used NodeMCU v2)](https://nettigo.pl/products/modul-wifi-nodemcu-v2-bezprzewodowy-modul-oparty-na-esp8266-12e)
-   1x  [SSD1306 (OLED 128x64 display)](https://nettigo.pl/products/wyswietlacz-oled-0-96-i2c-128x64-niebieski)
-   4x  [Wire F-F 20cm (I used 40 from Nettigo)](https://nettigo.pl/products/przewody-f-f-20-cm-40-szt)
- 1x MicroUSB charger for power project after finished

## Connection Scheme
![ConnectionScheme](https://majsterkowo.pl/wp-content/uploads/2018/09/nodemcu_i2c_oled.png?x85922&x85922)

## Module Programming
The code for the ESP8266 module I wrote in the Arduino IDE for which you will need to install the libraries:

 - [ESP8266 board driver](https://github.com/esp8266/Arduino)
 - [ESP8266-oled-ssd1306 library](https://github.com/ThingPulse/esp8266-oled-ssd1306)

The ESP8266 driver is installed using the Library Manager in Arduino IDE, while the board driver requires Arduino IDE preferences to enter the http://arduino.esp8266.com/stable/package_esp8266com_index.json link in additional URLs for the Tile Manager and then use the Tile Manager. I choosed NodeMCU V1 as the board for the project (despite using V2) because V2 is not available and the profile for V1 uses the same processor as V2.
Detailed instrunction how to install plate driver you can found on his GitHub page.

Now you can upload my sketch to the ESP8266 but **remember to change variables on the top of the file (your WiFi SSID, Password etc.)**

## Housing
The element of the project is also the housing whose files you will find on [Thingiverse](https://www.thingiverse.com/thing:3095499). After printing the stand, all you have to do is press the wires through the middle hole and then insert the ESP8266 into the stand from the bottom and the display into the upper module and depending what you choose: screw it with M2x12 screws or just glue it together.

![Cable Organization](http://qba.wtf/43097d9df2fa/Image%2525202018-09-11%252520at%25252010.12.26%252520PM.png)

*Method how to put cables in to the tunnel (temporarily removed connector housings)*

![NodeMCU](http://qba.wtf/74215a6435b1/Image%2525202018-09-11%252520at%25252010.13.13%252520PM.png)

*The NodeMCU should be pressed slightly under the slant (aiming the USB port at the hole)
The cables can be pushed through the screw holes (before re-inserting the connector housing)*

![USB Connector](http://qba.wtf/57d183f657c1/Image%2525202018-09-11%252520at%25252010.14.22%252520PM.png)

*The MicroUSB port should fit perfectly :)*

## Real project photos
![Printing](http://qba.wtf/6c5f4ed3e686/Image%2525202018-09-12%252520at%25252012.24.27%252520AM.png)

*Status when printing*

![Idle](http://qba.wtf/ed05b5a90ab3/Image%2525202018-09-12%252520at%25252012.25.05%252520AM.png)

*Status when idle*

## Video in action
You can see this project in action on [YouTube](https://www.youtube.com/watch?v=5jP-DI0TSRg)

## Other download
 - Housing STL model at [Thingiverse](https://www.thingiverse.com/thing:3095499)
