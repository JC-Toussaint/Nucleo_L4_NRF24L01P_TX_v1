#include "nRF24L01P.h"
#include "stm32l4xx_hal.h"
//#include "spi.h"
//#include "gpio.h"
//#include "main.h"


/* NRF24L01+ registers*/
#define NRF24L01_REG_CONFIG          0x00   //Configuration Register
#define NRF24L01_REG_EN_AA           0x01   //Enable �Auto Acknowledgment� Function
#define NRF24L01_REG_EN_RXADDR       0x02   //Enabled RX Addresses
#define NRF24L01_REG_SETUP_AW        0x03   //Setup of Address Widths (common for all data pipes)
#define NRF24L01_REG_SETUP_RETR      0x04   //Setup of Automatic Retransmission
#define NRF24L01_REG_RF_CH           0x05   //RF Channel
#define NRF24L01_REG_RF_SETUP        0x06   //RF Setup Register
#define NRF24L01_REG_STATUS          0x07   //Status Register
#define NRF24L01_REG_OBSERVE_TX      0x08   //Transmit observe registerf
#define NRF24L01_REG_RPD             0x09
#define NRF24L01_REG_RX_ADDR_P0      0x0A   //Receive address data pipe 0. 5 Bytes maximum length.
#define NRF24L01_REG_RX_ADDR_P1      0x0B   //Receive address data pipe 1. 5 Bytes maximum length.
#define NRF24L01_REG_RX_ADDR_P2      0x0C   //Receive address data pipe 2. Only LSB
#define NRF24L01_REG_RX_ADDR_P3      0x0D   //Receive address data pipe 3. Only LSB
#define NRF24L01_REG_RX_ADDR_P4      0x0E   //Receive address data pipe 4. Only LSB
#define NRF24L01_REG_RX_ADDR_P5      0x0F   //Receive address data pipe 5. Only LSB
#define NRF24L01_REG_TX_ADDR         0x10   //Transmit address. Used for a PTX device only
#define NRF24L01_REG_RX_PW_P0        0x11
#define NRF24L01_REG_RX_PW_P1        0x12
#define NRF24L01_REG_RX_PW_P2        0x13
#define NRF24L01_REG_RX_PW_P3        0x14
#define NRF24L01_REG_RX_PW_P4        0x15
#define NRF24L01_REG_RX_PW_P5        0x16
#define NRF24L01_REG_FIFO_STATUS     0x17   //FIFO Status Register
#define NRF24L01_REG_DYNPD           0x1C   //Enable dynamic payload length
#define NRF24L01_REG_FEATURE         0x1D

/* Registers default values */


#define NRF24L01_REG_DEFAULT_VAL_CONFIG         0x08
#define NRF24L01_REG_DEFAULT_VAL_EN_AA          0x3F
#define NRF24L01_REG_DEFAULT_VAL_EN_RXADDR      0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_AW       0x03
#define NRF24L01_REG_DEFAULT_VAL_SETUP_RETR     0x03
#define NRF24L01_REG_DEFAULT_VAL_RF_CH          0x05
#define NRF24L01_REG_DEFAULT_VAL_RF_SETUP       0x0E
#define NRF24L01_REG_DEFAULT_VAL_STATUS         0x0E
#define NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX     0x00
#define NRF24L01_REG_DEFAULT_VAL_RPD            0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4   0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4   0xC2
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2     0xC3
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3     0xC4
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4     0xC5
#define NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5     0xC6
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3      0xE7
#define NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4      0xE7
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P0       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P1       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P2       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P3       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P4       0x00
#define NRF24L01_REG_DEFAULT_VAL_RX_PW_P5       0x00
#define NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS    0x11
#define NRF24L01_REG_DEFAULT_VAL_DYNPD          0x00
#define NRF24L01_REG_DEFAULT_VAL_FEATURE        0x00




/* Configuration register*/
#define NRF24L01_MASK_RX_DR      6
#define NRF24L01_MASK_TX_DS      5
#define NRF24L01_MASK_MAX_RT     4
#define NRF24L01_EN_CRC          3
#define NRF24L01_CRCO            2
#define NRF24L01_PWR_UP          1
#define NRF24L01_PRIM_RX         0

