/* www.lutritech.it

  Voltmetro Wifi
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech
  https://twitter.com/lutritech

*/


// Collega RST al pin D0 del Wemos!

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <TimeLib.h>
#include <time.h>
#include <Timezone.h>    // https://github.com/JChristensen/Timezone

char auth[] = "VostroToken"; //token progetto server Blynk
char ssid[] = "VostraReteWifi";
char pass[] = "VostraPasswordWifi";


unsigned int raw = 0;
float volt = 0.0;
String voltString = "";
String currentDate = "";

const int sleepTimeS = 60; // Time to sleep (in seconds):

WiFiUDP ntpUDP;
int GTMOffset = 0; // SET TO UTC TIME
NTPClient timeClient(ntpUDP, "it.pool.ntp.org", GTMOffset * 60 * 60, 60 * 60 * 1000);

// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone CE(CEST, CET);

static tm getDateTimeByParams(long time) {
  struct tm *newtime;
  const time_t tim = time;
  newtime = localtime(&tim);
  return *newtime;
}

/**
   Input tm time format and return String with format pattern
*/
static String getDateTimeStringByParams(tm *newtime, char* pattern = (char *)" %H:%M:%S %d/%m/%Y") {
  char buffer[30];
  strftime(buffer, 30, pattern, newtime);
  return buffer;
}

/**
   Input time in epoch format format and return String with format pattern
*/
static String getEpochStringByParams(long time, char* pattern = (char *)" %H:%M:%S %d/%m/%Y") {
  tm newtime;
  newtime = getDateTimeByParams(time);
  return getDateTimeStringByParams(&newtime, pattern);
}

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  Serial.println("Mi sono svegliato");

  leggiTensione();
  Serial.println("tensione letta al setup"); //per debug
  Serial.println(voltString); //per debug


  Blynk.begin(auth, ssid, pass, IPAddress(x, y, z, w), 8080); //al posto di x, y, z, w mettere il vostro ip del server Blynk
  orologio();
}

void loop() {
  Blynk.run();
  //
  Serial.println("tensione inviata dal loop"); //per debug
  Serial.println(voltString); //per debug
  Blynk.virtualWrite(V2, voltString);

  Serial.println("data inviata nel loop"); //per debug
  //Serial.println(currentDate);  //per debug
  Blynk.virtualWrite(V3, currentDate );

  delay(250); //per connessione a blynk

  Serial.println("mi addormento");
  //ESP.deepSleep(sleepTimeS * 1000000); // nel caso volessi impostare una durata in secondi per il DeepSleep
  Serial.println("max deep sleep: " + uint64ToString(ESP.deepSleepMax())); //per debug
  ESP.deepSleep(ESP.deepSleepMax());//serve per avere la massima durata di sospensione
}


String leggiTensione() {
  unsigned int x;
  for (int i = 0; i < 100; i++) {
    raw = analogRead(A0) - 5 ; //5 è un offset trovato collegando A0 a GND
    volt = (raw + i * raw ) / (i + 1);  //calcola il valore medio per 100 acquisizioni
   yield(); //per evitare riavvi per causa del watchdog(WDT)
  }
  volt = raw /1023; 
  volt = volt / 0.07575;  // divido per rapporto di conversione trovato dalla formula del partitore

  voltString = String(volt);
  return voltString;
}


String orologio() {   // scarica l'ora del server ntp secondo il formato definito all'inizio
  timeClient.begin();
  delay ( 1000 );
  if (timeClient.update()) {
    unsigned long epoch = timeClient.getEpochTime();
    setTime(epoch);
  } else {
    Serial.print ( "aggiornamento ora fallito" );
  }
  currentDate = String (getEpochStringByParams(CE.toLocal(now())));
  Serial.println ( currentDate);
  return currentDate;
}

String uint64ToString(uint64_t input) { //permette di stampare sul monitor seriale un uint64
  String result = "";
  uint8_t base = 10;

  do {
    char c = input % base;
    input /= base;

    if (c < 10)
      c +='0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}