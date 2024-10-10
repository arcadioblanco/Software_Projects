#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <WiFiClientSecure.h>
#include <WiFi.h> 
#include <HTTPClient.h> 


#define SS_PIN    5
#define RST_PIN   15
MFRC522::MIFARE_Key key;
MFRC522 mfrc522(SS_PIN, RST_PIN);
byte ID[5];

const char* ssid     = "Test";
const char* password = "12345678";
String URL = "http://192.168.43.105/esp32/DU_imie_i_nazwisko.php" ;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
 
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  
  SPI.begin();   // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522 
}
 
void loop() {

	if (WiFi.status()==WL_CONNECTED)
	{
    
    
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
   
    byte bufferSize = mfrc522.uid.size;
	  String odczytane_ID;
    HTTPClient http;
	  http.begin(URL);
	  http.addHeader("Content-Type", "application/x-www-form-URLencoded");
	  odczytane_ID = "odczytane_ID=";

     for (byte i = 0; i < bufferSize; i++) {
          ID[i] = mfrc522.uid.uidByte[i];
     }
	   for (byte i = 0; i < bufferSize; i++) {
          odczytane_ID += String(ID[i]);
     }
	 Serial.print(odczytane_ID);
	 Serial.println();

 int httpCode = http.POST(odczytane_ID); 
 String payload = http.getString(); 
 Serial.print("URL : "); Serial.println(URL); 
 Serial.print("httpCode: "); Serial.println(httpCode);
 Serial.print("payload : "); Serial.println(payload);   
 Serial.println("--------------------------------------------------");	
     mfrc522.PICC_HaltA();
}
}
