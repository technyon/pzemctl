#include "EepromConfiguration.h"
#include <EEPROM.h>
#include <string.h>
#include <Arduino.h>

EepromConfiguration::EepromConfiguration(void (*callback)())
: _callback(callback)
{
    initialize();

    Serial.println(mqttServerAddress);
    Serial.println(dnsServerAddress);
    Serial.println(gatewayAddress);
    Serial.println(ipAddress);
    Serial.println(subnetMask);
}

void EepromConfiguration::initialize()
{
    for(int i=0; i < sizeof(mqttServerAddress); i++)
    {
        mqttServerAddress[i] = 0;
    }
    for(int i=0; i < sizeof(dnsServerAddress); i++)
    {
        dnsServerAddress[i] = 0;
    }
    for(int i=0; i < sizeof(gatewayAddress); i++)
    {
        gatewayAddress[i] = 0;
    }
    for(int i=0; i < sizeof(ipAddress); i++)
    {
        ipAddress[i] = 0;
    }
    for(int i=0; i < sizeof(subnetMask); i++)
    {
        subnetMask[i] = 0;
    }
    for(int i=0; i < sizeof(mac); i++)
    {
        mac[i] = 0;
    }
    for(int i=0; i < sizeof(mqttPublishInterval); i++)
    {
        mqttPublishInterval[i] = 0;
    }

    readEeprom();
}


void EepromConfiguration::readEeprom()
{
    if(!hasValidSignature())
    {
        Serial.println(F("No valid signature found in eeprom."));
        generateMacAddress();
        return;
    }

    for(int i=0; i < sizeof(mqttServerAddress); i++)
    {
        mqttServerAddress[i] = EEPROM[MQTT_EEPROM_OFFSET + i];
    }
    for(int i=0; i < sizeof(dnsServerAddress); i++)
    {
        dnsServerAddress[i] = EEPROM[DNS_EEPROM_OFFSET + i];
    }
    for(int i=0; i < sizeof(gatewayAddress); i++)
    {
        gatewayAddress[i] = EEPROM[GATEWAY_EEPROM_OFFSET + i];
    }
    for(int i=0; i < sizeof(ipAddress); i++)
    {
        ipAddress[i] = EEPROM[IP_EEPROM_OFFSET + i];
    }
    for(int i=0; i < sizeof(subnetMask); i++)
    {
        subnetMask[i] = EEPROM[SUBNET_MASK_OFFSET + i];
    }
    for(int i=0; i < sizeof(mac); i++)
    {
        mac[i] = EEPROM[MAC_OFFSET + i];
    }
    for(int i=0; i < sizeof(mqttPublishInterval); i++)
    {
        mqttPublishInterval[i] = EEPROM[MQTT_PUBLISH_INTERVAL + i];
    }
    parseMqttPublishInterval();
}

void EepromConfiguration::writeEeprom()
{
    Serial.println(F("Write EEPROM"));

    Serial.println(mqttServerAddress);
    Serial.println(dnsServerAddress);
    Serial.println(gatewayAddress);
    Serial.println(ipAddress);
    Serial.println(subnetMask);

    Serial.println(F(" --"));

    if(!hasValidSignature())
    {
        writeSignature();
    }

    for(int i=0; i < sizeof(mqttServerAddress); i++)
    {
        EEPROM.update(MQTT_EEPROM_OFFSET + i, mqttServerAddress[i]);
    }
    for(int i=0; i < sizeof(dnsServerAddress); i++)
    {
        EEPROM.update(DNS_EEPROM_OFFSET + i, dnsServerAddress[i]);
    }
    for(int i=0; i < sizeof(gatewayAddress); i++)
    {
        EEPROM.update(GATEWAY_EEPROM_OFFSET + i, gatewayAddress[i]);
    }
    for(int i=0; i < sizeof(ipAddress); i++)
    {
        EEPROM.update(IP_EEPROM_OFFSET + i, ipAddress[i]);
    }
    for(int i=0; i < sizeof(subnetMask); i++)
    {
        EEPROM.update(SUBNET_MASK_OFFSET + i, subnetMask[i]);
    }
    for(int i=0; i < sizeof(mac); i++)
    {
        EEPROM.update(MAC_OFFSET + i, mac[i]);
    }
    for(int i=0; i < sizeof(mqttPublishInterval); i++)
    {
        EEPROM.update(MQTT_PUBLISH_INTERVAL + i, mqttPublishInterval[i]);
    }

    parseMqttPublishInterval();

    _callback();
}

void EepromConfiguration::writeSignature()
{
    char sig[] = "pzem004t";
    for(int i=0; i < 9; i++)
    {
        EEPROM.update(i, sig[i]);
    }
}

bool EepromConfiguration::hasValidSignature()
{
    char sig[9];
    for(int i=0; i<9; i++)
    {
        sig[i] = EEPROM[i];
    }

    return strcmp(sig, "pzem004t") == 0;
}

void EepromConfiguration::generateMacAddress()
{
    mac[0] = 0x00;  // wiznet prefix
    mac[1] = 0x08;  // wiznet prefix
    mac[2] = 0xDC;  // wiznet prefix
    mac[3] = random(0,255);
    mac[4] = random(0,255);
    mac[5] = random(0,255);
}

void EepromConfiguration::clearEeprom()
{
    for (int i = 0 ; i < EEPROM.length() ; i++)
    {
        EEPROM.update(i, 0);
    }
}

void EepromConfiguration::parseMqttPublishInterval()
{
    _numericMqttPublishInterval = max(atoi(mqttPublishInterval), 200);
}
