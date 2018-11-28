/*
 Cooperative IOT Self Organizing Example
 SwitchDoc Labs, August 2015
 */
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#undef DEBUG

char ssid[] = "Neha Sadhvi";
char pass[] = "Casino007@@"; // your network password

#define VERSIONNUMBER 28
#define LOGGERIPINC 20
#define SWARMSIZE 5
// 30 seconds is too old - it must be dead
#define SWARMTOOOLD 30000
int mySwarmID = 0;
// Packet Types
#define LIGHT_UPDATE_PACKET 0
#define RESET_SWARM_PACKET 1
#define CHANGE_TEST_PACKET 2
#define RESET_ME_PACKET 3
#define DEFINE_SERVER_LOGGER_PACKET 4
#define LOG_TO_SERVER_PACKET 5
// delay 0-MAXDELAY seconds
#define MAXDELAY 10000
unsigned int localPort = 2910; // local port to listen for UDP packets
// master variables
boolean masterState = true; // True if master, False if not
int swarmClear[SWARMSIZE];
int swarmVersion[SWARMSIZE];
int swarmState[SWARMSIZE];
long swarmTimeStamp[SWARMSIZE]; // for aging
IPAddress serverAddress = IPAddress(0, 0, 0, 0); // default no IP Address
int swarmAddresses[SWARMSIZE]; // Swarm addresses

int photocellPin = 0;
unsigned long secondsCount = 0;
unsigned long lastSecondsCount = 0;
int clearColor;

const int PACKET_SIZE = 14; // Light Update Packet
const int BUFFERSIZE = 1024;
byte packetBuffer[BUFFERSIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
byte localBuffer[BUFFERSIZE];
WiFiUDP udp;
IPAddress localIP;

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("");
  Serial.println("--------------------------");
  Serial.println("LightSwarm");
  Serial.print("Version ");
  Serial.println(VERSIONNUMBER);
  Serial.println("--------------------------");
  Serial.println(F(" 09/03/2015"));
  Serial.print(F("Compiled at:"));
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println();
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);
  randomSeed (analogRead(A0));
  Serial.print("analogRead(A0)=");
  Serial.println(analogRead(A0));
  // everybody starts at 0 and changes from there
  mySwarmID = 0;
  // We start by connecting to a WiFi network
  Serial.print("LightSwarm Instance: ");
  Serial.println(mySwarmID);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  // initialize Swarm Address - we start out as swarmID of 0

  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  // initialize light sensor and arrays
  int i;
  for (i = 0; i < SWARMSIZE; i++) {
    swarmAddresses[i] = 0;
    swarmClear[i] = 0;
    swarmTimeStamp[i] = -1;
  }
  swarmClear[mySwarmID] = 0;
  swarmTimeStamp[mySwarmID] = 1; // I am always in time to myself
  swarmVersion[mySwarmID] = VERSIONNUMBER;
  swarmState[mySwarmID] = masterState;

  // set SwarmID based on IP address
  localIP = WiFi.localIP();

  swarmAddresses[0] = localIP[3];

  mySwarmID = 0;

  Serial.print("MySwarmID=");
  Serial.println(mySwarmID);
}


