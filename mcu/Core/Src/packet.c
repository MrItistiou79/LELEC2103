/*
 * packet.c
 */

#include "aes_ref.h"
#include "config.h"
#include "packet.h"
#include "main.h"
#include "utils.h"
#include "aes.h"

extern CRYP_HandleTypeDef hcryp; //iciiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii


const uint8_t AES_Key[16]  = {0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,
							0x00,0x00,0x00, 0x00,
							0x00, 0x00, 0x00, 0x00};




void tag_cbc_mac(uint8_t *tag, uint8_t *msg, size_t msg_len){
	//uint32_t statew[4] = {0};
	//uint8_t *state = (uint8_t*) statew;
	uint32_t statew[4] = {0};
	uint8_t *state = (uint8_t*) statew;

	size_t i;
	for (i=0; i < msg_len/16; i++) {
		for (int j=0; j<16; j++) {
			state[j] ^= msg[16*i+j];
		}
		//AES128_encrypt(state, AES_Key);
		// mettre fonction qui encrypt p.193
		// AL_StatusTypeDef HAL_CRYP_AESECB_Encrypt (CRYP_HandleTypeDef * hcryp, uint8_t * pPlainData, uint16_t Size, uint8_t * pCypherData, uint32_t Timeout)
		// state est un buffer qui va prendre le msg le transformer et le resortir mixé
		//printf("AES Encrypt cycles");

		//start_cycle_count(); //ça bug qd on fait ça !!
		HAL_CRYP_AESECB_Encrypt(&hcryp, state, 16, state, 0xFFFF);
		//stop_cycle_count("AES encrypt cycles");

		/*
		 * Parameters
		• hcryp: pointer to a CRYP_HandleTypeDef structure that contains the configuration information for CRYP
		module
		• pPlainData: Pointer to the plaintext buffer
		• Size: Length of the plaintext buffer in bytes, must be a multiple of 16.
		• pCypherData: Pointer to the cyphertext buffer
		• Timeout: Specify Timeout value
		 *
		 */
	}
	if (16*i != msg_len) {
		for (int j=0; j<msg_len-16*i; j++) {
			state[j] ^= msg[16*i+j];
		}
		//AES128_encrypt(state, AES_Key);
		//HAL_CRYP_AESECQsB_Encrypt (CRYP_HandleTypeDef * hcryp, state, msg_len/16, state, 0xFFFF);
		HAL_CRYP_AESECB_Encrypt(&hcryp, state, 16, state, 0xFFFF);
	}
	for (int j=0; j<16; j++) {
		tag[j] = state[j];
	}
}


/**
	uint8_t encrypt[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	for (int i = 0; i < msg_len; i++){
		printf("%u, ", *(msg+i));

	}
	printf("\n\r---------\n\r");
    HAL_CRYP_Init(&hcrypAES); //???????????????????????????????????????????????????????????????????


    HAL_CRYP_AESCBC_Encrypt(&hcrypAES, msg, 16, encrypt, 0xFFFF);
	for (int i = 0; i < msg_len; i++){
		printf("%u, ", *(msg+i));
	}
	printf("\n\r---------\n\r");

	for (int i = 0; i < 16; i++){
		printf("%u, ", *(encrypt+i));
	}
	printf("\n\r");
    for (int j=0; j<16; j++) {
        tag[j] = encrypt[j];
    }
}
**/

/*
Function name
HAL_StatusTypeDef HAL_CRYP_AESECB_Encrypt (CRYP_HandleTypeDef * hcryp, uint8_t * pPlainData,
uint16_t Size, uint8_t * pCypherData, uint32_t Timeout)

Function description
Encrypt pPlainData in AES ECB encryption mode.

Parameters
• hcryp: pointer to a CRYP_HandleTypeDef structure that contains the configuration information for CRYP
module
• pPlainData: Pointer to the plaintext buffer
• Size: Length of the plaintext buffer in bytes, must be a multiple of 16.
• pCypherData: Pointer to the cyphertext buffer
• Timeout: Specify Timeout value

Return values
• HAL: status

Notes
• This API is provided only to maintain compatibility with legacy software. Users should directly resort to
generic HAL_CRYPEx_AES() API instead (usage recommended).
HAL_CRYP_AESECB_Decrypt
*/






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