/* Enable auto acknowledgment*/
#define NRF24L01_ENAA_P5      5
#define NRF24L01_ENAA_P4      4
#define NRF24L01_ENAA_P3      3
#define NRF24L01_ENAA_P2      2
#define NRF24L01_ENAA_P1      1
#define NRF24L01_ENAA_P0      0

/* Enable rx addresses */
#define NRF24L01_ERX_P5         5
#define NRF24L01_ERX_P4         4
#define NRF24L01_ERX_P3         3
#define NRF24L01_ERX_P2         2
#define NRF24L01_ERX_P1         1
#define NRF24L01_ERX_P0         0

/* Setup of address width */
#define NRF24L01_AW            0 //2 bits

/* Setup of auto re-transmission*/
#define NRF24L01_ARD         4 //4 bits
#define NRF24L01_ARC         0 //4 bits

/* RF setup register*/
#define NRF24L01_PLL_LOCK    4
#define NRF24L01_RF_DR_LOW   5
#define NRF24L01_RF_DR_HIGH  3
#define NRF24L01_RF_DR       3
#define NRF24L01_RF_PWR      1 //2 bits

/* General status register */
#define NRF24L01_RX_DR        6
#define NRF24L01_TX_DS        5
#define NRF24L01_MAX_RT       4
#define NRF24L01_RX_P_NO      1 //3 bits
#define NRF24L01_TX_FULL      0

/* Transmit observe register */
#define NRF24L01_PLOS_CNT     4 //4 bits
#define NRF24L01_ARC_CNT      0 //4 bits

/* FIFO status*/
#define NRF24L01_TX_REUSE      6
#define NRF24L01_FIFO_FULL     5
#define NRF24L01_TX_EMPTY      4
#define NRF24L01_RX_FULL       1
#define NRF24L01_RX_EMPTY      0

//Dynamic length
#define NRF24L01_DPL_P0         0
#define NRF24L01_DPL_P1         1
#define NRF24L01_DPL_P2         2
#define NRF24L01_DPL_P3         3
#define NRF24L01_DPL_P4         4
#define NRF24L01_DPL_P5         5

/* Transmitter power*/
#define NRF24L01_M18DBM         0 //-18 dBm
#define NRF24L01_M12DBM         1 //-12 dBm
#define NRF24L01_M6DBM          2 //-6 dBm
#define NRF24L01_0DBM           3 //0 dBm

/* Data rates */
#define NRF24L01_2MBPS         0
#define NRF24L01_1MBPS         1
#define NRF24L01_250KBPS       2


#define		NRF24L01P_CRC_NONE	0
#define		NRF24L01P_CRC_8_BIT	8
#define		NRF24L01P_CRC_16_BIT	16


#define _NRF24L01P_CONFIG_CRC_MASK       (NRF24L01_EN_CRC|NRF24L01_CRCO)
#define _NRF24L01P_CONFIG_CRC_NONE       (0)
#define _NRF24L01P_CONFIG_CRC_8BIT       (NRF24L01_EN_CRC)
#define _NRF24L01P_CONFIG_CRC_16BIT      (NRF24L01_EN_CRC|NRF24L01_CRCO)


/* Configuration */
#define NRF24L01_CONFIG         ((1 << NRF24L01_EN_CRC) | (0 << NRF24L01_CRCO))

/* Instruction Mnemonics */
#define NRF24L01_REGISTER_MASK            0x1F

#define NRF24L01_READ_REGISTER_MASK(reg)   (0x00 | (NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_WRITE_REGISTER_MASK(reg)   (0x20 | (NRF24L01_REGISTER_MASK & reg)) //Last 5 bits will indicate reg. address
#define NRF24L01_R_RX_PAYLOAD_MASK        0x61
#define NRF24L01_W_TX_PAYLOAD_MASK        0xA0
#define NRF24L01_FLUSH_TX_MASK            0xE1
#define NRF24L01_FLUSH_RX_MASK            0xE2
#define NRF24L01_REUSE_TX_PL_MASK         0xE3
#define NRF24L01_ACTIVATE_MASK            0x50
#define NRF24L01_R_RX_PL_WID_MASK         0x60
#define NRF24L01_NOP_MASK                 0xFF

