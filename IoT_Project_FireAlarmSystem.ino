#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" 
#include <BlynkSimpleEsp8266.h>

char auth[] = "HeLJ3Hlko5d76Xk1TzzYsstZ_PYsmU-i";
char ssid[] = "ali";
char pass[] = "ali12345";

/* ThingSpeak Kurulumu */ 
unsigned long channelID =1996784; 
unsigned int field_no=1; 
const char* writeAPIKeyy = "T6F22R2UXQGNBR7N";   // Thingspeak write API Key for graph
const char* readAPIKey = "Okuma anahtarını giriniz";   // Thingspeak read API Key 

void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "l0vRYMqsqf2tCktn_XdeIB1PTecmSqOH5rNneHr2UJw";

//twitter thingspeak
const String writeAPIKey = "ZEB0WB4ZET1MZGRV";   // Thingspeak write API Key
const char* thingSpeakHost = "api.thingspeak.com";     //ThingSpeak sitesinin hostu
String thingtweetAPIKey = "TGCFG82LR4EHC4ID";    //ThingTweet sayfasından aldığımız API Keyi
int randNumber;
WiFiClient client;
float sicaklikdegeri;   // Analog değeri dönüştüreceğimiz sıcaklık değeri
float olculendeger;     // Ölçeceğimiz analog değer
String tsData;

BlynkTimer timer;
int pinValue = 0;

#define LED1 D1
#define LED2 D2
#define Buzzer D3
#define Sensor D0

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Sensor, INPUT);
  Blynk.begin(auth, ssid, pass);
   ThingSpeak.begin(client);         // ThingSpeak client sınıfı başlatılıyor
  timer.setInterval(1000L, notifiaction);
}
BLYNK_WRITE(V0) {
  pinValue = param.asInt();
}

void sendDataTS(int deger){
  ThingSpeak.writeField (channelID, field_no, deger, writeAPIKeyy);       // sıcaklık değerini gönder 
  Serial.println("\n"); 
}
void sendtweet(){
  //thingspeak tw
  randNumber=random(5,1000);
  Serial.print("//");
  Serial.println(randNumber);
  if (client.connect (thingSpeakHost, 80)) {        //Bu blokta sıcaklık değerini host urlsi yardımıyla
  String postStr = "field1=";                  //1 Nolu Fielda verileri göstereceğiz
  postStr+= String (sicaklikdegeri);
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
 client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
  client.print("\n\n");
  Serial.println(sicaklikdegeri);
  Serial.println ("Thingspeak'e sıcaklık degeri gönderildi");
}
  if (client.connect("api.thingspeak.com",80)) {
  Serial.println("TWİTTER CONNECTED");  //Tweet atılması için gereken blok
  tsData = "api_key="+thingtweetAPIKey+"&status="+"  #WARNING" + "  A fire was detected! Location : Kemalpaşa, Esentepe Kampüsü, Üniversite Cd., 54050 Serdivan/Sakarya "+"   // "+randNumber ;
  client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(tsData.length());
  client.print("\n\n");
  client.print(tsData);
  Serial.println("TWEETED");
}
client.stop();
}

void send_event(const char *event)
{
  Serial.print("Connecting to "); 
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
int deger;
void notifiaction() {
  int sensor = digitalRead(Sensor);
  if (pinValue == 1) {
    Serial.println("System is ON");
    if (sensor == 1) {
      digitalWrite(LED2, HIGH);
      digitalWrite(LED1, LOW);
      digitalWrite(Buzzer, LOW);
      deger=0; 
      sendDataTS(deger);
    } else if (sensor == 0) {
      Blynk.notify("WARNING! A fire was detected");
      digitalWrite(LED2, LOW);
      digitalWrite(LED1, HIGH);
      digitalWrite(Buzzer, HIGH);
      sendtweet(); 
      send_event("FIRE DETECTION");  
      send_event("Alert");
      deger=100; 
      sendDataTS(deger);
    }
  } else if (pinValue == 0) {
    Serial.println("System is OFF");
  }
}

void loop() {
 Blynk.run();
 timer.run();
}
