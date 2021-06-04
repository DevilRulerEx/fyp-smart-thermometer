# Smart Non-Contact Thermometer with ESP32

This project is done in accordance to the fufillment of an Electrical Engineering bachelor degree. You may use the following citation to reference my work:

**Chng,  W. P. (2021). Smart non-contact thermometer with ESP32 and modular components. Final Year Project (FYP), Nanyang Technological University, Singapore. https://hdl.handle.net/10356/149139**

You may also reached me on my [LinkedIn](https://www.linkedin.com/in/wee-ping-chng-34841320b/).

## The project prototype

(Breif description)

(Pictures, Videos)

## Components used:
* ESP32 (LilyGo TTGO T-Display ESP32 CP2104) [Link to store page](https://sea.banggood.com/TTGO-T-Display-ESP32-CP2104-WiFi-bluetooth-Module-1_14-Inch-LCD-Development-Board-LILYGO-for-Arduino-products-that-work-with-official-Arduino-boards-p-1522925.html?rmmds=myorder&cur_warehouse=GWTR)
* GM65 Barcode Scanner
* MLX90614 Temperature Sensor

![LilyGo TTGO Pin Diagram](Component%20Information/LilyGo%20TTGO%20T-Display%20ESP32/LilyGo%20TTGO%20Pin%20Diagram.png)
<img src="Component%20Information/MG65%20Barcode%20Scanner/pic03.png" width=47%/>
<img src="Component%20Information/MLX90614%20Temperature%20Sensor/pic01.jpg" width=47% align="right"/>

## Software used:
* Arduino IDE
* XAMPP
* PhpMyAdmin

<img src="Misc%20Images/arduino.png" width=40% align="left"/>
<img src="Misc%20Images/xamp.jpg" width=40% align="right"/>

<img src="Misc%20Images/phpmyadmin.png" width=50%/>

## Setting up the project
If you are already experienced in Arduino project (and I recommend starting with Arduinos projects first before delving into ESP32), programming an ESP32 can be familiar with you, but some setup are required in the Arduino IDE before you can program. Follow the steps in this page to setup the Arduino IDE [(Installing ESP32 in Arduino IDE)](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)

For data collection purposes, I used XAMPP to create a local host server to store all the temperature records. Of course you could use your own web servers.

If you are using the same componenets as I did, include the following libraries in Arduino IDE. Under Tools, set the board to be "TTGO LoRa32-OLED V1".

<img src="Misc%20Images/boardmanager.png"/>
<img src="Misc%20Images/tft_espi.png"/>
<img src="Misc%20Images/mlxlibrary.png"/>

<img src="Project%20Information/Circuit%20Diagram%20Sketch%202.png"/>
