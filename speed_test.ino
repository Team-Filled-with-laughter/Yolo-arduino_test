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
const int speed = 9;
const int In1 = 5;
const int In2 = 6;
const int overSpeed = 5;
const int Address = 0x50;

bool isOverspeed = false;
bool isEmergency = false;

int v1 = 0;
int v2 = 0;
int buffer = 0;

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD의 주소를 0x27로 설정

void setup() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  Serial.begin(115200);  // 시리얼 통신을 115200 보드레이트로 초기화
  pinMode(Sigpin1, INPUT);
  pinMode(Sigpin2, INPUT);
  pinMode(speed, OUTPUT);
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  MsTimer2::set(500,playLcd);
  MsTimer2::start();
  actuatorUp();
  lcd.init();            // LCD 초기화
  lcd.backlight();       // 백라이트 켜기
  lcd.clear();           // LCD 화면 지우기
}

void drawMessage(const char* line1, const char* line2, int cursorX) {
  u8g2.setFont(u8g2_font_unifont_t_korean1);
  u8g2.setFontDirection(0);
  u8g2.setCursor(20, 20);
  u8g2.print(line1);
  u8g2.setCursor(cursorX, 40);
  u8g2.print(line2);
}

void playLcd() {
u8g2.firstPage();
  do {
    if (analogRead(A4) < 700) {
      drawMessage("비가 내리니", "안전 운행하세요", 5);
    } else {
      drawMessage("즐거운 하루", "되세요!", 40);
    }
  } while (u8g2.nextPage());
}

void speedCheck1() {
  while (digitalRead(Sigpin1));
  while (!digitalRead(Sigpin1));
  unsigned long T = pulseIn(Sigpin1, HIGH) + pulseIn(Sigpin1, LOW); 

  if (T != 0)
  {
    double frequency = 1.0 / (double)T;
    v1 = int((frequency * 1e6) / 44.0);

    if (v1 <= 120) 
    {
      Serial.print("v1 Speed: ");
      Serial.print(v1);
      Serial.println(" km/h");
    }
    else // 속도 이상값 
    {
      Serial.print("Velocity Outlier!");
    }
  }
  else 
  { // 측정이 안된경우 
    Serial.print("No sensing!");
  }
}

void speedCheck2() {
  while (digitalRead(Sigpin2));
  while (!digitalRead(Sigpin2));
  unsigned long T = pulseIn(Sigpin2, HIGH) + pulseIn(Sigpin2, LOW); // 0.1s안에 HIGH값 안들어오면 0처리

  if (T != 0)
  {
    double frequency = 1.0 / (double)T;
    v2 = int((frequency * 1e6) / 44.0);

    if (v2 <= 120) 
    {
      Serial.print("v2 Speed: ");
      Serial.print(v2);
      Serial.println(" km/h");
    }
    else // 속도 이상값 
    {
      Serial.print("Velocity Outlier!");
    }
  }
  else 
  { // 측정이 안된경우 
    Serial.print("No sensing!");
  }
}

void actuatorDown() {
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  analogWrite(speed, 255);
}

void actuatorUp() {
  digitalWrite(In1, LOW);
  digitalWrite(In2, HIGH);
  analogWrite(speed, 255);
}

void detectEmergency() {
  buffer = Serial.available();  // 시리얼 버퍼에 있는 바이트 수를 읽음
  
  if (Serial.available() > 0) {
    char received = Serial.read();
    isEmergency = (received == '1') ? true : false; // 수정

    lcd.clear();  // LCD 화면 지우기
    lcd.setCursor(0, 0);  // 커서를 첫 번째 줄 첫 번째 열로 설정

    if (!isEmergency) {
      lcd.print("car");  // 비상 상황이 아니면 "car" 출력
    } else {
      lcd.print("ambulance");  // 비상 상황이면 "ambulance" 출력
    }

    lcd.setCursor(11, 0);  // 커서를 두 번째 줄 첫 번째 열로 설정
    lcd.print(buffer);  // 시리얼 버퍼에 있는 바이트 수 출력
    lcd.setCursor(0, 1);
    lcd.print(v1);
    lcd.setCursor(5, 1);
    lcd.print(v2);
  }
}

void loop() {

  speedCheck1();
  speedCheck2();

  detectEmergency();
  isOverspeed = (v1 >= overSpeed || v2 >= overSpeed) ? true : false; // 수정

  if (isOverspeed){ 

    if (!isEmergency){
      Serial.println("Overspeed!!!");
      actuatorDown();
      delay(3000);
      actuatorUp();
    }
  }
}
