#ifndef WEBSOCKET_CLASS_HPP
#define WEBSOCKET_CLASS_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <functional>

#ifndef MAX_COMMANDS
#define MAX_COMMANDS 10
#endif

class WebSocketsClass
{
private:
    WiFiServer server;
    struct CMD_FUNCTION
    {
        const char *cmd;
        std::function<void(WiFiClient &)> func;
    } cmdArr[MAX_COMMANDS];
    size_t cmdCount;

    void connectToWiFi(const char *SSID, const char *PASSWORD);
    void startAP(const char *SSID, const char *PASSWORD);

    String lastMessage;

public:
    WebSocketsClass() = delete;
    WebSocketsClass(size_t port);
    void begin(const char *SSID, const char *PASSWORD, bool isAP = false);
    void addCommand(const char *cmd, std::function<void(WiFiClient &)> func);
    void getCmdArr(WiFiClient &client);
    String getMessage();
    bool extractFloat(const char *key, float *value); // Function to extract a float based on a key
    void handle();
};

#endif
