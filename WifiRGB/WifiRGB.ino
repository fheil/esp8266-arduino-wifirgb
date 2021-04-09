#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <FadeLed.h>
#include <hl_DebugDefines_On.h>

#include "WifiRGB.h"
#include "web_admin.h"
#include "web_interface.h"
#include "web_iro_js.h"

#ifdef SAVE2EEPROM/admin
#include "WifiRGB2EEPROM.h"
#endif

FadeLed led[3] = {REDPIN, GREENPIN, BLUEPIN};
bool bPowerOn = true;
bool bModeSetupDevice = false;
byte btMode = modeOFF;
uint16_t nWifiAttempts = 0;
uint16_t j = 0;
double f = LED_BREATH_RATIO;

ESP8266WebServer server(80);
IPAddress clientIP(192, 168, 178, 254); //the IP of the device
IPAddress gateway(192, 168, 178, 1);    //the gateway
IPAddress subnet(255, 255, 255, 0);     //the subnet mask

#ifdef SAVE2EEPROM
// Declare a global object variable from the WiFiRGB2EEPROM class.
WiFiRGB2EEPROM eepromHandler;
// next line is deprecated (Part 1)
//WiFiRGB2EEPROM  eepromHandler("your_ssid", "your_wifi_password", , "wifi-ledrgb-01", 50, 0, 100);
#endif

void ledBuiltinFlasher (int, int);
void resetOutputs(void);
void setSavedOutputsPower(void);
void setSavedOutputsPowerOn(void);
void ledStopAll(void);
void Wheel(byte);
uint16_t nextIndex(uint16_t, uint16_t, bool);


void setup(void) {
  debugBegin(115200);
  // next line is deprecated (Part 2)
  //  eepromHandler.WriteRGBValues();

#ifdef SAVE2EEPROM
  setSavedOutputsPowerOn();
#endif

  pinMode(LED_BUILTIN, OUTPUT); // Initialize the BUILTIN_LED pin as an output

  WiFi.mode(WIFI_STA);
  WiFi.hostname(eepromHandler.getDeviceName());

#ifndef DHCP
  WiFi.config(clientIP, gateway, subnet); // Remove for DHCP
#endif
  WiFi.begin(eepromHandler.getSSIDName(), eepromHandler.getPWD());
  // for development only...
  //WiFi.begin(eepromHandler.getSSIDName(), String ("force ap mode by wrong password"));

  debugPrintln("");

  // Wait for connection
  while ((WiFi.status() != WL_CONNECTED) && (nWifiAttempts < WIFI_ATTEMPTS)) {
    // flash internal LED while connecting to WiFi
    nWifiAttempts++;
    ledBuiltinFlasher (4, 150);
  }

  if (nWifiAttempts >= WIFI_ATTEMPTS) { //mode: setup device
    bool bModeSetupDevice = true;
    debugPrintln("\nEntering SoftAP mode... ");
    digitalWrite(LED_BUILTIN, !LED_BUILTIN_MODE); // Turn the LED ON/OFF as #defined after wifi is connected

    WiFi.mode(WIFI_AP);           //only access point

    if (WiFi.softAP(SOFTAP_SSID, SOFTAP_PWD))
    {

      debugPrintln("SoftAP mode ready");
      debugPrint("set up your device under IP address: ");
      debugPrintln(WiFi.softAPIP());
      server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", String(FPSTR(WEBADMIN_PART1)) +
                    eepromHandler.getSSIDName() +
                    FPSTR(WEBADMIN_PART2) +
                    eepromHandler.getPWD() +
                    FPSTR(WEBADMIN_PART3) +
                    eepromHandler.getDeviceName() +
                    FPSTR(WEBADMIN_PART4) +
                    String(eepromHandler.getColorR()) +
                    FPSTR(WEBADMIN_PART5) +
                    String(eepromHandler.getColorR()) +
                    FPSTR(WEBADMIN_PART6) +
                    String(eepromHandler.getColorR()) +
                    FPSTR(WEBADMIN_PART7)); //Send web page
      });
    }
    else
    {
      debugPrintln("SoftAP mode FAILED! - check your setup, reboot device manually");
    }
  }
  else   // mode: normal operation
  {
    debugPrintln("");
    debugPrintln(String("Device |") + eepromHandler.getDeviceName() + "| successful connected to |" + eepromHandler.getSSIDName() + "| wifi network");
    debugPrint("IP address: ");
    debugPrintln(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
      debugPrintln("MDNS responder started");
    }

    digitalWrite(LED_BUILTIN, LED_BUILTIN_MODE); // Turn the LED ON/OFF as #defined after wifi is connected

#ifdef OTA
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      debugPrintln("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      debugPrintln("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      debugPrint("Error[");
      debugPrint(error);
      debugPrint("]");
      if (error == OTA_AUTH_ERROR) {
        debugPrintln("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        debugPrintln("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        debugPrintln("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        debugPrintln("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        debugPrintln("End Failed");
      }
    });
    ArduinoOTA.begin();
#endif

#ifndef SAVE2EEPROM
    // adjust the PWM range
    // see https://esp8266.github.io/Arduino/versions/2.0.0/doc/reference.html#analog-output
    analogWriteRange(255);

    resetOutputs();
#endif

    // Root and 404
    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);

    // iro.js User Interface and Javascript
    server.on("/ui", HTTP_GET, []() {
      server.send_P(200, "text/html", WEBINTERFACE);
    });
    server.on("/iro.min.js", HTTP_GET, []() {
      server.send_P(200, "application/javascript", IRO_JS);
    });
  }
  // REST-API
  server.on("/api/v1/state", HTTP_POST, handleApiRequest);
  server.on("/api/v1/reset", HTTP_GET, resetOutputs);

  server.begin();
  debugPrintln("WifiRGB HTTP server started");
}

