/* www.lutritech.it

  Sensore di Temperatura WIFI con sistema di notifica tramite invio email tramite SMTP e messaggio tramite BOT su Telegram
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech
  https://twitter.com/lutritech

*/

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> //  https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include <OneWire.h>    //	https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#define HTTP_WEB_PORT 80

//per invio email
WiFiClient espClient;
char server[] = "mail.smtp2go.com";   // SMTP Server esterno FREE
#define USERNAME "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" // Lo Username impostato su smtp2go.com e codificato in BASE64 su https://www.base64encode.org/
#define PASSWORD "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" // La password impostata su smtp2go.com e codificata in BASE64 su https://www.base64encode.org/

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
OneWire  ds(D2);  // on pin D4 (a 4.7K resistor is necessary)

ESP8266WebServer http_server(HTTP_WEB_PORT);
String temp = "24"; //default temperatura rilevata
String ip = "x.x.x.x"; //default ip
int isterisi = 0;
int soglia = 35; //default soglia massima della temperatura.
int attesa = 10; //con la soglia massima superata è il tempo che deve passare per l'invio della mail 600 sono circa 10 minuti
int offsetTemp = 7.5; // offset temperatura

// Initialize Telegram BOT
#define BOT_TOKEN "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)
// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "XXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages
unsigned long bot_lasttime; // last time messages' scan has been done
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

//variabile per il riavvio della scheda Wemos
bool RestartTriggered = false;

void setup(void)
{
  Serial.begin(9600);

  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  ip = WiFi.localIP().toString();
  //  Serial.println("Debug ip:");
  //  Serial.println(ip);

  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  bot.sendMessage(CHAT_ID, "Ciao mi sono avviato");
  bot.sendMessage(CHAT_ID, "Ecco la pagina web da visitare: ");
  bot.sendMessage(CHAT_ID, "http://" + ip + "/temp");

  Serial.println("Ecco i comandi dal Browser:");
  Serial.println("http://" + ip + "/temp");


  config_server_routing();
  http_server.begin();

  Serial.println(bot_lasttime);
}

void loop(void)
{
  http_server.handleClient();
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr))
  {
    ds.reset_search();
    delay(250);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7])
  {
    Serial.println("CRC is not valid!");
    return;
  }

  // the first ROM byte indicates which chip
  switch (addr[0])
  {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  delay(1000);
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {
    data[i] = ds.read();
  }

  // Convert the data to actual temperature
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10)
    {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else
  {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms

  }
  celsius = ((float)raw / 16.0) - offsetTemp; // tolgo un offset dalla temp rilevata
  // celsius = ((float)raw / 16.0);
  fahrenheit = celsius * 1.8 + 32.0;
  //Serial.println(celsius);
  temp = String(celsius, 1);
  Serial.println(temp); //per debug
  if (celsius > soglia)
  {
    Serial.println("Fa caldo! Soglia Superata ");
    isterisi++;
    Serial.println(isterisi);

    if (isterisi > attesa)
    {
      Serial.println(isterisi);
      sendEmail(temp, soglia);
      bot.sendMessage(CHAT_ID, "ATTENZIONE SOGLIA SUPERATA!\n La temperatura è di:" + temp, "");
      isterisi = 0; //azzero l'isterisi....manderà una nuova mail raggiunta nuovamente la soglia e l'attesa
    }
  } else isterisi = 0; //quando la temperatura torna sotto soglia azzero l'isterisi


  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

 if (RestartTriggered == true) {ESP.restart();}

}


byte sendEmail(String msg, int sogliaVal)

