#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>
#include <Servo.h>

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

Servo servo;
FirebaseData firebaseData;

int pinRELAY = 8; // 도어록 제어 핀 번호
int check_pwd_num; // 비밀번호 실패 횟수 
String check_qr; // QR 코드 성공여부 값 
String check_face; // 얼굴 인식 성공여부 값 
String check_pwd; // 비밀번호 입력 성공여부 값 
String check_otp; // otp 입력 성공여부 값 
String check_shoes; // 신발 인식 성공여부 값

void setup()
{
  Serial.begin(9600);
  delay(1000);
  Serial.println();

  Serial.print("Initialize IMU sensor...");
  if (!IMU.begin()) {
    Serial.println(" failed!");
    while (1);
  }
  Serial.println(" done");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  Serial.print("Connecting to WiFi...");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  // 릴레이 제어 핀을 출력으로 설정
  pinMode(pinRELAY, OUTPUT);

  // 서보모터 제어핀 설정
  servo.attach(7);
}

void loop()
{
  if (IMU.accelerationAvailable()) {
     if(Firebase.getInt(firebaseData, "/pwd_num")) {
      Serial.println("비밀번호 입력 실패 횟수 ok"); 
      if(firebaseData.dataType() == "int") {
        check_pwd_num = firebaseData.intData();
        // 값이 true일 때 도어록 잠금 해제
        if(check_pwd_num >= 5) {
          pinRELAY = 1; 
          Serial.println("도어록이 비활성화됩니다"); 
        }
      }
    }
    
    // 1. QR 코드 인식으로 잠금 해제
    if(Firebase.getString(firebaseData, "/check_qr")) { 
      // db에서 값을 제대로 가져왔는지 체크
      Serial.println("QR ok"); 
      if(firebaseData.dataType() == "string") {
        check_qr = firebaseData.stringData();
        // 값이 true일 때 도어록 잠금 해제
        if(check_qr == "true") {
          // 도어록 잠금 제어 (LOW와 HIGH는 한 세트)
          digitalWrite(pinRELAY, LOW);
          delay(1000);
          digitalWrite(pinRELAY, HIGH); 
          // 도어록 잠금이 계속 제어되는 것을 막기 위해 다시 값을 false로 변경
          if(Firebase.setString(firebaseData, "/check_qr", "false")) { 
            Serial.println("QR 코드 인식이 성공적으로 완료되었습니다.");
          }
        }
      }
    }

    // 2. 신발 인식이 성공적으로 된 경우 각도 조절
    if(Firebase.getString(firebaseData, "/shoes_detected")) {
      Serial.println("shoes ok"); 
      if(firebaseData.dataType() == "string") {
        check_shoes = firebaseData.stringData();
        // 값이 true일 때 도어록 잠금 해제
        if(check_shoes == "true") {
          // 서보모터 각도 조절
          servo.write(180);
        }
      }
    }

    // 3. 얼굴 인식으로 잠금 해제
    if(Firebase.getString(firebaseData, "/check_face")) {
      Serial.println("face ok"); 
      if(firebaseData.dataType() == "string") {
        check_face = firebaseData.stringData();
        // 값이 true일 때 도어록 잠금 해제
        if(check_face == "true") {
          digitalWrite(pinRELAY, LOW);
          delay(1000);
          digitalWrite(pinRELAY, HIGH); 
          if(Firebase.setString(firebaseData, "/check_face", "false")) { 
            if(Firebase.setString(firebaseData, "/shoes_detected", "false")) { 
              servo.write(0);
              Serial.println("얼굴 인식이 성공적으로 완료되었습니다.");
            }
          }
        }
      }
    }

    // 4. 비밀번호 입력으로 잠금 해제
    if(Firebase.getString(firebaseData, "/check_pwd")) {
      Serial.println("pwd ok"); 
      if(firebaseData.dataType() == "string") {
        check_pwd = firebaseData.stringData();
        // 값이 true일 때 도어록 잠금 해제
        if(check_pwd == "true") {
          digitalWrite(pinRELAY, LOW);
          delay(1000);
          digitalWrite(pinRELAY, HIGH); 
          if(Firebase.setString(firebaseData, "/check_pwd", "false")) { 
            Serial.println("비밀번호입력이 성공적으로 완료되었습니다.");
          }
        }
      }
    }

    // 5. OTP를 정확하게 입력한 경우 도어록 다시 활성화
    if(Firebase.getString(firebaseData, "/check_otp")) {
      Serial.println("otp ok"); 
      if(firebaseData.dataType() == "string") {
        check_otp = firebaseData.stringData();
        // 값이 true일 때 도어록을 다시 제어할 수 있도록 pin 번호를 원래대로 복구시킴
        if(check_otp == "true") {
          pinRELAY = 8;
          if(Firebase.setString(firebaseData, "/check_otp", "false")) { 
            if(Firebase.setInt(firebaseData, "/pwd_num", 0)) {
              Serial.println("도어록을 다시 활성화합니다."); 
            }
          }
        }
      }
    }
    delay(2000);
  }
  else {
    // 파이어베이스 연결에 실패하면 오류 메시지 출력
    Serial.println(IMU.accelerationAvailable());
  }
}