void loop(void) {
  server.handleClient();

  if (!bModeSetupDevice) {
    FadeLed::update();
    if (bPowerOn) {
      handleFading();
    }
#ifdef OTA
    ArduinoOTA.handle();
#endif

  }
}


void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266 wifi rgb!");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleApiRequest() {
  RGB rgb = {0, 0, 0};
  int brightness;

  debugPrintln("### API Request:");
  /*
    // DEBUG CODE
    String headers;
    headers += "HTTP Headers:\n";
    headers += server.headers();
    headers += "\n";

    for (uint8_t i = 0; i < server.headers(); i++) {
    headers += " " + server.headerName(i) + ": " + server.header(i) + "\n";
    }
    debugPrintln(headers);
  */
  if (server.hasArg("plain") == false) { //Check if body received
    server.send(200, "text/plain", "Body not received");
    return;
  }

  /*
    // DEBUG CODE
    String message;
    headers += "HTTP args:\n";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    debugPrintln(message);
    debugPrintln(server.arg("plain"));
  */

  const size_t bufferSize = JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(4) + 170;
  DynamicJsonDocument jsonDocument(bufferSize);
  deserializeJson(jsonDocument, server.arg("plain"));

  debugPrintln("JSON Body: ");
  serializeJson(jsonDocument, Serial);
  debugPrintln();

  JsonObject root = jsonDocument.as<JsonObject>();

  //leds are off, turn on, read rgb-values from EEPROM
#ifdef SAVE2EEPROM
  if ((!bPowerOn) && (root["state"] == "ON"))
  {
    debugPrintln("Reading RGB values from memory...");
    rgb.r = eepromHandler.getColorR();
    rgb.g = eepromHandler.getColorG();
    rgb.b = eepromHandler.getColorB();
    brightness = 100;

    bPowerOn = true;
  }
