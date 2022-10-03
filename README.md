# Incubator

Script for ESP32 using Arduino to connect to wifi as a client and request config data from heroku server as outlined in:

https://github.com/Tshetrim/HappyFish

Live front-end webpage to change configs:

https://qc-incubator.herokuapp.com/esp32/dashboard

Control light and day/night cycle, and fade-in/out duration. 
Configured to automatically connect to wifi on restart, and run on last config in event of connection loss and continues to try to reestablish connection. 
