/*
    Get date and time - uses the ezTime library at https://github.com/ropg/ezTime -
    and then show data from a DHT22 on a web page served by the Huzzah and
    push data to an MQTT server - uses library from https://pubsubclient.knolleary.net

    Duncan Wilson
    CASA0014 - 2 - Plant Monitor Workshop
    May 2020
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezTime.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// Sensors - DHT22 and Nails
uint8_t DHTPin = 12;        // on Pin 2 of the Huzzah
uint8_t soilPin = 0;      // ADC or A0 pin on Huzzah
float DHTValue = -1; //initial value of DHT22 sensor
float Moisture = 1; // initial value just in case web page is loaded before readMoisture called
int sensorVCC = 13;
int blueLED = 2;
float Temperature;
float Humidity;

DHT dht(DHTPin, DHTTYPE);   // Initialize DHT sensor.


// Wifi and MQTT
/*
**** please enter your sensitive data in the Secret tab/arduino_secrets.h
**** using format below

#define SECRET_SSID "ssid name"
#define SECRET_PASS "ssid password"
#define SECRET_MQTTUSER "user name - eg student"
#define SECRET_MQTTPASS "password";
 */
 #include "/Users/dylim/Documents/CASA/CE/plantMonitor/arduino_secrets.h"

const char* ssid     = SECRET_SSID;
const char* password = SECRET_PASS;
const char* mqttuser = SECRET_MQTTUSER;
const char* mqttpass = SECRET_MQTTPASS;

ESP8266WebServer server(80);
const char* mqtt_server = "mqtt.cetools.org";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
bool debugMode = true;
char* topicAddress = "student/CASA0014/plant/ucfnimx";

// Date and time
Timezone GB;



void setup() {
  // Set up LED to be controllable via broker
  // Initialize the BUILTIN_LED pin as an output
  // Turn the LED off by making the voltage HIGH
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  // Set up the outputs to control the soil sensor
  // switch and the blue LED for status indicator
  pinMode(sensorVCC, OUTPUT); 
  digitalWrite(sensorVCC, LOW);
  pinMode(blueLED, OUTPUT); 
  digitalWrite(blueLED, HIGH);

  // open serial connection for debug info
  Serial.begin(115200);
  delay(100);

  // start DHT sensor
  pinMode(DHTPin, INPUT);
  dht.begin();

  // run initialisation functions
  startWifi();
  startWebserver();
  syncDate();

  // start MQTT server
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

}

void loop() {
  //handler for receiving requests to webserver
  server.handleClient();
  //Bebug purpose. run every 2 secs
  if (debugMode){
    float t = readDHT22(1);
    float h = readDHT22(2);
    float m = readMoisture(0);
    client.publish(topicAddress, WiFi.localIP().toString().c_str());
    Serial.print("Temp:"); Serial.print(t); Serial.print(", ");
    Serial.print("Humid:"); Serial.print(h); Serial.print(", ");
    Serial.print("SoilMoist:"); Serial.print(m); Serial.print(", ");
    Serial.println();
    sendMQTT_n("Temperature", t);
    sendMQTT_n("Humidity", h);
    sendMQTT_n("Moisture", m);
    delay(2000);
    client.loop();
  }else{
    if (minuteChanged()) {
      Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
      client.publish(topicAddress, WiFi.localIP().toString().c_str());
      //Display DHT22 Values
      float t = readDHT22(1);
      float h = readDHT22(2);
      float m = readMoisture(0);
      Serial.print("Temp:"); Serial.print(t); Serial.print(", ");
      Serial.print("Humid:"); Serial.print(h); Serial.print(", ");
      Serial.print("SoilMoist:"); Serial.print(m); Serial.print(", ");
      Serial.println();
      sendMQTT_n("Temperature", t);
      sendMQTT_n("Humidity", h);
      sendMQTT_n("Moisture", m);
      delay(2000);
      client.loop();
    }
  }
}

//Read the Moisture sensor and return value
//Nomalised Value = 0, Raw Value = 1
float readMoisture(int mode){
  // power the sensor
  digitalWrite(sensorVCC, HIGH);
  digitalWrite(blueLED, LOW);
  delay(100);
  // read the value from the sensor:
  float moist = analogRead(soilPin);         
  digitalWrite(sensorVCC, LOW);  
  digitalWrite(blueLED, HIGH);
  delay(100);
  if(mode == 0){
    Serial.println(moist);
    return tanh(map(moist, 1, 500, 0, 3)) * 100;
    // return map(moist, 1, 1024, 0, 100);
  }else{
    return moist;
  };
}

