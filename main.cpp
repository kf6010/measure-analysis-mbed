#include <stdbool.h>
#include <mbed.h>
#include "C12832.h"
#include "MMA7660.h"
#include <stdio.h>

/* Function prototypes for thread tasks */

static void sampleSW3(void);
static void sampleAccel(void);
static void updateLCD(void);
static void updateSpeaker(void);
static void appTaskLedRed(void);

static void ledToggle(DigitalOut led);


static C12832 lcd(D11, D13, D12, D7, D10);

static float accelVal[3] = { 0.0f, 0.0f, 0.0f };

static bool sw3Pressed = false;

int main()
{
    /* Create the Threads */
	Thread sw_sampler;
	Thread acc_sampler;
	Thread lcd;
	Thread speaker;
	Thread led;

    /* Create the tasks */
    sw_sampler.start(sampleSW3);

    acc_sampler.start(sampleAccel);

    lcd.start(updateLCD);

	speaker.start(updateSpeaker);

    led.start(appTaskLedRed);

	lcd.join();
    /* Should never arrive here */
    return 0;
}

static void sampleSW3(void)
{
    static DigitalIn sw3(PTA4);

    while (true) {
        sw3Pressed = (sw3 == 0) ? true : false;
        thread_sleep_for(100);
    }
}

static void sampleAccel(void)
{
    static MMA7660 accel(D14, D15);

    while (true) {
        accelVal[0] = accel.x();
        accelVal[1] = accel.y();
        accelVal[2] = accel.z();
        thread_sleep_for(100);
    }
}

static void updateLCD(void)
{

    while (true) {
        lcd.locate(43, 0);
        lcd.printf("X: %0.2f", accelVal[0]);
        lcd.locate(43, 8);
        lcd.printf("Y: %0.2f", accelVal[1]);
        lcd.locate(43, 16);
        lcd.printf("Z: %0.2f", accelVal[2]);
        thread_sleep_for(100);
    }
}

static void updateSpeaker(void)
{
    static PwmOut speaker(D6);

    speaker.period_us(2272);
    speaker.pulsewidth_us(0);
    while (true) {
        if (sw3Pressed) {
            speaker.pulsewidth_us(1136);
        } else {
            speaker.pulsewidth_us(0);
        }
        thread_sleep_for(100);
    }
}

static void appTaskLedRed(void)
{
    DigitalOut red(LED_RED);

    red = 1;

    /* Task main loop */
    while (true) {
        ledToggle(red);
        thread_sleep_for(500);
    }
}

static void ledToggle(DigitalOut led)
{
    led = 1 - led;
}
