#ifndef LIBXBEE_XBEE_H
#define LIBXBEE_XBEE_H
#include <stdint.h>
#include <termios.h>

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

typedef struct xbee {
	int fd;
	const char* port;
	speed_t baudrate;
} xbee_t;

typedef struct xbee_frame {
	uint8_t type;
	uint8_t* data;
	uint16_t length;
} xbee_frame_t;

typedef struct xbee_data {
	uint16_t addr16;
	uint64_t addr64;
	uint8_t hops;
	uint8_t options;
	uint8_t* data;
	uint8_t length;
} xbee_data_t;

typedef struct xbee_transmit_status {
	uint8_t id;
	uint16_t addr16;
	uint8_t trials;
	uint8_t delivery_status;
	uint8_t discovery_status;
} xbee_transmit_status_t;

xbee_t *xbee_open(const char* port, const speed_t baudrate);
int xbee_close(xbee_t *xbee);

int xbee_send_frame(const xbee_t *xbee, const xbee_frame_t *frame);
xbee_frame_t *xbee_receive_frame(const xbee_t *xbee);
int xbee_free_frame(xbee_frame_t *frame);

int xbee_send_data(const xbee_t *xbee, const uint8_t id, const xbee_data_t* data);
int xbee_send(const xbee_t *xbee, const uint8_t id, const uint64_t addr64, const uint16_t addr16, const uint8_t hops, const uint8_t options, const uint8_t* data, const uint8_t length);
int xbee_free_data(xbee_data_t *data);

xbee_data_t *xbee_get_data(const xbee_frame_t *frame);
xbee_transmit_status_t *xbee_get_transmit_status(const xbee_frame_t *frame);

#endif

