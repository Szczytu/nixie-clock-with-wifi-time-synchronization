# nixie-clock-with-wifi-time-synchronization

The project shows the realization of a clock synchronized via NTP (Network Time Protocol), which connects to the server as a client via WiFi network.

<div align="center">
  <img src="https://github.com/user-attachments/assets/298220fc-c43b-4acf-943a-f71bef6a60b2">
  <img src="https://github.com/user-attachments/assets/ee1c5626-6051-4007-a467-95551fabe575">
</div>

## Connecting the clock to the internet 

Once the clock is connected to the mains power supply, the clock switches to the Access Point function and creates a WEB page where you need to configure the clock. 

<div align="center">
  <img src="https://github.com/user-attachments/assets/af882506-f91b-4b19-854e-57743f1a6c6d">
</div>

After logging into the created AP, the user will be automatically redirected to the WEB page (default address 192.168.4.1). To properly configure the time, first set the time zone and then proceed to the Wi-Fi configuration. The “Set time zone” button allows the user to select the appropriate time for the region. After confirming the selected time zone with the “Save” button, to complete the time configuration, go back to the configuration panel and select “Configure WiFi”.

<div align="center">
  <img src="https://github.com/user-attachments/assets/2b5aa388-52e8-4409-978e-2bc38e22a994">
</div>

If the user first sets up the Wi-Fi network, the clock enters station mode, ejects from the WEB page and starts downloading the universal time from the time server. To reconfigure, press the RESET button implemented on pin 2 (D4) of the NODEMCUv3+ ESP8266 module.
After saving the Wi-Fi network, the module starts downloading the time from the server, then the downloaded time is corrected by the time zone and is sent via the serial port to the station
destination - the arduino. In the event that the module is unable to connect to the selected network, it resets and starts working in AP mode again, allowing you to reconfigure. The time is corrected by time zone and sent to the microcontroller

### Prototype of nixie clock synchronized via wifi on contact boards

<div align="center">
  <video src="https://github.com/user-attachments/assets/46853f0a-dc1a-495f-91aa-ef3c661f1603" width="500" controls></video>
</div>

# Clock and PCB design

The clock board design is located in the folder "PCB Design and schematic NIXIE_CLOCK_WiFi". The project was created in EasyEDA.
## Schematic

<div align="center">
  <img src="https://github.com/user-attachments/assets/6005aa87-4761-4f30-8a50-4c1d686fae1f">
</div>

## Overview of connecting the Arduino Mega 2560 to the NODEMCUv3 module

### Tab.1 

<div align="center">
  <img src="https://github.com/user-attachments/assets/1c0e8ade-3116-4d32-b64e-5017babe3ac1">
</div>

### Tab.2 

<div align="center">
  <img src="https://github.com/user-attachments/assets/6cb5523f-dff4-43be-88b6-d197a895c401">
</div>

## 2D & 3D model

<div align="center">
  <img src="https://github.com/user-attachments/assets/0b7f61f3-279c-4aae-88c6-4f93528c5caf">
  <img src="https://github.com/user-attachments/assets/fa523a78-6afe-419e-af66-6c50c42176a7">
  <img src="https://github.com/user-attachments/assets/7ae4ea59-546d-4fb6-8a0d-9475b08432b7">
</div>