#define NRF24L01_TRANSMISSON_OK          0
#define NRF24L01_MESSAGE_LOST            1

#define NRF24L01_CHECK_BIT(reg, bit)       (reg & (1 << bit))
// EN_AA register:
#define _NRF24L01P_EN_AA_NONE            0

typedef struct {
   uint8_t PayloadSize;            //Payload size
   uint8_t Channel;               //Channel selected
   TM_NRF24L01_OutputPower_t OutPwr;   //Output power
   TM_NRF24L01_DataRate_t DataRate;   //Data rate
} TM_NRF24L01_t;

/* Private functions */
//void TM_NRF24L01_InitPins(void);
//void TM_NRF24L01_WriteBit(uint8_t reg, uint8_t bit, uint8_t value);
//uint8_t TM_NRF24L01_ReadBit(uint8_t reg, uint8_t bit);
//uint8_t TM_NRF24L01_ReadRegister(uint8_t reg);
//void TM_NRF24L01_ReadRegisterMulti(uint8_t reg, uint8_t* data, uint8_t count);
//void TM_NRF24L01_WriteRegisterMulti(uint8_t reg, uint8_t *data, uint8_t count);
//void TM_NRF24L01_SoftwareReset(void);
//uint8_t TM_NRF24L01_RxFifoEmpty(void);

