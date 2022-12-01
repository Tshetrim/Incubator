<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Tshetrim/Incubator">
    <img src="images/incubator-image.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Incubator</h3>

  <p align="center">
    Automated Controllable IOT Lighting
    <br />
    <a href="https://github.com/Tshetrim/Incubator"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/Tshetrim/Incubator">View Demo</a>
    ·
    <a href="https://github.com/Tshetrim/Incubator/issues">Report Bug</a>
    ·
    <a href="https://github.com/Tshetrim/Incubator/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li><a href="#usage">Usage</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project


Script for ESP32 using Arduino to connect to wifi as a client and request config data from heroku server as outlined in:

https://github.com/Tshetrim/HappyFish


<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

**note: The term Incubator and ESP32 will be used interchangably here, the ESP32 is the microcontroller embedded within the incubator for this use case** 
### 1. Starting the Incubator (specific use case)
    Once the Incubator is connected to the outlet, turn on the main switch then make sure the O/I light switch on the top is also switched towards the I
    That will turn on the ESP32 which allows the light changing 
### 2. Connecting the Incubator (ESP32) to Wi-Fi    --- (only needs to be once, or if changing Wi-Fi)
    - Once the Incubator is on, the light will be RED for 60 seconds 
    - In that timeframe you can connect to the Incubator using a device such as a phone or laptop by going to the networks
        - Look for the Incubator's ssid: Incubator AP 
        - Press connect and enter the password: password
    - Once you connect to the Incubator, certain devices like an IPhone may immediately display the Wifi-Provisioning web portal, if not 
        - Goto a webrowser and type in: 
            http://192.168.4.1/
        - the web portal should load in (note the portal will likely be quite slow due to ESP32's hardware) 
            1. Press: Configure Wi-Fi
            2. A list of routers you can connect to will be scanned and displayed. 
            3. Click the SSID of the router you would like to connect to or type it in the SSID box
            4. Enter the password or leave blank if no password is needed (for our usecase, QCSecured and leave blank)
        - Once saved, the Incubator will close the web portal and attempt to connect to the router with the credientials put in 
        - The Incubator will be BLUE while trying to connect, then if successful, will turn GREEN
### 3. Controlling the Incubator 

#### Logging In to make changes: 
    - Once the Incubator is connected to Wi-Fi, you can access the dashboard by going to this website: 
        https://qc-incubator.up.railway.app/esp32
    - Enter the password (to be sent separetely) and you will be able to make changes to the settings that will save 
    - note make sure you are logged in for changes to save, if you read the message at the top, it will notify you if you are not logged in 

#### Setting Controls:  
    - Time: Once you can view the dashboard, the first thing to note is the Incubator is running on UTC TIME, the one outlined in a green box.
    - Day/Night cycle: You can change the start and end start times by typing in a number between 00:00 and 23:59 respectively 
    - Fade Duration: You can change the duration of the fade in and fade out by sliding the bar or entering a number in minutes 
        - Note fade in is when the light will gradually increase in brightness when allowing light, and fade out is gradually decreasing in blindness
        - To have no fade in/ fade out, simply set the duration to 0 
    - Color: Slide the RGB colors to change the color of the incubator

<img src="images/dashboard.png" alt="dashboard webpage" width="200" height="200">

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/github_username/repo_name.svg?style=for-the-badge
[contributors-url]: https://github.com/Tshetrim/Incubator/graphs/contributors
[product-screenshot]: images/incubator-image.png