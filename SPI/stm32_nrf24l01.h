
#ifndef	NRF24L01_H
#define NRF24L01_H 100

#ifdef __cplusplus
extern "C" {
#endif

/*
GND			GND			Ground
VCC			3.3V		3.3V
CE			PD8			RF activated pin
CSN			PD7			Chip select pin for SPI
SCK			PB12		SCK pin for SPI
MOSI		PC12		MOSI pin for SPI
MISO		PC11		MISO pin for SPI
IRQ			Not used	Interrupt pin. Goes low when active. Pin functionality is active, but not used in library
*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"

SPI_HandleTypeDef hspi1;
	
/* Default SPI used */
#ifndef	NRF24L01_SPI
#define NRF24L01_SPI	hspi1
#endif

/* SPI chip enable pin */
#ifndef	NRF24L01_CSN_PIN
#define NRF24L01_CSN_PORT	GPIOC
#define NRF24L01_CSN_PIN	GPIO_PIN_6
#endif	

/* SPI enable for transmitting */
#ifndef	NRF24L01_CE_PIN
#define NRF24L01_CE_PORT	GPIOC
#define NRF24L01_CE_PIN		GPIO_PIN_7
#endif

/* SPI configuration */
#define NRF24L01_CE_LOW		HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_RESET)
#define NRF24L01_CE_HIGH	HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_SET)
#define	NRF24L01_CSN_LOW	HAL_GPIO_WritePin(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN, GPIO_PIN_RESET)
#define	NRF24L01_CSN_HIGH	HAL_GPIO_WritePin(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN, GPIO_PIN_SET)


/* Interrupt masks */
#define NRF24L01_IRQ_DATA_READY		0x40		/* Data ready for receive */
#define NRF24L01_IRQ_TRAN_OK		0x20		/* Transmission went OK */
#define NRF24L01_IRQ_MAX_RT			0x10		/* Max retransmissions reached, last transmission failed */

/* Interrupt Structure */
typedef union _NRF24L01_IRQ_t {
	struct {
		uint8_t reserved0:4;
		uint8_t MaxRT:1;			/* Set to 1 if MAX retransmissions flag is set*/
		uint8_t DataSent:1;			/* Set to 1 if last transmission is OK*/
		uint8_t DataReady:1;		/* Set to 1 if data are ready to be read */
		uint8_t reserved1:1;		
	} F;
	uint8_t Status;					/* NRF status register value */
} NRF24L01_IRQ_t;


/* Transmission status enumeration */
typedef enum _NRF24L01_Transmit_Status_t {
	NRF24L01_Transmit_Status_Lost = 0x00,		/* Message is lost, reached maximum number of retransmissions */
	NRF24L01_Transmit_Status_OK  = 0x01,     	/* Message sent successfully */
	NRF24L01_Transmit_Status_Sending = 0xFF, 	/* Message is still sending */
}	NRF24L01_Transmit_Status_t;

/* Data rate enumeration */
typedef enum _NRF24L01_DataRate_t  {
	NRF24L01_DataRate_2M = 0x00,		/* Data rate set to 2Mbps */
	NRF24L01_DataRate_1M,
	NRF24L01_DataRate_25k,
}	NRF24L01_DataRate_t;

/* Output power enumration */
typedef enum _NRF24L01_OutputPower_t {
	NRF24L01_OutputPower_M18dBm = 0x00,		/* Output power set to -18dBm */
	NRF24L01_OutputPower_M12dBm,
	NRF24L01_OutputPower_M6dBm,
	NRF24L01_OutputPower_0dBm,
}	NRF24L01_OutputPower_t;

/** Innitializes NRF24L01+
*	channel: channel you will use for communication, from 0 to 125 eg. wprking frequency 2.4 to 2.525 Ghz
*	payload_size: maximum data to be sent in one packet from one NRF to another.
*	Maximal payload size is 32bytes
**/
uint8_t NRF24L01_Init(uint8_t channel, uint8_t payload_size);


/** Sets own address. This is used for settings own id when communication with other modules
	"Own" address of one device must be the same as "TX" address of other device (and vice versa),
	if you want to get successful communication
	*adr: Pointer to 5-bytes length array with address
**/
void NRF24L01_SetMyAddress(uint8_t* adr);


/** Sets address you will communicate with "own" address of one device must be the same as "TX"
	address of other device (and vice versa)
	if you want to get successful communication
**/
void NRF24L01_SetTxAddress(uint8_t* adr);


/* Gets number of retransmission needed in last transmission 
	Number of retransmission, between 0 and 15
*/
uint8_t NRF24L01_GetRetransmissionsCount(void);


/** Sets NRF24L01+ to TX mode
	In this mode is NRF able to send data to another NRF module 
**/
void NRF24L01_PowerUpTx(void);


/* Sets NRF24L01+ to RX mode */
void NRF24L01_PowerUpRx(void);


/* Sets NRF24L01+ to power down mode 
	In power down mode, you are not able to transmit/receive data.
	You can wake up device using NRF24L01_PowerUpTx() or NRF24L01_PowerUpRx() functions
*/
void NRF24L01_PowerDown(void);


/* Gets transmission status. Return is based on NRF24L01_Transmit_Status_t enumeration */
NRF24L01_Transmit_Status_t NRF24L01_GetRetransmissionsStatus(void);

/* Transmits data with NRF24L01+ to another NRF module */
void NRF24L01_Transmit(uint8_t *data);

/* Checks if data is ready to be read from NRF24L01+
	- 0: no data available for receive in bufferReturns
	- > 0: data is ready to be collected 
*/
uint8_t NRF24L01_DataReady(void);


/* Gets data from nRF24L01+ 
	*data: pointer to 8-bits arrary where data from NRF will be saved 
*/
void NRF24L01_GetData(uint8_t *data);


/* Sets working channel
	channel value is just an offset in units Mhz from 2.4Ghz
	For example, if you select channel 65, the operation frequency will be set to 2.456Ghz.
	channel: RF channel where device will operate 
*/
void NRF24L01_SetChannel(uint8_t channel);

/*  Sets RG parameters for nRF24L01+
	DataRate: data rate selection for NRF module. This parameter can be a value NRF24L01_DataRate_t enumeration
	OutPwr: Output power selection for NRF module. This parameter can be a value of NRF24L01_OutputPower_t enumeration
*/
void NRF24L01_SetRF(NRF24L01_DataRate_t DataRate, NRF24L01_OutputPower_t OutPwr);


/* Gets nRF+ status register value */
uint8_t NRF24L01_GetStatus(void);


/* Reads interrupts from nRF+
	*IRQ: Pointer to NRF24L01_IRQ_t where IRQ status will be saved 
	-  0: No interrupts are active
	-  > 0: at least one interrupt is active 
*/
uint8_t NRF24L01_Read_Interrupts(NRF24L01_IRQ_t* IRQ);

/* Clear interrupt status */
void NRF24L01_Clear_Interrupts(void);

/* Private */
void NRF24L01_WriteRegister(uint8_t reg, uint8_t value);


#ifdef __cplusplus
}
#endif

#endif

