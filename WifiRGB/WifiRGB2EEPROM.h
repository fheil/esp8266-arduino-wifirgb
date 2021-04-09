/*
  WiFiRGB2EEPROM.h - for read/write configuration
  into the builtin EEPROM
  Written by F. Heil, February 4th, 2021
*/
#ifndef WiFiRGB2EEPROM_h
#define WiFiRGB2EEPROM_h

#include "Arduino.h"

// EEPROM MAP addresses
#define _EPROM_MEMORY_SIZE_   512
#define _EPROM_ADDRESS_START_ 0

//some len definitions
#define _SSID_LEN_ 32
#define _PWD_LEN_ 64
#define _DEVICE_NAME_LEN_ 64

struct MyObject {
  char wifi_ssid[_SSID_LEN_];
  char wifi_pwd[_PWD_LEN_];
  char device_name[_DEVICE_NAME_LEN_]; 
  int r;
  int g;
  int b;
};

class WiFiRGB2EEPROM
{
  public:
    WiFiRGB2EEPROM();
    WiFiRGB2EEPROM(char*, char*);
    WiFiRGB2EEPROM(char*, char*, int, int, int);
    WiFiRGB2EEPROM(char*, char*, char*, int, int, int);
    WiFiRGB2EEPROM(int, int, int);
    void setSSIDName(char* );
    char * getSSIDName();
    void setPWD(char* );
    char * getPWD();
    void setDeviceName(char* );
    char * getDeviceName();
    void WriteRGBValues();
    void ReadRGBValues();
    void setColorR(int);
    void setColorG(int);
    void setColorB(int);
    int getColorR();
    int getColorG();
    int getColorB();
   private:  
    char _ssidAP[_SSID_LEN_];
    char _passwordAP[_PWD_LEN_];
    char _device_name[_DEVICE_NAME_LEN_];
    int _color_r;
    int _color_g;
    int _color_b;
};

#endif //#ifndef WiFiRGBEEPROM_h
