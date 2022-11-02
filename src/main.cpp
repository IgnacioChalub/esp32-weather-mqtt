/*
 *     Project 07-dht - main.cpp
 *
 *     Austral 2022 - IntroComp - EAM 
 *
 *   Temperature and humidity measurement using DHT-XX sensor
 *
 *         Requieres following libraries
 *         - DHT Sensor Library
 *         - Adafruit Unified Sensor Library
 */

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

/*
 *      Definitions in platformio.ini
 *      DHTPIN      GPIO where DHTxx data is connected
 *      DHTTYPE     Constant defining sensor type: DHT11 or DHT22
 *      SERIAL_BAUD Serial port Baud rate
 */

DHT dht(DHTPIN, DHTTYPE);   // New DHT type object

const int LED_PORT = 13;
const char* ssid = "UA-Alumnos";
const char* password = "41umn05WLC";
const char* MQTT_NAME = "BULAHC";
const char* mqttUser = "/";
const char* mqttPassword = "/";
const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;


static char client_name[20];


static void activate_ultimate_watering_system() {
    digitalWrite(2, LOW);
    delay(3000);
    digitalWrite(2, HIGH);
}


//message recived from server
static void callback(char *topic, byte *payload, unsigned int length) {
    payload[length] = 0;
    Serial.printf("For topic: \"%s\", message \"%s\"\n\r", topic, payload );
    activate_ultimate_watering_system();
}

//  do_subscribe: subscription to a topic

static void do_subscribe( const char *ptopic ) {
    client.subscribe( ptopic );
}

//  do_connect(): called to connect or reconnect to MQTT broker

static void do_connect(void) {

    while (!client.connected())
    {
        Serial.println("Connecting to MQTT...");

        if (client.connect( client_name, mqttUser, mqttPassword))
            Serial.printf( "client %s connected\n\r", client_name );
        else
        {
            Serial.print("MQTT connection: failed with state ");
            Serial.println(client.state());
            delay(5000);
        }
    }
    do_subscribe("environment-data-micro");
    Serial.printf( "Connected to %s\n\r", mqttServer );

}


/*
 *  Public functions
 */

//  init_mqtt: connection to an MQTT server

void init_mqtt(void) {
    // sprintf(client_name,"%s%u",MQTT_NAME,get_board_number());
    //Serial.printf("client name = %s\n\r", client_name);
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    do_connect();
}




//  mqtt_comms: manage communications with MQTT server

void
mqtt_comms(void)
{
    if( !client.connected() )
        do_connect();
    client.loop();
}

//  do_publish: publish message as MAIN_TOPIC/SUB_TOPIC+BoardNumber/*ptopic

void
do_publish( const char *ptopic, const char *message )
{
    // char topic[50];

    // sprintf( topic, "%s/%s%u/%s", MAIN_TOPIC, SUB_TOPIC, get_board_number(), ptopic );
    client.publish( ptopic, message );
    Serial.printf( "%s: %s %s\n\r", __FUNCTION__, ptopic, message );
}

void
setup(void)
{
    pinMode(13,OUTPUT);
    pinMode(2,OUTPUT);
    digitalWrite(2,HIGH);

    Serial.begin(115200);
    Serial.printf( "\n\r\nDHT%d test!\n\r", DHTTYPE);
    dht.begin();

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.printf(".");
        delay(100);
    }

    Serial.printf("Connected to the WiFi network");
    Serial.printf("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    init_mqtt();
}

void
loop()
{
    

    float h, t, f;

    delay(5000);                    // It is a low speed sensor !!
    
    //do_publish("environment-data", "some data"); 

    // digitalWrite(13,HIGH);

    h = dht.readHumidity();
    t = dht.readTemperature();     // in Celsius
    f = dht.readTemperature(true); // in Fahrenheit

    if (isnan(h) || isnan(t) || isnan(f))   //    Failed measurement ?
        Serial.printf("Measurement failure");
    else
        do_publish("environment-data-server", "some data"); 
        Serial.printf( "DHT%d -> Humedad: %5.1f %%, Temperatura: %5.1f C | %5.1f F\n\r", DHTTYPE, h, t, f );

    mqtt_comms();
}


