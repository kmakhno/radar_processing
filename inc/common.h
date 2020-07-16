#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

/* Parameters for changing sweep period of sawtooth signal */
#define TSWEEP_100mcs  0U
#define TSWEEP_50mcs   1U
#define TSWEEP_33_5mcs 2U
#define TSWEEP_25mcs   4U
/***********************************************************/

/* Parameters for changing bandwidth of LFM */
#define BANDWIDTH_1GHz   0U
#define BANDWIDTH_500MHz 1U
#define BANDWIDTH_250MHz 2U
/********************************************/

/* Commands from PC */
#define START_CMD 128U
#define STOP_CMD  255U
/********************/

struct settings
{
	uint8_t tsweep;
	uint8_t bandwidth;
};

struct packets
{
	uint8_t cmd;
	struct settings stg;
};

#endif /* COMMON_H */
