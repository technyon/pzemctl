#include <src/Arduino_FreeRTOS.h>
#include "WebServer.h"

namespace web
{
   WebServer::WebServer(EthernetClient *ethClient, Configuration* configuration)
            : _ethClient(ethClient),
              _server(80),
              _configuration(configuration)
    {

    }

    void WebServer::update()
    {
        if(!_enabled) return;

        // Create a client connection
        EthernetClient client = _server.available();
        if (client)
        {
            int index = 0;
            char message[200];

            while (client.connected())
            {
                if (client.available())
                {
                    char c = client.read();

                    //read char by char HTTP request
                    if (index < sizeof(message) - 1)
                    {
                        message[index] = c;
                        index++;
                    }
                    message[index] = 0;

                    //if HTTP request has ended
                    if (c == '\n')
                    {
                        serveHtml(client);
                    }
                }
            }

            char *token = strtok(message, "?=&");
            char *lastToken = nullptr;

            bool configChanged = false;
            while (token != nullptr)
            {
                if(lastToken != nullptr)
                {
                    TokenType lastTokenType = getParameterType(lastToken);
                    TokenType tokenType = getParameterType(token);

                    if(lastTokenType == TokenType::MQTT_SERVER && tokenType == TokenType::NONE)
                    {
                        configChanged = true;
                        strcpy(_configuration->mqttServerAddress, token);
                    }
                    else if(lastTokenType == TokenType::DNS_SERVER && tokenType == TokenType::NONE)
                    {
                        configChanged = true;
                        strcpy(_configuration->dnsServerAddress, token);
                    }
                    else if(lastTokenType == TokenType::GATEWAY && tokenType == TokenType::NONE)
                    {
                        configChanged = true;
                        strcpy(_configuration->gatewayAddress, token);
                    }
                    else if(lastTokenType == TokenType::IP_ADDRESS && tokenType == TokenType::NONE)
                    {
                        configChanged = true;
                        strcpy(_configuration->ipAddress, token);
                    }
                    else if(lastTokenType == TokenType::SUBNET_MASK && tokenType == TokenType::NONE)
                    {
                        configChanged = true;
                        strcpy(_configuration->subnetMask, token);
                    }
                    else if(lastTokenType == TokenType::MQTT_PUBLISH_INTERVAL && tokenType == TokenType::NONE)
                    {
                        configChanged = true;
                        strcpy(_configuration->mqttPublishInterval, token);
                    }
                }
                lastToken = token;
                token = strtok(nullptr, "?=&");
            }

            if(configChanged)
            {
                _configuration->writeEeprom();
                _enabled = false;
            }
        }
    }

    void WebServer::serveHtml(EthernetClient& client)
    {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();

        client.println("<HTML>");
        client.println("<HEAD>");
        client.println("<TITLE>PZEM004t Energy Monitor</TITLE>");
        client.println("</HEAD>");
        client.println("<BODY>");

        client.println("<FORM ACTION=method=get >");

        client.print("MQTT Server: <INPUT TYPE=TEXT VALUE=\"");
        client.print(_configuration->mqttServerAddress);
        client.println("\" NAME=\"MQTTSERVER\" SIZE=\"25\" MAXLENGTH=\"40\"><BR>");

        client.print("DNS Server: <INPUT TYPE=TEXT VALUE=\"");
        client.print(_configuration->dnsServerAddress);
        client.println("\" NAME=\"DNSSERVER\" SIZE=\"25\" MAXLENGTH=\"16\"><BR>");

        client.print("Gateway: <INPUT TYPE=TEXT VALUE=\"");
        client.print(_configuration->gatewayAddress);
        client.println("\" NAME=\"GATEWAY\" SIZE=\"25\" MAXLENGTH=\"16\"><BR>");

        client.print("IP Address: <INPUT TYPE=TEXT VALUE=\"");
        client.print(_configuration->ipAddress);
        client.println("\" NAME=\"IPADDRESS\" SIZE=\"25\" MAXLENGTH=\"16\"><BR>");

        client.print("Subnet mask: <INPUT TYPE=TEXT VALUE=\"");
        client.print(_configuration->subnetMask);
        client.println("\" NAME=\"SUBNET\" SIZE=\"25\" MAXLENGTH=\"16\"><BR>");

        client.print("MQTT publish interval (ms): <INPUT TYPE=TEXT VALUE=\"");
        client.print(_configuration->mqttPublishInterval);
        client.println("\" NAME=\"INTERVAL\" SIZE=\"25\" MAXLENGTH=\"6\"><BR>");

        client.println("<INPUT TYPE=SUBMIT NAME=\"submit\" VALUE=\"Save\">");

        client.println("</FORM>");

        client.println("<BR>");

        client.println("</BODY>");
        client.println("</HTML>");

        vTaskDelay( 5 / portTICK_PERIOD_MS);
        //stopping client
        client.stop();
    }

    TokenType WebServer::getParameterType(char *&token)
    {
        if (strcmp(token, "MQTTSERVER") == 0)
        {
            return TokenType::MQTT_SERVER;
        }
        if (strcmp(token, "DNSSERVER") == 0)
        {
            return TokenType::DNS_SERVER;
        }
        if (strcmp(token, "GATEWAY") == 0)
        {
            return TokenType::GATEWAY;
        }
        if (strcmp(token, "IPADDRESS") == 0)
        {
            return TokenType::IP_ADDRESS;
        }
        if (strcmp(token, "SUBNET") == 0)
        {
            return TokenType::SUBNET_MASK;
        }
        if (strcmp(token, "INTERVAL") == 0)
        {
            return TokenType::MQTT_PUBLISH_INTERVAL;
        }

        return TokenType::NONE;
    }

    void WebServer::enable()
    {
        _enabled = true;
    }

    void WebServer::disable()
    {
        _enabled = false;
    }

    bool WebServer::enabled()
    {
        return _enabled;
    }
}