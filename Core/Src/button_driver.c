#include "cmsis_os2.h"
#include "button_driver.h"
#include "main.h"

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
 
    while (1)
    {
        flags = osEventFlagsWait(evt_id, FLAGS_MSK, osFlagsWaitAny,
            osWaitForever);

        while (1)
        {
            flags = osEventFlagsWait(evt_id, FLAGS_MSK, osFlagsWaitAny, 50);
            if (flags == osFlagsErrorTimeout)
            {
                if (HAL_GPIO_ReadPin(pulsador_1_GPIO_Port, pulsador_1_Pin))
                {
                    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
                    break;
                }
            }

        }
    }
}