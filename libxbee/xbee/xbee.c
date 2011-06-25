#include "xbee/xbee.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

xbee_t *xbee_open(const char* port, const speed_t baudrate)
{
	int fd = open(port, O_RDWR | O_NOCTTY);
	if(fd < 0) return NULL;

	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag = CLOCAL | CREAD | CS8;
	options.c_iflag = 0;
	options.c_oflag = 0;
	options.c_lflag = 0;
	cfsetispeed(&options, baudrate);
	cfsetospeed(&options, baudrate);
	tcsetattr(fd, TCSANOW, &options);

	xbee_t *xbee = (xbee_t*)malloc(sizeof(xbee_t));
	xbee->fd = fd;
	xbee->port = port;
	xbee->baudrate = baudrate;
	return xbee;
}

int xbee_close(xbee_t *xbee)
{
	if(!xbee) return 1;
	if(xbee->fd >= 0) close(xbee->fd);
	free(xbee);
	return 0;
}


int xbee_send_frame(const xbee_t *xbee, const xbee_frame_t *frame)
{
	if (xbee == NULL || xbee->fd < 0 || frame == NULL)
	{
		return 1;
	}

	uint8_t *buf = (uint8_t*)malloc(sizeof(uint8_t)*(5+frame->length));
	if ( buf == NULL )
	{
		return 1;
	}

	buf[0] = XBEE_START_DELIMITER;
	buf[1] = ((frame->length + 1) >> 8) & 0xff;
	buf[2] = (frame->length + 1) & 0xff;
	uint8_t checksum = 0;	
	buf[3] = frame->type;
	checksum += buf[3];
	uint16_t i = 0;
	for (i = 0; i < frame->length; ++i)
	{
		buf[4 + i] = frame->data[i];
		checksum += buf[4 + i];
	}
	buf[4 + frame->length] = 0xff - checksum;

	size_t size = 0;
	ssize_t w;
	while (size < 5 + frame->length)
	{
		w = write(xbee->fd, buf + size, 5 + frame->length - size);
		if (w < 0) break;
		size += w;
	}
	free(buf);
	return 0;
}

xbee_frame_t *xbee_receive_frame(const xbee_t *xbee) {
	if (xbee == NULL || xbee->fd < 0) return NULL;

	xbee_frame_t* frame;
	ssize_t r;
	uint8_t buf[3];
	size_t size;
	uint8_t checksum;
	uint16_t i;

	for(;;) {
        
		// look for star delimiter
		for(;;) {
			r = read(xbee->fd, buf, 1);
			if(r < 0) return NULL;
			if(r == 1 && buf[0] == XBEE_START_DELIMITER) break;
		}

		// get size and type
		size = 0;
		while(size < 3) {
			r = read(xbee->fd, buf + size, 3 - size);
			if(r < 0) return NULL;
			size += r;
		}
		frame = (xbee_frame_t*)malloc(sizeof(xbee_frame_t));
		frame->length = ((buf[0] << 8) & 0xff00) | (buf[1] & 0xff);
		if (frame->length < 1) {
			free(frame);
			return NULL;
		}
		frame->length--;
		checksum = 0;
		frame->type = buf[2]; // type
		checksum += frame->type;

		// get the data part
		size = 0;
		frame->data = (uint8_t*)malloc(sizeof(uint8_t)*frame->length);

		while(size < frame->length) {
			r = read(xbee->fd, frame->data + size, frame->length - size);
			if(r < 0) {
				xbee_free_frame(frame);
				return NULL;
			}
			size += r;
		}
		for(i=0;i<frame->length;++i)
			checksum += frame->data[i]; // add up the checksum

		// get checksum
		for(;;) {
			r = read(xbee->fd, buf, 1);
			if(r < 0) {
				xbee_free_frame(frame);
				return NULL;
			}
			if(r == 1) break;
		}
		checksum += buf[0]; // checksum
		if(checksum == 0xff) // check checksum
			return frame;
		xbee_free_frame(frame);
	}
	return NULL;
}

int xbee_free_frame(xbee_frame_t *frame) {
	if(!frame) return 1;
	if(frame->data) free(frame->data);
	free(frame);
	return 0;
}


