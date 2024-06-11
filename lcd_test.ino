#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD의 주소를 0x27로 설정

bool isEmergency = false;
int buffer = 0;

void setup() {
  Serial.begin(115200);  // 시리얼 통신을 115200 보드레이트로 초기화
  lcd.init();            // LCD 초기화
  lcd.backlight();       // 백라이트 켜기
  lcd.clear();           // LCD 화면 지우기
}

void loop() {
  buffer = Serial.available();  // 시리얼 버퍼에 있는 바이트 수를 읽음

  if (Serial.available() > 0) {
    char received = Serial.read();  // 들어오는 바이트를 읽음
    isEmergency = (received == '1');  // 받은 값이 '1'이면 isEmergency를 true로 설정

    lcd.clear();  // LCD 화면 지우기
    lcd.setCursor(0, 0);  // 커서를 첫 번째 줄 첫 번째 열로 설정

    if (!isEmergency) {
      lcd.print("car");  // 비상 상황이 아니면 "car" 출력
    } else {
      lcd.print("ambulance");  // 비상 상황이면 "ambulance" 출력
    }

  }
  lcd.setCursor(0, 1);  // 커서를 두 번째 줄 첫 번째 열로 설정
    lcd.print(buffer);  // 시리얼 버퍼에 있는 바이트 수 출력
}
