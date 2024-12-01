#include "tasks/ContainerTask.hpp"
#include <Arduino.h>
#include <avr/sleep.h>

#define SLEEP_TIME 30000
#define OPEN_TIME 15000
#define CLOSE_TIME 2000

ContainerTask::ContainerTask(int period) {
    Task::init(period);
    this->state = AWAKE;
    this->ts = millis();
}

void ContainerTask::setFlag(Flag* flag) {
    this->flag = flag;
}

void ContainerTask::setDevices(Button* openButton, Button* closeButton, Led* greenLed, Led* redLed, Display* display, Door* door, UserDetector* userDetector) {
    this->openButton = openButton;
    this->closeButton = closeButton;
    this->greenLed = greenLed;
    this->redLed = redLed;
    this->display = display;
    this->door = door;
    this->userDetector = userDetector;

    greenLed->switchOn();
    this->display->setText(DISPLAY_POSITION, "PRESS OPEN TO");
    this->display->setText(DISPLAY_POSITION_2, "ENTER WASTE");
}

void ContainerTask::tick() {
    Serial.print("FLAG: ");
    Serial.println(this->flag->getValue());
    
    switch (this->state) {
    case AWAKE:
        if(millis() - ts >= SLEEP_TIME) {
            this->state = SLEEP;
            this->display->clear();
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
            sleep_mode();  

            sleep_disable();
            this->state = AWAKE;
            this->display->clear();
            if(this->flag->getValue() == NONE) {
                this->display->setText(DISPLAY_POSITION, "PRESS OPEN TO");
                this->display->setText(DISPLAY_POSITION_2, "ENTER WASTE");
            } else if(this->flag->getValue() == FULL_ALLARM) {
                this->display->setText(DISPLAY_POSITION, "CONTAINER FULL");
            } else {
                this->display->setText(DISPLAY_POSITION, "PROBLEM DETECTED");
            }
            this->ts = millis();
        }
        //guarda la coda degli eventi (ci stanno dentro l'user sensor, i bottoni e il movimento del motore)
        // dopo che hai aperto ↓
        // this->display->setText(DISPLAY_POSITION, "PRESS CLOSE WHEN DONE");
        // this->ts = millis();
        break;
    case OPEN:
        //guarda la coda
        if(this->flag->getValue() != NONE) {
            this->state = AWAKE;
            this->ts = millis();
        }
        if(millis() - ts >= OPEN_TIME) {
            this->state = CLOSE;
            this->door->close();
            this->display->clear();
            this->display->setText(DISPLAY_POSITION, "WASTE RECEIVED");
            this->ts = millis();
        }
        break;
    case CLOSE:
        if(millis() - ts >= CLOSE_TIME) {
            this->state = AWAKE;
            if(this->flag->getValue() == NONE) {
                this->display->clear();
                this->display->setText(DISPLAY_POSITION, "PRESS OPEN TO");
                this->display->setText(DISPLAY_POSITION_2, "ENTER WASTE");
            }
            this->ts = millis();
        }
        break;
    default: break;
    }
}
