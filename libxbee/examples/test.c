#include <stdio.h>
#include <stdlib.h>
#include "xbee/xbee.h"

void dump_frame(xbee_frame_t *frame)
{
    uint16_t i = 0;
    printf("\tType: 0x%02x\n", frame->type);
    printf("\tLength: 0x%04x\n", frame->length);
    printf("\tData:");
    for (i = 0; i < frame->length; i++)
    {
        printf(" %02x", frame->data[i]);
    }
    printf("\n");
}

void dump_data(xbee_data_t *data)
{
    uint8_t i = 0;
    printf("\t16-bit Address: 0x%04x\n", data->addr16);
    printf("\t64-bit Address: 0x%016lx\n", data->addr64);
    printf("\tHops: 0x%02x\n", data->hops);
    printf("\tOptions: 0x%02x\n", data->options);
    printf("\tLength: 0x%02x\n", data->length);
    printf("\tData:");
    for (i = 0; i < data->length; i++)
    {
        printf(" %02x", data->data[i]);
    }
    printf("\n");
}


void dump_transmit_status(xbee_transmit_status_t *status)
{
    printf("\tId: 0x%02x\n", status->id);
    printf("\t16-bit Address: 0x%04x\n", status->addr16);
    printf("\tTrials: 0x%02x\n", status->trials);
    printf("\tDelivery Status: 0x%02x\n", status->delivery_status);
    printf("\tDiscovery Status: 0x%02x\n", status->discovery_status);
}

int main(int argc, char **argv)
{
	// 7e 00 1c 10 01 00 00 00 00 00 00 ff ff ff fe 00 00 48 65 6c 6c 6f 20 57 6f 72 6c 64 21 0d 0a 9f
    
    char buffer[72];
    memset(buffer, 0, 72);

	xbee_t *xbee = xbee_open(argv[1], B9600);
	printf("Xbee connected at %s %i\n", xbee->port, xbee->baudrate);

    uint16_t addr16 = 0xFFFEU;
    uint64_t addr64 = 0x0000FFFFUL;
    uint8_t id = 1;

	xbee_frame_t *frame = NULL;
    xbee_transmit_status_t *status = NULL;
	xbee_data_t *data = NULL;
    xbee_data_t sdata;
	for(;;)
	{
        sprintf(buffer, "Time is now: %x", time());
        printf("Sending unicast packet: %s\n", buffer);
        sdata.addr16 = addr16;
        sdata.addr64 = addr64;
        sdata.hops = 0;
        sdata.options = 0;
        sdata.data = (const uint8_t*)buffer;
        sdata.length = strlen(buffer) + 1;
        dump_data(&sdata);
        
	if (!xbee_send_data(xbee, id, &sdata))
        {
            frame = xbee_receive_frame(xbee);
            if(frame)
            {
                status = xbee_get_transmit_status(frame);
                if (status)
                {
                    printf("Delivery status:\n");
                    dump_transmit_status(status);
                    if (status->id == id) id += 2;
                }
                xbee_free_frame(frame);
                frame = NULL;
            }
        }

        printf("\nListening for incoming packets ...\n");
        frame = xbee_receive_frame(xbee);
        if (frame)
        {
            printf("Received frame:\n");
            dump_frame(frame);
            data = xbee_get_data(frame);
            if (data)
            {
                printf("Frame is data:\n");
                dump_data(data);
                //addr16 = data->addr16;
                //addr64 = data->addr64;
            }
            xbee_free_frame(frame);
            frame = NULL;
        }

        
	}
	return 0;
}