void loop() {

  secondsCount = millis();

  uint16_t r, g, b, c;
  int y;

  tcs.getRawData(&r, &g, &b, &c);
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");

  clearColor = c;

  swarmClear[mySwarmID] = clearColor;
  
  y=map(clearColor,1500,60000,0,255);
  analogWrite(D6,y);
  delay(300);

//  delay(50);
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.print(".");
  } else {
    Serial.print(cb);
    // We've received a packet, read the data from it
    while(cb){
      udp.read(packetBuffer, PACKET_SIZE);
      // for (i = 0; i < PACKET_SIZE; i++) {
      //   localBuffer[i] = packetBuffer[i]
      // }

      // read the packet into the buffer 
      Serial.print("packetbuffer[1] =");
      Serial.println(packetBuffer[1]);
      if (packetBuffer[1] == LIGHT_UPDATE_PACKET) {
        Serial.print("LIGHT_UPDATE_PACKET received from LightSwarm #");
        Serial.println(packetBuffer[2]);
        int swarmIndex = setAndReturnMySwarmIndex(packetBuffer[2]);
        Serial.print("LS Packet Recieved from #");
        Serial.print(packetBuffer[2]);
        Serial.print(" SwarmState:");
        if (packetBuffer[3] == 0)
          Serial.print("SLAVE");
        else
          Serial.print("MASTER");
        Serial.print(" Photo Cell Reading:");
        Serial.print(packetBuffer[5] * 256 + packetBuffer[6]);
        Serial.print(" Version=");
        Serial.println(packetBuffer[4]);
        // record the incoming clear color
        swarmClear[swarmIndex] = packetBuffer[5] * 256 + packetBuffer[6];
        swarmVersion[swarmIndex] = packetBuffer[4];
        swarmState[swarmIndex] = packetBuffer[3];
        swarmTimeStamp[swarmIndex] = millis();
        // Check to see if I am master!
        checkAndSetIfMaster();
      }
      if (packetBuffer[1] == RESET_SWARM_PACKET) {
        Serial.println(">>>>>>>>>RESET_SWARM_PACKET Packet Recieved");
        masterState = true;
        Serial.println(
            "Reset Swarm: I just BECAME Master (and everybody else!)");
        digitalWrite(D7, HIGH);
      }
      if (packetBuffer[1] == DEFINE_SERVER_LOGGER_PACKET) {
      Serial.println(">>>>>>>>>DEFINE_SERVER_LOGGER_PACKET Packet Recieved");
      serverAddress = IPAddress(packetBuffer[4], packetBuffer[5],
          packetBuffer[6], packetBuffer[7]);
      Serial.print("Server address received: ");
      Serial.println(serverAddress);
      }
      cb = udp.parsePacket();
    }
  }

  Serial.print("MasterStatus:");
  if (masterState == true) {
    Serial.print("MASTER");
    digitalWrite(D7, HIGH);
    sendLogToServer();
  } else {
    Serial.print("SLAVE");
    digitalWrite(D7, LOW);
  }
  Serial.print("/cc=");
  Serial.print(clearColor);
  Serial.print("/KS:");
  Serial.println(serverAddress);

  Serial.println("--------");

  int i;
  for (i = 0; i < SWARMSIZE; i++) {
    Serial.print("swarmAddress[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(swarmAddresses[i]);
  }
  Serial.println("--------");

  if((secondsCount - lastSecondsCount) >= MAXDELAY)
  {
    secondsCount = millis();
    lastSecondsCount = secondsCount;
    broadcastARandomUpdatePacket();
    sendLogToServer();
   }
}

// send an LIGHT Packet request to the swarms at the given address
unsigned long sendLightUpdatePacket(IPAddress & address) {
  memset(packetBuffer, 0, PACKET_SIZE);
  // Initialize values needed to form Light Packet
  // (see URL above for details on the packets)
  packetBuffer[0] = 0xF0; // StartByte
  packetBuffer[1] = LIGHT_UPDATE_PACKET; // Packet Type
  packetBuffer[2] = localIP[3]; // Sending Swarm Number
  packetBuffer[3] = masterState; // 0 = slave, 1 = master
  packetBuffer[4] = VERSIONNUMBER; // Software Version
  packetBuffer[5] = (clearColor & 0xFF00) >> 8; // Clear High Byte
  packetBuffer[6] = (clearColor & 0x00FF); // Clear Low Byte
  packetBuffer[7] = 0x00; // Red High Byte
  packetBuffer[8] = 0x00; // Red Low Byte
  packetBuffer[9] = 0x00; // green High Byte
  packetBuffer[10] = 0x00; // green Low Byte
  packetBuffer[11] = 0x00; // blue High Byte
  packetBuffer[12] = 0x00; // blue Low Byte
  packetBuffer[13] = 0x0F; //End Byte
  // all Light Packet fields have been given values, now
  // you can send a packet requesting coordination
  udp.beginPacketMulticast(address, localPort, WiFi.localIP());
  udp.write(packetBuffer, PACKET_SIZE);
  udp.endPacket();
}

void broadcastARandomUpdatePacket() {
  int sendToLightSwarm = 255;
  Serial.print("Broadcast ToSwarm = ");
  Serial.print(sendToLightSwarm);
  Serial.print(" ");
  IPAddress sendSwarmAddress(192, 168, 43, sendToLightSwarm); // my Swarm Address
  sendLightUpdatePacket(sendSwarmAddress);
}