//Read DHT22 sensor and return value
//temperature = 1, humidity = 2
float readDHT22(int mode){
  switch (mode)
  {
  case 1:
    DHTValue = dht.readTemperature(); // Gets the values of the temperature
    return DHTValue;
    break;
  case 2:
    DHTValue = dht.readHumidity(); // Gets the values of the humidity
    return DHTValue;
    break;
  default:
    Serial.print("DHT22 mode error : Select from 'temperature' or 'humidity'.");
    DHTValue = -1;
    return DHTValue;
    break;
  }
}

void startWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  // check to see if connected and wait until you are
  while (WiFi.status() != WL_CONNECTED) {
    switch (WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.print(ssid);
        Serial.println(" - Not Exist.");
        break;
      case WL_CONNECT_FAILED:
        Serial.print(ssid);
        Serial.println(" - Connection Fail.");
        break;
      case WL_CONNECTION_LOST:
        Serial.print(ssid);
        Serial.println(" - Connection List.");
        break;
      case WL_WRONG_PASSWORD:
        Serial.print(ssid);
        Serial.println(" - Wrong Password.");
        break;
      case WL_DISCONNECTED:
        Serial.print(ssid);
        Serial.println(" - Disconnected.");
        break;
      default:
        Serial.print(ssid);
        Serial.println(" - Error(Default Code).");
        break;
    }
    delay(1000);
    Serial.print("Connecting...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
}

bool sendMQTT_n(char* topic, float value) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(isRepeated(topic, value)){
    Serial.print(topic);
    Serial.println(" - Same Value Detected.");
    return false;
  }else{
    dtostrf(value, 7, 2, msg); 
    // snprintf (msg, 50, "%.1f", value);
    char sendingAddress[20];
    strcpy(sendingAddress, topicAddress);
    strcat(sendingAddress, "/");
    strcat(sendingAddress, topic);
    client.publish(sendingAddress, msg);
    return true;
  }
}

bool isRepeated(char* topic, float value){
  if(topic == "Humidity"){
    if(Humidity == value){
      return true;
    }else{
      Humidity = value;
      return false;
    }
  }else if (topic == "Temperature"){
    if(Temperature == value){
      return true;
    }else{
      Temperature = value;
      return false;
    }
  }else{
    if(Moisture == value){
      return true;
    }else{
      Moisture = value;
      return false;
    }
  }
}

void sendMQTT() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  snprintf (msg, 50, "%.1f", Temperature);
  Serial.print("Publish message for t: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnimx/temperature", msg);

  Humidity = dht.readHumidity(); // Gets the values of the humidity
  snprintf (msg, 50, "%.0f", Humidity);
  Serial.print("Publish message for h: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnimx/humidity", msg);

  //Moisture = analogRead(soilPin);   // moisture read by readMoisture function
  snprintf (msg, 50, "%.0i", Moisture);
  Serial.print("Publish message for m: ");
  Serial.println(msg);
  client.publish("student/CASA0014/plant/ucfnimx/moisture", msg);

}

void callback(char* topic, byte* payload, unsigned int length) {
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  // for (int i = 0; i < length; i++) {
  //   Serial.print((char)payload[i]);
  // }
  // Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-UCFNIMX";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect with clientID, username and password
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("student/CASA0014/plant/ucfnimx/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void startWebserver() {
  // when connected and IP address obtained start HTTP server  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");  
}

void handle_OnConnect() {
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity
  server.send(200, "text/html", SendHTML(readDHT22(1), readDHT22(2), readMoisture(0)));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat, float Humiditystat, float Moisturestat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>DY Plant Monitor</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<img src='https://www.google.com/url?sa=i&url=https%3A%2F%2Fgiphy.com%2Fexplore%2Fnot-quite-my-tempo&psig=AOvVaw180Ig-_XHSeR7B_uK__uLo&ust=1667406159464000&source=images&cd=vfe&ved=0CAwQjRxqFwoTCLidy5CyjfsCFQAAAAAdAAAAABAE' alt='Need Water Face'>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP8266 Huzzah DHT22 Report</h1>\n";

  ptr += "<p>Temperature: ";
  ptr += Temperaturestat;
  ptr += " C</p>";
  ptr += "<p>Humidity: ";
  ptr += Humiditystat;
  ptr += "%</p>";
  ptr += "<p>Moisture: ";
  ptr += Moisturestat;
  ptr += "</p>";
  ptr += "<p>Sampled on: ";
  ptr += GB.dateTime("l,");
  ptr += "<br>";
  ptr += GB.dateTime("d-M-y H:i:s T");
  ptr += "</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}