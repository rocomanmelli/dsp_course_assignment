#include "cmsis_os2.h"
#include "read_accel.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f401_discovery_accelerometer.h"

static ACCELERO_DrvTypeDef *AccelerometerDrv;
extern UART_HandleTypeDef huart2;

#define BUFFER_LENGTH 2000


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
    int16_t data_z[BUFFER_LENGTH];
    uint8_t new_data;
    uint16_t index;
    for(;;)
    {
        /* Data acquisition. */
        index = 0;
        while (index < BUFFER_LENGTH)
        {
            new_data = COMPASSACCELERO_IO_Read(ACC_I2C_ADDRESS,
                LSM303DLHC_STATUS_REG_A);
            if (new_data && 0x8)
            {
                BSP_ACCELERO_GetXYZ(pDataXYZ);
                data_z[index] = pDataXYZ[2];
                index++;
            }
        }

        /* Send data through UART. */
        HAL_UART_Transmit(&huart2, (uint8_t *) data_z, BUFFER_LENGTH * 2,
            HAL_UART_STATE_TIMEOUT);
        
        /* Process data. */
        
    }
}
