#ifndef SOLAR_H
#define SOLAR_H

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include "soc/timer_group_reg.h"

#include "freertos/FreeRTOS.h"  ///< FreeRTOS kernel API
#include "freertos/queue.h"     ///< Queue for inter-task communication
#include "freertos/task.h"      ///< Task creation/management

#define LEFT 0xFF10EF             ///< Value when left button is pressed
#define RIGHT 0xFF5AA5            ///< Value when right button is pressed
#define REPEAT 0xFFFFFFFFFFFFFFFF ///< Value when pressing the same button on the remote

#define LCD_ADDRESS 0x27

#define FREQ 200

#define DEGR 32

#define PHOTO_PIN 1
#define BUZZER_PIN 2
#define SDA_PIN 8
#define SCL_PIN 9
#define STEP_IN1 11
#define STEP_IN2 12
#define STEP_IN3 13
#define STEP_IN4 14
#define IR_PIN 36

extern QueueHandle_t lightQueue;///< lightLevelQueue
extern QueueHandle_t remoteQueue;///< remoteQueue

void panelSetup();
void remoteSetup();
void lightSetup();

#endif