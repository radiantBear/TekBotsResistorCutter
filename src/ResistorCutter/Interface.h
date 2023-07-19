/*
 * Functionality for providing a user interface with a Display object and Joystick object
 *
 * Nathaniel Baird
 * bairdn@oregonstate.edu
 *
 * Started:      07/12/2023
 * Last updated: 07/18/2023
 */

#include "Display.h"      // For LCD screen
#include "Joystick.h"     // For joystick control
#include "LocalHost.h"    // For sending info to connected devices

class Interface {
private:
    Display       display;
    Joystick      joystick;
    LocalHost     localHost;
    unsigned long lastUpdate;
    uint8_t       currentSelection;
    unsigned int  rPerKit, kits, percent;
    bool          running, debounced, switchPressed;
    void (*callbackFn)(bool);

    void handleJoystick() {
        if(joystick.getVertical() && (debounced || millis() - lastUpdate >= 500)) {
            lastUpdate = millis();
            debounced = false;

            if (joystick.getUp()) {
                currentSelection = (currentSelection + 2) % 3;
            } else if (joystick.getDown()) {
                ++currentSelection %= 3;
            }

            display.updateAll(currentSelection, rPerKit, kits, percent, running);
            localHost.updatePageInfo(rPerKit, kits, running);
        } else if(joystick.getHorizontal() && (debounced || millis() - lastUpdate >= 250) && !joystick.getVertical()) {
            lastUpdate = millis();
            debounced = false;

            if (currentSelection == 0) {
                // Max of 10 per kit
                if (joystick.getLeft()) {
                    rPerKit = (rPerKit + 8) % 10 + 1;
                } else {
                    rPerKit = rPerKit % 10 + 1;
                }
            } else if (currentSelection == 1) {
                // Max of 50 kits
                if (joystick.getLeft()) {
                    kits = (kits + 48) % 50 + 1;
                } else {
                    kits = kits % 50 + 1;
                }
            }

            display.updateAll(currentSelection, rPerKit, kits, percent, running);
            localHost.updatePageInfo(rPerKit, kits, running);
        }
    }

    void handleSwitch() {
        if(!switchPressed && currentSelection == 2) {
            switchPressed = true;
        
            running = !running;

            display.updateAll(currentSelection, rPerKit, kits, percent, running);
            localHost.updatePageInfo(rPerKit, kits, running);

            callbackFn(running);

            if(running) {
                if(running != 2) running = false;
                else prevRunning = false;

                percent = 0;
                display.updateAll(currentSelection, rPerKit, kits, percent, running);
                localHost.updatePageInfo(rPerKit, kits, running);
            }
        }
    }

public:
    Interface(int8_t dispClk, int8_t dispDin, int8_t dispDc, int8_t dispCe, int8_t dispRst,
        int8_t jstkX, int8_t jstkY, int8_t jstkSw)
        : display(dispClk, dispDin, dispDc, dispCe, dispRst), joystick(jstkX, jstkY, jstkSw) {
            lastUpdate = currentSelection = percent = 0;
            rPerKit = kits = 1;
            debounced = true;
            running = switchPressed = false;
            display.updateAll(currentSelection, rPerKit, kits, percent, running);
            localHost.updatePageInfo(rPerKit, kits, running);
    }
    
    // WARNING: MUST call this function in the main .ino script's setup function, NOT before
    void setup() {
        localHost.setup();
    }

    void update(int percent = 0) {
        if(!running) { 
            if(joystick.getUncentered()) {
                handleJoystick();
            } else {
                debounced = true;
            }
        } else {
            this->percent = percent;
            display.printProgress(percent, running);
        }

        if(joystick.getSwitch()) {
            handleSwitch();
        } else {
            switchPressed = false;
        }
    }

    void setButtonListener(void (*callbackFn)(bool)) {
        this->callbackFn = callbackFn;
    }

    int getResistorsPerKit() {
        return rPerKit;
    }

    int getKits() {
        return kits;
    }
    
    bool getRunning() {
        return running;
    }
};
