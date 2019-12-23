#include "cmsis_os2.h"
#include "read_accel.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f401_discovery_accelerometer.h"
#include "arm_math.h"
#include "arm_const_structs.h"


/*
 * Less than 1 s of samples at highest accelerometer ODR. It corresponds to
 * 1024 samples actually but buffer has 2048 because it has real and imaginary
 * part.
 */
#define BUFFER_LENGTH 2048


static ACCELERO_DrvTypeDef *AccelerometerDrv;
extern UART_HandleTypeDef huart2;
#ifdef SIMULATE_PROCESSING
extern float32_t input_data[BUFFER_LENGTH];
#endif
float32_t data_z[BUFFER_LENGTH];
float32_t data_z_output[BUFFER_LENGTH / 2];


void InitReadAccelTask()
{
	/* Take into account that sometimes it can happen that
	 * the accelerometer gets stuck so the driver says that
	 * the I2C is busy. In this case, a power off - power on
	 * must be performed.*/
    BSP_ACCELERO_Init();
    AccelerometerDrv = &Lsm303dlhcDrv;
}


void ReadAccelTask(void *argument)
{
    int16_t pDataXYZ[3];
    uint8_t new_data;
    uint16_t index;

    for(;;)
    {
        osDelay(100);

        /*
         * If the motor is turned on, this task keeps on collecting and
         * processing data. When it detects unbalance the motor is stopped.
         */
        if (HAL_GPIO_ReadPin(motor_GPIO_Port, motor_Pin) == 0){
            /* Data acquisition. */
            index = 0;
#ifndef SIMULATE_PROCESSING
            /* THESE LINES DO NOT DO WHAT ARE SUPPOSED TO DO */
            while (index < BUFFER_LENGTH)
            {
                new_data = COMPASSACCELERO_IO_Read(ACC_I2C_ADDRESS,
                    LSM303DLHC_STATUS_REG_A);
                if (new_data && 0x8)
                {
                    BSP_ACCELERO_GetXYZ(pDataXYZ);
                    /*
                     * Transform to g by multiplying for (4.f / 32767.f) 
                     * because the scale is from -4 to 4 g and because it has
                     * 16 bits.
                     */
                    data_z[index++] = pDataXYZ[2] * 0.000122074f;
                    data_z[index++] = 0.f; /* Imaginary part. */
                }
            }
#else
            while (index < BUFFER_LENGTH)
            {
                data_z[index] = input_data[index];
                index++;
            }
            osDelay(760); /* Simulate acquisition time of ~1024 samples. */
#endif

            arm_cfft_f32(&arm_cfft_sR_f32_len1024, data_z, 0, 1);
            arm_cmplx_mag_f32(data_z, data_z_output, 1024);

            /*
             * 50 was chosen as the threshold because it is the average between
             * 24 and 76 that are the values respectively got for normal and
             * abnormal data for 25 Hz.
             * The reason why 25 Hz corresponds to the 19th element of
             * data_z_output is because the magnitude of the FFT (i.e.,
             * data_z_output) has 1024 elements but only the first 512 are
             * relevant (last 512 are the same but mirrored). Then, the 512nd
             * element of data_z_output corresponds to sample frequency divided
             * by 2 (1344 / 2 = 672) because of Nyquist so 25 Hz corresponds to
             * 512 * 25 / 672 = 19.
             */
            if (data_z_output[19] > 50.f)
            {
                /* Turn off the motor and the led. */
                HAL_GPIO_WritePin(motor_GPIO_Port, motor_Pin, GPIO_PIN_SET);
                HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
                    /* Send data through UART. */
                HAL_UART_Transmit(&huart2, "Motor was stopped.\n", 18,
                    HAL_UART_STATE_TIMEOUT);
            }
        }
    }
}
