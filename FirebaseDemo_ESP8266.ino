#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Time.h>

#define FIREBASE_HOST "wintercapstonedesign-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "CVZnha3EeEZqdRPeb1mBW08XiOdmExztb3ofAslm"
#define WIFI_SSID "iPhone 13"
#define WIFI_PASSWORD "BRUV-fE8L-0Rgn-Ej6x"

#define DOORLOCKPIN 8

// 1. db에서 input_password 가져와서 user id의 lock_pwd와 비교
// 1-1 값이 일치하면 도어록 잠금 해제
// 1-2 잠금해제 되면 현재 날짜랑 시간, user id를 db에 저장 (door_open)

// 2. check_qr, check_face 값 가져와서 true일 때 도어록 잠금 해제
// 2-1 qr, face 각각 나눠서 로그 저장 

// 3. otp는 어떻게? 

String user_id = "bae0000";
String kind="";

void setup() {
  Serial.begin(9600);
  pinMode(DOORLOCKPIN, OUTPUT);
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {

//  Firebase.setFloat("number", 42.0);
//
//  if (Firebase.failed()) {
//      Serial.print("setting /number failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  String input_pwd = Firebase.getString("input_password"); // 안드로이드 앱에서 사용자가 입력한 비밀번호
//  String real_pwd = Firebase.getString("user/"+ user_id + "/lock_pwd"); // 사용자가 등록한 비밀번호

    if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
    }
    else {
      if(Firebase.getString("input_password") == Firebase.getString("user/"+ user_id + "/lock_pwd")) {
        Serial.println("same");
        DoorOpen();
      }
      else if(isOpen("check_qr")==true){
        kind="QR";
        DoorOpen();
      }
      else if(isOpen("check_face")==true){
        kind="FACE";
        DoorOpen();
      }
      else {
        Serial.println("different");  
      }
    }

  // update value
//  Firebase.setFloat("number", 43.0);
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /number failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  delay(1000);
//
//  // get value 
//  Serial.print("number: ");
//  Serial.println(Firebase.getFloat("number"));
//  delay(1000);
//
//  // remove value
//  Firebase.remove("number");
//  delay(1000);
//
//  // set string value
//  Firebase.setString("message", "hello world");
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /message failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  delay(1000);
//  
//  // set bool value
//  Firebase.setBool("truth", false);
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("setting /truth failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  delay(1000);
//
//  // append a new value to /logs
//  String name = Firebase.pushInt("logs", n++);
//  // handle error
//  if (Firebase.failed()) {
//      Serial.print("pushing /logs failed:");
//      Serial.println(Firebase.error());  
//      return;
//  }
//  Serial.print("pushed: /logs/");
//  Serial.println(name);
//  delay(1000);
}

void DoorOpen(){
  digitalWrite(DOORLOCKPIN,HIGH);
  delay(750);
  digitalWrite(DOORLOCKPIN,LOW);
}

bool isOpen(String condition){
  if(Firebase.getBool(condition)==true){
    String date=year()+"/"+month()+"/"+day();
    String dayTime=hour()+":"+minute()+":"+second();
    Firebase.setBool(condition,false);
    n++;
    Firebase.setString("door_open/"+user_id+"/"+condition+"/"+n+"/date",date);
    Firebase.setBool("door_open/"+user_id+"/"+condition+"/"+n+"/success",true);
    Firebase.setString("door_open/"+user_id+"/"+condition+"/"+n+"/user_id",user_id);
    //String real_pwd = Firebase.getString("user/"+ user_id + "/lock_pwd");
    return true;
  }
  return false;
}
