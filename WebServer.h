#pragma once

#include <EthernetClient.h>
#include "EepromConfiguration.h"

namespace web
{
    enum class TokenType
    {
        NONE,
        MQTT_SERVER,
        DNS_SERVER,
        GATEWAY,
        IP_ADDRESS,
        SUBNET_MASK,
        MQTT_PUBLISH_INTERVAL
    };

    class WebServer
    {
    public:
        explicit WebServer(EthernetClient *ethClient, EepromConfiguration* configuration);
        virtual ~WebServer() = default;

        void update();
        void enable();
        void disable();

        [[nodiscard]] bool enabled();

    private:
        void serveHtml(EthernetClient& client);
        TokenType getParameterType(char*& token);

        EthernetClient *_ethClient;
        EthernetServer _server;

        EepromConfiguration* _configuration;
        bool _enabled = false;
    };
}