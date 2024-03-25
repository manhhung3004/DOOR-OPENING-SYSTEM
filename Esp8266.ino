#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <AESLib.h>

const char* ssid = "Thanh Van";
const char* password = "0903175436";

AESLib aeslib;


// Khóa AES - 128 bit (16 bytes) được tạo ngẫu nhiên hihi
byte aes_key[16] = {
  0x76, 0x2F, 0x3A, 0xD9, 0x17, 0xF1, 0x36, 0x58,
  0x94, 0xBF, 0xE6, 0x9B, 0x2E, 0x35, 0xA2, 0x6A
};

// IV - 16 bytes được tạo ngẫu nhiên hihi
byte aes_iv[N_BLOCK] = {
  0x59, 0xB1, 0x7A, 0x0C, 0x7A, 0x36, 0x6D, 0x1A,
  0x23, 0x79, 0x8F, 0x5F, 0x62, 0x3B, 0x3F, 0xCC
};

String passwordkey = "";
String idCard = "";
String status = "";

SoftwareSerial uart(D6, D5); // Khai báo chân TX, RX của NodeMCU.

String data = ""; //Lưu trữ log từ mega
WiFiClient client; // Đối tượng WiFiClient để sử dụng trong HTTPClient
String apiKeyValue = "tPmAT5Ab3j7F9";

void sendToServer(String data) {
  // Tạo đối tượng HTTPClient
  HTTPClient http;
  // Gửi yêu cầu POST đến URL cụ thể
  String url = "http://192.168.100.119/Web_LockDoor/sources/get_data.php";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Tách chuỗi thành phần signal và idCard
  Serial.print(data);
  parseData(data, passwordkey, idCard);
  Serial.print("Dữ liệu password: ");
  Serial.println(passwordkey);
  Serial.print("Dữ liệu id card: ");
  Serial.println(idCard);
  //String httpRequestData = "api_key=" + apiKeyValue + "&Passwordkey=" + passwordkey + "&Idcard=" + idCard;
  String httpRequestData = apiKeyValue + "&" + passwordkey + "&" + idCard;
  String encrypted_message = encrypt_message(httpRequestData, aes_key, aes_iv);
  String encrypted_message_post = "post=" + encrypted_message;
  Serial.print("data truyền đi ");
  Serial.println(encrypted_message_post);
  // truyền lại dữ liệu về arduino
  int httpResponseCode = http.POST(encrypted_message_post);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Response:");
    status = response; 
    Serial.print(status);
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpResponseCode);
  }
  passwordkey = 0;
  idCard = 0;
  // Giải phóng tài nguyên và đóng kết nối
  http.end();
}

void parseData(String data, String& passwordkey, String& idCard) {
  if (data.length() == 4) {
    passwordkey = data;
    idCard = 0;
  } else {
    idCard = data;
    passwordkey = 0;
  }
}

void setup() {
  Serial.begin(115200);
  uart.begin(115200); 
  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Đọc dữ liệu từ Mega2560
  if (uart.available()) {
    data = "";
    data = uart.readString();
    Serial.println("...............");
    Serial.println("Received from Mega2560: ");
    sendToServer(data);
     if (status == "1"){
      uart.print("1");
    }else if (status == "2") {
      //if it wrong
      uart.print("2");
    } else if( status == "3"){
      // if it alerts
      uart.print("3");
    }
    delay(5000);
  }
}
// Hàm mã hóa
String encrypt_message(String msg, byte key[], byte iv[]) {
  char data[msg.length() + 1]; // Buffer cho dữ liệu gốc
  msg.toCharArray(data, msg.length() + 1);
  
  char encrypted[2 * msg.length() + 1]; // Buffer cho dữ liệu mã hóa
  aeslib.encrypt((byte*)data,  sizeof(data),(byte*)encrypted, key, sizeof(key), iv);

  String encrypted_str = "";
  for (unsigned int i = 0; i < sizeof(encrypted); i++) {
    char str[3];
    sprintf(str, "%02x", (int)encrypted[i]);
    encrypted_str += str;
  }
  return encrypted_str;
}