#endif
  ledStopAll();
  FadeLed::update();

  //only save values to EEPROM, when switching off
  if (root["state"] == "OFF")
  {
    debugPrintln("State OFF found: switching off");
    ledSetAllTime(LED_FADE_TIME);
    // Set output to off
    ledSetAllRGB (0, 0 , 0);
    FadeLed::update();

#ifdef SAVE2EEPROM
    //save color to EEPROM
    if (bPowerOn) {
      eepromHandler.WriteRGBValues();
    }
#endif

    bPowerOn = false;

    server.send(200, "application/json", server.arg("plain"));
    return;
  }
  //set values to eeprom
  if (root["state"] == "SET")
  {
    char buf[_PWD_LEN_];
    debugPrintln("State SET found: saving to eeprom");

    strcpy (buf, root["wifiSSID"]);
    eepromHandler.setSSIDName(buf);

    strcpy (buf, root["wifiPWD"]);
    eepromHandler.setPWD(buf);

    strcpy (buf, root["deviceName"]);
    eepromHandler.setDeviceName(buf);
  }

  j = 0; //reset my index to never get out of index
  // support different modes
  if (root["mode"] == "RAINBOW") {
    ledSetAllTime(LED_RAINBOW_TIME);

    btMode = modeRAINBOW;
    server.send(200, "application/json", server.arg("plain"));
    return;
  }
  if (root["mode"] == "SUNSET") {
    ledSetAllTime(LED_SUNSET_TIME);

    btMode = modeSUNSET;
    server.send(200, "application/json", server.arg("plain"));
    return;
  }
  if (root["mode"] == "POLAR") {
    ledSetAllTime(LED_POLAR_TIME);

    btMode = modePOLAR;
    server.send(200, "application/json", server.arg("plain"));
    return;
  }
  if (root["mode"] == "TRUERGB") {
    ledSetAllTime(LED_TRUERGB_TIME);

    btMode = modeTRUERGB;
    server.send(200, "application/json", server.arg("plain"));
    return;
  }
  if (root["mode"] == "2OFRGB") {
    ledSetAllTime(LED_2OFRGB_TIME);

    btMode = mode2OFRGB;
    server.send(200, "application/json", server.arg("plain"));
    return;
  }

  // for static modes we change color, brightness etc. so get more values from the ui
  brightness = root["brightness"];
  debugPrint("Brightness: ");
  debugPrintln(brightness);

  // DEBUG: color
  debugPrint("Color: ");
  serializeJson(root["color"], Serial);
  debugPrintln();

  JsonObject color = root["color"];
  // If RGB mode: Parse RGB values
  if (color["mode"] == "rgb") {
    // Indeed, the JsonVariant returned by root["..."] has a special implementation of the == operator that knows how to compare string safely.
    // See https://arduinojson.org/faq/why-does-my-device-crash-or-reboot/
    debugPrintln("Reading RGB values from Json...");

    rgb.r = map(color["r"], 0, 255, 0, 100);
    rgb.g = map(color["g"], 0, 255, 0, 100);
    rgb.b = map(color["b"], 0, 255, 0, 100);
  }

  // If HSV mode: Parse HSV values
  if (color["mode"] == "hsv") {
    // Indeed, the JsonVariant returned by root["..."] has a special implementation of the == operator that knows how to compare string safely.
    // See https://arduinojson.org/faq/why-does-my-device-crash-or-reboot/
    debugPrintln("Reading HSV values...");
    rgb = hsvToRgb(color["h"], color["s"], color["v"]);
  }

  // DEBUG: Parsed values
  debugPrintln("Parsed Values:");
  debugPrint("r=");
  debugPrint(rgb.r);
  debugPrint(", g=");
  debugPrint(rgb.g);
  debugPrint(", b=");
  debugPrintln(rgb.b);

  if (root["mode"] == "SOLID") {
    ledSetAllTime(LED_FADE_TIME);

    btMode = modeSOLID;
  }
  if (root["mode"] == "FLASH") {
    ledSetAllTime(LED_FLASH_TIME);

    btMode = modeFLASH;
  }
  if (root["mode"] == "BREATHE") {
    ledSetAllTime(LED_FADE_TIME * 2);

    btMode = modeBREATHE;
  }


  //set values to eeprom
  if (root["state"] == "SET")
  {
    debugPrint("   SETUP: ");
    debugPrintln(eepromHandler.getSSIDName());
    debugPrint("   SETUP: ");
    debugPrintln(eepromHandler.getPWD());
    debugPrint("   SETUP: ");
    debugPrintln(eepromHandler.getDeviceName());
    debugPrintln("   SETUP: Values to save:");
    debugPrint("   SETUP: r=");
    debugPrint(rgb.r);
    debugPrint(", g=");
    debugPrint(rgb.g);
    debugPrint(", b=");
    debugPrintln(rgb.b);

#ifdef SAVE2EEPROM
    eepromHandler.WriteRGBValues();
    server.send(200, "text/html", "Restart_Ok");
    delay(500);
    ESP.restart();
#endif
    // code will never be reached...
    server.send(200, "application/json", server.arg("plain"));
    return;
  }

  // Set Output
  ledSetAllRGB (rgb.r, rgb.g , rgb.b);

