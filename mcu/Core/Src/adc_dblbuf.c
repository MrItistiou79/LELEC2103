#include <adc_dblbuf.h>
#include "config.h"
#include "main.h"
#include "spectrogram.h"
#include "arm_math.h"
#include "utils.h"
#include "s2lp.h"
#include "packet.h"


static volatile uint16_t ADCDoubleBuf[2*ADC_BUF_SIZE]; /* ADC group regular conversion data (array of data) */ //ADCDoubleBuf[2*ADC_BUF_SIZE] avec ADC_BUF_SIZE = SAMPLES_PER_MELVEC = 512
static volatile uint16_t* ADCData[2] = {&ADCDoubleBuf[0], &ADCDoubleBuf[ADC_BUF_SIZE]};
static volatile uint8_t ADCDataRdy[2] = {0, 0};

static volatile uint8_t cur_melvec = 0;
static q15_t mel_vectors[N_MELVECS][MELVEC_LENGTH];

static uint32_t packet_cnt = 0;

static volatile int32_t rem_n_bufs = 0;

int StartADCAcq(int32_t n_bufs) {
	rem_n_bufs = n_bufs;
	cur_melvec = 0;
	if (rem_n_bufs != 0) {
		return HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADCDoubleBuf, 2*ADC_BUF_SIZE);
	} else {
		return HAL_OK;
	}
}

int IsADCFinished(void) {
	return (rem_n_bufs == 0);
}

static void StopADCAcq() {
	HAL_ADC_Stop_DMA(&hadc1);
}

static void print_spectrogram(void) {
#if (DEBUGP == 1)
	start_cycle_count();
	DEBUG_PRINT("Acquisition complete, sending the following FVs\r\n");
	for(unsigned int j=0; j < N_MELVECS; j++) {
		DEBUG_PRINT("FV #%u:\t", j+1);
		for(unsigned int i=0; i < MELVEC_LENGTH; i++) {
			DEBUG_PRINT("%.2f, ", q15_to_float(mel_vectors[j][i]));
		}
		DEBUG_PRINT("\r\n");
	}
	stop_cycle_count("Print FV");
#endif
}

static void print_encoded_packet(uint8_t *packet) {
#if (DEBUGP == 1)
	char hex_encoded_packet[2*PACKET_LENGTH+1];
	hex_encode(hex_encoded_packet, packet, PACKET_LENGTH);
	DEBUG_PRINT("DF:HEX:%s\r\n", hex_encoded_packet);
#endif
}

static void encode_packet(uint8_t * packet, uint32_t * packet_cnt) {
	// BE encoding of each mel coef
	for (size_t i=0; i<N_MELVECS; i++) {
		for (size_t j=0; j<MELVEC_LENGTH; j++) {
			(packet+PACKET_HEADER_LENGTH)[(i*MELVEC_LENGTH+j)*2]   = mel_vectors[i][j] >> 8;
			(packet+PACKET_HEADER_LENGTH)[(i*MELVEC_LENGTH+j)*2+1] = mel_vectors[i][j] & 0xFF;
		}
	}
	// Write header and tag into the packet.
	make_packet(packet, PAYLOAD_LENGTH, 0, *packet_cnt);
	*packet_cnt += 1;
	if (*packet_cnt == 0) {
		// Should not happen as packet_cnt is 32-bit and we send at most 1 packet per second.
		DEBUG_PRINT("Packet counter overflow.\r\n");
		Error_Handler();
	}
}



static void send_spectrogram() {
	uint8_t packet[PACKET_LENGTH];

	//code signal energy threshold -------------------------------------------------------------------------------------- ici
	//int sum = 0;
	//for (size_t i=0; i<N_MELVECS; i++) {
			//for (size_t j=0; j<MELVEC_LENGTH; j++) {
				//sum += (mel_vectors[i][j])^2; //on additionne le carré de chaque sample de la matrice
			//}
	//}
	//if (sum > 2000){
	start_cycle_count();
	encode_packet(packet, &packet_cnt);
	stop_cycle_count("Encode packet");
	//print_encoded(packet)
	start_cycle_count();

	// put antenna in mode sleep when not receiving or sending packet
	S2LP_WakeUp(); // wake up the antenna iciiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii voir s2lp.c fonction que les tuteurs ont fait for put
	S2LP_Send(packet, PACKET_LENGTH);
	S2LP_Sleep(); //sleep the antenna

	stop_cycle_count("Send packet");

	print_encoded_packet(packet);
	//}
}


/*
static void send_spectrogram() {
	uint8_t packet[PACKET_LENGTH];

	start_cycle_count();
	encode_packet(packet, &packet_cnt);
	stop_cycle_count("Encode packet");

	start_cycle_count();
	S2LP_Send(packet, PACKET_LENGTH);
	stop_cycle_count("Send packet");

	print_encoded_packet(packet);
}
*/


// Function for calculating /iciiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
int n = 2*ADC_BUF_SIZE;
static float variance(uint16_t a[n]) { //NB : (Ne pas calculer toutle vecteur mais bien que au mileiu par ex (voir schéma Balaz)

	// Compute mean (average of elements)
	double sum = 0;
	for (int i = 0; i < n; i++){
		sum += a[i];
	}
	double mean = (double)sum / (double) n;

	//compute sum squared differences with mean
	double sqDiff = 0;
	for (int i = 0; i<n;i++) {
		sqDiff+= (a[i] -mean ) * (a[i] - mean);

	}
	return (float)sqDiff / n;

}

/*
static void threshold_depasse(int threshold){
	int variance = 0;
	for (int i = 0; i<2*ADC_BUF_SIZE; i++){
		variance +=
	}
}
*/

static void ADC_Callback(int buf_cplt) { //on remplit la moitié du buffer et pendant que l'autre est traité alors on remplit l'autre pour pas perdre de temps
	if (rem_n_bufs != -1) {
		rem_n_bufs--;
	}
	if (rem_n_bufs == 0) {
		StopADCAcq();
	} else if (ADCDataRdy[1-buf_cplt]) {
		DEBUG_PRINT("Error: ADC Data buffer full\r\n");
		Error_Handler();
	}
	ADCDataRdy[buf_cplt] = 1;

	printf("%d \n", variance(ADCData[buf_cplt]));
	//printf("\n");
	if (variance(ADCData[buf_cplt]) > 8080000){ //iciiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii 10000000 passe pas et 1000000 passe : ok for chainsaw, handsaw, helicopter,
		//start_cycle_count();
		Spectrogram_Format((q15_t *)ADCData[buf_cplt]);
		Spectrogram_Compute((q15_t *)ADCData[buf_cplt], mel_vectors[cur_melvec]);

		cur_melvec++;
		//printf(cur_melvec);

		//stop_cycle_count("spectrogram");
		//ADCDataRdy[buf_cplt] = 0;

		if (rem_n_bufs == 0) {
			print_spectrogram();
			send_spectrogram();
		}
		ADCDataRdy[buf_cplt] = 0;

	} else {
		ADCDataRdy[buf_cplt] = 0;
	}


}


//extern uint8_t have_to_record; // variable qui va dire de record quand est mise à 1 et prend information

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//sinon ici en fonction du buffer qui est rempli ADCDoubleBuf[2*ADC_BUF_SIZE]
	//if (ADCDoubleBuf[2*ADC_BUF_SIZE] ){

	//if(have_to_record){
	ADC_Callback(1);
		//printf("1");}
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	//if(have_to_record)
	ADC_Callback(0);
		//printf("0);}
}


//TOKEN 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
