/*
 * card_reader.c
 *
 * Created: 23-3-2022 16:04:48
 *  Author: leonv
 */ 

// wait(): busy waiting for 'ms' millisecond
// used library: util/delay.h
void wait(int ms)
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms(1);
	}
}

void spi_masterInit(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
											// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
											// Mode = 0: CPOL=0, CPPH=0;
}

// Write a byte from master to slave
uint8_t spi_write( uint8_t data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while(!(SPSR & BIT(SPIF))); 			// Wait for transmission complete
	return SPDR;
}

// Write a byte from master to slave and read a byte from slave
// nice to have; not used here
// char spi_writeRead( unsigned char data )
// {
// 	SPDR = data;							// Load byte to Data register --> starts transmission
// 	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete
// 	data = SPDR;							// New received data (eventually, MISO) in SPDR
// 	return data;							// Return received byte
// }

// Write a word = address byte + data byte from master to slave
// void spi_writeWord ( unsigned char adress, unsigned char data ) {
// 	ENABLE_CHIP();
// 	spi_write((address<<1)&0x7);
// 	spi_write(data);
// 	DISABLE_CHIP();
// }

uint8_t RC522_read(uint8_t reg)
{
	uint8_t data;
	ENABLE_CHIP();
	spi_write(((reg<<1)&0x7E)|0x80);
	data = spi_write(0x00);
	DISABLE_CHIP();
	return data;
}

void RC522_write(uint8_t reg, uint8_t data)
{
	ENABLE_CHIP();
	spi_write((reg<<1)&0x7E);
	spi_write(data);
	DISABLE_CHIP();
}

void mfrc522_reset()
{
	RC522_write(CommandReg,SoftReset_CMD);
}

void RC522_init()
{
	uint8_t byte;
	mfrc522_reset();
	
	RC522_write(TModeReg, 0x8D);
	RC522_write(TPrescalerReg, 0x3E);
	RC522_write(TReloadReg_1, 30);
	RC522_write(TReloadReg_2, 0);
	RC522_write(TxASKReg, 0x40);
	RC522_write(ModeReg, 0x3D);
	
	byte = RC522_read(TxControlReg);
	if(!(byte & 0x03))
	{
		RC522_write(TxControlReg, byte|0x03);
	}
}

// int main()
// {
// 	// initialize
// 	DDRB=0x01;					  	// Set PB0 pin as output for display select
// 	spi_masterInit();              	// Initialize spi module
// 	displayDriverInit();            // Initialize display chip
// 
// 	// clear display (all zero's)
// 	//for (char i =1; i<=2; i++) was 2, maar moest 4 zijn
// 	for (char i =1; i<=4; i++)
// 	{
// 		spi_writeWord(i, 0);
// 	}
// 	wait(1000);
// 	
// 	for (int i = 0; i < 10000; i++)
// 	{
// 		writeLedDisplay(i);
// 		wait(10);
// 	}
// 	
// 	wait(1000);
// 
// 
// 
// 	return (1);
// }

uint8_t	RC522_request(uint8_t req_mode, uint8_t * tag_type)
{
	uint8_t  status;
	uint32_t backBits;//The received data bits

	RC522_write(BitFramingReg, 0x07);//TxLastBists = BitFramingReg[2..0]	???
	
	tag_type[0] = req_mode;
	status = RC522_to_card(Transceive_CMD, tag_type, 1, tag_type, &backBits);

	if ((status != CARD_FOUND) || (backBits != 0x10))
	{
		status = ERROR;
	}
	
	return status;
}

uint8_t RC522_to_card(uint8_t cmd, uint8_t *send_data, uint8_t send_data_len, uint8_t *back_data, uint32_t *back_data_len)
{
	uint8_t status = ERROR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint8_t	tmp;
	uint32_t i;

	switch (cmd)
	{
		case MFAuthent_CMD:		//Certification cards close
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case Transceive_CMD:	//Transmit FIFO data
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
		break;
	}
	
	//mfrc522_write(ComIEnReg, irqEn|0x80);	//Interrupt request
	n=RC522_read(ComIrqReg);
	RC522_write(ComIrqReg,n&(~0x80));//clear all interrupt bits
	n=RC522_read(FIFOLevelReg);
	RC522_write(FIFOLevelReg,n|0x80);//flush FIFO data
	
	RC522_write(CommandReg, Idle_CMD);	//NO action; Cancel the current cmd???

	//Writing data to the FIFO
	for (i=0; i<send_data_len; i++)
	{
		RC522_write(FIFODataReg, send_data[i]);
	}

	//Execute the cmd
	RC522_write(CommandReg, cmd);
	if (cmd == Transceive_CMD)
	{
		n=RC522_read(BitFramingReg);
		RC522_write(BitFramingReg,n|0x80);
	}
	
	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	do
	{
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = RC522_read(ComIrqReg);
		i--;
	}
	while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	tmp=RC522_read(BitFramingReg);
	RC522_write(BitFramingReg,tmp&(~0x80));
	
	if (i != 0)
	{
		if(!(RC522_read(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
		{
			status = CARD_FOUND;
			if (n & irqEn & 0x01)
			{
				status = CARD_NOT_FOUND;			//??
			}

			if (cmd == Transceive_CMD)
			{
				n = RC522_read(FIFOLevelReg);
				lastBits = RC522_read(ControlReg) & 0x07;
				if (lastBits)
				{
					*back_data_len = (n-1)*8 + lastBits;
				}
				else
				{
					*back_data_len = n*8;
				}

				if (n == 0)
				{
					n = 1;
				}
				if (n > MAX_LEN)
				{
					n = MAX_LEN;
				}
				
				//Reading the received data in FIFO
				for (i=0; i<n; i++)
				{
					back_data[i] = RC522_read(FIFODataReg);
				}
			}
		}
		else
		{
			status = ERROR;
		}
		
	}
	
	//SetBitMask(ControlReg,0x80);           //timer stops
	//mfrc522_write(cmdReg, PCD_IDLE);

	return status;
}

uint8_t RC522_get_card_serial(uint8_t * serial_out)
{
	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck=0;
	uint32_t unLen;
	
	RC522_write(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]
	
	serial_out[0] = PICC_ANTICOLL;
	serial_out[1] = 0x20;
	status = RC522_to_card(Transceive_CMD, serial_out, 2, serial_out, &unLen);

	if (status == CARD_FOUND)
	{
		//Check card serial number
		for (i=0; i<4; i++)
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