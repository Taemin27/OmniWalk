#include <Preferences.h>
Preferences pref;

#include <PS2MouseHandler.h>
#define MOUSE_DATA 5
#define MOUSE_CLOCK 19
PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA, PS2_MOUSE_REMOTE);

#include "BluetoothSerial.h"
const char *pin = "1234";           // Make sure to change this to a private secure pin
String device_name = "OmniWalk_R";  // Change the name to OmniWalk_L for left device

#include <WiFi.h>

String WiFi_SSID;
String WiFi_PSWD;

BluetoothSerial BT;
bool BTStatus = false;

void btStatus(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    BTStatus = true;
    Serial.println("Bluetooth Connected");
  }

  else if (event == ESP_SPP_CLOSE_EVT) {
    BTStatus = false;
    Serial.println("Bluetooth Disconnected");
  }
}

void setup() {
  // Serial Init
  Serial.begin(115200);

  // Mouse Init
  delay(1000);
  if (mouse.initialise() != 0) {
    // mouse error
    Serial.println("[Mouse] Mouse Initialization Failed");
    while(true);
  };
  mouse.set_scaling_2_1();
  mouse.set_resolution(1);

  // WiFi Init
  WiFi.mode(WIFI_STA);
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
  }
  tryWiFi();

  // Bluetooth Init
  BT.begin(device_name, false);
  BT.register_callback(btStatus);
}

void loop() {
  switch (WiFi.status()) {
    case WL_CONNECTION_LOST:
      Serial.println("[WiFi] Connection was lost");
      break;
    case WL_CONNECTED:  // Send mouse values if WiFi is connected
      // Send mouse x and y data
      break;
    case WL_DISCONNECTED:
      break;
    default:
      break;
  }

  if (BTStatus) {  // Recieve data if if Bluetooth is connected
    mouse.get_data();
    BT.print(mouse.x_movement());
    BT.print(", ");
    BT.println(mouse.y_movement());
    while (BT.available()) {
      String data = BT.readString();
      data.trim();
      
      if (data.startsWith("SSID: ")) {
        pref.begin("wifi", false);
        pref.putString("ssid", data.substring(6));
        pref.end();

        WiFi_SSID = data.substring(6);
        Serial.println("[WiFi] WiFi SSID: " + WiFi_SSID);
      } 
      else if (data.startsWith("PSWD:")) {
        pref.begin("wifi", false);
        pref.putString("pswd", data.substring(6));
        pref.end();

        WiFi_PSWD = data.substring(6);
        Serial.println("[WiFi] WiFi Password: " + WiFi_PSWD);
      } 
      else if (data.startsWith("Init WiFi")) {
        tryWiFi();
      }
    }
  }
}

bool tryWiFi() {
  pref.begin("wifi", true);

  if (pref.isKey("ssid") && pref.isKey("pswd")) {
    WiFi_SSID = pref.getString("ssid");
    WiFi_PSWD = pref.getString("pswd");
    WiFi.begin(WiFi_SSID, WiFi_PSWD);
    return(connectWifi());
  } else {
    Serial.println("[Pref] No saved WiFi");
    return false;
  }
  pref.end();
}

bool connectWifi() {
  int tryDelay = 500;
  int numberOfTries = 20;
  while (true) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed. Check the WiFi password");
        WiFi.disconnect();
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi not connected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi connected");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return true;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);

    if (numberOfTries <= 0) {
      Serial.println("[WiFi] Failed due to unknown reason");
      WiFi.disconnect();
      return false;
    } else {
      numberOfTries--;
    }
  }
}