int xbee_free_data(xbee_data_t *data) {
	if(!data) return 1;
	if(data->data) free(data->data);
	free(data);
	return 0;
}

int xbee_send(const xbee_t *xbee, const uint8_t id, const uint64_t addr64, const uint16_t addr16, const uint8_t hops, const uint8_t options, const uint8_t* data, const uint8_t length) {
	if(xbee == NULL || xbee->fd < 0 || data == NULL) return 1;
	xbee_frame_t *frame = (xbee_frame_t*)malloc(sizeof(xbee_frame_t));
	frame->type = XBEE_ZIGBEE_TRANSMIT_REQUEST_FRAME;
	frame->length = length + 13;
	frame->data = (uint8_t*)malloc(sizeof(uint8_t)*frame->length);
	frame->data[0] = id;
	frame->data[1] = (addr64 >> 56) & 0xff; // msb
	frame->data[2] = (addr64 >> 48) & 0xff;
	frame->data[3] = (addr64 >> 40) & 0xff;
	frame->data[4] = (addr64 >> 32) & 0xff;
	frame->data[5] = (addr64 >> 24) & 0xff;
	frame->data[6] = (addr64 >> 16) & 0xff;
	frame->data[7] = (addr64 >> 8) & 0xff;
	frame->data[8] = addr64 & 0xff; // lsb
	frame->data[9] = (addr16 >> 8) & 0xff; // msb
	frame->data[10] = addr16 & 0xff; // lsb
	frame->data[11] = hops; // hops
	frame->data[12] = options & 0x08; // options
	int i;
	for(i=0;i<length;++i) // data
		frame->data[i+13] = data[i];
	xbee_send_frame(xbee, frame);
	xbee_free_frame(frame);
	return 0;
}

int xbee_send_data(const xbee_t *xbee, const uint8_t id, const xbee_data_t* data) {
	if(xbee == NULL || xbee->fd < 0 || data == NULL) return 1;
	return xbee_send(xbee, id, data->addr64, data->addr16, data->hops, data->options, data->data, data->length);
}

xbee_data_t *xbee_get_data(const xbee_frame_t *frame) {
	if(frame == NULL) return NULL;
	if(frame->type != XBEE_ZIGBEE_RECEIVE_PACKET_FRAME) return NULL;
	if(frame->length < 11) return NULL;
	xbee_data_t* data = (xbee_data_t*)malloc(sizeof(xbee_data_t));
	data->addr64 = 0;
	data->addr64 |= ((uint64_t)frame->data[0] << 56);
	data->addr64 |= ((uint64_t)frame->data[1] << 48);
	data->addr64 |= ((uint64_t)frame->data[2] << 40);
	data->addr64 |= ((uint64_t)frame->data[3] << 32);
	data->addr64 |= ((uint64_t)frame->data[4] << 24);
	data->addr64 |= ((uint64_t)frame->data[5] << 16);
	data->addr64 |= ((uint64_t)frame->data[6] << 8);
	data->addr64 |= ((uint64_t)frame->data[7]);
	data->addr16 = 0;
	data->addr16 |= ((uint16_t)frame->data[8] << 8);
	data->addr16 |= ((uint16_t)frame->data[9]);
	data->options = frame->data[10] & 0x03;
	data->length = frame->length - 11;
	data->hops = 0;
	data->data = (uint8_t*)malloc(sizeof(uint8_t)*data->length);
	uint16_t i;
	for(i=0;i<data->length;++i)
		data->data[i] = frame->data[i+11];
	return data;
}

xbee_transmit_status_t *xbee_get_transmit_status(const xbee_frame_t *frame) {
	if(frame == NULL) return NULL;
	if(frame->type != XBEE_ZIGBEE_TRANSMIT_STATUS_FRAME) return NULL;
	if(frame->length != 6) return NULL;
	xbee_transmit_status_t* status = (xbee_transmit_status_t*)malloc(sizeof(xbee_transmit_status_t));
	status->id = frame->data[0];
	status->addr16 = 0;
	status->addr16 |= ((uint16_t)frame->data[1] << 8);
	status->addr16 |= ((uint16_t)frame->data[2]);
	status->trials = frame->data[3];
	status->delivery_status = frame->data[4];
	status->discovery_status = frame->data[5];
	return status;
}
