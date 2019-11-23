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
    uint32_t all_flags, last_flag;
 
    while (1)
    {
        /* To capture whatever event was received */
        all_flags = osEventFlagsWait(evt_id, FLAGS_MSK, osFlagsWaitAny,
            osWaitForever);

        /*
         * To be able to handle other events before debouncing time for the
         * first received event finishes
         */
        while (1)
        {
            /*
             * Wait for an event from another button or the timeout to check if
             * the first pressed button is still pressed
             */ 
            last_flag = osEventFlagsWait(evt_id, FLAGS_MSK, osFlagsWaitAny,
                100);

            /*
             * If timeout happened, check every button that previously sent an
             * event and then get out of the inner while (1). Normal operation
             * should most times led to this if.
             */
            if (last_flag == osFlagsErrorTimeout)
            {
                if ((all_flags & FLAGS_MSK1) &&
                    HAL_GPIO_ReadPin(pulsador_1_GPIO_Port, pulsador_1_Pin))
                {
                    /* Do something */
                    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
                }
                if ((all_flags & FLAGS_MSK2) &&
                    HAL_GPIO_ReadPin(pulsador_2_GPIO_Port, pulsador_2_Pin))
                {
                    /* Do something */
                    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
                }
                if ((all_flags & FLAGS_MSK3) &&
                    HAL_GPIO_ReadPin(pulsador_3_GPIO_Port, pulsador_3_Pin))
                {
                    /* Do something */
                    HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
                }
                if ((all_flags & FLAGS_MSK4) &&
                    HAL_GPIO_ReadPin(pulsador_4_GPIO_Port, pulsador_4_Pin))
                {
                    /* Do something */
                    HAL_GPIO_TogglePin(LD6_GPIO_Port, LD6_Pin);
                }
                all_flags = 0;
                break;
            }
            /*
             * If another valid event happened (that is why a comparison
             * against higher FLAG_MSK used + 1 is done) then it is added to
             * all_flags and the wait time to debounce starts again when
             * execution comes back to the start of the inner while
             */
            else if (last_flag <= (FLAGS_MSK4 + 1))
            {
                all_flags |= last_flag;
            }
            else
            {
                /*
                 * Execution should never gets here unless something really
                 * bad happened with the CMSIS event module. Don't know what
                 * osFlagsErrorUnknown means.
                 */
                while (1);
            }
            
        }
    }
}