static TM_NRF24L01_t TM_NRF24L01_Struct;
/**********************************************************************************************************/
void TM_NRF24L01_InitPins(void)
{
   /* CSN high = disable SPI */
   NRF24L01_CSN_HIGH;

   /* CE low = disable TX/RX */
   NRF24L01_CE_LOW;
}
/**********************************************************************************************************/
void NRF24L01_FlushTX(SPI_HandleTypeDef *hspi)
{
	uint8_t pData = NRF24L01_FLUSH_TX_MASK;
	do{
				NRF24L01_CSN_LOW;
				HAL_SPI_Transmit(hspi, &pData, 1, 200);
				NRF24L01_CSN_HIGH;
	}while(0);

}
/**********************************************************************************************************/
void NRF24L01_FlushRX(SPI_HandleTypeDef *hspi)
{
	uint8_t pData = NRF24L01_FLUSH_RX_MASK;
	do{
				NRF24L01_CSN_LOW;
				HAL_SPI_Transmit(hspi, &pData, 1, 200);
				NRF24L01_CSN_HIGH;
	}while(0);
}
/**********************************************************************************************************/
void TM_NRF24L01_WriteRegister(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value)
{
   NRF24L01_CSN_LOW;

   uint8_t pData = NRF24L01_WRITE_REGISTER_MASK(reg);
   HAL_SPI_Transmit(hspi, &pData, 1, 200);
   HAL_SPI_Transmit(hspi, &value, 1, 200);

   NRF24L01_CSN_HIGH;
}
/**********************************************************************************************************/
void TM_NRF24L01_WriteRegisterMulti(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t *data, uint8_t count)
{
	 NRF24L01_CSN_LOW;

	 uint8_t pData = NRF24L01_WRITE_REGISTER_MASK(reg);
	 HAL_SPI_Transmit(hspi, &pData, 1, 200);
   HAL_SPI_Transmit(hspi, data, count, 200);

   NRF24L01_CSN_HIGH;
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t reg)
{
	 NRF24L01_CSN_LOW;

   uint8_t value;
	 uint8_t pData = NRF24L01_READ_REGISTER_MASK(reg);
   HAL_SPI_Transmit(hspi, &pData, 1, 200);

	 pData = NRF24L01_NOP_MASK;
	 HAL_SPI_TransmitReceive(hspi, &pData, &value, 1, 200);

   NRF24L01_CSN_HIGH;

   return value;
}
/**********************************************************************************************************/
void TM_NRF24L01_ReadRegisterMulti(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t* data, uint8_t count)
{
	 NRF24L01_CSN_LOW;

	 uint8_t pData = NRF24L01_READ_REGISTER_MASK(reg);
   HAL_SPI_Transmit(hspi, &pData, 1, 200);


	 pData = NRF24L01_NOP_MASK;
	 HAL_SPI_TransmitReceive(hspi, &pData, data, count, 200);

   NRF24L01_CSN_HIGH;
}
/**********************************************************************************************************/
void TM_NRF24L01_WriteBit(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t bit, uint8_t value)
{
   uint8_t tmp;

   tmp = TM_NRF24L01_ReadRegister(hspi, reg);

   if (value)
		{
      tmp |= 1 << bit;
    }
		else
	  {
      tmp &= ~(1 << bit);
    }

   TM_NRF24L01_WriteRegister(hspi, reg, tmp);
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_ReadBit(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t bit)
{
   uint8_t tmp;
   tmp = TM_NRF24L01_ReadRegister(hspi, reg);

   if (!NRF24L01_CHECK_BIT(tmp, bit))
	 {
      return 0;
   }
   return 1;
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_Init(SPI_HandleTypeDef *hspi, uint8_t channel, uint8_t payload_size)
{
   /* Initialize CE and CSN pins */
   TM_NRF24L01_InitPins();

   /* Max payload is 32bytes */
   if (payload_size > 32) {
      payload_size = 32;
   }

   /* Fill structure */
   TM_NRF24L01_Struct.Channel = !channel; /* Set channel to some different value for TM_NRF24L01_SetChannel() function */
   TM_NRF24L01_Struct.PayloadSize = payload_size;
   TM_NRF24L01_Struct.OutPwr = TM_NRF24L01_OutputPower_0dBm;
   TM_NRF24L01_Struct.DataRate = TM_NRF24L01_DataRate_2M;

   /* Reset nRF24L01+ to power on registers values */
   TM_NRF24L01_SoftwareReset(hspi);

   /* Channel select */
   TM_NRF24L01_SetChannel(hspi, channel);

   /* Set pipeline to max possible 32 bytes */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P0, TM_NRF24L01_Struct.PayloadSize); // Auto-ACK pipe
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P1, TM_NRF24L01_Struct.PayloadSize); // Data TM_NRF24L01_SetMyAddress pipe
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P2, TM_NRF24L01_Struct.PayloadSize);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P3, TM_NRF24L01_Struct.PayloadSize);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P4, TM_NRF24L01_Struct.PayloadSize);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P5, TM_NRF24L01_Struct.PayloadSize);

   /* Set RF settings (2mbps, output power) */
   TM_NRF24L01_SetRF(hspi, TM_NRF24L01_Struct.DataRate, TM_NRF24L01_Struct.OutPwr);

   /* Config register */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_CONFIG, NRF24L01_CONFIG);

   /* Enable auto-acknowledgment for all pipes */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_EN_AA, 0x3F);

   /* Enable RX addresses */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_EN_RXADDR, 0x3F);

   /* Auto retransmit delay: 1000 (4x250) us and Up to 15 retransmit trials */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_SETUP_RETR, 0x4F);

   /* Dynamic length configurations: No dynamic length */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_DYNPD, (0 << NRF24L01_DPL_P0) | (0 << NRF24L01_DPL_P1) | (0 << NRF24L01_DPL_P2) | (0 << NRF24L01_DPL_P3) | (0 << NRF24L01_DPL_P4) | (0 << NRF24L01_DPL_P5));

   /* Clear FIFOs */
   NRF24L01_FlushTX(hspi);
   NRF24L01_FlushRX(hspi);

   /* Clear interrupts */
   TM_NRF24L01_Clear_Interrupts(hspi);

   /* Go to RX mode */
   TM_NRF24L01_PowerUpRx(hspi);

   /* Return OK */
   return 1;
}
/**********************************************************************************************************/
void TM_NRF24L01_SetMyAddress(SPI_HandleTypeDef *hspi, uint8_t *adr)
{
   NRF24L01_CE_LOW;
   TM_NRF24L01_WriteRegisterMulti(hspi, NRF24L01_REG_RX_ADDR_P1, adr, 5);
   NRF24L01_CE_HIGH;
}
/**********************************************************************************************************/
void TM_NRF24L01_SetTxAddress(SPI_HandleTypeDef *hspi, uint8_t *adr)
{
   TM_NRF24L01_WriteRegisterMulti(hspi, NRF24L01_REG_RX_ADDR_P0, adr, 5);
   TM_NRF24L01_WriteRegisterMulti(hspi, NRF24L01_REG_TX_ADDR, adr, 5);
}
/**********************************************************************************************************/
void TM_NRF24L01_PowerUpTx(SPI_HandleTypeDef *hspi)
{
   TM_NRF24L01_Clear_Interrupts(hspi);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_CONFIG, NRF24L01_CONFIG | (0 << NRF24L01_PRIM_RX) | (1 << NRF24L01_PWR_UP));
}
/**********************************************************************************************************/
void TM_NRF24L01_PowerUpRx(SPI_HandleTypeDef *hspi)
{
   /* Disable RX/TX mode */
   NRF24L01_CE_LOW;
   /* Clear RX buffer */
   NRF24L01_FlushRX(hspi);
   /* Clear interrupts */
   TM_NRF24L01_Clear_Interrupts(hspi);
   /* Setup RX mode */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_CONFIG, NRF24L01_CONFIG | 1 << NRF24L01_PWR_UP | 1 << NRF24L01_PRIM_RX);
   /* Start listening */
   NRF24L01_CE_HIGH;
}
/**********************************************************************************************************/
void TM_NRF24L01_PowerDown(SPI_HandleTypeDef *hspi)
{
   NRF24L01_CE_LOW;
   TM_NRF24L01_WriteBit(hspi, NRF24L01_REG_CONFIG, NRF24L01_PWR_UP, 0);
}
/**********************************************************************************************************/
void TM_NRF24L01_Transmit(SPI_HandleTypeDef *hspi, uint8_t *data)
{
	 uint8_t pData = NRF24L01_W_TX_PAYLOAD_MASK;
   uint8_t count = TM_NRF24L01_Struct.PayloadSize;

   /* Chip enable put to low, disable it */
   NRF24L01_CE_LOW;

   /* Go to power up tx mode */
   TM_NRF24L01_PowerUpTx(hspi);

   /* Clear TX FIFO from NRF24L01+ */
   NRF24L01_FlushTX(hspi);

   /* Send payload to nRF24L01+ */
   NRF24L01_CSN_LOW;
   /* Send write payload command */
	 HAL_SPI_Transmit(hspi, &pData, 1, 1000);
   /* Fill payload with data*/
	 HAL_SPI_Transmit(hspi, data, count, 1000);
   /* Disable SPI */
   NRF24L01_CSN_HIGH;

   /* Send data! */
   NRF24L01_CE_HIGH;
}
/**********************************************************************************************************/
void TM_NRF24L01_GetData(SPI_HandleTypeDef *hspi, uint8_t* data)
{
   /* Pull down chip select */
   NRF24L01_CSN_LOW;
   /* Send read payload command*/
	 uint8_t pData = NRF24L01_R_RX_PAYLOAD_MASK;
	 HAL_SPI_Transmit(hspi, &pData, 1, 200);
   /* Read payload */
   HAL_SPI_TransmitReceive(hspi, data, data,	TM_NRF24L01_Struct.PayloadSize, 200);
   /* Pull up chip select */
   NRF24L01_CSN_HIGH;

   /* Reset status register, clear RX_DR interrupt flag */
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_STATUS, (1 << NRF24L01_RX_DR));
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_DataReady(SPI_HandleTypeDef *hspi)
{
   uint8_t status = TM_NRF24L01_GetStatus(hspi);

   if (NRF24L01_CHECK_BIT(status, NRF24L01_RX_DR))
	 {
      return 1;
   }
   return !TM_NRF24L01_RxFifoEmpty(hspi);
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_RxFifoEmpty(SPI_HandleTypeDef *hspi)
{
   uint8_t reg = TM_NRF24L01_ReadRegister(hspi, NRF24L01_REG_FIFO_STATUS);
   return NRF24L01_CHECK_BIT(reg, NRF24L01_RX_EMPTY);
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_GetStatus(SPI_HandleTypeDef *hspi)
{
   uint8_t status;

   NRF24L01_CSN_LOW;
   /* First received byte is always status register */
	 uint8_t pData = NRF24L01_REG_STATUS;
	 //uint8_t pData = NRF24L01_NOP_MASK;
	 HAL_SPI_TransmitReceive(hspi, &pData, &status,	1, 200);
   /* Pull up chip select */
   NRF24L01_CSN_HIGH;

   return status;
}
/**********************************************************************************************************/
TM_NRF24L01_Transmit_Status_t TM_NRF24L01_GetTransmissionStatus(SPI_HandleTypeDef *hspi)
{
   uint8_t status = TM_NRF24L01_GetStatus(hspi);
   if (NRF24L01_CHECK_BIT(status, NRF24L01_TX_DS)) {
      /* Successfully sent */
      return TM_NRF24L01_Transmit_Status_Ok;
   } else if (NRF24L01_CHECK_BIT(status, NRF24L01_MAX_RT)) {
      /* Message lost */
      return TM_NRF24L01_Transmit_Status_Lost;
   }

   /* Still sending */
   return TM_NRF24L01_Transmit_Status_Sending;
}
/**********************************************************************************************************/
void TM_NRF24L01_SoftwareReset(SPI_HandleTypeDef *hspi)
{
   uint8_t data[5];

   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_CONFIG,       NRF24L01_REG_DEFAULT_VAL_CONFIG);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_EN_AA,      NRF24L01_REG_DEFAULT_VAL_EN_AA);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_EN_RXADDR,    NRF24L01_REG_DEFAULT_VAL_EN_RXADDR);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_SETUP_AW,    NRF24L01_REG_DEFAULT_VAL_SETUP_AW);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_SETUP_RETR,    NRF24L01_REG_DEFAULT_VAL_SETUP_RETR);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RF_CH,       NRF24L01_REG_DEFAULT_VAL_RF_CH);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RF_SETUP,    NRF24L01_REG_DEFAULT_VAL_RF_SETUP);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_STATUS,       NRF24L01_REG_DEFAULT_VAL_STATUS);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_OBSERVE_TX,    NRF24L01_REG_DEFAULT_VAL_OBSERVE_TX);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RPD,       NRF24L01_REG_DEFAULT_VAL_RPD);

   //P0
   data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_0;
   data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_1;
   data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_2;
   data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_3;
   data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P0_4;
   TM_NRF24L01_WriteRegisterMulti(hspi, NRF24L01_REG_RX_ADDR_P0, data, 5);

   //P1
   data[0] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_0;
   data[1] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_1;
   data[2] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_2;
   data[3] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_3;
   data[4] = NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P1_4;
   TM_NRF24L01_WriteRegisterMulti(hspi, NRF24L01_REG_RX_ADDR_P1, data, 5);

   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_ADDR_P2,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P2);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_ADDR_P3,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P3);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_ADDR_P4,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P4);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_ADDR_P5,    NRF24L01_REG_DEFAULT_VAL_RX_ADDR_P5);

   //TX
   data[0] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_0;
   data[1] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_1;
   data[2] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_2;
   data[3] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_3;
   data[4] = NRF24L01_REG_DEFAULT_VAL_TX_ADDR_4;
   TM_NRF24L01_WriteRegisterMulti(hspi, NRF24L01_REG_TX_ADDR, data, 5);

   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P0,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P0);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P1,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P1);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P2,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P2);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P3,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P3);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P4,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P4);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RX_PW_P5,    NRF24L01_REG_DEFAULT_VAL_RX_PW_P5);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_FIFO_STATUS, NRF24L01_REG_DEFAULT_VAL_FIFO_STATUS);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_DYNPD,       NRF24L01_REG_DEFAULT_VAL_DYNPD);
   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_FEATURE,    NRF24L01_REG_DEFAULT_VAL_FEATURE);
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_GetRetransmissionsCount(SPI_HandleTypeDef *hspi)
{
   /* Low 4 bits */
   return TM_NRF24L01_ReadRegister(hspi, NRF24L01_REG_OBSERVE_TX) & 0x0F;
}
/**********************************************************************************************************/
void TM_NRF24L01_SetChannel(SPI_HandleTypeDef *hspi, uint8_t channel)
{
   if (channel <= 125 && channel != TM_NRF24L01_Struct.Channel)
	 {
      /* Store new channel setting */
      TM_NRF24L01_Struct.Channel = channel;
      /* Write channel */
      TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RF_CH, channel);
   }
}
/**********************************************************************************************************/
void TM_NRF24L01_SetRF(SPI_HandleTypeDef *hspi, TM_NRF24L01_DataRate_t DataRate, TM_NRF24L01_OutputPower_t OutPwr)
{
   uint8_t tmp = 0;
   TM_NRF24L01_Struct.DataRate = DataRate;
   TM_NRF24L01_Struct.OutPwr = OutPwr;

   if (DataRate == TM_NRF24L01_DataRate_2M) {
      tmp |= 1 << NRF24L01_RF_DR_HIGH;
   } else if (DataRate == TM_NRF24L01_DataRate_250k) {
      tmp |= 1 << NRF24L01_RF_DR_LOW;
   }
   /* If 1Mbps, all bits set to 0 */

   if (OutPwr == TM_NRF24L01_OutputPower_0dBm) {
      tmp |= 3 << NRF24L01_RF_PWR;
   } else if (OutPwr == TM_NRF24L01_OutputPower_M6dBm) {
      tmp |= 2 << NRF24L01_RF_PWR;
   } else if (OutPwr == TM_NRF24L01_OutputPower_M12dBm) {
      tmp |= 1 << NRF24L01_RF_PWR;
   }

   TM_NRF24L01_WriteRegister(hspi, NRF24L01_REG_RF_SETUP, tmp);
}
/**********************************************************************************************************/
uint8_t TM_NRF24L01_Read_Interrupts(SPI_HandleTypeDef *hspi, TM_NRF24L01_IRQ_t* IRQ)
{
   IRQ->Status = TM_NRF24L01_GetStatus(hspi);
   return IRQ->Status;
}
/**********************************************************************************************************/
void TM_NRF24L01_Clear_Interrupts(SPI_HandleTypeDef *hspi)
{
   TM_NRF24L01_WriteRegister(hspi, 0x07, 0x70);
}

