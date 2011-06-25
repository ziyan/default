#include "hardware.h"

#include <stdio.h>
#include <stdlib.h>
#include "xbee.h"

void xbee_send_frame(xbee_frame_t* frame) {
	putchar(XBEE_START_DELIMITER); // 0x7E
	putchar(((frame->length + 1) >> 8) & 0xff); // MSB
	putchar((frame->length + 1) & 0xff); // LSB
	uint8_t checksum = 0;
	checksum += frame->type & 0xff;
	putchar(frame->type); // ID
	uint16_t i;
	for(i=0;i<frame->length;++i) {
		checksum += frame->data[i] & 0xff;
		putchar(frame->data[i]);
	}
	putchar(0xff-checksum); // checksum
};

xbee_frame_t *xbee_receive_frame(void) {
	while(getchar() != XBEE_START_DELIMITER) ; // 0x7E
	xbee_frame_t* frame = (xbee_frame_t*)malloc(sizeof(xbee_frame_t));
	frame->length = (getchar() << 8) & 0xff00; // MSB
	frame->length |= getchar() & 0xff; // LSB
	if (frame->length < 1) {
		free(frame);
		return NULL;
	}
	frame->length --;
	uint8_t checksum = 0;
	frame->type = getchar(); // ID
	checksum += frame->type & 0xff;
	frame->data = (uint8_t*)malloc(sizeof(uint8_t)*frame->length);
	uint16_t i;
	for(i=0;i<frame->length;++i) {
		frame->data[i] = getchar() & 0xff;
		checksum += frame->data[i];
	}
	checksum += getchar() & 0xff; // checksum
	if(checksum == 0xff) // check checksum
		return frame;
	xbee_free_frame(frame);
	return NULL;
}

void xbee_free_frame(xbee_frame_t* frame) {
	free(frame->data);
	free(frame);
}

uint16_t xbee_send(uint8_t frame_id, uint64_t addr64, uint16_t addr16, uint8_t options, uint8_t* data, uint8_t length) {
	xbee_frame_t* frame = (xbee_frame_t*)malloc(sizeof(xbee_frame_t));
	frame->type = XBEE_ZIGBEE_TRANSMIT_REQUEST_FRAME;
	frame->length = length + 13;
	frame->data = (uint8_t*)malloc(sizeof(uint8_t)*frame->length);
	frame->data[0] = frame_id; // frame id
	frame->data[1] = (addr64 >> 56) & 0xff; // msb
	frame->data[2] = (addr64 >> 48) & 0xff;
	frame->data[3] = (addr64 >> 40) & 0xff;
	frame->data[4] = (addr64 >> 32) & 0xff;
	frame->data[5] = (addr64 >> 24) & 0xff;
	frame->data[6] = (addr64 >> 16) & 0xff;
	frame->data[7] = (addr64 >> 8) & 0xff;
	frame->data[8] = (addr64) & 0xff; // lsb
	frame->data[9] = (addr16 >> 8) & 0xff; // msb
	frame->data[10] = (addr16) & 0xff; // lsb
	frame->data[11] = 0; // hops
	frame->data[12] = options & 0x08; // options
	uint16_t i;
	for(i=0;i<length;++i) // data
		frame->data[i+13] = data[i];
	xbee_send_frame(frame);
	xbee_free_frame(frame);
	if (frame_id==0) return 0xFFFF;
	frame = xbee_receive_frame();
	if(frame==NULL) return 0xFFFF;
	if(frame->type == XBEE_ZIGBEE_TRANSMIT_STATUS_FRAME &&
		frame->length == 6 &&
		frame->data[0] == frame_id &&
		frame->data[4] == 0) {
		addr16 = (frame->data[1] << 8) & 0xff;
		addr16 |= (frame->data[2]) & 0xff;
		xbee_free_frame(frame);
		return addr16;
	}
	xbee_free_frame(frame);
	return 0xFFFF;
}


uint16_t xbee_send_data(uint8_t frame_id, xbee_data_t* data) {
	return xbee_send(frame_id, data->addr64, data->addr16, data->options, data->data, data->length);
}
void xbee_free_data(xbee_data_t* data) {
	free(data->data);
	free(data);
}
xbee_data_t *xbee_receive_data(void) {
	xbee_frame_t *frame = xbee_receive_frame();
	xbee_data_t *data = xbee_get_data(frame);
	xbee_free_frame(frame);
	return data;
}
xbee_data_t *xbee_get_data(xbee_frame_t* frame) {
	if(frame == NULL) return NULL;
	if(frame->type != XBEE_ZIGBEE_RECEIVE_PACKET_FRAME) return NULL;
	if(frame->length < 11) return NULL;
	xbee_data_t* data = (xbee_data_t*)malloc(sizeof(xbee_data_t));
	data->addr64 = 0;
	data->addr64 |= ((uint64_t)frame->data[0] << 56) 	& 0xff00000000000000UL;
	data->addr64 |= ((uint64_t)frame->data[1] << 48) 	& 0x00ff000000000000UL;
	data->addr64 |= ((uint64_t)frame->data[2] << 40) 	& 0x0000ff0000000000UL;
	data->addr64 |= ((uint64_t)frame->data[3] << 32) 	& 0x000000ff00000000UL;
	data->addr64 |= ((uint64_t)frame->data[4] << 24) 	& 0x00000000ff000000UL;
	data->addr64 |= ((uint64_t)frame->data[5] << 16) 	& 0x0000000000ff0000UL;
	data->addr64 |= (frame->data[6] << 8) 			& 0x000000000000ff00UL;
	data->addr64 |= (frame->data[7]) 			& 0x00000000000000ffUL;
	data->addr16 = 0;
	data->addr16 |= (frame->data[8] << 8) 	& 0xff00U;
	data->addr16 |= (frame->data[9]) 	& 0x00ffU;
	data->options = frame->data[10] & 0x03;
	data->length = frame->length - 11;
	data->data = (uint8_t*)malloc(sizeof(uint8_t)*data->length);
	uint16_t i;
	for(i=0;i<data->length;++i)
		data->data[i] = frame->data[i+11];
	return data;
}

