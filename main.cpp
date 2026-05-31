#include <Arduino.h>

// Arduino Sketch forward declaration
void setup();
void loop();

int main(void) {
    init();
    setup();

    while (true) {
        loop();
        yield();
    }
}
