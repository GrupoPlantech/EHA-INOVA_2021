//#define BLYNK_DEBUG

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLkkPm9u8q"
#define BLYNK_DEVICE_NAME "Estufa"
char auth[] = "g5OLhvoCHXEfZXfIu-WtJaZbKy-9PwCH";

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

bool modo = 0;
bool acbomba = 0;
bool acluz = 0;
bool accooler = 0;
bool ledbomba = 0;
bool ledluz = 0;
bool ledcooler = 0;

int temp = 0;
int umid = 0;
int hora = 0;
int minu = 0;

#define DHTPIN D6
#define ledOn D7
#define luz D5
#define cooler D1 
#define bomba D2
int sensorNv = A0;
 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <NTPtimeESP.h>
#define DEBUG_ON

NTPtime NTPch("br.pool.ntp.org");       // Servidor ntp -> "br.pool.ntp.org"
char *ssid      = "iPhone";               // WiFi SSID
char *password  = "gutoguto";         // WiFi senha
 
#define DHTTYPE    DHT11

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

strDateTime dateTime;

BLYNK_WRITE(V0){
  bool AutoMan = param.asInt();
  modo = AutoMan;
  Blynk.virtualWrite(V1, modo);
}

BLYNK_WRITE(V2){
  bool Btbomba = param.asInt();
  acbomba = Btbomba;
}

BLYNK_WRITE(V4){
  bool Btluz = param.asInt();
  acluz = Btluz;
}

BLYNK_WRITE(V6){
  bool Btcooler = param.asInt();
  accooler = Btcooler;
}

void setup() {
     
  Serial.begin(115200);
  
  pinMode(modo,  OUTPUT);
  pinMode(ledOn,  OUTPUT);
  pinMode(luz,    OUTPUT);
  pinMode(cooler, OUTPUT);
  pinMode(bomba,  OUTPUT);

    digitalWrite(luz, HIGH);  
    digitalWrite(cooler, HIGH);  
    digitalWrite(bomba, HIGH); 
    
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //espera conectar na rede
    Serial.print(".");
    digitalWrite(ledOn, HIGH);
    delay(500);
    digitalWrite(ledOn, LOW);
  }
  Serial.println("WiFi connected");
  
  //inicia o sensor
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);

  // Determina o delay entre leituras, baseado nos detalhes do sensor.
  delayMS = sensor.min_delay / 1000;
  
  Blynk.begin(auth, ssid, password);
}
          
void loop() {

  dateTime = NTPch.getNTPtime(-3, 0); //(defazagem do horario UTC(br -3), (1=h de verao/ 0=s/ h de verao))

  if(dateTime.valid){

     Blynk.run();
 
   digitalWrite(ledOn, HIGH);                               //o acionamento é normal-trasnsistor nPn
   Blynk.virtualWrite(V12, ledOn);
   
  //NTPch.printDateTime(dateTime);

    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualsecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMonth = dateTime.month;
    byte actualday =dateTime.day;
    byte actualdayofWeek = dateTime.dayofWeek;
  
   Blynk.virtualWrite(V10, actualHour);
   Blynk.virtualWrite(V11, actualMinute);
    
        Serial.print(F("Hora: "));
        Serial.print(actualHour );
        Serial.print(":");
        Serial.println( actualMinute);
        Serial.println("");
        
  
  // Delay entre leituras.
  delay(delayMS);

  sensors_event_t event;
    
    int leituraNv = analogRead(sensorNv);
    int nivel = map(leituraNv,0,1024,0,100);

     dht.temperature().getEvent(&event);
     temp = event.temperature; 
     
     Blynk.virtualWrite(V8, temp);  
       
     if (isnan(event.temperature)) {
      Serial.println(F("Erro na leitura da temperatura!"));
      }
     else {
        Serial.print(F("Temperatura: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
          }
              
     dht.humidity().getEvent(&event);
     umid = event.relative_humidity;
     
     Blynk.virtualWrite(V9, umid);
     
      if (isnan(event.relative_humidity)) {
        Serial.println(F("Erro na leitura da umidade!"));
        }
      else {
        Serial.print(F("Umidade: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
           }
    
    if (modo == LOW){
        
      if ((temp > 25) ||(umid > 80))
      {
      Serial.println ("cooler on ");
      digitalWrite(cooler, LOW);                 //o acionamento é invertido-trasnsistor pNp
      Blynk.virtualWrite(V7, HIGH); 
      }
      
      else
      {
      Serial.println ("cooler off ");
      digitalWrite(cooler, HIGH);                //o acionamento é invertido-trasnsistor pNp
      Blynk.virtualWrite(V7, LOW); 
      }

      if ((actualHour >= 18) || (actualHour <= 6))
      {
      Serial.println ("luz on ");  
      digitalWrite(luz, LOW);                    //o acionamento é invertido-trasnsistor pNp
      Blynk.virtualWrite(V5, HIGH);     
      } 
      else
      {
      Serial.println ("luz off ");
      digitalWrite(luz, HIGH);                   //o acionamento é invertido-trasnsistor pNp
      Blynk.virtualWrite(V5, LOW); 
      }
      
      if (nivel > 30)
      {
      Serial.println("Nível = ok ");
      digitalWrite(bomba, LOW);                  //o acionamento é invertido-trasnsistor pNp
      Blynk.virtualWrite(V3, HIGH);      
      }
      else
      {
      Serial.println("Nível = baixo! ");
      digitalWrite(bomba, HIGH);                 //o acionamento é invertido-trasnsistor pNp 
      Blynk.virtualWrite(V3, LOW);       
      }
    
      }
        
      else{

      if (acbomba == HIGH)
      {
        Serial.println("Nível = ok ");
        digitalWrite(bomba, LOW);                //o acionamento é invertido-trasnsistor pNp
        Blynk.virtualWrite(V3, HIGH);
      }
      
      else
      {
        Serial.println("Nível = baixo! ");
        digitalWrite(bomba, HIGH);               //o acionamento é invertido-trasnsistor pNp
        Blynk.virtualWrite(V3, LOW);
      }
      if (acluz == HIGH)
      {
        Serial.println ("luz on ");
        digitalWrite(luz, LOW);                  //o acionamento é invertido-trasnsistor pNp
        Blynk.virtualWrite(V5, HIGH);
      }
      else
      {
        Serial.println ("luz off ");
        digitalWrite(luz, HIGH);                 //o acionamento é normal-trasnsistor pNp
        Blynk.virtualWrite(V5, LOW);
      }
     
        if (accooler == HIGH)
      {
        Serial.println ("cooler on ");       
        digitalWrite(cooler, LOW);               //o acionamento é invertido-trasnsistor pNp
        Blynk.virtualWrite(V7, HIGH);
      }
      else
      {
        Serial.println ("cooler off ");      
        digitalWrite(cooler, HIGH);              //o acionamento é invertido-trasnsistor pNp
        Blynk.virtualWrite(V7, LOW);
      }
      }           
        Serial.println("");
        Serial.println(""); 
    }
  }