/**********************************************************************************************************/
void TM_NRF24L01_setCrcWidth(SPI_HandleTypeDef *hspi, uint8_t width) {

	uint8_t config = TM_NRF24L01_ReadRegister(hspi,NRF24L01_REG_CONFIG) & ~_NRF24L01P_CONFIG_CRC_MASK;

    switch ( width ) {

        case NRF24L01P_CRC_NONE:
            config |= _NRF24L01P_CONFIG_CRC_NONE;
            break;

        case NRF24L01P_CRC_8_BIT:
            config |= _NRF24L01P_CONFIG_CRC_8BIT;
            break;

        case NRF24L01P_CRC_16_BIT:
            config |= _NRF24L01P_CONFIG_CRC_16BIT;
            break;

        default:
            //error( "nRF24L01P: Invalid CRC Width setting %d\r\n", width );
            return;

    }

    TM_NRF24L01_WriteRegister(hspi,NRF24L01_REG_CONFIG, config);
    //setRegister(_NRF24L01P_REG_CONFIG, config);

}

/**********************************************************************************************************/
void TM_NRF24L01_disableAutoAcknowledge(SPI_HandleTypeDef *hspi) {

	TM_NRF24L01_WriteRegister(hspi,NRF24L01_REG_EN_AA, _NRF24L01P_EN_AA_NONE);

}
