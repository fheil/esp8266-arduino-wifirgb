#include "WifiRGBDefs.h"

#define SOFTAP_SSID "WifiRGB Setup Device"
#define SOFTAP_PWD  "secure_secret"

/*defines for #ifdef & #ifndef preprocessing directives, conditional compilation of the code */
#define DHCP                    // if defined, DHCP will be used for IP-address

#define WIFI_ATTEMPTS      10   // how often to try to connect to wifi before AP mode is started

#define SAVE2EEPROM             // if defined, rgb-values are stored and restored from esp8266 EEPROM

//#define OTA                     // for OTA (over the air) updates 

#define USE_R                   // if defined, the channel for color R (red) will be used
#define USE_G                   // if defined, the channel for color G (green) will be used
#define USE_B                   // if defined, the channel for color B (blue) will be used
//#define USE_WW                  // if defined, the channel for color WW (warm white) will be used

#define LED_FADE_TIME    1000   // default time to fade LEDs ON/OFF
#define LED_FLASH_TIME    100   // default time to flash LEDs ON/OFF
#define LED_BREATH_RATIO  0.6   // default ratio for breathing mode (amount led brightness will be decreased

#define LED_RAINBOW_TIME  500   // default time to cycle through colors in rainbow-mode

#define LED_SUNSET_TIME  5000   // default time to cycle through colors in sunset-mode

#define LED_POLAR_TIME   5000   // default time to cycle through colors in polar-mode

#define LED_TRUERGB_TIME 2500   // default time to cycle through colors in truergb -mode

#define LED_2OFRGB_TIME  2500   // default time to cycle through colors in 2ofrgb-mode

RGB RGBGradient[4][3] = {
  {
    { 95,   7,  67}, //sunset
    { 97,   4,  17},
    { 100, 55,   0}
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
