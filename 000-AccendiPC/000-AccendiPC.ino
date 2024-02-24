/* www.lutritech.it

  Accendere il PC da remoto senza Wake-on-LAN con la scheda Wemos D1 Lite ESP8266 e modulo relè: Guida completa
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech
  https://twitter.com/lutritech

*/

#include <ESP8266WiFi.h>
#include <Espalexa.h>

// inserisci qui le tue credenziali del WiFi
#define STASSID "NOME-TUA-RETE-WIFI"
#define STAPSK "PASSWORD-TUA-RETE-WIFI"

const int relayPin = D1;   //lo shield relay del wemos utilizza sempre questo pin

//callback functions
void firstLightChanged(uint8_t brightness);

const char* ssid = STASSID;
const char* pass = STAPSK;

// device name
String Device_1_Name = "pc";  // è il nome del dispositivo che verrà utilizzato con Alexa
Espalexa espalexa;

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define your devices here.
  espalexa.addDevice(Device_1_Name, firstLightChanged);  //simplest definition, default state off
  espalexa.begin();
  digitalWrite(relayPin, LOW); //all'accensione il contatto è aperto
}

void loop() {
  espalexa.loop();
  delay(500);
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
  //Control the device
  if (brightness) {
    Serial.println(brightness);
    if (brightness == 255) {
      Serial.println("Accendo il pc");
      digitalWrite(relayPin, HIGH);
      delay(1000); //come se tenessi premuto il pulsante per un secondo
      digitalWrite(relayPin, LOW);
    }

  } else {
    Serial.println(brightness);
    Serial.println("Spengo il pc");
    digitalWrite(relayPin, HIGH);
    delay(5000); //come se tenessi premuto il pulsante per 5 secondi
    digitalWrite(relayPin, LOW);
  }
}
