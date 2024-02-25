/* www.lutritech.it

  Robot Car comandato via Rete
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech
  https://twitter.com/lutritech

*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include "bender.h"              //contiene il codice per animare il display con bender
#include <Servo.h>

#define HTTP_WEB_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50
#define MOTORE_SX_PIN1 D0
#define MOTORE_SX_PIN2 D1
#define MOTORE_DX_PIN1 D2
#define MOTORE_DX_PIN2 D3
#define PINZA_PIN D4  //D4 PER GESTIONE PINZA ROBOT
#define AVANTI 1
#define INDIETRO 0
#define BUZZERPIN D5 //D5 è IN PIN PER IL CLACSON


//configurazine Blynk
char auth[] = "Mettete il vostro token di Blynk";

int posizionePinza = 0;

IPAddress local_IP(192, 168, 10, 10); //192.168.10.10 E' L'INDIRIZZO CHE SI ASSEGNA QUANDO VA IN MODALITà DI CONFIG
IPAddress gateway(192, 168, 10, 10);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer http_server(HTTP_WEB_PORT);

Servo myservo;  // oggetto myservo

void setup(void) {
  Serial.begin(115200);

  pinMode(MOTORE_SX_PIN1, OUTPUT);
  pinMode(MOTORE_SX_PIN2, OUTPUT);
  pinMode(MOTORE_DX_PIN1, OUTPUT);
  pinMode(MOTORE_DX_PIN2, OUTPUT);

  pinMode(BUZZERPIN, OUTPUT); //PIN PER IL CLACSON

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();    //PUò SERVIRE IN CASO DI TEST

  //set custom ip for portal
  wifiManager.setAPStaticIPConfig(local_IP, gateway, subnet);


  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("RobotCar-01");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  //MOSTRO IL MAC ADDRESS DELLA SCHEDA ARDUINO
  Serial.printf("MAC address = %s\n", WiFi.softAPmacAddress().c_str());
  Serial.println(WiFi.hostname());

  Blynk.config(auth);


  config_server_routing();
  http_server.begin();
  Serial.println("RoboCar HTTP Server Started");
  robocar_pinza_aperta();

}

void config_server_routing() {
  http_server.on("/", HTTP_GET, []() {
    http_server.send(200, "text/html", "Benvenuto sul ESP8266 RoboCar Web Server");
  });
  http_server.on("/robocar", HTTP_GET, muovi_robocar);
}

void muovi_robocar() {
  String comando = String(http_server.arg(0));
  Serial.print("Ricevuto comando = ");
  Serial.println(comando);

  http_server.send(200, "text/html", "robocar - comando ricevuto");

  if (comando == "0")
    robocar_fermo();
  else if (comando == "1")
    robocar_avanti();
  else if (comando == "2")
    robocar_indietro();
  else if (comando == "4")
    robocar_sinistra();
  else if (comando == "3")
    robocar_destra();
  else if (comando == "5")
    robocar_clacson();
  else if (comando == "6")
    robocar_gira_antiorario();
  else if (comando == "7")
    robocar_gira_orario();
  else if (comando == "8")
    robocar_pinza_aperta();
  else if (comando == "9")
    robocar_pinza_chiusa();

  else Serial.println("Comando non riconosciuto");
}

void motore_sx_avanti() {
  Serial.println("motore_sx avanti");

  digitalWrite(MOTORE_SX_PIN1, HIGH);
  digitalWrite(MOTORE_SX_PIN2, LOW);
}

void motore_sx_indietro() {
  Serial.println("motore_sx indietro");

  digitalWrite(MOTORE_SX_PIN1, LOW);
  digitalWrite(MOTORE_SX_PIN2, HIGH);
}

void motore_sx_fermo() {
  Serial.println("motore_sx fermo");
  digitalWrite(MOTORE_SX_PIN1, LOW);
  digitalWrite(MOTORE_SX_PIN2, LOW);
}

void motore_dx_avanti() {
  Serial.println("motore_dx avanti");
  digitalWrite(MOTORE_DX_PIN1, HIGH);
  digitalWrite(MOTORE_DX_PIN2, LOW);
}

void motore_dx_indietro() {
  Serial.println("motore_dx indietro");
  digitalWrite(MOTORE_DX_PIN1, LOW);
  digitalWrite(MOTORE_DX_PIN2, HIGH);
}

void motore_dx_fermo() {
  Serial.println("motore_dx fermo");
  digitalWrite(MOTORE_DX_PIN1, LOW);
  digitalWrite(MOTORE_DX_PIN2, LOW);
}

void robocar_avanti() {
  Serial.println("vado avanti");
  motore_sx_avanti();
  motore_dx_avanti();
}

void robocar_indietro() {
  Serial.println("vado indietro");
  motore_sx_indietro();
  motore_dx_indietro();
}

void robocar_destra() {
  Serial.println("giro a sinistra");
  motore_dx_avanti();
}

void robocar_sinistra() {
  Serial.println("giro a destra");
  motore_sx_avanti();
}

void robocar_fermo() {
  motore_sx_fermo();
  motore_dx_fermo();
}

void robocar_gira_orario() {
  Serial.println("giro ORARIO");
  motore_dx_avanti();
  motore_sx_indietro();
}

void robocar_gira_antiorario() {
  Serial.println("giro ANTIORARIO");
  motore_dx_indietro();
  motore_sx_avanti();
}

void robocar_clacson() {
  tone(BUZZERPIN, 440, 1000);
  delay(500);
}

BLYNK_WRITE(V4)// leggo l'indirizzo di chi sta usando l'applicazione
{
  String pinValue = param.asStr();
  Serial.print("Coordinate GPS: ");
  Serial.println(pinValue);
}


BLYNK_WRITE(V5)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V5 to a variable

  switch (pinValue)
  {
    case 0: {  //fermo

        Serial.println("fermo");
        robocar_fermo();
        break;
      }

    case 1: {  //avanti

        Serial.println("avanti");
        robocar_avanti();
        break;
      }


    case 2: {   //indietro

        Serial.println("indietro");
        robocar_indietro();
        break;
      }

    case 3: {   //sx

        Serial.println("sx");
        robocar_destra();
        break;
      }

    case 4: {   //dx

        Serial.println("dx");
        robocar_sinistra();
        break;
      }

    case 5: {   //clacson

        Serial.println("clacson");
        robocar_clacson();
        break;
      }
    case 6: {   //antioriario

        Serial.println("antioriario");
        robocar_gira_antiorario();
        break;
      }

    case 7: {   //oriario

        Serial.println("oriario");
        robocar_gira_orario();
        break;
      }


    case 8: {  //pinza aperta
        Serial.println("apro pinza");
        robocar_pinza_aperta();
        break;
      }



    case 9: {   //pinza chiusa
        Serial.println("chiudo pinza");
        robocar_pinza_chiusa();
        break;
      }


    default:
      Serial.println("altro comando");
      break;
  }
}

void robocar_pinza_aperta() {
  myservo.attach(PINZA_PIN);
  delay(300);
  myservo.write(5);
  delay(300);
  myservo.detach();
}

void robocar_pinza_chiusa() {
  myservo.attach(PINZA_PIN);
  delay(300);
  myservo.write(170);
  delay(300);
  myservo.detach();
}

void loop(void) {
  Blynk.run();
  http_server.handleClient();
}

