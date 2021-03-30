/*
  WiFiRGB2EEPROM.cpp - read/write configuration
  into the builtin EEPROM
  Written by F. Heil, February 4th, 2021
*/
#include "Arduino.h"
#include "WifiRGB2EEPROM.h"
#include <hl_DebugDefines_Off.h>

#include <EEPROM.h>

WiFiRGB2EEPROM::WiFiRGB2EEPROM() 
{
  setColorR(255);
  setColorG(255);
  setColorB(255);
}

WiFiRGB2EEPROM::WiFiRGB2EEPROM(int r, int g, int b) 
{
  setColorR(r);
  setColorG(g);
  setColorB(b);
}

void WiFiRGB2EEPROM::WriteRGBValues()
{
  MyObject color;	
  color.r = getColorR();
  color.g = getColorG();
  color.b = getColorB();  
  
  debugPrintln("");

  debugPrintln("WiFiRGB2EEPROM: start internal EEPROM writing... ");
  EEPROM.begin(_EPROM_MEMORY_SIZE_);

  EEPROM.put(_EPROM_ADDRESS_START_, color);
  delay(100);

  debugPrintln("WiFiRGB2EEPROM: ... done.");

  EEPROM.commit();
  debugPrintln("WiFiRGB2EEPROM: ... commited.");
  EEPROM.end();
}

void WiFiRGB2EEPROM::ReadRGBValues()
{
  MyObject color; 
 
  debugPrintln("");
  debugPrintln("WifiRGB2EEPROM: start internal EEPROM reading... ");
  EEPROM.begin(_EPROM_MEMORY_SIZE_);

  EEPROM.get(_EPROM_ADDRESS_START_, color);
  EEPROM.end();

  debugPrintln("WiFiRGB2EEPROM: ... done.");
  
  setColorR(color.r);
  setColorG(color.g);
  setColorB(color.b);  
}

void WiFiRGB2EEPROM::setColorR(int r) {
  _color_r = r;
  } 
  
int WiFiRGB2EEPROM::getColorR() {
  return _color_r;
  } 

void WiFiRGB2EEPROM::setColorG(int g) {
  _color_g = g;
  } 
  
int WiFiRGB2EEPROM::getColorG() {
  return _color_g;
  } 

void WiFiRGB2EEPROM::setColorB(int b) {
  _color_b = b;
  } 
  
int WiFiRGB2EEPROM::getColorB() {
  return _color_b;
  } 
