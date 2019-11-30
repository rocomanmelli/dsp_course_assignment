#include "cmsis_os2.h"
#include "read_accel.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f401_discovery_accelerometer.h"

static ACCELERO_DrvTypeDef *AccelerometerDrv;
extern UART_HandleTypeDef huart2;


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
  for(;;)
  {
    new_data = COMPASSACCELERO_IO_Read(ACC_I2C_ADDRESS, LSM303DLHC_STATUS_REG_A);
    if (new_data && 0x8){
      BSP_ACCELERO_GetXYZ(pDataXYZ);
      uint8_t hola[20];
      itoa(pDataXYZ[0], hola, 10);
      HAL_UART_Transmit(&huart2, hola, strlen(hola) + 1, HAL_UART_STATE_TIMEOUT);
    }
    else{
      pDataXYZ[0] = 0;
      pDataXYZ[1] = 0;
      pDataXYZ[2] = 0;
    }
    
  }
}
