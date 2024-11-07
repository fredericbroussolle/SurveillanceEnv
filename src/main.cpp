
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configuration WiFi
const char* ssid = "xxxxxxxxx";          // Remplacez par le nom de votre réseau WiFi
const char* password = "xxxxxxxxx"; // Remplacez par votre mot de passe WiFi

// Configuration du broker MQTT
const char* mqtt_server = "xxxxxxxxxxx"; // Remplacez par l'adresse IP de votre broker MQTT
const int mqtt_port = 1883;
const char* topicTemp = "xxxxxxxxx";          // Remplacez par le nom de votre réseau WiFi
const char* topicHum = "xxxxxxxxx"; // Remplacez par votre mot de passe WiFi

// Configuration du capteur DHT11
#define DHTPIN xxxxxxx       // Broche où le Data du DHT22 est connecté
#define DHTTYPE xxxxxxx    // Type du capteur (DHT22/DHT11)
DHT dht(DHTPIN, DHTTYPE);

// Création des objets WiFi et MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();              // Initialisation du capteur DHT
  setup_wifi();             // Connexion au WiFi
  client.setServer(mqtt_server, mqtt_port);  // Configuration du broker MQTT
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion au réseau WiFi : ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connecté au WiFi");
}

void reconnect() {
  // Boucle jusqu'à ce que la connexion au broker MQTT soit établie
  while (!client.connected()) {
    Serial.print("Connexion au broker MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connecté");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" ; nouvelle tentative dans 5 secondes");
      delay(5000);
    }
  }
}

void loop() {
  // Connexion au broker MQTT si déconnecté
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lecture des valeurs de température et d'humidité
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Vérification si les valeurs sont valides
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Erreur de lecture du capteur DHT !");
    return;
  }

  // Conversion des valeurs en chaîne de caractères
  String temperature_str = String(temperature);
  String humidity_str = String(humidity);

  // Publication des données sur les topics MQTT
  client.publish(topicTemp, temperature_str.c_str());
  client.publish(topicHum, humidity_str.c_str());

  // Affichage des valeurs dans le moniteur série
  Serial.print("Température : ");
  Serial.print(temperature_str);
  Serial.println(" °C");

  Serial.print("Humidité : ");
  Serial.print(humidity_str);
  Serial.println(" %");

  delay(20000);  // Attente de 2 secondes avant la prochaine lecture
}
