#include "WifiRGBDefs.h"

#define ssid = "your_ssid";
#define password = "your_wifi_password"; 
#define deviceName = "wifi-rgb";

RGB RGBGradient[4][3] = {
  {
    { 95,   7,  67}, //sunset
    { 97,   4,  17},
    { 93,  60,   7}
  },  
  {
    {  0, 100, 100}, //polar
    {  0, 100,  50},
    {  0,   0, 100}
  },  
  {
    {100,   0,   0}, //trueRGB
    {  0, 100,   0},
    {  0,   0, 100}
  },  
  {
    { 50,  50,   0}, //2ofRGB
    {  0,  50,  50},
    { 50,   0,  50}
  }
};

/*defines for #ifdef & #ifndef preprocessing directives, conditional compilation of the code */
#define DHCP                    // if defined, DHCP will be used for IP-address

#define LED_BUILTIN_MODE HIGH   // Turn the LED ON/OFF by making the voltage LOW/HIGH

#define SAVE2EEPROM             // if defined, rgb-values are stored and restored from esp8266 EEPROM

#define OTA                     // for OTA (over the air) updates 

#define LED_FADE_TIME    1000   // default time to fade LEDs ON/OFF
#define LED_FLASH_TIME    100   // default time to flash LEDs ON/OFF
#define LED_FLASH_RATIO   0.6   // default time to flash LEDs ON/OFF

#define LED_RAINBOW_TIME  500   // default time to cycle through colors in rainbow-mode

#define LED_SUNSET_TIME  2000   // default time to cycle through colors in sunset-mode

#define LED_POLAR_TIME   1000   // default time to cycle through colors in polar-mode

#define LED_TRUERGB_TIME 2000   // default time to cycle through colors in truergb -mode

#define LED_2OFRGB_TIME  2000   // default time to cycle through colors in 2ofrgb-mode
