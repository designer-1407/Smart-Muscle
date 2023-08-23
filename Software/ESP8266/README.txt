Here you will read:
- what code is represented in this directory;
- general information about the code
- how to program the ESP8266;


General information:

	The code for the device is divided into 3 programs.
	"main_esp" - for the main device that acts as a web server.
		"main_esp" also has an additional data file structure. This is the HTML/CSS/JS code of a web page for visualizing measurements.

	"sender_data" - code for additional devices. 
	
	"mac_addres" - code for getting mac addres of the ESP8266.

	"main_esp":
		in this code can be customized:
		- ssid and password for the system's local Wi-Fi network.
		- BOARD_ID of the head unit is always set to 0.
		- The maximum number of devices you have.

	"sender_data":
		in this code can be customized:
		- The ID on line 9 for each device must be unique (from 1 to the number of devices you specified in "main_esp").
		- Mac address of the master device. To determine the mac address of the master device, use "mac_addres"

	"mac_addres":
		After loading and running the code, the mac address of the device will be displayed in the serial port. 
		The code must be written to the device you choose as the master. 


How to program the ESP8266:

	1) To program the ESP8266, you will need a USB-UART converter.
	2) Programming is done in the Arduino IDE environment. Only version 1 is suitable. 
	3) How to set up the environment for ESP programming can be found here: https://www.instructables.com/Setting-Up-the-Arduino-IDE-to-Program-ESP8266/
	4) The programming process is as follows:
		- Connect the Rx, Tx and GND lines from the USB-UART converter to the device when it is turned off. On the device board, Rx and Tx are indicated for ESP8266. 
			Therefore, Rx from the USB-UART converter must be connected to Tx on the device.
		- Hold down the SW-1 button on the board.
		- Turn on the device.
		- Now you can release the button. The device is already in the programming mode.

		-In the Arduino IDE environment:
			- Select the COM port to which the device is connected.
			- Select the device to be programmed. In our case it is "NodeMCU 1.0 (ESP-12E Module)".
			- Select Flash Size: "4MB (FS:2MB OTA:~1019KB)".
			- Write the program code. 

		- Restart the device without touching the button.
	
	5) In addition to the program code, you also need to upload an HTML file to the MAIN device. 
		- Connect the Rx, Tx and GND lines from the USB-UART converter to the device when it is turned off. On the device board, Rx and Tx are indicated for ESP8266. 
			Therefore, Rx from the USB-UART converter must be connected to Tx on the device.
		- Hold down the SW-1 button on the board.
		- Turn on the device.
		- Now you can release the button. The device is already in the programming mode.

		-In the Arduino IDE environment:
			- Select the COM port to which the device is connected.
			- Select the device to be programmed. In our case it is "NodeMCU 1.0 (ESP-12E Module)".
			- Select Flash Size: "4MB (FS:2MB OTA:~1019KB)".
			- Click Tools -> ESP8266 Sketch Data Dpload.

		- Restart the device without touching the button.


