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

struct MyObject {
  int r;
  int g;
  int b;
};

class WiFiRGB2EEPROM
{
  public:
    WiFiRGB2EEPROM();
    WiFiRGB2EEPROM(int, int, int);
    void WriteRGBValues();
    void ReadRGBValues();
    void setColorR(int);
    void setColorG(int);
    void setColorB(int);
    int getColorR();
    int getColorG();
    int getColorB();
   private:  
    int _color_r;
    int _color_g;
    int _color_b;
};

#endif //#ifndef WiFiRGBEEPROM_h
