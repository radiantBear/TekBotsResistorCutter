/*
 * Resistor Reel Cutter for TekBots resistor reels
 *
 * Nathaniel Baird
 * bairdn@oregonstate.edu
 *
 * Started:      07/10/2023
 * Last updated: 07/12/2023
 */

#include "pins.h"       // List of all pin connections
#include "Interface.h"  // For I/O using the LCD and joystick

Interface interface(CLK_PIN, DIN_PIN, DC_PIN, CE_PIN, RST_PIN, VRx_PIN, VRy_PIN, SW_PIN);

void printStuff(bool state);

void setup() {
    Serial.begin(115200);
    while(!Serial); // Wait for serial port to connect

    interface.setButtonListener(printStuff);
}

void loop() {
    interface.update();
}

void printStuff(bool state) {
    Serial.printf("Button pressed! Machine is %s.", interface.getRunning() ? "on" : "off");
    if(state) Serial.printf(" Cutting groups of %d resistors for %d kits.", interface.getResistorsPerKit(), interface.getKits());
    else Serial.println();

    for(int i = 0; i <= 100; i++) {
        interface.update(i);// call function to update completion bar
        delay(50);
    }
}