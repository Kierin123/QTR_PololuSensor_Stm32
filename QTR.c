/*
 * QTR.c
 *
 *  Created on: Jun 8, 2022
 *      Author: Marcin Kierinkiewicz
 */

#include <QTR.h>

static void QTR_readPriv(QTR *sensor, uint32_t *sensorValues)
{
    uint32_t startTime = 0, time = 0;

    if (sensor->sensorPins == NULL)
    {
        return;
    }

    for (uint32_t i = 0; i < sensor->sensorCount; i++)
    {
        sensorValues[i] = MAX_READ_TIMEOUT;
        GPIO_SetPinAs_Output(sensor->sensorPins[i].port, sensor->sensorPins[i].pin);
        HAL_GPIO_WritePin(sensor->sensorPins[i].port, sensor->sensorPins[i].pin, GPIO_PIN_SET);
    }

    DWT_delay_us(10);

    __disable_irq();

    startTime = DWT_getTick_us();
    time = 0;

    for (uint32_t i = 0; i < sensor->sensorCount; i++)
    {
        GPIO_SetPinAs_Input(sensor->sensorPins[i].port, sensor->sensorPins[i].pin);
    }

    __enable_irq();

    while (time < MAX_READ_TIMEOUT)
    {
        __disable_irq();

        time = DWT_getTick_us() - startTime;
        for (uint32_t i = 0; i < sensor->sensorCount; i++)
        {
            if (
                (HAL_GPIO_ReadPin(sensor->sensorPins[i].port, sensor->sensorPins[i].pin) == GPIO_PIN_RESET) &&
                (time < sensorValues[i]))
            {
                sensorValues[i] = time;
            }
        }
    }

    __enable_irq();
}

static int32_t QTR_readLinePriv(QTR *sensor, uint32_t *sensorValues, QTR_readMode readMode, uint32_t LineToRead)
{
    // uint32_t sensorCount = sensor->sensorCount;
    int32_t linePosition = -1;
    QTR_read(sensor, sensorValues, readMode);
    for (uint32_t i = 0; i < sensor->sensorCount; i++)
    {
        switch (LineToRead)
        {
        case BLACK:
            if ((CalibrationOn.minValue[i]) < sensorValues[i])
            {
                linePosition = i;
            }
            break;
        case WHITE:
             if ((CalibrationOn.maxValue[i]) > sensorValues[i])
            {
                linePosition = i;
            }
            break;
        default:
            break;
        }
    }
    return linePosition;
}


void QTR_calibrate(QTR *sensor, Calibration *calibration, QTR_readMode mode)
{
    uint32_t sensorCount = sensor->sensorCount;
    uint32_t sensorValues[sensorCount];
    uint32_t maxsensorValues[sensorCount];
    uint32_t minsensorValues[sensorCount];

    uint32_t *oldMax = calibration->maxValue;
   calibration->maxValue = (uint32_t *)realloc(calibration->maxValue, sizeof(uint32_t) * sensorCount);

    if (calibration->maxValue == NULL)
    {
        free(oldMax);
        return;
    }

    uint32_t *oldMin = calibration->minValue;
    calibration->minValue = (uint32_t *)realloc(calibration->minValue, sizeof(uint32_t) * sensorCount);

    if (calibration->minValue == NULL)
    {
        free(oldMin);
        return;
    }

    for (uint32_t i = 0; i < sensorCount; i++)
    {
        calibration->maxValue[i] = 0;
        calibration->minValue[i] = MAX_READ_TIMEOUT;
    }

    // Calibraion

    for (uint32_t i = 0; i < 10; i++)
    {

        QTR_read(sensor, sensorValues, mode);
        for (uint32_t j = 0; j < sensorCount; j++)
        {
            if ((i == 0) || (sensorValues[j] > maxsensorValues[j]))
            {
                maxsensorValues[j] = sensorValues[j];
            }

            if ((i == 0) || (sensorValues[j] < minsensorValues[j]))
            {
                minsensorValues[j] = sensorValues[j];
            }
        }
    }

    for (uint32_t i = 0; i < sensorCount; i++)
    {

        if (minsensorValues[i] > calibration->maxValue[i])
        {
            calibration->maxValue[i] = minsensorValues[i];
        }

        if (maxsensorValues[i] < calibration->minValue[i])
        {
            calibration->minValue[i] = maxsensorValues[i];
        }
    }
}

void QTR_emitter(QTR *sensor, QTR_readMode mode)
{
    switch (mode)
    {
    case On:
        HAL_GPIO_WritePin(sensor->oddEmiterPin.port, sensor->oddEmiterPin.pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(sensor->evenEmiterPin.port, sensor->evenEmiterPin.pin, GPIO_PIN_SET);
        break;

    case Off:
        HAL_GPIO_WritePin(sensor->oddEmiterPin.port, sensor->oddEmiterPin.pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(sensor->evenEmiterPin.port, sensor->evenEmiterPin.pin, GPIO_PIN_RESET);
        break;

    case OddOn:
        HAL_GPIO_WritePin(sensor->oddEmiterPin.port, sensor->oddEmiterPin.pin, GPIO_PIN_SET);
        break;

    case EvenOn:
        HAL_GPIO_WritePin(sensor->evenEmiterPin.port, sensor->evenEmiterPin.pin, GPIO_PIN_SET);
        break;

    default:
        break;
    }
}

void QTR_read(QTR *sensor, uint32_t *sensorValues, QTR_readMode mode)
{
    QTR_emitter(sensor, mode);
    QTR_readPriv(sensor, sensorValues);
    QTR_emitter(sensor, Off);
}

int32_t QTR_readLine(QTR *sensor, uint32_t *sensorValues, uint32_t LineToRead)
{
    return QTR_readLinePriv(sensor, sensorValues, OddOn, LineToRead );
}