#ifdef SAVE2EEPROM
  if ((btMode == modeSOLID) || (btMode == modeBREATHE) || (btMode == modeFLASH))
    eepromHandlerSetAllRGB (rgb.r, rgb.g, rgb.b);
#endif

  server.send(200, "application/json", server.arg("plain"));
}

void handleFading()
{
  if (btMode == modeSOLID) {
    ledSetAllRGB (eepromHandler.getColorR(), eepromHandler.getColorG(), eepromHandler.getColorB());
  }
  else {
    if (led[iR].done() && led[iG].done() && led[iB].done()) {
      switch (btMode)
      {
        case modeFLASH:
          {
            if (led[iR].get() || led[iG].get() || led[iB].get())
            {
              led[iR].off();
              led[iG].off();
              led[iB].off();
            }
            //or at off
            else {
              //then we are done fading down, let's fade up again
              ledSetAllRGB (eepromHandler.getColorR(), eepromHandler.getColorG(), eepromHandler.getColorB());
            }
            break;
          }
        case modeBREATHE:
          {
            if ((led[iR].get() == eepromHandler.getColorR()) &&
                (led[iG].get() == eepromHandler.getColorG()) &&
                (led[iB].get() == eepromHandler.getColorB()) )
            {
              //fading done, fade down
              ledSetAllRGB ((int) (f * eepromHandler.getColorR()), (int) (f * eepromHandler.getColorG()), (int) (f * eepromHandler.getColorB()));
            }
            //fade up
            else {
              ledSetAllRGB (eepromHandler.getColorR(), eepromHandler.getColorG(), eepromHandler.getColorB());
            }

            break;
          }
        case modeRAINBOW:
          {
            Wheel(j);
            j = nextIndex(j, 100, false);

            ledSetAllRGB (eepromHandler.getColorR(), eepromHandler.getColorG(), eepromHandler.getColorB());
            break;
          }

        case modeSUNSET:
          {
            j = nextIndex(j, 3, true);

            ledSetAllRGB (RGBGradient[btMode - modeSUNSET][j].r, RGBGradient[btMode - modeSUNSET][j].g, RGBGradient[btMode - modeSUNSET][j].b);
            break;
          }
        case modePOLAR:
          {
            j = nextIndex(j, 3, true);

            ledSetAllRGB (RGBGradient[btMode - modeSUNSET][j].r, RGBGradient[btMode - modeSUNSET][j].g, RGBGradient[btMode - modeSUNSET][j].b);
            break;
          }
        case modeTRUERGB:
          {
            j = nextIndex(j, 3, false);

            ledSetAllRGB (RGBGradient[btMode - modeSUNSET][j].r, RGBGradient[btMode - modeSUNSET][j].g, RGBGradient[btMode - modeSUNSET][j].b);
            break;
          }
        case mode2OFRGB:
          {
            j = nextIndex(j, 3, false);

            ledSetAllRGB (RGBGradient[btMode - modeSUNSET][j].r, RGBGradient[btMode - modeSUNSET][j].g, RGBGradient[btMode - modeSUNSET][j].b);
            break;
          }
        default:
          {
            //ledSetAllRGB (eepromHandler.getColorR(), eepromHandler.getColorG(), eepromHandler.getColorB());
            break;
          }
      }
    }
  }
}

void resetOutputs() {
  led[iR].begin(0);
  led[iG].begin(0);
  led[iB].begin(0);

  FadeLed::update();
}

