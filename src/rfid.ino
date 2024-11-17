#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h> 
#include <HTTPClient.h> 
#include <esp_task_wdt.h>
#include <ESP32Ping.h>


#define OLED_RESET -1
#define SS_PIN    5
#define RST_PIN   15
#define LOCK_SIMULATOR 4
#define WATCHDOG_RESET 3

Adafruit_SSD1306 display(OLED_RESET);
MFRC522::MIFARE_Key key;
MFRC522 mfrc522(SS_PIN, RST_PIN);

byte ID[5];
uint16_t probki [16];
int usrednij_probki()
{
    int suma=0;
    int srednia=0;
    for (uint8_t i=0; i<16; i++)
    {
        probki[i] = analogRead(32);
        suma += (int)probki[i];
    }
    srednia = suma/16;
    srednia = map(srednia, 0, 2900, 0,100);
    return srednia;
}
const unsigned char lock_bitmap [] PROGMEM = {
	0x00, 0x07, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xff, 0x00, 
	0x00, 0xfc, 0x3f, 0x00, 0x01, 0xf0, 0x0f, 0x80, 0x01, 0xe0, 0x07, 0x80, 0x03, 0xe0, 0x07, 0xc0, 
	0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 
	0x03, 0xc0, 0x03, 0xc0, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 
	0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xf8, 0x1f, 0xf0, 
	0x0f, 0xf8, 0x1f, 0xf0, 0x0f, 0xf8, 0x1f, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 
	0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 0x0f, 0xfc, 0x3f, 0xf0, 
	0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0, 0x0f, 0xff, 0xff, 0xf0
};
const unsigned char zamek_odblok [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 
	0x00, 0x3f, 0xfc, 0x00, 0x00, 0x78, 0x1e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 
	0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0xc0, 
	0x03, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x07, 0xfe, 0x7f, 0xe0, 
	0x07, 0xfc, 0x3f, 0xe0, 0x07, 0xfc, 0x3f, 0xe0, 0x07, 0xfe, 0x7f, 0xe0, 0x07, 0xfe, 0x7f, 0xe0, 
	0x07, 0xfe, 0x7f, 0xe0, 0x07, 0xfe, 0x7f, 0xe0, 0x07, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xc0, 
	0x03, 0xff, 0xff, 0xc0, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const char* ssid     = "UPC1975F41";
const char* password = "ccavyr4Rjmbd"; 

String baza_URL = "http://192.168.0.220/esp32/sprawdz_uzytkownika.php";
HTTPClient baza;
void setup() {
  
pinMode(LOCK_SIMULATOR,OUTPUT);

Serial.begin(115200);

esp_task_wdt_init(WATCHDOG_RESET, true);
esp_task_wdt_add(NULL);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
  
  
display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
display.clearDisplay();
 
SPI.begin();   
mfrc522.PCD_Init(); 
display.clearDisplay();
 
}

void loop() {
  
    display.setTextColor(WHITE, BLACK);
    display.setTextSize(1.5);
    display.setCursor(0,0);
    digitalWrite(LOCK_SIMULATOR, HIGH);       
    /*Uśrednianie próbek z ADC - monitorowanie stanu zasilania*/
    int avg = usrednij_probki();

    /*Pingowanie hosta*/
    bool success = Ping.ping("192.168.0.220");
         if (success == true)
	 {                      
            display.clearDisplay();
            display.setTextColor(WHITE, BLACK);
            if (avg < 15)
            {
                  display.println("Bateria:       Wi-Fi:");
                  display.println(String(avg)+"%" + "          Aktywne");
                  display.println();
                  display.println(" WYMIENIC AKUMULATOR!");
                  display.display();
            }
            display.println("Bateria:       Wi-Fi:");
            display.println(String(avg)+"%" + "           Aktywne");
            display.println();
            display.println("    PRZYLOZ KARTE: ");
            display.display();

    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
   
    byte bufferSize = mfrc522.uid.size;
	  String wyslij_ID;
    
    baza.begin(baza_URL);
    baza.addHeader("Content-Type", "application/x-www-form-URLencoded");
    wyslij_ID = "wyslij_ID=";

            for (byte i = 0; i < bufferSize; i++) {
                  ID[i] = mfrc522.uid.uidByte[i];
            }
            for (byte i = 0; i < bufferSize; i++) {
                  wyslij_ID += String(ID[i]);
            }

    Serial.println(wyslij_ID);
    baza.POST(wyslij_ID); 
    String payload = baza.getString(); 

            if (payload == "OK")
            {
                digitalWrite(LOCK_SIMULATOR, LOW);
                display.clearDisplay();
                display.drawBitmap(0,0,zamek_odblok,32,32, WHITE);
                display.setCursor(40, 5);
                display.print("ZAAKCEPTOWANO");
                display.setCursor(40, 15);
                display.print("DOSTEP");
                display.display();
            }

            else if (payload == "NOK")
            {
                display.clearDisplay();
                display.drawBitmap(0,0,lock_bitmap,32,32, WHITE);
                display.setCursor(40, 5);
                display.print("ODRZUCONO");
                display.setCursor(40, 15);
                display.print("DOSTEP");
                display.display();
             }
    baza.end();
    
    mfrc522.PICC_HaltA();
    delay(3000);
    display.clearDisplay();
} 


else {
    display.clearDisplay();
    display.println("Bateria:       Wi-Fi:");
    display.println(String(avg)+"%" + "        Nieaktywne");
    display.println();
    display.println("    DRZWI OTWARTE");
    display.display();
  }

                
}
 