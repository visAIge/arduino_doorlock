#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>

#define FIREBASE_HOST "wintercapstonedesign-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "CVZnha3EeEZqdRPeb1mBW08XiOdmExztb3ofAslm"
#define WIFI_SSID "배선영의 iPhone"
#define WIFI_PASSWORD "qotjsdud67"

FirebaseData firebaseData;

int pinRELAY = 8; // 도어록 제어 핀 번호
String check_qr; // QR 코드 성공여부 값 가져오기

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
}

void loop()
{
  // Read IMU acceleration data
  if (IMU.accelerationAvailable()) {
    // QR 코드 정상 판별 여부 확인하기
    if(Firebase.getString(firebaseData, "/check_qr")) { 
      // 데이터베이스에 값을 제대로 가져왔는지 체크
      Serial.println("ok"); 
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
    delay(2000);
  }
  else {
    // 파이어베이스 연결에 실패하면 오류 메시지 출력
    Serial.println(IMU.accelerationAvailable());
  }
}
