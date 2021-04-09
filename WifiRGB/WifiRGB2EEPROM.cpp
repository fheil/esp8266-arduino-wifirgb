/*
  WiFiRGB2EEPROM.cpp - read/write configuration
  into the builtin EEPROM
  Written by F. Heil, February 4th, 2021
*/
#include "Arduino.h"
#include "WifiRGB2EEPROM.h"
#include <hl_DebugDefines_On.h>

#include <EEPROM.h>

WiFiRGB2EEPROM::WiFiRGB2EEPROM() 
{
  setColorR(50);
  setColorG(0);
  setColorB(100);
}

WiFiRGB2EEPROM::WiFiRGB2EEPROM(int r, int g, int b) 
{
  setColorR(r);
  setColorG(g);
  setColorB(b);
}

WiFiRGB2EEPROM::WiFiRGB2EEPROM(char * wlanName, char * password) 
{
  setSSIDName(wlanName);
  setPWD(password);
}

WiFiRGB2EEPROM::WiFiRGB2EEPROM(char * wlanName, char * password, int r, int g, int b) 
{
  setSSIDName(wlanName);
  setPWD(password);

  setColorR(r);
  setColorG(g);
  setColorB(b);
}

WiFiRGB2EEPROM::WiFiRGB2EEPROM(char * wlanName, char * password, char * devicename, int r, int g, int b) 
{
  setSSIDName(wlanName);
  setPWD(password);

  setDeviceName(devicename);

  setColorR(r);
  setColorG(g);
  setColorB(b);
}
void WiFiRGB2EEPROM::WriteRGBValues()
{
  MyObject myObjVar;	
int x=0;
  strncpy (myObjVar.wifi_ssid, getSSIDName(),_SSID_LEN_);
  strncpy (myObjVar.wifi_pwd, getPWD(), _PWD_LEN_);
  
  strncpy (myObjVar.device_name, getDeviceName(), _DEVICE_NAME_LEN_);

  myObjVar.r = getColorR();
  myObjVar.g = getColorG();
  myObjVar.b = getColorB();  
  
  debugPrintln("");

  debugPrintln("WiFiRGB2EEPROM: start internal EEPROM writing... ");
  x = sizeof(myObjVar);
  debugPrint("sizeof = ");
  debugPrintln(x);

  
  EEPROM.begin(_EPROM_MEMORY_SIZE_);

  EEPROM.put(_EPROM_ADDRESS_START_, myObjVar);
  delay(100);

  debugPrintln("WiFiRGB2EEPROM: ... done.");

  EEPROM.commit();
  debugPrintln("WiFiRGB2EEPROM: ... commited.");
  EEPROM.end();
}

void WiFiRGB2EEPROM::ReadRGBValues()
{
  MyObject myObjVar; 
 
  debugPrintln("");
  debugPrintln("WifiRGB2EEPROM: start internal EEPROM reading... ");
  EEPROM.begin(_EPROM_MEMORY_SIZE_);

  EEPROM.get(_EPROM_ADDRESS_START_, myObjVar);
  EEPROM.end();

  debugPrintln("WiFiRGB2EEPROM: ... done.");

  setSSIDName(myObjVar.wifi_ssid);
  setPWD(myObjVar.wifi_pwd);
  setDeviceName(myObjVar.device_name);
  setColorR(myObjVar.r);
  setColorG(myObjVar.g);
  setColorB(myObjVar.b);  
}

void WiFiRGB2EEPROM::setSSIDName(char* wlanName) {
  strncpy (_ssidAP, wlanName, _SSID_LEN_);
  } 
  
char * WiFiRGB2EEPROM::getSSIDName() {
  return _ssidAP;
  } 

void WiFiRGB2EEPROM::setPWD(char* password) {
  strncpy (_passwordAP, password, _PWD_LEN_);
  } 
  
char * WiFiRGB2EEPROM::getPWD() {
  return _passwordAP;
  } 

void WiFiRGB2EEPROM::setDeviceName(char* devicename) {
  strncpy (_device_name, devicename, _DEVICE_NAME_LEN_);
  } 
  
char * WiFiRGB2EEPROM::getDeviceName() {
  return _device_name;
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
