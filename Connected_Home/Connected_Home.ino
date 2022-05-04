#if defined(ESP32)
#include <FirebaseESP32.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#define WIFI_SSID "AADS-GUEST-SC"
#define WIFI_PASSWORD "5m9Bg3zVYp2v"

#define FIREBASE_HOST "https://my-home-d35d6-default-rtdb.europe-west1.firebasedatabase.app" // change here
#define FIREBASE_AUTH "6CrOPpPHJJ5Wgf85A0NJ1PC5Q3dSmdXF8CE58kSh"
#include <ShiftRegister74HC595.h>

ShiftRegister74HC595<1> sr(21, 22, 23);

FirebaseData firebaseData;

void setup ()
{
  Serial.begin(9600);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ") ;
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.getString(firebaseData, "/Led1Status"))
  {
    String ledstatus = firebaseData.stringData();
    if (ledstatus.toInt() == 1) {
      Serial.println("on3");
      sr.setAllHigh(); // set all pins HIGH
      sr.set(0, LOW);
    }
    else {
      Serial.println("off3");
      sr.setAllHigh(); // set all pins HIGH
      sr.set(0, HIGH);
    }
  }
  else {
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }
}