#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Time.h>
#include <Servo.h>


#define FIREBASE_HOST "wintercapstonedesign-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "CVZnha3EeEZqdRPeb1mBW08XiOdmExztb3ofAslm"
#define WIFI_SSID "iPhone 13"
#define WIFI_PASSWORD "BRUV-fE8L-0Rgn-Ej6x"
WiFiServer server(80);

#define SERVOMOTORPIN 0
#define DOORLOCKPIN 2

String user_id = "bae0000";
String kind="";

Servo servo;
unsigned long OldClock=0;
int angle=0;
int count=0;

void setup(){
  Serial.begin(9600);
  
  pinMode(SERVOMOTORPIN,OUTPUT);
  servo.attach(SERVOMOTORPIN);
  servo.write(0);

  pinMode(DOORLOCKPIN,OUTPUT);
  digitalWrite(DOORLOCKPIN,0);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected : ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop(){
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
    }
    if(Firebase.getInt("pwd_num")<5){
      if(Firebase.getString("shoes_detected")=="true"){
        while(true){
            unsigned long now=millis();
            if(now-OldClock>=100){
            OldClock=now;
            servo.write(count*30);
            count++;
            if(count>6) count=0;
            if(Firebase.getString("face_detected")=="true"){
              break;
            }
          } 
        }
      }
      if(Firebase.getString("check_face")=="true"||
      Firebase.getString("check_pwd")=="true"||
      Firebase.getString("check_qr")=="true"){
        Serial.println("Door Open");
        digitalWrite(DOORLOCKPIN,HIGH);
        delay(750);
        digitalWrite(DOORLOCKPIN,LOW);
        Firebase.setString("check_face","false");
        Firebase.setString("check_otp","false");
        Firebase.setString("check_pwd","false");
        Firebase.setString("check_qr","false");
      }
    }
    else{
      if(Firebase.getString("check_otp")=="true"){
        Firebase.setInt("pwd_num",0);
        Firebase.setString("check_otp","false");
      }
    }
}