void checkAndSetIfMaster() {
  int i;
  for (i = 0; i < SWARMSIZE; i++) {
#ifdef DEBUG
    Serial.print("swarmClear[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.print(swarmClear[i]);
    Serial.print(" swarmTimeStamp[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(swarmTimeStamp[i]);
#endif
    Serial.print("#");
    Serial.print(i);
    Serial.print("/");
    Serial.print(swarmState[i]);
    Serial.print("/");
    Serial.print(swarmVersion[i]);
    Serial.print(":");
    // age data
    int howLongAgo = millis() - swarmTimeStamp[i];
    if (swarmTimeStamp[i] == 0) {
      Serial.print("TO ");
    } else if (swarmTimeStamp[i] == -1) {
      Serial.print("NP ");
    } else if (swarmTimeStamp[i] == 1) {
      Serial.print("ME ");
    } else if (howLongAgo > SWARMTOOOLD) {
      Serial.print("TO ");
      swarmTimeStamp[i] = 0;
      swarmClear[i] = 0;
    } else {
      Serial.print("PR ");
    }
  }
  Serial.println();
  boolean setMaster = true;
  for (i = 0; i < SWARMSIZE; i++) {
    if (swarmClear[mySwarmID] >= swarmClear[i]) {
      // I might be master!
    } else {
      // nope, not master
      setMaster = false;
      break;
    }
  }
  if (setMaster == true) {
    if (masterState == false) {
      Serial.println("I just BECAME Master");
      digitalWrite(D7, HIGH);
      sendLogToServer();
    }
    masterState = true;
  } else {
    if (masterState == true) {
      Serial.println("I just LOST Master");
      digitalWrite(D7, LOW);
    }
    masterState = false;
  }
  swarmState[mySwarmID] = masterState;
}


int setAndReturnMySwarmIndex(int incomingID) {
  int i;
  for (i = 0; i < SWARMSIZE; i++) {
    if (swarmAddresses[i] == incomingID) {
      return i;
    } else if (swarmAddresses[i] == 0) // not in the system, so put it in
        {

      swarmAddresses[i] = incomingID;
      Serial.print("incomingID ");
      Serial.print(incomingID);
      Serial.print(" assigned #");
      Serial.println(i);
      return i;
    }

  }

  // if we get here, then we have a new swarm member.
  // Delete the oldest swarm member and add the new one in
  // (this will probably be the one that dropped out)

  int oldSwarmID;
  long oldTime;
  oldTime = millis();
  for (i = 0; i < SWARMSIZE; i++) {
    if (oldTime > swarmTimeStamp[i]) {
      oldTime = swarmTimeStamp[i];
      oldSwarmID = i;
    }

  }
  // remove the old one and put this one in....
  swarmAddresses[oldSwarmID] = incomingID;
  // the rest will be filled in by Light Packet Receive

}


// send log packet to Server if master and server address defined
void sendLogToServer() {
  
  if (masterState == true) {
    // now check for server address defined
    if ((serverAddress[0] == 0) && (serverAddress[1] == 0)) {
      return; // we are done. not defined
    } 
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, BUFFERSIZE);
    // Initialize values needed to form Light Packet
    // (see URL above for details on the packets)
    packetBuffer[0] = 0xF0; // StartByte
    packetBuffer[1] = LOG_TO_SERVER_PACKET; // Packet Type
    packetBuffer[2] = localIP[3]; // Sending Swarm Number
    packetBuffer[3] = 0x00;
    packetBuffer[4] = VERSIONNUMBER; // Software Version
    packetBuffer[5] = clearColor / 256; // Clear High Byte
    packetBuffer[6] = clearColor % 256; // Clear Low Byte
    packetBuffer[7] = 0x00; // Red High Byte
    packetBuffer[8] = 0x00; // Red Low Byte
    packetBuffer[9] = 0x00; // green High Byte
    packetBuffer[10] = 0x00; // green Low Byte
    packetBuffer[11] = 0x00; // blue High Byte
    packetBuffer[12] = 0x00; // blue Low Byte
    packetBuffer[13] = 0x0F; //End Byte
    
    Serial.print("Sending Log to Server:");
  
    udp.beginPacket(serverAddress, localPort); //
    udp.write(packetBuffer, PACKET_SIZE);
    udp.endPacket();
  }
}
