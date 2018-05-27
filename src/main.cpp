/*
 *******************************************************************************
 * Project URL: https://github.com/gotbadger/iot-button
 *
 *******************************************************************************
 * Copyright Phil Hayton 2018.

 * Distributed under the MIT License.
 * (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
 *******************************************************************************
 */

// set details here
#define NET "network_name"
#define PASS "password"
#define MQTT_ID "ESP_BUTTON_1"
#define MQTT_SERVER "192.168.1.165"

// Include library  
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//#define DEBUG
#define HW_UART_SPEED 9600L
#define LOG_PRINTFLN(fmt, ...) logfln(fmt, ##__VA_ARGS__)
#define LOG_SIZE_MAX 128
void logfln(const char *fmt, ...)
{
    char buf[LOG_SIZE_MAX];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, LOG_SIZE_MAX, fmt, ap);
    va_end(ap);
    #ifdef DEBUG
    Serial.println(buf);
    #endif
}

// We want to measure power
ADC_MODE(ADC_VCC);

WiFiClient espClient;
PubSubClient client(espClient);

void send_message()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        // Attempt to connect
        if (client.connect(MQTT_ID))
        {
            uint16_t v = ESP.getVcc();
            float_t v_cal = ((float)v/1024.0f);
            char v_str[10];
            dtostrf(v_cal, 5, 3, v_str);
            client.publish(MQTT_ID "/button/0", "1");
            client.publish(MQTT_ID "/vcc", v_str);
        }
    }
}

void setup_wifi()
{
    // wifi
    WiFi.mode(WIFI_STA);
    WiFi.hostname(MQTT_ID);
    WiFi.begin(NET, PASS);
    LOG_PRINTFLN("\n");
    LOG_PRINTFLN("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        LOG_PRINTFLN(".");
    }
    LOG_PRINTFLN("Connected to WiFi");
    LOG_PRINTFLN("IP: %s", WiFi.localIP().toString().c_str());
}

// callback for mqtt
void callback(char *topic, byte *payload, unsigned int length)
{
    LOG_PRINTFLN("callback on %s", topic);
}

void setup() {
    #ifdef DEBUG
    // serial
    Serial.begin(HW_UART_SPEED);
    while (!Serial)
        ;
    #endif
    setup_wifi();
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
}

void loop(){
    send_message();
    delay(1000);
    ESP.deepSleep(0);
}