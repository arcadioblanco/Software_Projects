#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <WiFi.h> 
#include <HTTPClient.h> 
#include "bitmapy.h"


#define OLED_RESET -1
#define SS_PIN    5
#define RST_PIN   15
Adafruit_SSD1306 display(OLED_RESET);
MFRC522::MIFARE_Key key;
MFRC522 mfrc522(SS_PIN, RST_PIN);
byte ID[5];


const char* ssid     = "Test";
const char* password = "12345678";
String sprawdz_URL = "http://192.168.1.15/esp32/sprawdz_uzytkownika.php";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
 
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  
  SPI.begin();   // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  
  
  display.drawBitmap(0, 0, pwr_bitmap, 128, 32, WHITE);
  display.display();

  delay(2000);
  display.clearDisplay();
  display.drawBitmap(0, 0, wefim_bitmap, 128, 32, WHITE);
  display.display();
   
  delay(2000);
  display.clearDisplay();
 
}

void loop() {

	if (WiFi.status()==WL_CONNECTED)
	{
    
	display.setTextColor(WHITE, BLACK);
    display.setTextSize(1);
    display.setCursor(0,0);

    display.println("Przyloz karte...");
    display.println();
    display.display();

    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
   
    display.println();
    byte bufferSize = mfrc522.uid.size;
	String wyslij_ID;
    HTTPClient http;
	http.begin(sprawdz_URL);
	http.addHeader("Content-Type", "application/x-www-form-URLencoded");
	wyslij_ID = "wyslij_ID=";

     for (byte i = 0; i < bufferSize; i++) {
          ID[i] = mfrc522.uid.uidByte[i];
     }
	   for (byte i = 0; i < bufferSize; i++) {
          wyslij_ID += String(ID[i]);
     }
	 Serial.print(wyslij_ID);
	 Serial.println();

    int httpCode = http.POST(wyslij_ID); 
    String payload = http.getString(); 

  Serial.print("sprawdz_URL : "); Serial.println(sprawdz_URL); 
  Serial.print("Data: "); Serial.println(wyslij_ID); 
  Serial.print("httpCode: "); Serial.println(httpCode); 
  Serial.print("payload : "); Serial.println(payload); 
  Serial.println("--------------------------------------------------");
	
	if (payload == "OK")
	display.println("Potwierdzono - prosze wejsc!");

	else if (payload == "NOK")
	display.println("Odrzucono - sprobuj ponownie");

    display.println();
    display.display();

    // Halt PICC
    mfrc522.PICC_HaltA();
    delay(3000);      
    display.clearDisplay();
	}
	else {
    Serial.println("WiFi Disconnected");
  }
}
