#include "WebSocketsClass.hpp"



void giveStatus(WiFiClient& client)
{
  String boardName;
    #if defined(ESP32)
        boardName = "ESP32";
        uint32_t flashSize = ESP.getFlashChipSize();
        uint32_t memoryUsed = ESP.getHeapSize() - ESP.getFreeHeap();
    #elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
        boardName = "Raspberry Pi Pico W";
        uint32_t flashSize = (uint32_t) (PICO_FLASH_SIZE_BYTES / 1024);  // Pico's flash size in KB
        uint32_t memoryUsed = rp2040.getUsedHeap();  // Function that returns heap usage for Pico
    #else
        boardName = "Unknown Board";
        uint32_t flashSize = 0;
        uint32_t memoryUsed = 0;
    #endif

    client.println("Current board: " + boardName + " Flash Size: " + String(flashSize) + " bytes" + " Memory Used: " + String(memoryUsed) + " bytes");
}

void sayHello(WiFiClient& client) {
    client.println("HELLO PYTHON <3");
}

void giveNumber(WiFiClient& client) {
    float numberToGive = random(0, 100) / 10.0;
    client.println("numberToGive = " + String(numberToGive));
}

void giveNumbers(WiFiClient& client) {
    float numberToGive = random(0, 100) / 10.0;
    float numberToGive2 = random(0, 100) / 10.0;
    float numberToGive3 = random(0, 100) / 10.0;
    client.println("numberToGive = " + String(numberToGive) + "; "+ "numberToGive2 = " + String(numberToGive2)+ "; "+ "numberToGive3 = " + String(numberToGive3));
}



bool ledState = false;

void changeLed(WiFiClient& client) {
  ledState = !ledState;
  digitalWrite(4, ledState ? HIGH : LOW); // Set pin 4 to HIGH if ledState is true, otherwise set it to LOW.
  String message = "LED IS NOW ";
  message += (ledState) ? "ON" : "OFF"; // Append "ON" or "OFF" to the message depending on ledState.
  client.println(message); // Send the message to the client.
}

WebSocketsClass webServer(80); // Initialize a WebSocket server on port 80.

float x = 0; // Variable to store a float value sent by the client.

void setValue(WiFiClient& client)
{
  webServer.extractFloat("=", &x); // Extract the float value from the command after '=' and store it in x.
  client.println("that var is now " + String(x)); // Send the updated value of x to the client.
}

void setup() {
    Serial.begin(115200); // Initialize serial communication at 115200 baud.
    while (!Serial) { delay(100); } // Wait for the serial port to be ready.
    Serial.println(F("Setup complete")); // Indicate that setup is complete.

    // Start the WebSocket server with the given Wi-Fi credentials.
    // If the third parameter is true, the ESP will act as a router (Access Point mode).
    webServer.begin("WIFINAME", "PASSWORD", false);

    // Register commands and associate them with their respective functions.
    // The client sends a command, and the ESP executes the corresponding function.
    webServer.addCommand("HELLO WORLD", sayHello); // Responds to the "HELLO WORLD" command with sayHello().
    webServer.addCommand("GIVE NUMBER", giveNumber); // Responds with a single number.
    webServer.addCommand("GIVE NUMBERS", giveNumbers); // Sends multiple numbers, which the client can interpret as a dictionary.
    webServer.addCommand("GIVE STATUS", giveStatus); // Responds with the current status of the ESP or other data.
    webServer.addCommand("CHANGE LED", changeLed); // Toggles the state of an LED.
    webServer.addCommand("X=", setValue); // Sets the variable x to the value sent after "X=".

    pinMode(4, OUTPUT); // Configure pin 4 as an output for controlling the LED.
}

void loop() {
    webServer.handle(); // Continuously handle client connections and process incoming commands.
}