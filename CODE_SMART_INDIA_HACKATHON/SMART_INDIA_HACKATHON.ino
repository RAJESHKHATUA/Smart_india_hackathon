#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClient.h>

const char* server = "api.thingspeak.com";
const char* apiKey = "ISLX7QBI8GP68YGB";  // connects to the thingspeak cloud platform
const int fieldNumber1 = 1;
const int fieldNumber2 = 2;

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3_2p0uUW6"
#define BLYNK_TEMPLATE_NAME "Smart india hackathon"  //connects to the blynk cloud platform to display data in the blynk app
#define BLYNK_AUTH_TOKEN "Ds2L46lDk_SAs5UDMmZQW1Js0dpgznHG"
#include <BlynkMultiClient.h>

// My WiFi credentials.
const char* ssid = "Your wifi ssid";    //your wifi credentials
const char* pass = "Your wifi password";
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
  static WiFiClient blynkWiFiClient;
  const int SensorDataPin = 2;     

   OneWire oneWire(SensorDataPin);
   DallasTemperature sensors(&oneWire);
   int Heart_sensor = 88;
 Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire, -1);

  void setup() {
     Serial.begin(9600);
     //if (WiFi.status() != WL_CONNECTED)
    connectWiFi();
  // Setup Blynk
  Blynk.addClient("WiFi", blynkWiFiClient, 80);
  Blynk.config(BLYNK_AUTH_TOKEN);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  Serial.println(F("SSD1306 allocation failed"));
  for(;;);
}
  delay(2000);
display.clearDisplay();
display.setTextColor(WHITE);


}

void loop()
{ 
  // Reconnect WiFi

  float temperature_Celsius = sensors.getTempCByIndex(0);
  float temperature_F = sensors.getTempFByIndex(0);
  float temperature_Fahrenheit = (temperature_Celsius*9.0/5.0) + 32.0;    //celcius to fahrenheit conversion formulae
  //float temperature_F = sensors.getTempFByIndex(0);
  Serial.print("Temperature = "); Serial.print(temperature_Fahrenheit); Serial.println(" *F");
  //display.print("Temperature: "); display.print(temperature_Celsius); display.println(" *C");
 
  
  delay(2000);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.clearDisplay();

  sensors.requestTemperatures(); 
  
  Serial.print("Temperature = "); Serial.print(temperature_Fahrenheit); Serial.println(" *F");
  //display.print("Temperature: "); display.print(temperature_Celsius); display.println(" *C");
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(temperature_Fahrenheit);   //displays body temperature in fahrenheit
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);  //displays the symbol of degree
  display.write(167);
  display.setTextSize(2);
  display.print("F");
  display.display();
  delay(5000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Heart Rate : ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(Heart_sensor);
  display.print("bpm");
  display.display();
  delay(3000);
  display.clearDisplay();
  delay(1000);
  Blynk.virtualWrite(V0 ,temperature_Fahrenheit);
  Blynk.run();

  WiFiClient client;

  // Construct the URL for the ThingSpeak update
  String url1 = "/update?api_key=" + String(apiKey) + "&field" + String(fieldNumber1) + "=" + String(temperature_Fahrenheit);
  String url2 = "/update?api_key=" + String(apiKey) + "&field" + String(fieldNumber2) + "=" + String(Heart_sensor);

  // Connect to ThingSpeak
  if (client.connect(server, 80))
  {
    // Send an HTTP GET request to update the channel
    client.print("GET " + url1 + " HTTP/1.1\r\n");
    client.print("GET " + url2 + " HTTP/1.1\r\n");
    client.print("Host: " + String(server) + "\r\n");
    client.print("Connection: close\r\n\r\n");
    delay(500); // Wait for server response

    // Read and print the server response
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    client.stop();
  } 
  else
  {
    Serial.println("Failed to connect to ThingSpeak");
  }

  // Wait for a delay before sending the next data point
  delay(3000); // Send data every 3 seconds

}
void connectWiFi()
{ //function to connect with the wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (pass && strlen(pass)) {
    WiFi.begin((char*)ssid, (char*)pass);
  } else {
    WiFi.begin((char*)ssid);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     Serial.print(WiFi.status());
     Serial.print(WL_CONNECTED);
  }
} 
