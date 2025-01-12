#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h> // เพิ่มไลบรารี EEPROM

LiquidCrystal_I2C lcd(0x27, 16, 2);

// กำหนดขาเซนเซอร์
int sensorOneCoin = 8;
int sensorTwoCoin = 9;
int sensorFiveCoin = 10;
int sensorTenCoin = 11;
int resetButton = 7; // ปุ่ม Reset (ต่อกับขา 7)
// กำหนดขา Buzzer
int buzzerPin = 6;

// ตัวแปรเก็บสถานะ
int oneCoin = 0;
int twoCoin = 0;
int fiveCoin = 0;
int tenCoin = 0;
int totalCoin = 0;

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Count Coin");
  lcd.setCursor(2, 1);
  lcd.print("BornToBeDev");
  delay(500);

  // กำหนด pinMode สำหรับเซนเซอร์และปุ่ม Reset
  pinMode(sensorOneCoin, INPUT_PULLUP);
  pinMode(sensorTwoCoin, INPUT_PULLUP);
  pinMode(sensorFiveCoin, INPUT_PULLUP);
  pinMode(sensorTenCoin, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP); // ใช้ INPUT_PULLUP สำหรับปุ่ม Reset
  pinMode(buzzerPin, OUTPUT); // กำหนด pinMode สำหรับ Buzzer
  Serial.begin(9600);

  // โหลดค่าที่บันทึกไว้จาก EEPROM
  totalCoin = EEPROM.read(0); // อ่านค่าที่อยู่ตำแหน่ง 0
  totalCoin |= EEPROM.read(1) << 8; // รวมค่าจากตำแหน่ง 1 (สำหรับตัวเลขมากกว่า 255)

  // อัปเดตหน้าจอเมื่อเริ่มต้น
  updateDisplay();
}

void loop() {
  // อ่านค่าจากเซนเซอร์
  oneCoin = digitalRead(sensorOneCoin);
  twoCoin = digitalRead(sensorTwoCoin);
  fiveCoin = digitalRead(sensorFiveCoin);
  tenCoin = digitalRead(sensorTenCoin);

  // ตรวจจับเหรียญและเพิ่มค่าในตัวแปร
  if (oneCoin == LOW) {
    totalCoin += 1;
    playCoinSound(1);
    updateDisplay();
    delay(250); // ป้องกันการอ่านซ้ำ
  }

  if (twoCoin == LOW) {
    totalCoin += 2;
    playCoinSound(2); 
    updateDisplay();
    delay(250);
  }

  if (fiveCoin == LOW) {
    totalCoin += 5;
    playCoinSound(5);
    updateDisplay();
    delay(250);
  }

  if (tenCoin == LOW) {
    totalCoin += 10;
    playCoinSound(10);
    updateDisplay();
    delay(250);
  }

  // ตรวจสอบการกดปุ่ม Reset
  if (digitalRead(resetButton) == LOW) { // หากปุ่ม Reset ถูกกด
    resetTotalCoin();
    delay(500); // หน่วงเวลาป้องกันการกดซ้ำ
  }
}

// ฟังก์ชันสำหรับอัปเดตหน้าจอ
void updateDisplay() {
  lcd.clear();

  // แสดงหัวข้อด้วยสัญลักษณ์ตกแต่ง
  lcd.setCursor(0, 0);
  lcd.print("== Total Coin ==");

  // จัดตำแหน่งตัวเลขตรงกลาง พร้อมหน่วย Baht
  String displayText = String(totalCoin) + " Baht"; // สร้างข้อความรวมหน่วย
  lcd.setCursor((16 - displayText.length()) / 2, 1); // คำนวณตำแหน่งตรงกลาง
  lcd.print(displayText);
  saveToEEPROM(totalCoin);
}

// ฟังก์ชันสำหรับบันทึกค่าลงใน EEPROM
void saveToEEPROM(int value) {
  EEPROM.write(0, value & 0xFF); // บันทึก byte แรก (ต่ำสุด)
  EEPROM.write(1, (value >> 8) & 0xFF); // บันทึก byte ที่สอง (สูงสุด)
}

// ฟังก์ชัน Reset ค่าเหรียญทั้งหมด
void resetTotalCoin() {
  totalCoin = 0; // รีเซ็ตค่าเป็น 0
  saveToEEPROM(totalCoin); // บันทึกค่าใหม่ลงใน EEPROM
  updateDisplay(); // อัปเดตหน้าจอแสดงผล
  lcd.setCursor(0, 1);
  lcd.print("  Reset Complete  "); // แสดงข้อความ Reset สำเร็จ
  delay(1000); // หน่วงเวลาให้ผู้ใช้อ่านข้อความ
  updateDisplay(); // อัปเดตหน้าจอแสดงผล
}

void playCoinSound(int coinValue) {
  // ระยะเวลาของแต่ละโน้ต (ms)
  int duration = 100; 

  // สร้างเมโลดี้สำหรับเสียงหยอดเหรียญ
  switch (coinValue) {
    case 1: // เสียงเหรียญ 1 บาท
      tone(buzzerPin, 800, duration);
      delay(duration);
      tone(buzzerPin, 1000, duration);
      delay(duration);
      break;

    case 2: // เสียงเหรียญ 2 บาท
      tone(buzzerPin, 900, duration);
      delay(duration);
      tone(buzzerPin, 1100, duration);
      delay(duration);
      break;

    case 5: // เสียงเหรียญ 5 บาท
      tone(buzzerPin, 800, duration);
      delay(duration);
      tone(buzzerPin, 1200, duration);
      delay(duration);
      tone(buzzerPin, 1000, duration);
      delay(duration);
      break;

    case 10: // เสียงเหรียญ 10 บาท
      tone(buzzerPin, 600, duration);
      delay(duration);
      tone(buzzerPin, 900, duration);
      delay(duration);
      tone(buzzerPin, 1200, duration);
      delay(duration);
      break;
  }
  noTone(buzzerPin); // หยุดเสียงหลังเล่นเมโลดี้จบ
}
