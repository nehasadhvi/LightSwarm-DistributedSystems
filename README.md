# LightSwarm-DistributedSystems
  
  ### IoT Distributed System Connection
<ul>
  <li>This project is a distributed system of ESP8266 devices which can communicate with each other by sending broadcast messages to exchange RGB-light sensor readings. The device with the highest reading, will then forward the readings to the Raspberry Pi as the server</li>
  <li>I used WiFi network to connect between ESP8266 and Raspberry Pi</li>
  <li>Each ESP 8266 device has an RGB sensor and broadcasts its readings to others whenever the network is silent for 100ms. ESP devices have (green) LEDs connected, which shows different brightness (using PWM) according to the light sensor readings</li>
  <li>One of them (who has the highest reading) will become the master that will send the log data to Raspberry Pi as the server. The Master will light up a red LED, in addition to its green LED</li>
  <li>Raspberry Pi will display the Master's photocell data trace on its window screen</li>
  <li>Raspberry Pi is connected with a button. When pushed, it resets all ESP 8266 devices and turns on its yellow LED</li>
  <li>The system works with 2-4 ESP 8266 devices dynamically</li>
</ul>
  ### IoT Cloud Connection
<ul>
  <li>Then I used Node-RED to connect Raspberry Pi to Google Firebase. The data are then sent from Raspberry Pi to Firebase, and saved on a database</li>
  <li>Once they are in the Firebase, we can use any software to access it anytime from anywhere for any purpose. We can also use Node-RED to send some live messages to your phone in real time</li>
  <li>Here, I am sending data from Firebase whenever the database is updated, to Slack installed on smart phone. The data is also extracted from the Firebase to our laptop for further analysis.</li>
</ul>

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. You can clone this repository :

Using SSH -
```
git@github.com:nehasadhvi/LightSwarm-DistributedSystems.git
```

Using HTTPS -
```
https://github.com/nehasadhvi/LightSwarm-DistributedSystems.git
```

### Prerequisites

You will need to have Arduino IDE installed on your OS to code your ESP8266. You can install it from :

```
https://www.arduino.cc/en/Main/Software
```
In order to run your Python code on RaspberryPi to connect it to ESP8266 devices over WiFi, you need to download these libraries on Raspbian:

netifaces Library -
```
pip install netifaces
```

## Built With

* [C++] - Used to code ESP8266 aurduino acting as Light Swarms
* [Python] - Used to code Raspberry Pi acting as base station for the Light Swarms
* [Node-RED](https://flows.nodered.org/node/node-red-contrib-firebase) - Used to connect base station to Firebase
* [Firebase](https://firebase.google.com/products/realtime-database/) - Used store data received by base station into database
* [Slack] - Used to obtain data from Firebase and display those data on real time on smart phones

## Authors

* **Neha Sadhvi** - *Initial work* - [neheasadhvi](https://github.com/nehasadhvi)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
