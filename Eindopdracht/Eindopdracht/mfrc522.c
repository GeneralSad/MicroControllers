/*
 * main.c
 *
 * Created: 3/23/2022 12:40:10 PM
 *  Author: Ewout, Leon
 */

#include "mfrc522.h"
#include "spi.h"
#include "wait.h"

void MFRC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData);

void mfrc522_init()
{
	uint8_t byte;
	mfrc522_reset();

	mfrc522_write(TModeReg, 0x8D);
	mfrc522_write(TPrescalerReg, 0x3E);
	mfrc522_write(TReloadReg_1, 30);
	mfrc522_write(TReloadReg_2, 0);
	mfrc522_write(TxASKReg, 0x40);
	mfrc522_write(ModeReg, 0x3D);

	byte = mfrc522_read(TxControlReg);
	if (!(byte & 0x03))
	{
		mfrc522_write(TxControlReg, byte | 0x03);
	}
}

void mfrc522_write(uint8_t reg, uint8_t data)
{
	ENABLE_CHIP();
	spi_transmit((reg << 1) & 0x7E);
	spi_transmit(data);
	DISABLE_CHIP();
}

uint8_t mfrc522_read(uint8_t reg)
{
	uint8_t data;
	ENABLE_CHIP();
	spi_transmit(((reg << 1) & 0x7E) | 0x80);
	data = spi_transmit(0x00);
	DISABLE_CHIP();
	return data;
}

void MFRC522_SetBitMask(uint8_t reg, uint8_t mask)
{
	mfrc522_write(reg, mfrc522_read(reg) | mask);
}

void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
	mfrc522_write(reg, mfrc522_read(reg) & (~mask));
}

void mfrc522_reset()
{
	mfrc522_write(CommandReg, SoftReset_CMD);
}

uint8_t mfrc522_request(uint8_t req_mode, uint8_t *tag_type)
{
	uint8_t status;
	uint32_t backBits; // The received data bits

	mfrc522_write(BitFramingReg, 0x07); // TxLastBists = BitFramingReg[2..0]	???

	tag_type[0] = req_mode;
	status = mfrc522_to_card(Transceive_CMD, tag_type, 1, tag_type, &backBits);

	if ((status != CARD_FOUND) || (backBits != 0x10))
	{
		status = ERROR;
	}

	return status;
}

uint8_t mfrc522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len)
{
	uint8_t status = ERROR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint32_t i;

	switch (cmd)
	{
	case MFAuthent_CMD: // Certification cards close
	{
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	}
	case Transceive_CMD: // Transmit FIFO data
	{
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	}
	default:
		break;
	}

	mfrc522_write(ComIEnReg, irqEn | 0x80);
	MFRC522_ClearBitMask(ComIrqReg, 0x80);
	MFRC522_SetBitMask(FIFOLevelReg, 0x80);

	mfrc522_write(CommandReg, Idle_CMD);

	//Writing data to the FIFO
	for (i = 0; i < send_data_len; i++) {
		mfrc522_write(FIFODataReg, send_data[i]);
	}

	//Execute the command
	mfrc522_write(CommandReg, cmd);
	if (cmd == Transceive_CMD) {
		MFRC522_SetBitMask(BitFramingReg, 0x80);//StartSend=1,transmission of data starts
	}

	// Waiting to receive data to complete
	i = 2000;//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	do {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		//wait(2);
		n = mfrc522_read(ComIrqReg);
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

	MFRC522_ClearBitMask(BitFramingReg, 0x80);	
	
	
	if (i != 0) {
		if (!(mfrc522_read(ErrorReg) & 0x1B)) {

			if (n & irqEn & 0x01) {
				status = CARD_NOT_FOUND;
				} else {
				status = CARD_FOUND;
			}

			if (cmd == Transceive_CMD) {
				n = mfrc522_read(FIFOLevelReg);
				lastBits = mfrc522_read(ControlReg) & 0x07;
				if (lastBits) {
					*back_data_len = (n - 1) * 8 + lastBits;
					} else {
					*back_data_len = n * 8;
				}

				if (n == 0) {
					n = 1;
				}
				if (n > MAX_LEN) {
					n = MAX_LEN;
				}

				//Reading the received data in FIFO
				for (i = 0; i < n; i++) {
					back_data[i] = mfrc522_read(FIFODataReg);
				}
			}
			} else {
			status = ERROR;
		}
	}

	// SetBitMask(ControlReg,0x80);           //timer stops
	// mfrc522_write(cmdReg, PCD_IDLE);

	return status;
}

