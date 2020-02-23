#ifndef BOARD_SD_CARD_H
#define BOARD_SD_CARD_H

#include "sam.h"

typedef enum
{
	SD_DISCONNECTED,
	SD_CONNECTED
} board_sd_status;

void board_sd_card_config(void);

board_sd_status board_get_sd_card_status(void);

#endif