#if defined(ESP32)
#include <FirebaseESP32.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#define WIFI_SSID "AADS-GUEST-SC"
#define WIFI_PASSWORD "5m9Bg3zVYp2v"
#include <DHT.h>

#define FIREBASE_HOST "https://my-home-d35d6-default-rtdb.europe-west1.firebasedatabase.app" // change here
#define FIREBASE_AUTH "6CrOPpPHJJ5Wgf85A0NJ1PC5Q3dSmdXF8CE58kSh"
#include <ShiftRegister74HC595.h>

ShiftRegister74HC595<1> sr(21, 22, 23);

FirebaseData firebaseData;

class Device {
  public:
    Device() {}

    Device(int _pin, std::string _path) {
        setPin(_pin);
        setPath(_path);
    }

    ~Device() {}

    int getPin() const {
      return pin;
    }

    std::string getPath() const {
      return path;
    }

    void setPin(int _pin) {
      pin = _pin;
    }

    void setPath(std::string _path) {
      path = _path;
    }

    void setDeviceState() {
      if (Firebase.getString(firebaseData, path)) {
        String ledstatus = firebaseData.stringData();
        if (ledstatus.toInt() == 1) {
          Serial.println("on3");
          //sr.setAllHigh(); // set all pins HIGH
          sr.set(pin, LOW);
        } else {
          Serial.println("off3");
          //sr.setAllHigh(); // set all pins HIGH
          sr.set(pin, HIGH);
        }
      } else {
        Serial.print("Error in getInt, ");
        Serial.println(firebaseData.errorReason());
      }
    }
    virtual void uploadDeviceValue() {
      
    }
  protected:
    int pin;
    std::string path;
};

class Temp : public Device {
  public:
    Temp(int _pin, std::string _path, DHT _sensor): Device(_pin, _path), sensor(_sensor) {};
    ~Temp() {};
    void uploadDeviceValue() override {
        float temp = sensor.readTemperature();
       if (Firebase.RTDB.setFloat(&firebaseData, path, temp)){
        Serial.println("PASSED");
        Serial.println("PATH: " + firebaseData.dataPath());
        Serial.println("TYPE: " + firebaseData.dataType());
      }
      else {
        Serial.println("FAILED");
        Serial.println("REASON: " + firebaseData.errorReason());
    }
    };
  private:
    DHT sensor;
};

Temp dht(16, "/temperature", DHT(7, DHT22));

const int led_nb = 16;

Device led[led_nb] = {
   Device(0, "/Led_bathroom_0"),
   Device(1, "/Led_bathroom1_1"),
   Device(2, "/Led_bathroom2_1"),
   Device(3, "/Led_bedroom_0"),
   Device(4, "/Led_bedroom1_1"),
   Device(5, "/Led_bedroom2_1"),
   Device(6, "/Led_bedroom3_1"),
   Device(7, "/Led_livingroom_0"),
   Device(8, "/Led_livingroom2_0"),
   Device(9, "/Led_kitchen_0"),
   Device(10, "/Led_entrance_0"),
   Device(11, "/Door_garage_0"),
   Device(12, "/Led_pool_0"),
   Device(13, "/Led_dressing_1"),
   Device(14, "/Led_corridor_1"),
   Device(15, "/Led_terrace_1")
};

Device BrasseurLivingRoom(17, "/Brasseur_livingroom_0");

Device tv(18, "/TvStatus");

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
  sr.setAllHigh();
}

void loop() {
  for (int i = 0; i < led_nb; i++) {
    led[i].setDeviceState();  
  }
  BrasseurLivingRoom.setDeviceState();
  tv.setDeviceState();
  dht.uploadDeviceValue();
  
}