#ifdef SAVE2EEPROM
void setSavedOutputs() {
  debugPrintln("");
  debugPrintln("_______________________________");
  debugPrintln("EEPROM: start restore data ...");

  eepromHandler.ReadRGBValues();

  debugPrint("    SSID read from EEPROM:       |");
  debugPrint(eepromHandler.getSSIDName());
  debugPrintln("|");
  debugPrint("    PWD read from EEPROM:        |");
  // be aware of the the meaning by enabling next line
  //debugPrint(eepromHandler.getPWD());
  debugPrintln("|");
  debugPrint("    DeviceName read from EEPROM: |");
  debugPrint(eepromHandler.getDeviceName());
  debugPrintln("|");
  debugPrintln("EEPROM: finished, data restored");
  debugPrintln("_______________________________");

  ledSetAllRGB (eepromHandler.getColorR(), eepromHandler.getColorG(), eepromHandler.getColorB());
}

void setSavedOutputsPowerOn() {
  resetOutputs();

  ledSetAllTime(LED_FADE_TIME);

  FadeLed::update();

  setSavedOutputs();

  while (!led[iR].done() || !led[iG].done() || !led[iB].done())
  {
    FadeLed::update();
  }
}
#endif

void Wheel(byte WheelPos) {
  if (WheelPos < 33)
  {
    eepromHandlerSetAllRGB ((WheelPos * 3), (100 - WheelPos * 3), 0);
  }
  else if (WheelPos < 66)
  {
    WheelPos -= 33;
    eepromHandlerSetAllRGB ((100 - WheelPos * 3), 0, (WheelPos * 3));
  }
  else {
    WheelPos -= 66;
    eepromHandlerSetAllRGB (0, (WheelPos * 3), (100 - WheelPos * 3));
  }
}

// this is a modified version of https://gist.github.com/hdznrrd/656996
RGB hsvToRgb(double h, double s, double v) {
  int i;
  double f, p, q, t;
  byte r, g, b;

  h = max(0.0, min(360.0, h));
  s = max(0.0, min(100.0, s));
  v = max(0.0, min(100.0, v));

  s /= 100;
  v /= 100;

  if (s == 0) {
    // Achromatic (grey)
    r = g = b = round(v * 255);
    return {0, 0, 0};
  }

  h /= 60; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));
  switch (i) {
    case 0:
      r = round(255 * v);
      g = round(255 * t);
      b = round(255 * p);
      break;
    case 1:
      r = round(255 * q);
      g = round(255 * v);
      b = round(255 * p);
      break;
    case 2:
      r = round(255 * p);
      g = round(255 * v);
      b = round(255 * t);
      break;
    case 3:
      r = round(255 * p);
      g = round(255 * q);
      b = round(255 * v);
      break;
    case 4:
      r = round(255 * t);
      g = round(255 * p);
      b = round(255 * v);
      break;
    default: // case 5:
      r = round(255 * v);
      g = round(255 * p);
      b = round(255 * q);
  }
  return {r, g, b};
}

void eepromHandlerSetAllRGB (int r, int g, int b)
{
  eepromHandler.setColorR(r);
  eepromHandler.setColorG(g);
  eepromHandler.setColorB(b);
}

void ledSetAllRGB (int r, int g, int b)
{
#ifdef USE_R
  led[iR].set(r);
#else
  led[iR].set(0);
#endif

#ifdef USE_G
  led[iG].set(g);
#else
  led[iG].set(0);
#endif

#ifdef USE_B
  led[iB].set(b);
#else
  led[iB].set(0);
#endif
}

void ledStopAll(void)
{
  led[iR].stop();
  led[iG].stop();
  led[iB].stop();
}

void ledSetAllTime(int t)
{
  led[iR].setTime(t, true);
  led[iG].setTime(t, true);
  led[iB].setTime(t, true);
}

uint16_t nextIndex(uint16_t idx, uint16_t steps, bool bModeUpAndDown)
{
  static int nStep = 1;

  if (bModeUpAndDown)
  {
    if (idx >= (steps - 1))
    {
      nStep = -1;
    }

    if (idx <= 0)
    {
      nStep = 1;
    }
    idx = idx +  nStep;
  }
  else
  {
    idx ++;
    idx %= steps;
  }
  return idx;
}

void ledBuiltinFlasher (int count, int delayTime) {
  for (int i = 0; i < count * 2; i++) {
    digitalWrite (LED_BUILTIN, i % 2);
    debugPrint(".");
    delay (delayTime);
  }
}