uint8_t mfrc522_get_card_serial(uint8_t *serial_out)
{
	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint32_t unLen;

	mfrc522_write(BitFramingReg, 0x00); // TxLastBists = BitFramingReg[2..0]

	serial_out[0] = PICC_ANTICOLL;
	serial_out[1] = 0x20;
	status = mfrc522_to_card(Transceive_CMD, serial_out, 2, serial_out, &unLen);

	if (status == CARD_FOUND)
	{
		// Check card serial number
		for (i = 0; i < 4; i++)
		{
			serNumCheck ^= serial_out[i];
		}
		if (serNumCheck != serial_out[i])
		{
			status = ERROR;
		}
	}
	return status;
}

uint8_t mfrc522_get_card_select(uint8_t *serial_out)
{
	uint8_t status;
	uint32_t unLen;

	uint8_t csnByte0 = serial_out[0];
	uint8_t csnByte1 = serial_out[1];
	uint8_t csnByte2 = serial_out[2];
	uint8_t csnByte3 = serial_out[3];

	mfrc522_write(BitFramingReg, 0x00); // TxLastBists = BitFramingReg[2..0]

	serial_out[0] = PICC_SElECTTAG;
	serial_out[1] = 0x70;
	serial_out[2] = csnByte0;
	serial_out[3] = csnByte1;
	serial_out[4] = csnByte2;
	serial_out[5] = csnByte3;
	serial_out[6] = 0;

	MFRC522_CalculateCRC(serial_out, 7, &serial_out[7]);

	status = mfrc522_to_card(Transceive_CMD, serial_out, 9, serial_out, &unLen);
	uint8_t size;
	if ((status == 1) && (unLen == 0x18)) {
		size = serial_out[0];
	} else {
		size = 0;
	}

	return size;
}

uint8_t mfrc522_get_card_auth(uint8_t *serial_out)
{
	uint8_t csnByte0 = serial_out[0];
	uint8_t csnByte1 = serial_out[1];
	uint8_t csnByte2 = serial_out[2];
	uint8_t csnByte3 = serial_out[3];

	uint8_t status;
	// uint8_t i;
	// uint8_t serNumCheck=0;
	uint32_t unLen;

	mfrc522_write(BitFramingReg, 0x00); // TxLastBists = BitFramingReg[2..0]

	serial_out[0] = PICC_AUTHENT1A;
	serial_out[1] = 0x01;
	serial_out[2] = 0xD3;
	serial_out[3] = 0xF7;
	serial_out[4] = 0xD3;
	serial_out[5] = 0xF7;
	serial_out[6] = 0xD3;
	serial_out[7] = 0xF7;
	serial_out[8] = csnByte0;
	serial_out[9] = csnByte1;
	serial_out[10] = csnByte2;
	serial_out[11] = csnByte3;

	status = mfrc522_to_card(MFAuthent_CMD, serial_out, 12, serial_out, &unLen);

	// 	if (status == CARD_FOUND)
	// 	{
	// 		//Check card serial number
	// 		for (i=0; i<4; i++)
	// 		{
	// 			serNumCheck ^= serial_out[i];
	// 		}
	// 		if (serNumCheck != serial_out[i])
	// 		{
	// 			status = ERROR;
	// 		}
	// 	}
	return status;
}



void MFRC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
	uint8_t i, n;

	MFRC522_ClearBitMask(DivIrqReg, 0x04);	// CRCIrq = 0
	MFRC522_SetBitMask(FIFOLevelReg, 0x80); // Clear the FIFO pointer
	// Write_MFRC522(CommandReg, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i < len; i++)
	{
		mfrc522_write(FIFODataReg, *(pIndata + i));
	}
	mfrc522_write(CommandReg, CalcCRC_CMD);

	// Wait CRC calculation is complete
	i = 0xFF;
	do
	{
		n = mfrc522_read(DivIrqReg);
		i--;
	} while ((i != 0) && !(n & 0x04)); // CRCIrq = 1

	// Read CRC calculation result
	pOutData[0] = mfrc522_read(CRCResultReg_2);
	pOutData[1] = mfrc522_read(CRCResultReg_1);
}