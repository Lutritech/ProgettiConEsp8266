/* www.lutritech.it
Inviare notifiche Push

Come inviare notifiche push gratis - versione https
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech

*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// Inserisci il nome della tua rete WiFi e la password
const char* ssid = "Wifi";
const char* password = "wifi";

// Inserisci il tuo token API di Pushover e l'utente
const char* pushover_token = "abbrbd76ngbqs1c9ejf1rzkcu1faqc";
const char* pushover_user = "hgfgjhgwdjhgfweity92445";

//Pushover API endpoint
const char* pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";

//Certificato di Pushover (valido fino al 15/01/2038)
const char *PUSHOVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
                  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
                  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
                  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
                  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
                  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
                  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
                  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
                  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
                  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
                  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
                  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
                  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
                  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
                  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
                  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
                  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
                  "MrY=\n"
                  "-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione alla rete WiFi...");
  }
  Serial.println("Connesso alla rete WiFi!");

  //HTTPS POST
  if (WiFi.status() == WL_CONNECTED) {
    // Creazione del JSON con le informazioni richieste
    StaticJsonDocument<512> notification; 
    notification["token"] = pushover_token; //obbligatorio
    notification["user"] = pushover_user; //obbligatorio
    notification["message"] = "Messaggio della Notifica in HTTPS"; //obbligatorio
    notification["title"] = "Titolo Notifica Sicura"; //opzionale
    notification["url"] = ""; //opzionale
    notification["url_title"] = ""; //opzionale
    notification["html"] = ""; //opzionale
    notification["priority"] = ""; //opzionale
    notification["sound"] = "cosmic"; //opzionale
    notification["timestamp"] = ""; //opzionale

    // Converte il JSON in una stringa
    String jsonStringNotification;
    serializeJson(notification, jsonStringNotification);

    // Crea un oggetto WiFiClientSecure object
    WiFiClientSecure client;
    // imposta il certificato
    client.setCACert(PUSHOVER_ROOT_CA);

    // Crea un oggetto HTTPClient
    HTTPClient https;

    // Specifica l'endpoint
    https.begin(client, pushoverApiEndpoint);

    // Aggiunge gli headers
    https.addHeader("Content-Type", "application/json");

    // Manda la richiesta POST con i dati JSON
    int httpResponseCode = https.POST(jsonStringNotification);

    // Check the response
    if (httpResponseCode > 0) {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
      String response = https.getString();
      Serial.println("Response:");
      Serial.println(response);
    } else {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
    }

    // Chiudi la connessione
    https.end();
  }
}

void loop() {
  // Aggiungi qui il codice per il loop principale
}