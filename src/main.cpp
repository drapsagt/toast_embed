#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_AHTX0.h>

// WiFi credentials

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);

// MQTT Topics
const char *topic_temperature = "sensors/temperature";
const char *topic_humidity = "sensors/humidity";
const char *topic_status = "device/status";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_AHTX0 aht;

// Variables for sensor readings
float temperature = 0.0;
float humidity = 0.0;
unsigned long lastMsg = 0;
const unsigned long MSG_INTERVAL = 5000; // Send data every 5 seconds

void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message;
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Handle incoming messages here
  // Example: if message on "device/command" topic is "restart", restart ESP32
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(client_id, mqtt_username, mqtt_password))
    {
      Serial.println("connected");

      // Once connected, publish an announcement
      client.publish(topic_status, "ESP32 Connected");

      // Subscribe to command topic
      client.subscribe("device/command");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishSensorData()
{
  // Read data from AHT20 sensor
  sensors_event_t humidity_event, temp_event;
  aht.getEvent(&humidity_event, &temp_event);

  temperature = temp_event.temperature;
  humidity = humidity_event.relative_humidity;

  // Check if readings are valid
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from AHT20 sensor!");
    return;
  }

  // Convert to strings
  char tempString[8];
  char humString[8];
  dtostrf(temperature, 1, 2, tempString);
  dtostrf(humidity, 1, 2, humString);

  // Publish the data
  Serial.print("Publishing temperature: ");
  Serial.print(tempString);
  Serial.println("°C");
  client.publish(topic_temperature, tempString);

  Serial.print("Publishing humidity: ");
  Serial.print(humString);
  Serial.println("%");
  client.publish(topic_humidity, humString);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting ESP32 MQTT Client");

  // Initialize AHT20 sensor
  if (!aht.begin())
  {
    Serial.println("Could not find AHT? Check wiring");
    while (1)
      delay(10);
  }
  Serial.println("AHT20 sensor found");

  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // Initialize random seed
  randomSeed(analogRead(0));
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > MSG_INTERVAL)
  {
    lastMsg = now;
    publishSensorData();
  }
}