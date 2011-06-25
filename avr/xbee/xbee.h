#ifndef AVR_ZIYAN_XBEE_H
#define AVR_ZIYAN_XBEE_H

#define XBEE_MODEM_STATUS_FRAME 0x8A
#define XBEE_AT_COMMAND_FRAME 0x08
#define XBEE_AT_COMMAND_QUERY_PARAMETER_VALUE_FRAME 0x09
#define XBEE_AT_COMMAND_RESPONSE_FRAME 0x88
#define XBEE_REMOTE_COMMAND_REQUEST_FRAME 0x17
#define XBEE_REMOTE_COMMAND_RESPONSE_FRAME 0x97
#define XBEE_ZIGBEE_TRANSMIT_REQUEST_FRAME 0x10
#define XBEE_EXPLICIT_ADDRESSING_ZIGBEE_COMMAND_FRAME 0x11
#define XBEE_ZIGBEE_TRANSMIT_STATUS_FRAME 0x8B
#define XBEE_ZIGBEE_RECEIVE_PACKET_FRAME 0x90
#define XBEE_ZIGBEE_EXPLICIT_RX_INDICATOR_FRAME 0x91
#define XBEE_ZIGBEE_IO_DATA_SAMPLE_RX_INDICATOR_FRAME 0x92
#define XBEE_SENSOR_READ_INDICATOR_FRAME 0x94
#define XBEE_NODE_IDENTIFICATION_INDICATOR_FRAME 0x95

#define XBEE_START_DELIMITER 0x7E
#define XBEE_MAX_DATA_LENGTH 72

#include <inttypes.h>

typedef struct {
	uint8_t type;
	uint8_t* data;
	uint16_t length;
} xbee_frame_t;

typedef struct {
	uint64_t addr64;
	uint16_t addr16;
	uint8_t options;
	uint8_t* data;
	uint8_t length;
} xbee_data_t;

/**
 * Send a raw frame
 */
void xbee_send_frame(xbee_frame_t* frame);

/**
 * Receive a raw frame (blocking)
 */
xbee_frame_t *xbee_receive_frame(void);

/**
 * Free a frame
 */
void xbee_free_frame(xbee_frame_t* frame);

uint16_t xbee_send(uint8_t frame_id, uint64_t addr64, uint16_t addr16, uint8_t options, uint8_t* data, uint8_t length);
uint16_t xbee_send_data(uint8_t frame_id, xbee_data_t* data);
void xbee_free_data(xbee_data_t* data);
xbee_data_t *xbee_receive_data(void);
xbee_data_t *xbee_get_data(xbee_frame_t* frame);

#endif
