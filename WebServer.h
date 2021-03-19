#pragma once

#include <EthernetClient.h>
#include "Configuration.h"


namespace web
{
    enum class TokenType
    {
        NONE,
        MQTT_SERVER,
        DNS_SERVER,
        GATEWAY,
        IP_ADDRESS,
        SUBNET_MASK
    };

    class WebServer
    {
    public:
        explicit WebServer(EthernetClient *ethClient, Configuration* configuration);

        virtual ~WebServer() = default;

        void initialize();

        void update();

    private:
        void serveHtml(EthernetClient& client);
        TokenType getParameterType(char*& token);

        EthernetClient *_ethClient;
        EthernetServer _server;

        Configuration* _configuration;


    };
}