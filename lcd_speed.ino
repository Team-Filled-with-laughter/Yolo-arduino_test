#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <MsTimer2.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#endif

const int Sigpin1 = 8; 
const int Sigpin2 = 12;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD의 주소를 0x27로 설정

bool isEmergency = false;

int v1 = 0;
int v2 = 0;

void setup() {
  Serial.begin(115200);  // 시리얼 통신을 115200 보드레이트로 초기화
  lcd.init();            // LCD 초기화
  lcd.backlight();       // 백라이트 켜기
  lcd.clear();           // LCD 화면 지우기
  pinMode(Sigpin1, INPUT);
  pinMode(Sigpin2, INPUT);
}

void speedCheck1() {
  while (digitalRead(Sigpin1));
  while (!digitalRead(Sigpin1));
  unsigned long T = pulseIn(Sigpin1, HIGH, 1000000) + pulseIn(Sigpin1, LOW, 1000000); // 타임아웃 1초 설정

  if (T != 0)
  {
    double frequency = 1.0 / (double)T;
    v1 = int((frequency * 1e6) / 44.0);

    if (v1 > 120) {
      v1 = 0;
    }
  } 
  else { // 측정이 안된경우 
    v1 = 0;
  }
  lcd.setCursor(0, 1);
  lcd.print(v1);
}

void speedCheck2() {
  while (digitalRead(Sigpin2));
  while (!digitalRead(Sigpin2));
  unsigned long T = pulseIn(Sigpin2, HIGH, 1000000) + pulseIn(Sigpin2, LOW, 1000000); // 타임아웃 1초 설정

  if (T != 0)
  {
    double frequency = 1.0 / (double)T;
    v2 = int((frequency * 1e6) / 44.0);

    if (v2 > 120) {
      v2 = 0;
    }
  } 
  else { // 측정이 안된경우 
    v2 = 0;
  }
  lcd.setCursor(5, 1);
  lcd.print(v2);
}

void detectEmergency() {
  if (Serial.available() > 0) {
    char received = Serial.read();
    isEmergency = (received == '1');

    lcd.clear();  // LCD 화면 지우기
    lcd.setCursor(0, 0);  // 커서를 첫 번째 줄 첫 번째 열로 설정

    if (!isEmergency) {
      lcd.print("car");  // 비상 상황이 아니면 "car" 출력
    } else {
      lcd.print("ambulance");  // 비상 상황이면 "ambulance" 출력
    }

    lcd.setCursor(11, 0);  // 커서를 첫 번째 줄 11번째 열로 설정
    lcd.print((int)Serial.available());  // 시리얼 버퍼에 있는 바이트 수 출력
  }
}

void loop() {
  static unsigned long lastCheck1 = 0;
  static unsigned long lastCheck2 = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - lastCheck1 >= 1000) { 
    lastCheck1 = currentMillis;
    speedCheck1();
  }

  // speedCheck2을 1초마다 실행
  if (currentMillis - lastCheck2 >= 1000) { 
    lastCheck2 = currentMillis;
    speedCheck2();
  }

  detectEmergency();
}
