//VISITA IL TUTORIAL E ISCRIVITI AL MIO CANALE
//https://www.youtube.com/watch?v=xdIcr5uys3o
// 

/* www.lutritech.it

  Telecomando IR smart a comando vocale
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech
  https://twitter.com/lutritech

*/



#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager Serve per gestire la connessione wifi tramite il browser visitando 192.168.4.1
#include <DNSServer.h>

#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRtimer.h>
#include <IRutils.h>
#include <ir_Argo.h>
#include <ir_Coolix.h>
#include <ir_Daikin.h>
#include <ir_Fujitsu.h>
#include <ir_Gree.h>
#include <ir_Haier.h>
#include <ir_Hitachi.h>
#include <ir_Kelvinator.h>
#include <ir_LG.h>
#include <ir_Magiquest.h>
#include <ir_Midea.h>
#include <ir_Mitsubishi.h>
#include <ir_NEC.h>
#include <ir_Panasonic.h>
#include <ir_Samsung.h>
#include <ir_Toshiba.h>
#include <ir_Trotec.h>
#include <ir_Whirlpool.h>
#include <time.h>

//Gestione per il display oled
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Myfont.h" //contiene il font usato per il display
#include "icone.h" //contine le icone da visualizzare nel display

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char auth[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxx";  // INSERITE QUI IL VOSTRO TOKEN DI Blynk

int comando = 0;
const uint16_t kIrLed = 15;  //GPIO15 è il pin D8
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// catturato con IRrecvDumpV2.ino power on home teather
uint16_t rawData[73] = {4506, 4488,  564, 558,  564, 600,  520, 1678,  562, 1680,  562, 558,  562, 1678,  564, 598,  542, 536,  562, 556,  562, 558,  562, 1678,  562, 1678,  564, 558,  564, 1676,  564, 602,  520, 600,  520, 556,  584, 1654,  566, 1678,  564, 1676,  564, 1676,  586, 534,  586, 536,  560, 558,  564, 1676,  564, 558,  562, 558,  562, 600,  520, 558,  584, 1654,  586, 1654,  564, 1678,  564, 46948,  4508, 4486,  564, 1690,  564};

//per l'ora
int timezone = 3600;
int dst = 0;

BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // leggo il valore del pin V0 dal cloud di Blynk
  comando = pinValue;
  display.clearDisplay();

  switch (comando)
  {
    case 0: {
#if SEND_NEC
        irsend.sendNEC(3772793023, 32);
#endif  // SEND_NEC 9
        Serial.print("Power TV:"); //tv on-off
        Serial.println(comando);

        display.drawBitmap(0, 0,  powerTV, 50, 50, WHITE);
        //scriviSuDisplay("Power TV");
        display.display();
        delay(3000);
        break;
      }


    case 1: {
#if SEND_NEC
        irsend.sendNEC(3772837903, 32);
#endif  // SEND_NEC 9
        Serial.print("MUTO:"); //MUTO
        Serial.println(comando);

        scriviSuDisplay("Muto");
        break;
      }


    case 2: {
#if SEND_NEC
        irsend.sendNEC(3772795063, 32);
#endif  // SEND_NEC 
        Serial.print("canale successivo:"); //p+
        Serial.println(comando);

        scriviSuDisplay("Canale successivo");
        break;
      }


    case 3: {
#if SEND_NEC
        irsend.sendNEC(3772778743, 32);
#endif  // SEND_NEC 
        Serial.print("canale precendente:"); //p-
        Serial.println(comando);

        scriviSuDisplay("Canale indietro");
        break;
      }

    case 4: {

        irsend.sendNEC(3772833823, 32);
        delay(100);
        irsend.sendNEC(3772833823, 32);
        delay(100);
        irsend.sendNEC(3772833823, 32);
        delay(100);
        irsend.sendNEC(3772833823, 32);
        delay(100);
        irsend.sendNEC(3772833823, 32);
        delay(100);

        Serial.print("Aumento Volume TV:"); //volume+
        //scriviSuDisplay("Aumento Volume");
        display.clearDisplay();
        display.drawBitmap(0, 0,  audioPiu, 50, 50, WHITE);
        display.display();
        delay(3000);
        break;
      }


    case 5: {

        irsend.sendNEC(3772829743, 32);
        delay(100);
        irsend.sendNEC(3772829743, 32);
        delay(100);
        irsend.sendNEC(3772829743, 32);
        delay(100);
        irsend.sendNEC(3772829743, 32);
        delay(100);
        irsend.sendNEC(3772829743, 32);
        delay(100);

        Serial.print("Diminuisco Volume TV"); //volume-
        Serial.println(comando);

        scriviSuDisplay("Abbasso Volume");
        break;
      }


    case 6: {
#if SEND_NEC
        irsend.sendNEC(3772784863, 32);
#endif  // SEND_NEC 9
        Serial.print("RAI1:"); // Rai1
        Serial.println(comando);

        scriviSuDisplay("RA1");
        break;
      }
    case 7: {
#if SEND_NEC
        irsend.sendNEC(3772817503, 32);
#endif  // SEND_NEC 9
        Serial.print("RAI2:"); //Rai2
        Serial.println(comando);

        scriviSuDisplay("RAI2");
        break;
      }

    case 8: {
#if SEND_NEC
        irsend.sendNEC(3772801183, 32);
#endif  // SEND_NEC 9
        Serial.print("RAI3:"); //RAI3
        Serial.println(comando);

        scriviSuDisplay("RAI3");

        break;
      }
    case 9: {
#if SEND_NEC
        irsend.sendNEC(3772780783, 32);
#endif  // SEND_NEC 9
        Serial.print("RETE4:"); //RETE4
        Serial.println(comando);

        display.clearDisplay();
        display.drawBitmap(0, 0,  RETE4, 50, 50, WHITE);
        //display.print("RETE4"); //RETE4
        display.display();
        delay(3000);
        break;
      }
    case 10: {
#if SEND_NEC
        irsend.sendNEC(3772813423, 32);
#endif  // SEND_NEC 9
        Serial.print("CANALE5:"); //CANALE5
        Serial.println(comando);

        display.clearDisplay();
        display.drawBitmap(0, 0,  CANALE5, 50, 50, WHITE);
        //display.print("CANALE5"); //CANALE5
        display.display();
        delay(3000);
        break;
      }
    case 11: {
#if SEND_NEC
        irsend.sendNEC(3772797103, 32);
#endif  // SEND_NEC 9
        Serial.print("italia1:"); //italia uno
        Serial.println(comando);

        display.clearDisplay();
        display.drawBitmap(0, 0,  ITALIA1, 50, 50, WHITE);
        display.display();
        delay(3000);
        break;
      }
    case 12: {
#if SEND_NEC
        irsend.sendNEC(3772788943, 32);
#endif  // SEND_NEC 9
        Serial.print("LA7:"); //LA7
        Serial.println(comando);
        scriviSuDisplay("LA7");
        break;
      }

    case 13: {
#if SEND_NEC
        irsend.sendNEC(3772821583, 32);
#endif  // SEND_NEC 9
        Serial.print("Canale8:"); //8
        Serial.println(comando);
        scriviSuDisplay("Canale8");
        break;
      }

    case 14: {
#if SEND_NEC
        irsend.sendNEC(3772805263, 32);
#endif  // SEND_NEC 9
        Serial.print("Canale9:"); //9
        Serial.println(comando);

        scriviSuDisplay("Canale9");
        break;
      }


    case 15: {
#if SEND_NEC
        irsend.sendNEC(3772782313, 32);
#endif  // SEND_NEC 9
        Serial.print("TV ok"); //15
        Serial.println(comando);
        scriviSuDisplay("TV OK");
        break;
      }



    case 16: {
#if SEND_NEC
        irsend.sendNEC(3772809343, 32);
        delay(100);
        irsend.sendNEC(3772810873, 32); //freccia giu
        //delay(100);
        //irsend.sendNEC(3772782313, 32); //ok
#endif  // SEND_NEC 9
        Serial.print("Sorgente TV"); //15
        Serial.println(comando);

        scriviSuDisplay("Sorgente TV");

        break;
      }




    case 20: {
#if SEND_NEC
        irsend.sendNEC(16753245, 32);
#endif  // SEND_NEC 9
        Serial.print("Accendo VideoProiettore:");
        Serial.println(comando);

        display.clearDisplay();
        display.drawBitmap(0, 0,  proiettore, 50, 50, WHITE);
        display.display();
        delay(3000);
        break;
      }


    case 30: {
#if SEND_RAW
        irsend.sendRaw(rawData, 73, 38);  // mando i rawdata a 38Khz a LG.
#endif  // SEND_RAW
        Serial.println("Hifi Power:");
        Serial.println(comando);

        display.clearDisplay();
        display.drawBitmap(0, 0,  stereo, 120, 60, WHITE);
        display.display();
        delay(3000);
        //scriviSuDisplay("Stereo Power");
        break;
      }


    case 31: {
#if SEND_NEC
        irsend.sendNEC(0x343451AE, 32);
#endif  // SEND_NEC 9
        Serial.print("Sorgente Stereo:");
        Serial.println(comando);

        scriviSuDisplay("Sorgente Stereo");

        break;
      }


      delay(1000);
    default: {
        Serial.print("comando non codificato:");
        Serial.println(comando);

        scriviSuDisplay("non codificato");

        break;
      }
  }
}


