#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <FirebaseESP32.h>

const char* ssid = "nard";
const char* password = "iloveyoug";

#define FIREBASE_HOST " link/ID ng firebase "
#define FIREBASE_AUTH "database link" 

static const int servoPin = 13;
static const int buttonPin = 12;

Servo servo1;
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to Wi-Fi");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  servo1.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  String mealTime = getMealTime("lunch");

  Serial.println("Meal Time: " + mealTime);
  String currentTime = getCurrentTime();

  if (currentTime == mealTime) {
    Serial.println("It's meal time!");

    servo1.write(180);
    delay(500);
    servo1.write(0);
  }
  if (digitalRead(buttonPin) == LOW) {
    servo1.write(90);
    delay(500);
    servo1.write(0);

    HTTPClient http;
    http.begin("http://your-server.com/button");
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST("{\"buttonPressed\": true}");

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error in sending POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();

    delay(1000);
  }

  delay(1000);
}
String getMealTime(String mealType) {
  String time = "";
  if (Firebase.getString(firebaseData, "meal_time/" + mealType)) {
    time = firebaseData.stringData();
  } else {
    Serial.println("Failed to get meal time");
  }
  return time;
}
String getCurrentTime() {
  return "12:00";
}
