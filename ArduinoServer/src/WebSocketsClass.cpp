#include "WebSocketsClass.hpp"

// Constructor to initialize server and store Wi-Fi credentials
WebSocketsClass::WebSocketsClass(size_t port)
    : server(port), cmdCount(0)
{
    // You don't connect to WiFi here anymore, it's done in begin()
}

// The begin method to connect Wi-Fi and start the server
void WebSocketsClass::begin(const char *SSID, const char *PASSWORD, bool isAP)
{
    if (isAP)
    {
        startAP(SSID, PASSWORD);
    }
    else
    {
        connectToWiFi(SSID, PASSWORD);
    }
    server.begin(); // Start the server here
    this->addCommand("GETCMD", [this](WiFiClient &client)
                     { this->getCmdArr(client); });
}

// Connect to WiFi with timeout handling
void WebSocketsClass::connectToWiFi(const char *SSID, const char *PASSWORD)
{
    Serial.println("Connecting to WiFi...");
    WiFi.begin(SSID, PASSWORD);
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000)
    { // Timeout 15 seconds
        delay(1000);
        Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConnected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Failed to connect to WiFi");
        // Optionally retry or handle WiFi failure
    }
}

// Start Access Point mode
void WebSocketsClass::startAP(const char *SSID, const char *PASSWORD)
{
    Serial.println("Starting Access Point...");
    WiFi.softAP(SSID, PASSWORD);
    Serial.println("AP Started!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
}

// Adds a command and its corresponding function to be executed
void WebSocketsClass::addCommand(const char *cmd, std::function<void(WiFiClient &)> func)
{
    if (cmdCount < MAX_COMMANDS)
    {
        cmdArr[cmdCount].cmd = cmd;
        cmdArr[cmdCount].func = func;
        cmdCount++;
    }
    else
    {
        Serial.println("Error: Command array full");
    }
}

// Sends list of registered commands to the client
void WebSocketsClass::getCmdArr(WiFiClient &client)
{
    char message[200]; // Predefined size for efficiency
    size_t index = 0;
    for (uint8_t i = 0; i < cmdCount; ++i)
    {
        index += snprintf(message + index, sizeof(message) - index, "%s", cmdArr[i].cmd);
        if (i < cmdCount - 1)
        {
            message[index++] = ';'; // Add semicolon separator
        }
    }
    message[index] = '\0'; // Null-terminate the string
    client.println(message);
}

String WebSocketsClass::getMessage()
{
    return lastMessage;
}

bool WebSocketsClass::extractFloat(const char *key, float *value)
{
    int index = lastMessage.indexOf(key);
    if (index != -1 && value != nullptr)
    {
        String valueStr = lastMessage.substring(index + strlen(key)); // Extract the value part
        *value = valueStr.toFloat();                                  // Convert to float and return

        return true;
    }
    return false; // Return NaN if the key is not found
}

// Handle client connections and process received commands
void WebSocketsClass::handle()
{
    WiFiClient client = server.available(); // Listen for incoming clients
    if (client)
    {
        Serial.println("Client connected");

        String message = "";
        while (client.connected())
        {
            if (client.available())
            {
                char c = client.read();
                if (c == '\n')
                {
                    lastMessage = message; // Store the received message
                    Serial.println("Received: " + message);

                    bool commandFound = false;
                    for (size_t i = 0; i < cmdCount; i++)
                    {
                        if (message.startsWith(cmdArr[i].cmd))
                        {
                            cmdArr[i].func(client); // Execute the associated function
                            commandFound = true;
                            break;
                        }
                    }

                    if (!commandFound)
                    {
                        client.println("UNKNOWN COMMAND");
                    }
                    message = ""; // Clear the message after processing
                }
                else
                {
                    message += c;
                }
            }
        }

        client.stop();
        Serial.println("Client disconnected");
    }
}