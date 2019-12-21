#define DWT_CTRL_ADDRESS (0xE0001000)
#define DWT_CYCCNT_ADDRESS (0xE0001004)

#include "stdint.h"

void dwt_init(void);
void dwt_reset(void);
uint32_t dwt_get_count(void);