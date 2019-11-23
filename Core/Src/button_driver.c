#include "cmsis_os2.h"
#include "button_driver.h"

osEventFlagsId_t evt_id;

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
void InitButtonTask()
{
  evt_id = osEventFlagsNew(NULL);
}

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
void ButtonTask(void *argument)
{
    uint32_t flags;
 
  while (1) {
    flags = osEventFlagsWait(evt_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
    //handle event
  }
}