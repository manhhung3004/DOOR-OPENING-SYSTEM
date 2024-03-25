#include <Servo.h> // Khai báo thư viện sử dụng cho động cơ
#include <LiquidCrystal.h> 
#include <Keypad.h> // Khai báo thư viện Keypad
#include <SPI.h>
#include <MFRC522.h>
#include <stdio.h>
#include <stdlib.h>

String data;

Servo myServo;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const byte ROWS = 4; // Bốn hàng
const byte COLS = 3; // Ba cột
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPin[4] = {22, 24, 26, 28}; // ROW PINS OF KEYPAD
byte colPin[3] = {30, 32, 34}; // COLUMN PINS OF KEYPAD
Keypad keypad = Keypad(makeKeymap(keys), rowPin, colPin, ROWS, COLS); // MAPPING THE KEYPAD.

char str[4] = {' ', ' ', ' ', ' '};
int i;
int buzzerPin = 8;

#define SS_PIN 53
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Tạo đối tượng MFRC522

void setup()
{
  myServo.attach(10); // Khai báo chân điều khiển động cơ
  lcd.begin(16, 2); // Khai báo sử dụng LCD với 16 cột và 2 hàng
  pinMode(buzzerPin, OUTPUT);
  analogWrite(6, 120);
  lcd.clear();
  lcd.print(" Enter Password");

  SPI.begin(); // Khởi động giao tiếp SPI
  mfrc522.PCD_Init(); // Khởi động module RC522
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop()
{
    data = "";
    char key = keypad.getKey(); // Ký tự nhập vào sẽ gán cho biến key
    if (Serial1.available()) {
      String data = Serial1.readString();
      Serial.print("Receiced form mcu:");
      Serial.println(data);
    }

    //Trường hợp nhập key
    if (key) // Nhập mật khẩu
    {
        if (i == 0)
        {
            str[0] = key;
            lcd.setCursor(6, 1);
            lcd.print(str[0]);
            delay(200); // Ký tự hiển thị trên màn hình LCD trong 1s
            lcd.setCursor(6, 1);
            lcd.print("*"); // Ký tự được che bởi dấu *
        }
        if (i == 1)
        {
            str[1] = key;
            lcd.setCursor(7, 1);
            lcd.print(str[1]);
            delay(200);
            lcd.setCursor(7, 1);
            lcd.print("*");
        }
        if (i == 2)
        {
            str[2] = key;
            lcd.setCursor(8, 1);
            lcd.print(str[2]);
            delay(200);
            lcd.setCursor(8, 1);
            lcd.print("*");
        }
        if (i == 3)
        {
            str[3] = key;
            lcd.setCursor(9, 1);
            lcd.print(str[3]);
            delay(200);
            lcd.setCursor(9, 1);
            lcd.print("*");

            String keyInput="";
            //Gửi key nhập vào đến mcu
            for (int i = 0; i <= 3; i++){
              keyInput += str[i];
            }

            Serial1.print(keyInput);
            keyInput = "";            
        }
        i = i + 1;
    }
    
    //Trường hợp quẹt thẻ
    // Kiểm tra xem có thẻ RFID được đặt gần module hay không
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Đọc UID của thẻ RFID
      Serial.print("UID tag: ");
      String content = "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      
      //Gửi ID tag đến mcu
      Serial1.print(content.substring(1));      
      Serial.println(content.substring(1));
      content = "";
    }
    mfrc522.PICC_HaltA();  // Dừng giao tiếp với thẻ
    mfrc522.PCD_StopCrypto1();  // Ngừng mã hóa

    //Nhận thông tin từ mcu gửi về
    if (Serial1.available()) {    
      data = Serial1.readString();  
    } 

    int signal = data.toInt();
    // Serial.print(signal);
    switch (signal){
      case 1:
      {
        lcd.clear();
        lcd.print("    Correct!");
        delay(1000);
        myServo.write(180); // Mở cửa
        lcd.clear();
        lcd.print("      Opened!");
        i = 0; //Reset i
        
        Serial.println(signal);
        break;
      }
      case 2:
      {
        lcd.clear();
        lcd.print("    Correct!");
        delay(1000);
        myServo.write(180); // Mở cửa
        lcd.clear();
        lcd.print("      Opened!");
        Serial.println(signal);
        break;
      }
      case 3:
      {
        lcd.clear();
        lcd.print("   Incorrect!");
        delay(1000);
        lcd.clear();
        lcd.print("   Try Again!");
        delay(1000);
        lcd.clear();
        lcd.print(" Enter Password");
      }
      case 4:
      {
        lcd.clear();
        lcd.setCursor(0, 0); // Đặt con trỏ vị trí hàng 1, cột 0
        lcd.print("   Incorrect!");
        lcd.setCursor(0, 1); // Đặt con trỏ vị trí hàng 2, cột 0
        lcd.print("     Error!!!");
        Serial.println(signal);

        for (int i = 0; i < 20; i++)
        {
            digitalWrite(buzzerPin, HIGH);
            delay(250);
            digitalWrite(buzzerPin, LOW);
            delay(250);
            str[0] = keypad.getKey();
        }
        break;
      }
      case 5:
      {
        lcd.clear();
        myServo.write(90);
        lcd.print("     Closed!");
        // Serial1.println("Signal=Close");
        delay(1000);
        lcd.clear();
        lcd.print(" Enter Password");
        Serial.println(signal);
        break;
      }
      default:
        break;
    }

    //Đóng cửa
    switch (key)
    {
    case '#':
    {
        lcd.clear();
        myServo.write(90);
        lcd.print("     Closed!");
        // Serial1.println("Signal=Close");
        delay(1000);
        lcd.clear();
        lcd.print(" Enter Password");
        i = 0;
        break;
    }
    default: break;
    }
}