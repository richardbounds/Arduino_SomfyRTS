#include <EEPROM.h>
#define PORT_TX 5
 
#define SYMBOL 640
#define RAISE 0x2
#define STOP 0x1
#define LOWER 0x4
#define PROG 0x8
#define EEPROM_ADDRESS 0
 
#define VERSION 1

//Adapted from https://www.romainpiquard.fr/article-133-controler-ses-volets-somfy-avec-un-arduino.php

byte frame[7];
byte checksum;
const int remoteCount = 32;
 
struct Remote
{
  unsigned long remoteID;
  unsigned int rollingCode;
};
 
struct SomfyController
{
  int appVersion;
  Remote remotes[remoteCount];
};


SomfyController somfyControllers;
Remote newRemotes [remoteCount];

uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
 
void BuildFrame(unsigned long remoteID, unsigned int rollingCode, byte *frame, byte button);
void SendCommand(byte *frame, byte sync);
void processRTSCommand(String data);
 
 
void setup()
{
  Serial.begin(115200);
  DDRD |= 1<<PORT_TX;
  PORTD &= !(1<<PORT_TX);
 
  EEPROM.get(EEPROM_ADDRESS, somfyControllers);
 
  if (somfyControllers.appVersion < VERSION)
  {
    Serial.println("The version of the application in memory is not in the correct version or the memory is empty");
    somfyControllers.appVersion = VERSION;
    
    for (int i=0; i< remoteCount; i++) {
      newRemotes[i] = {0x123456 + i, 0};
    }
    memcpy(&somfyControllers.remotes, &newRemotes, sizeof(newRemotes));
    EEPROM.put(EEPROM_ADDRESS, somfyControllers);
  }
 
  for (int i = 0; i < (sizeof(somfyControllers.remotes) / sizeof(Remote)); i++)
  {
    Remote currentRemote = somfyControllers.remotes[i];
    Serial.print("Remote ["); Serial.print(i); Serial.println("]");
    Serial.print("\tRemote ID : "); Serial.println(currentRemote.remoteID, HEX);
    Serial.print("\tCurrent counter : "); Serial.println(currentRemote.rollingCode);
  }
}
 
void loop()
{
  if (Serial.available())
  {
    String data = "";
    char instruction = Serial.read();
    int remote = Serial.parseInt();
    
    while (Serial.available())
    {
      Serial.read();
    }

    Serial.print("Data: " + String(instruction) + "," + String(remote));
    Serial.println(); 
    processRTSCommand(instruction, remote);
  }
}


void processRTSCommand(char instruction, int remotePosition) {
  
      Serial.print("Remote "); Serial.println(remotePosition);
 
      Remote remote = somfyControllers.remotes[remotePosition];
      unsigned long remoteID = remote.remoteID;
      unsigned int rollingCode = remote.rollingCode;
      
      Serial.println("");
      if (instruction == 'u')
      {
        Serial.println("Raise");
        BuildFrame(remoteID, rollingCode, frame, RAISE);
       }
      else if (instruction == 's')
      {
        Serial.println("Stop");
        BuildFrame(remoteID, rollingCode, frame, STOP);
      }
      else if (instruction == 'd')
      {
        Serial.println("Lower");
        BuildFrame(remoteID, rollingCode, frame, LOWER);
      }
      else if (instruction == 'p')
      {
        Serial.println("Prog");
        BuildFrame(remoteID, rollingCode, frame, PROG);
      }
      else
      {
        Serial.println("Unrecognised instruction");
      }
  
      Serial.println("");
      SendCommand(frame, 2);
      for (int i = 0; i < 2; i++)
      {
        SendCommand(frame, 7);
      }
  
      //Increments the counter and saves it in memory
      somfyControllers.remotes[remotePosition].rollingCode++;
      EEPROM.put(EEPROM_ADDRESS, somfyControllers);

}
 
 
void BuildFrame(unsigned long remoteID, unsigned int rollingCode, byte *frame, byte button)
{
  frame[0] = 0xA7;
  frame[1] = button << 4;
  frame[2] = rollingCode >> 8;
  frame[3] = rollingCode;
  frame[4] = remoteID >> 16;
  frame[5] = remoteID >>  8;
  frame[6] = remoteID;
 
  Serial.print("Frame         : ");
  for (byte i = 0; i < 7; i++)
  {
    if (frame[i] >> 4 == 0)
    {
      Serial.print("0");
    }
    Serial.print(frame[i], HEX); Serial.print(" ");
  }
 
  checksum = 0;
  for (byte i = 0; i < 7; i++)
  {
    checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
  }
  checksum &= 0b1111;
 
  frame[1] |= checksum;
 
 
  Serial.println(""); Serial.print("Avec checksum : ");
  for (byte i = 0; i < 7; i++)
  {
    if (frame[i] >> 4 == 0)
    {
      Serial.print("0");
    }
    Serial.print(frame[i], HEX); Serial.print(" ");
  }
 
  for (byte i = 1; i < 7; i++)
  {
    frame[i] ^= frame[i-1];
  }
 
  Serial.println(""); Serial.print("Obfuscation    : ");
  for (byte i = 0; i < 7; i++)
  {
    if (frame[i] >> 4 == 0)
    {
      Serial.print("0");
    }
    Serial.print(frame[i], HEX); Serial.print(" ");
  }
  Serial.println("");
  Serial.print("Compteur  : "); Serial.println(rollingCode);
}
 
void SendCommand(byte *frame, byte sync)
{
  if (sync == 2)
  {
    PORTD |= 1<<PORT_TX;
    delayMicroseconds(9415);
    PORTD &= !(1<<PORT_TX);
    delayMicroseconds(89565);
  }
 
  for (int i = 0; i < sync; i++)
  {
    PORTD |= 1<<PORT_TX;
    delayMicroseconds(4*SYMBOL);
    PORTD &= !(1<<PORT_TX);
    delayMicroseconds(4*SYMBOL);
  }
 
  PORTD |= 1<<PORT_TX;
  delayMicroseconds(4550);
  PORTD &= !(1<<PORT_TX);
  delayMicroseconds(SYMBOL);
  
  for (byte i = 0; i < 56; i++)
  {
    if (((frame[i/8] >> (7 - (i%8))) & 1) == 1)
    {
      PORTD &= !(1<<PORT_TX);
      delayMicroseconds(SYMBOL);
      PORTD ^= 1<<PORT_TX;
      delayMicroseconds(SYMBOL);
    }
    else
    {
      PORTD |= (1<<PORT_TX);
      delayMicroseconds(SYMBOL);
      PORTD ^= 1<<PORT_TX;
      delayMicroseconds(SYMBOL);
    }
  }
  
  PORTD &= !(1<<PORT_TX);
  delayMicroseconds(30415);
}
