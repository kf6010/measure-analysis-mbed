#include <stdbool.h>
#include <ucos_ii.h>
#include <mbed.h>
#include "C12832.h"
#include "MMA7660.h"
#include <stdio.h>

/*
*********************************************************************************************************
*                                            APPLICATION TASK PRIORITIES
*********************************************************************************************************
*/

typedef enum {
  SAMPLE_SW3_PRIO,
  SAMPLE_ACCEL_PRIO,
  UPDATE_LCD_PRIO,
  UPDATE_SPEAKER_PRIO,
  LED_RED_PRIO,
} taskPriorities_t;

/*
*********************************************************************************************************
*                                            APPLICATION TASK STACKS
*********************************************************************************************************
*/

#define  SAMPLE_SW3_STK_SIZE           256
#define  SAMPLE_ACCEL_STK_SIZE         256
#define  UPDATE_LCD_STK_SIZE           256
#define  UPDATE_SPEAKER_STK_SIZE       256
#define  LED_RED_STK_SIZE              256

static OS_STK sampleSW3Stk[SAMPLE_SW3_STK_SIZE];
static OS_STK sampleAccelStk[SAMPLE_ACCEL_STK_SIZE];
static OS_STK updateLCDStk[UPDATE_LCD_STK_SIZE];
static OS_STK updateSpeakerStk[UPDATE_SPEAKER_STK_SIZE];
static OS_STK ledRedStk[LED_RED_STK_SIZE];

/*
*********************************************************************************************************
*                                            APPLICATION FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void sampleSW3(void *pdata);
static void sampleAccel(void *pdata);
static void updateLCD(void *pdata);
static void updateSpeaker(void *pdata);
static void appTaskLedRed(void *pdata);

static void ledToggle(DigitalOut led);
/*
*********************************************************************************************************
*                                            GLOBAL TYPES AND VARIABLES 
*********************************************************************************************************
*/

static C12832 lcd(D11, D13, D12, D7, D10);

static float accelVal[3] = {0.0f, 0.0f, 0.0f};
static bool sw3Pressed = false;

/*
*********************************************************************************************************
*                                            GLOBAL FUNCTION DEFINITIONS
*********************************************************************************************************
*/

int main() {

  /* Initialise the OS */
  OSInit();                                                   

  /* Create the tasks */
  OSTaskCreate(sampleSW3,                               
               (void *)0,
               (OS_STK *)&sampleSW3Stk[SAMPLE_SW3_STK_SIZE - 1],
               SAMPLE_SW3_PRIO);

  OSTaskCreate(sampleAccel,                               
               (void *)0,
               (OS_STK *)&sampleAccelStk[SAMPLE_ACCEL_STK_SIZE - 1],
               SAMPLE_ACCEL_PRIO);

  OSTaskCreate(updateLCD,                               
               (void *)0,
               (OS_STK *)&updateLCDStk[UPDATE_LCD_STK_SIZE - 1],
               UPDATE_LCD_PRIO);
  
  OSTaskCreate(updateSpeaker,                               
               (void *)0,
               (OS_STK *)&updateSpeakerStk[UPDATE_SPEAKER_STK_SIZE - 1],
               UPDATE_SPEAKER_PRIO);
  
  OSTaskCreate(appTaskLedRed,                               
               (void *)0,
               (OS_STK *)&ledRedStk[LED_RED_STK_SIZE - 1],
               LED_RED_PRIO);
  
  /* Start the OS */
  OSStart();                                                  
  
  /* Should never arrive here */ 
  return 0;      
}

/*
*********************************************************************************************************
*                                            APPLICATION TASK DEFINITIONS
*********************************************************************************************************
*/

static void sampleSW3(void *pdata) {
  static DigitalIn sw3(PTA4);

  /* Start the OS ticker -- must be done in the highest priority task */
  SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
  while (true) {
    sw3Pressed = (sw3 == 0) ? true : false;
    OSTimeDlyHMSM(0,0,0,100);
  } 
}

static void sampleAccel(void *pdata) {
  static MMA7660 accel(D14, D15);

  while (true) {
    accelVal[0] = accel.x();
    accelVal[1] = accel.y();
    accelVal[2] = accel.z();
    OSTimeDlyHMSM(0,0,0,100);
  } 
}

static void updateLCD(void *pdata) {

  lcd.cls();
  while (true) {
    lcd.locate(43, 0);
    lcd.printf("X: %0.2f", accelVal[0]);
    lcd.locate(43, 8);
    lcd.printf("Y: %0.2f", accelVal[1]);
    lcd.locate(43, 16);
    lcd.printf("Z: %0.2f", accelVal[2]);
    OSTimeDlyHMSM(0,0,0,100);
  } 
}

static void updateSpeaker(void *pdata) {
  static PwmOut speaker(D6);

  speaker.period_us(2272);
  speaker.pulsewidth_us(0);
  while (true) {
    if (sw3Pressed) {
      speaker.pulsewidth_us(1136);
    } 
    else {
      speaker.pulsewidth_us(0);
    }
    OSTimeDlyHMSM(0,0,0,100);
  } 
}

static void appTaskLedRed(void *pdata) {
  DigitalOut red(LED_RED);
  
  red = 1;

  /* Task main loop */
  while (true) {
    ledToggle(red);
    OSTimeDlyHMSM(0,0,0,500);
  }
}

static void ledToggle(DigitalOut led) {
  led = 1 - led;
}
