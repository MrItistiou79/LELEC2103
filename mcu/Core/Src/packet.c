/*
 * packet.c
 */

#include "aes_ref.h"
#include "config.h"
#include "packet.h"
#include "main.h"
#include "utils.h"

const uint8_t AES_Key[16]  = {
                            0x01,0x02,0x03,0x04,
							0x05,0x06,0x07,0x08,
							0x09,0x0A,0x0B,0x0C,
							0x0D,0x0E,0x0F,0x00};

void tag_cbc_mac(uint8_t *tag, const uint8_t *msg, size_t msg_len) {
	uint32_t statew[4] = {0};
	uint8_t *state = (uint8_t*) statew;
    size_t i;
    for (i=0; i < msg_len/16; i++) {
        for (int j=0; j<16; j++) {
            state[j] ^= msg[16*i+j];
        }
        AES128_encrypt(state, AES_Key);
    }
    if (16*i != msg_len) {
        for (int j=0; j<msg_len-16*i; j++) {
            state[j] ^= msg[16*i+j];
        }
        AES128_encrypt(state, AES_Key);
    }
    for (int j=0; j<16; j++) {
        tag[j] = state[j];
    }
}

// Assumes payload is already in place in the packet
int make_packet(uint8_t *packet, size_t payload_len, uint8_t sender_id, uint32_t serial) {
    size_t packet_len = payload_len + PACKET_HEADER_LENGTH + PACKET_TAG_LENGTH;
    packet[0] = 0;
    packet[1] = sender_id;
    packet[2] = payload_len >> 8;
    packet[3] = payload_len & 0xFF;
    packet[4] = (serial >> 24) & 0xFF;
    packet[5] = (serial >> 16) & 0xFF;
    packet[6] = (serial >> 8) & 0xFF;
    packet[7] = serial & 0xFF;

    tag_cbc_mac(packet + payload_len + PACKET_HEADER_LENGTH, packet, payload_len + PACKET_HEADER_LENGTH);

    return packet_len;
}

