#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#define AWS_IOT_PUBLISH_TOPIC   "lamda"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
#define lamp 2
#define gpio_0 0
String myid;
bool sflag;
bool report=0;
bool reportstate;
bool check=0;
int switc;
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

//switch configurations
const int S1 = 25;
const int S2 = 27;
const int S3 = 22;
const int S4 = 19;
const int S5 = 17;
const int S6 = 16;

//relay configurations
const int r1 = 33;
const int r2 = 26;
const int r3 = 21;
const int r4 = 18;
const int r5 = 5;
const int r6 = 4;

volatile unsigned long lastDebounceTime = 0;
volatile unsigned long debounceDelay = 10;
bool state1,state2,state3,state4,state5,state6;

void RelayControl(int sw,bool sta)
{
check=1;
report=1;
switc=sw;
reportstate=sta;
switch(sw)
  {
    case 1:
    digitalWrite(r1,!sta);
    break;

    case 2:
    digitalWrite(r2,!sta);
    break;

    case 3:
    digitalWrite(r3,!sta);
    break;

    case 4:
    digitalWrite(r4,!sta);;
    break;

    case 5:
    digitalWrite(r5,!sta);
    break;

    case 6:
    digitalWrite(r6,!sta);
    break;
  }

}

void SwitchControl(int sw)
{
  report=1;
  switc=sw;
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
  Serial.println("working" + String(sw));
  switch(sw)
  {
    case 1:
    state1=!state1;
    reportstate=state1;
    digitalWrite(r1,!state1);
    Serial.println(state1);
    break;

    case 2:
    state2=!state2;
    reportstate=state2;
    digitalWrite(r2,!state2);
    Serial.println(state2);
    break;

    case 3:
    state3=!state3;
    reportstate=state3;
    digitalWrite(r3,!state3);
    Serial.println(state3);
    break;

    case 4:
    state4=!state4;
    reportstate=state4;
    digitalWrite(r4,!state4);
    Serial.println(state4);
    break;

    case 5:
    state5=!state5;
    reportstate=state5;
    digitalWrite(r5,!state5);
    Serial.println(state5);
    break;

    case 6:
    state6=!state6;
    reportstate=state6;
    digitalWrite(r6,!state6);
    Serial.println(state6);
    break;
  }
  lastDebounceTime = millis();
  }
}

void ReportState()
{
  if (!client.connected()) {
    connectAWS();
  }
  StaticJsonDocument<200> doc;
  doc["type"] = "action";
  doc["id"] = "123";
  doc["Switch_id"] = (myid +"-"+String(switc));
  doc["stat"]= reportstate; 
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  report=0;
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  else
  {
  // Subscribe to a topic
  client.subscribe(myid.c_str());
  Serial.println("AWS IoT Connected!");
  }
 
 
}
 
void sendConfigData()
{
   //write code to find the connected switch
  Serial.print("sending message : ");
  Serial.println(AWS_IOT_PUBLISH_TOPIC);
  StaticJsonDocument<200> doc;
  doc["type"] = "config";
  doc["id"] = "123";
  doc["Switch_id"] = "";
  doc["stat"]= false;
  //doc["device_id"] = myid;
  JsonArray list = doc.createNestedArray("device_list");
  // Add pin no of relay as values to the list

  list.add(myid +"."+"1");
  list.add(myid +"."+"2");
  list.add(myid +"."+"3");
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  int Switc = doc["Switch"]; // int has to be replaced with the datattype of the incomming file
  bool stat=doc["stat"];

   Serial.println();
   Serial.println(Switc); 
   Serial.println(stat);
   RelayControl(Switc,stat);
  /*for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]); // Pring payload content
  }*/
  Serial.println();
}
 
void publishMessage()   // to publish msg to aws
{
  if (!client.connected()) {
    connectAWS();
  }
  Serial.print("sending message : ");
  Serial.println(AWS_IOT_PUBLISH_TOPIC);
  StaticJsonDocument<200> doc;
  doc["myid"] = myid;
  doc["state"] = sflag;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

}
 
 
void setup()
{
  Serial.begin(115200);
  myid=WiFi.macAddress();
  myid.replace(":","_");
  //sendConfigData();
  Serial.println("my id : " + myid);
  pinMode (lamp, OUTPUT);
  pinMode (gpio_0, INPUT);
  digitalWrite(lamp, LOW);
  sflag=true;
  
  //switch
  pinMode(S1, INPUT_PULLDOWN);
  pinMode(S2, INPUT_PULLDOWN);
  pinMode(S3, INPUT_PULLDOWN);
  pinMode(S4, INPUT_PULLDOWN);
  pinMode(S5, INPUT_PULLDOWN);
  pinMode(S6, INPUT_PULLDOWN);
  
  //relay
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(r4, OUTPUT);
  pinMode(r5, OUTPUT);
  pinMode(r6, OUTPUT);
  delay(10);
  state1=digitalRead(S1);
  state2=digitalRead(S2);
  state3=digitalRead(S3);
  state4=digitalRead(S4);
  state5=digitalRead(S5);
  state6=digitalRead(S6); 
  digitalWrite(r1,!state1);
  digitalWrite(r2,!state2);
  digitalWrite(r3,!state3);
  digitalWrite(r4,!state4);
  digitalWrite(r5,!state5);
  digitalWrite(r6,!state6);

  //config();
  attachInterrupt(digitalPinToInterrupt(S1), [] { SwitchControl(1); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(S2), [] { SwitchControl(2); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(S3), [] { SwitchControl(3); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(S4), [] { SwitchControl(4); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(S5), [] { SwitchControl(5); }, CHANGE);
  attachInterrupt(digitalPinToInterrupt(S6), [] { SwitchControl(6); }, CHANGE);
  connectAWS();
}


void loop()
{
  if(check)
  {
  state1=digitalRead(S1);
  state2=digitalRead(S2);
  state3=digitalRead(S3);
  state4=digitalRead(S4);
  state5=digitalRead(S5);
  state6=digitalRead(S6);
  check=0;
  }
  if (!client.connected()) {
    connectAWS(); // Reconnect to AWS IoT if disconnected
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Reconnect to WiFi
    delay(500);
    Serial.print(".");
   // Serial.println("WiFi reconnected.");
  }
  client.loop();
  if (digitalRead(gpio_0) == LOW)
  {
    while (digitalRead(gpio_0) == LOW);
    sflag=!sflag;
    digitalWrite(lamp, sflag);
    publishMessage(); 
  }
  if(report)
  ReportState();
  delay(100);
}