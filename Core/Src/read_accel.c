#include "cmsis_os2.h"
#include "read_accel.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f401_discovery_accelerometer.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define SIMULATE_PROCESSING
#define BUFFER_LENGTH 2688 /* 2 seconds of samples at highest ACC ODR. */

static ACCELERO_DrvTypeDef *AccelerometerDrv;
extern UART_HandleTypeDef huart2;
#ifdef SIMULATE_PROCESSING
extern float32_t input_data[BUFFER_LENGTH];
#endif
//extern float32_t testInput_f32_10khz[2048];

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
#ifdef SIMULATE_PROCESSING
    float32_t *data_z = input_data;
#else
    float32_t data_z[BUFFER_LENGTH];
#endif
    float32_t data_z_output[1024];
    uint8_t new_data;
    uint16_t index;
    for(;;)
    {
        osDelay(100);
        #if 0
        #ifndef SIMULATE_PROCESSING
        /* Data acquisition. */
        index = 0;
        while (index < BUFFER_LENGTH)
        {
            new_data = COMPASSACCELERO_IO_Read(ACC_I2C_ADDRESS,
                LSM303DLHC_STATUS_REG_A);
            if (new_data && 0x8)
            {
                BSP_ACCELERO_GetXYZ(pDataXYZ);
                data_z[index] = pDataXYZ[2] * 0.000122074f;
                /*
                 * Transform to g by multiplying for (4.f / 32767.f) because the
                 * scale is from -4 to 4 g and because it has 16 bits.
                 */
                index++;
            }
        }
        #endif

        arm_cfft_f32(&arm_cfft_sR_f32_len1024, data_z, 0, 1);
        arm_cmplx_mag_f32(data_z, data_z_output, 1024);


        /* Send data through UART. */
        HAL_UART_Transmit(&huart2, (uint8_t *) data_z, BUFFER_LENGTH * 2,
            HAL_UART_STATE_TIMEOUT);
        
        /* Process data. */
        #endif
    }
}
