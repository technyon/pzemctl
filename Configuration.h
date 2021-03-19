#pragma once

class Configuration
{
public:
    Configuration(void (*callback)());
    virtual ~Configuration() = default;

    char mqttServerAddress[40] = "";
    char dnsServerAddress[17] = "";
    char gatewayAddress[17] = "";
    char ipAddress[17] = "192.168.168.168";
    char subnetMask[17] = "255.255.255.0";

    void writeEeprom();

private:
    const int MQTT_EEPROM_OFFSET = 10;
    const int DNS_EEPROM_OFFSET = MQTT_EEPROM_OFFSET + sizeof(mqttServerAddress);
    const int GATEWAY_EEPROM_OFFSET = DNS_EEPROM_OFFSET + sizeof(dnsServerAddress);
    const int IP_EEPROM_OFFSET = GATEWAY_EEPROM_OFFSET + sizeof(gatewayAddress);
    const int SUBNET_MASK_OFFSET = IP_EEPROM_OFFSET + sizeof(ipAddress);

    void readEeprom();
    void writeSignature();
    bool hasValidSignature();

    void initialize();

    void (*_callback)();
};
