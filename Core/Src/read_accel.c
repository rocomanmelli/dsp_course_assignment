#include "cmsis_os2.h"
#include "read_accel.h"
#include "main.h"
#include "stm32f401_discovery_accelerometer.h"


void InitReadAccelTask()
{
	/* Take into account that sometimes it can happen that
	 * the accelerometer gets stuck so the driver says that
	 * the I2C is busy. In this case, a power off - power on
	 * must be performed.*/
    BSP_ACCELERO_Init();
}


void ReadAccelTask(void *argument)
{
    int16_t pDataXYZ[3];
  for(;;)
  {
    osDelay(1);
    BSP_ACCELERO_GetXYZ(pDataXYZ);
  }
}
