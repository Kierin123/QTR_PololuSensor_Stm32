#ifndef QTR_H
#define QTR_H

#include <DWT_Timer.h>
#include <main.h>
#include <gpio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_READ_TIMEOUT 2500
#define MAX_SENSOR_COUNT 16
#define BLACK 0
#define WHITE 1

typedef enum
{
    On,
    Off,
    OddOn,
    EvenOn
} QTR_readMode;

typedef struct
{
    uint32_t pin;
    GPIO_TypeDef *port;

} sensorPin;

typedef __packed struct
{
    uint32_t *maxValue;
    uint32_t *minValue;
} Calibration;

Calibration CalibrationOn;
Calibration CalibrationOff;

typedef struct
{
    sensorPin oddEmiterPin;
    sensorPin evenEmiterPin;
    uint8_t sensorCount;
    sensorPin *sensorPins;
    // Calibration calibrationData;
} QTR;

// void QTR_init(QTR *sensor);
void QTR_calibrate(QTR *sensor, Calibration *calibration, QTR_readMode mode);
void QTR_emitter(QTR *sensor, QTR_readMode mode);
void QTR_read(QTR *sensor, uint32_t *sensorValues, QTR_readMode mode);
int32_t QTR_readLine(QTR *sensor, uint32_t *sensorValues, uint32_t mode);

#endif // QTR_H