{
  if (espClient.connect(server, 25) == 1)
  {
    Serial.println(F("connected"));
  }

  else

  {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!emailResp())
    return 0;

  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp())
    return 0;

  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp())
    return 0;
  Serial.println(F("Sending User"));

  espClient.println(USERNAME); // Lo Username impostato su smtp2go.com e codificato su codificato in BASE64 su https://www.base64encode.org/
  if (!emailResp())
    return 0;
  Serial.println(F("Sending Password"));
  espClient.println(PASSWORD);          //Password impostata su smtp2go.com e codificato su codificato in BASE64 su https://www.base64encode.org/
  if (!emailResp())
    return 0;
  Serial.println(F("Sending From"));


  Serial.println(F("Sending From"));

  espClient.println(F("MAIL From: xyz@gmail.com")); // Enter Sender Mail Id
  if (!emailResp())
    return 0;

  Serial.println(F("Sending To"));
  espClient.println(F("RCPT TO: redazione@lutritech.it")); // Enter Receiver Mail Id
  if (!emailResp())
    return 0;

  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp())
    return 0;

  Serial.println(F("Sending email"));
  espClient.println(F("To:  redazione@lutritech.it")); // Enter Receiver Mail Id

  // change to your address

  espClient.println(F("From: ControlloTemperatura@lutritech.it")); // E' l'indirizzo che si vedrà nel campo destinatario della mail ricevuta

  espClient.println(F("Subject: ATTENZIONE SOGLIA TEMPERATURA SUPERATA!"));

  espClient.println(F("Temperatura Rilevata: "));

  espClient.println((msg));

  espClient.println(F("Soglia Impostata: "));

  espClient.println((sogliaVal));

  espClient.println(F("."));
  if (!emailResp())
    return 0;


  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp())  return 0;

  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;

}



byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available())
  {
    delay(1);
    loopCount++;
    if (loopCount > 20000)
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4') return 0;

  return 1;
}



void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("CONFIG MODE");
  Serial.println("VISIT");
  Serial.println("http://" + ip);
  delay (2000);

  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

////web server/////

void config_server_routing() {
  http_server.on("/temp", HTTP_GET, handleRoot );
  http_server.on("/login", HTTP_POST, handleLogin );
  http_server.on("/soglia", HTTP_POST, handleSoglia );
}


void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  http_server.send(200, "text/html", "<HTML><HEAD><TITLE>Controllo temperatura</TITLE></HEAD><BODY><CENTER><h1>TEMPERATURA : " + temp +
                   "</h1><h2>soglia impostata a: " + soglia + "<br><form action=\"/login\" method=\"POST\"><input type=\"text\" name=\"username\" placeholder=\"Username\"></br><input type=\"password\" name=\"password\" placeholder=\"Password\"></br><input type=\"submit\" value=\"Login\"></form>" +
                   "</h2></CENTER></BODY></HTML>");
}

void handleLogin() {                         // If a POST request is made to URI /login
  if ( ! http_server.hasArg("username") || ! http_server.hasArg("password")
       || http_server.arg("username") == NULL || http_server.arg("password") == NULL) { // If the POST request doesn't have username and password data
    http_server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  if (http_server.arg("username") == "Admin!" && http_server.arg("password") == "Temp!") { // If both the username and the password are correct
    http_server.send(200, "text/html", "<h1>Welcome, " + http_server.arg("username") + "!</h1><p>Login successful</p>" +
                     "<form action=\"/soglia\" method=\"POST\"><input type=\"number\" name=\"soglia\" placeholder=\"Soglia\" value=\"25\"></br><input type=\"submit\" value=\"Soglia\"></form>"
                    );
  } else {                                                                              // Username and password don't match
    http_server.send(401, "text/plain", "401: Unauthorized");
  }
}

void handleSoglia() {
  String sogl = "";
  sogl = http_server.arg("soglia");
  soglia = sogl.toInt();
  Serial.println("La soglia ora è impostata a : "); //per debug
  Serial.println(soglia); //per debug
  http_server.send(200, "text/html", "<h1>Soglia Configurata</h1>");

}

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  String answer;
  for (int i = 0; i < numNewMessages; i++)
  {
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
    if (msg.text == "/help")
      answer = "Ciao Sono LutriBot usa i comandi \n/mostra \n/soglia";
    else if (msg.text == "/mostra")
      answer =  msg.from_name + " la temperatura è: " + temp;
    else if (msg.text == "/soglia")
      answer =  msg.from_name + " la soglia impostata è: " + soglia;
     else if (msg.text == "/riavvia") {
      Serial.println("mi sto riavviando");
      answer = " Mi sto per Riavviare";
      RestartTriggered = true;
    }
    else
      answer = msg.text + " Non lo capisco";

    bot.sendMessage(msg.chat_id, answer, "Markdown");
  }
}

void bot_setup()
{
  const String commands = F("["
                            "{\"command\":\"help\",  \"description\":\"Lutri ti aiuta\"},"
                            "{\"command\":\"soglia\",  \"description\":\"Vedi soglia impostata\"},"
                            "{\"command\":\"mostra\", \"description\":\"Vedi temperatura\"},"
                            "{\"command\":\"riavvia\",  \"description\":\"Riavvialo\"}"
                            "]");
  bot.setMyCommands(commands);
}