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

/*
 *      Definitions in platformio.ini
 *      DHTPIN      GPIO where DHTxx data is connected
 *      DHTTYPE     Constant defining sensor type: DHT11 or DHT22
 *      SERIAL_BAUD Serial port Baud rate
 */

DHT dht(DHTPIN, DHTTYPE);   // New DHT type object

const int LED_PORT = 13;

static void activate_ultimate_watering_system() {
    digitalWrite(13, HIGH);
    delay(3000);
    digitalWrite(13, LOW);
}

void
setup(void)
{
    pinMode(13,OUTPUT);

    Serial.begin(115200);
    Serial.printf( "\n\r\nDHT%d test!\n\r", DHTTYPE);
    dht.begin();
}

void
loop()
{
    float h, t, f;

    delay(2000);                    // It is a low speed sensor !!

    activate_ultimate_watering_system();

    h = dht.readHumidity();
    t = dht.readTemperature();     // in Celsius
    f = dht.readTemperature(true); // in Fahrenheit

    Serial.printf("Measurement failure");


    if (isnan(h) || isnan(t) || isnan(f))   //    Failed measurement ?
        Serial.printf("Measurement failure");
    else
        Serial.printf( "DHT%d -> Humedad: %5.1f %%, Temperatura: %5.1f C | %5.1f F\n\r", DHTTYPE, h, t, f );
}


