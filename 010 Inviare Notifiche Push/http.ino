/*https://www.lutritech.it/inviare-notifiche-push/
Inviare notifiche Push

Come inviare notifiche push gratis - versione http 
  
  https://www.youtube.com/@Lutritech
  https://www.facebook.com/lutritech.it
  https://www.instagram.com/Lutritech
  https://github.com/Lutritech

*/
#include <WiFi.h>
#include <HTTPClient.h>

// Inserisci il nome della tua rete WiFi e la password
const char* ssid = "Wifi";
const char* password = "wifi";

// Inserisci il tuo token API di Pushover e l'utente
const char* pushover_token = "abbrbd76ngbqs1c9ejf1rzkcu1faqc";
const char* pushover_user = "hgfgjhgwdjhgfweity92445";

void setup() {
  Serial.begin(115200);

  // Connessione alla rete WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connessione alla rete WiFi...");
  }
  Serial.println("Connesso alla rete WiFi!");

  // Invia una notifica push
  sendPushoverNotification("Titolo Notifica", "Messaggio della Notifica HTML");
}

void loop() {
  // Aggiungi qui il codice per il loop principale
}

void sendPushoverNotification(const char* title, const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.pushover.net/1/messages.json");

    // Aggiungi l'header per indicare che stiamo inviando dati di tipo application/x-www-form-urlencoded
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Costruisci il messaggio HTML
    String body = "token=" + String(pushover_token) + "&user=" + String(pushover_user) + "&title=" + String(title) + "&message=" + String(message) + "&html=1";

    // Invia la richiesta HTTP POST
    int httpResponseCode = http.POST(body);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Errore nella richiesta: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi non connesso");
  }
}
