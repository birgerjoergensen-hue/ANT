#include <Arduino.h>
#include <bluefruit.h>

BLEService controlService = BLEService(0x180F); // Battery Service (dummy)
BLECharacteristic commandChar = BLECharacteristic(0x2A19);
BLECharacteristic statusChar = BLECharacteristic(0x2A1A);

// Variables
volatile uint8_t lastCommand = 0;

// Callback wenn Daten empfangen werden
void write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
    if (len > 0) {
        lastCommand = data[0];

        Serial.print("Command received: ");
        Serial.println(lastCommand);

        // Beispiel: LED steuern
        if (lastCommand == 1) {
            digitalWrite(LED_BUILTIN, HIGH);
        } else if (lastCommand == 0) {
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    Bluefruit.begin();
    Bluefruit.setTxPower(4);
    Bluefruit.setName("NRF52840_CTRL");

    // Service
    controlService.begin();

    // Command Characteristic (write)
    commandChar.setProperties(CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP);
    commandChar.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    commandChar.setWriteCallback(write_callback);
    commandChar.begin();

    // Status Characteristic (notify)
    statusChar.setProperties(CHR_PROPS_NOTIFY);
    statusChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    statusChar.begin();

    // Advertising starten
    Bluefruit.Advertising.addService(controlService);
    Bluefruit.Advertising.setInterval(32, 244);
    Bluefruit.Advertising.start();

    Serial.println("BLE Device Ready");
}

void loop() {
    // Status senden (z. B. alle 1 Sekunde)
    static uint32_t last = 0;

    if (millis() - last > 1000) {
        last = millis();

        uint8_t status = analogRead(A0) / 4;
        statusChar.notify(&status, 1);
    }
}
