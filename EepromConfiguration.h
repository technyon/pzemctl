#pragma once

#include <stdint.h>

class EepromConfiguration
{
public:
    EepromConfiguration(void (*callback)());
    virtual ~EepromConfiguration() = default;

    char mqttServerAddress[41] = "";
    char dnsServerAddress[17] = "";
    char gatewayAddress[17] = "";
    char ipAddress[17] = "192.168.168.168";
    char subnetMask[17] = "255.255.255.0";
    uint8_t mac[6];
    char mqttPublishInterval[7] = "5000";

    void writeEeprom();
    void clearEeprom();
    bool hasValidSignature();

    long numericMqttPublishInterval()
    {
        return _numericMqttPublishInterval;
    }


private:
    const int MQTT_EEPROM_OFFSET = 10;
    const int DNS_EEPROM_OFFSET = MQTT_EEPROM_OFFSET + sizeof(mqttServerAddress);
    const int GATEWAY_EEPROM_OFFSET = DNS_EEPROM_OFFSET + sizeof(dnsServerAddress);
    const int IP_EEPROM_OFFSET = GATEWAY_EEPROM_OFFSET + sizeof(gatewayAddress);
    const int SUBNET_MASK_OFFSET = IP_EEPROM_OFFSET + sizeof(ipAddress);
    const int MAC_OFFSET = SUBNET_MASK_OFFSET + sizeof(subnetMask);
    const int MQTT_PUBLISH_INTERVAL = MAC_OFFSET + sizeof(mac);

    void readEeprom();
    void writeSignature();
    void initialize();
    void generateMacAddress();
    void parseMqttPublishInterval();

    void (*_callback)();
    long _numericMqttPublishInterval = 1000;
};
