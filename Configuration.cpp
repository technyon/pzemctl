#include "Configuration.h"
#include <EEPROM.h>
#include <string.h>
#include <Arduino.h>

Configuration::Configuration()
{
    initialize();

    Serial.println(mqttServerAddress);
    Serial.println(dnsServerAddress);
    Serial.println(gatewayAddress);
    Serial.println(ipAddress);
    Serial.println(subnetMask);
}

void Configuration::initialize()
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

    readEeprom();
}


void Configuration::readEeprom()
{
    if(!hasValidSignature())
    {
        Serial.println(F("No valid signature found in eeprom."));
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
}

void Configuration::writeEeprom()
{
    Serial.println(F("Write EEPROM"));

    Serial.println(mqttServerAddress);
    Serial.println(dnsServerAddress);
    Serial.println(gatewayAddress);
    Serial.println(ipAddress);
    Serial.println(subnetMask);

    Serial.println(F(" --"));

    writeSignature();

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
}

void Configuration::writeSignature()
{
    char sig[] = "pzem004t";
    for(int i=0; i < 9; i++)
    {
        EEPROM.update(i, sig[i]);
    }
}

bool Configuration::hasValidSignature()
{
    char sig[9];
    for(int i=0; i<9; i++)
    {
        sig[i] = EEPROM[i];
    }

    return strcmp(sig, "pzem004t") == 0;
}