void setup() {
  irsend.begin();
  Serial.begin(9600);
  WiFiManager wifiManager;

  //cancello le info sulle reti salvate...usato per test
  //wifiManager.resetSettings();

  //configTime(timezone, dst, "ntp1.inrim.it", "time.nist.gov");
  configTime(timezone, dst, "ntp1.inrim.it");
  Serial.println("inizializzo display");

  //per display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // ---> L INDIRIZZO CORRETTO è QUESTO 0x3C 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); // Clear the bufferd
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner


  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  display.println("Connecting to ");
  Blynk.config(auth);

  display.println("");
  display.println("WiFi connected");
  display.println(WiFi.localIP()); // Print the IP address
  display.display();
  delay(3000);
  //

  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  Blynk.run();
  orologio();
}

void orologio() {
  //gestisco come si vedrà sul display
  display.clearDisplay(); // Pulisco il buffer
  display.setFont(&Roboto_Thin_26);
  //display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 25);

  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  //////
  display.print(p_tm->tm_mday);
  display.print("/");
  display.print(p_tm->tm_mon + 1);
  display.print("/");
  display.print(p_tm->tm_year - 100); //mi da le ultime due cifre del millennio quindi 19

  display.setCursor(0, 60);
  display.print(p_tm->tm_hour);
  display.print(":");
  display.print(p_tm->tm_min);
  display.print(":");
  display.print(p_tm->tm_sec);
  display.display();

  delay(1000);  //aggiorno ogni secondo
}

void scriviSuDisplay (String st)
{
  //display.setTextSize(2);
  display.clearDisplay();
  display.setFont(&Roboto_Thin_26);
  display.setCursor(0, 25);
  display.print(st);
  display.display();
  delay (2000);
}


void configModeCallback (WiFiManager *myWiFiManager) {  //viene invocata nel caso non si riesca a collegarsi alla rete salvata
  Serial.println("config mode");
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("CONFIG MODE");
  display.println("VISIT");
  display.println("http://"+WiFi.softAPIP().toString());
  display.display();
  delay (2000);

  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}
