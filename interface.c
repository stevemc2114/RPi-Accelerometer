#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>

//Author2: Steve McClure

#if 0
int main(int argc, char **argv)
{
	//Setup SPI pins
	bcm2835_spi_begin();
	
	//Set CS pins polarity to low
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
	
	//Set SPI clock speed
	//	BCM2835_SPI_CLOCK_DIVIDER_65536 = 0,       ///< 65536 = 262.144us = 3.814697260kHz (total H+L clock period) 
	//	BCM2835_SPI_CLOCK_DIVIDER_32768 = 32768,   ///< 32768 = 131.072us = 7.629394531kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_16384 = 16384,   ///< 16384 = 65.536us = 15.25878906kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_8192  = 8192,    ///< 8192 = 32.768us = 30/51757813kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_4096  = 4096,    ///< 4096 = 16.384us = 61.03515625kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_2048  = 2048,    ///< 2048 = 8.192us = 122.0703125kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_1024  = 1024,    ///< 1024 = 4.096us = 244.140625kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_512   = 512,     ///< 512 = 2.048us = 488.28125kHz
	//	BCM2835_SPI_CLOCK_DIVIDER_256   = 256,     ///< 256 = 1.024us = 976.5625MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_128   = 128,     ///< 128 = 512ns = = 1.953125MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_64    = 64,      ///< 64 = 256ns = 3.90625MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_32    = 32,      ///< 32 = 128ns = 7.8125MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_16    = 16,      ///< 16 = 64ns = 15.625MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_8     = 8,       ///< 8 = 32ns = 31.25MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_4     = 4,       ///< 4 = 16ns = 62.5MHz
	//	BCM2835_SPI_CLOCK_DIVIDER_2     = 2,       ///< 2 = 8ns = 125MHz, fastest you can get
	//	BCM2835_SPI_CLOCK_DIVIDER_1     = 1,       ///< 1 = 262.144us = 3.814697260kHz, same as 0/65536
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);

	//Set SPI data mode
	//	BCM2835_SPI_MODE0 = 0,  // CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	//	BCM2835_SPI_MODE1 = 1,  // CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE2 = 2,  // CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
	//	BCM2835_SPI_MODE3 = 3,  // CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);		//(SPI_MODE_# | SPI_CS_HIGH)=Chip Select active high, (SPI_MODE_# | SPI_NO_CS)=1 device per bus no Chip Select

	//Set with CS pin to use for next transfers
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

	//Transfer 1 byte
	//uint8_t data;
	//data = bcm2835_spi_transfer((uint8_t)0x55);
	
	//Transfer many bytes
	char data_buffer[10];
	int Count;
	for (Count = 0; Count < 10; Count++)
		data_buffer[Count] = 0x80 + Count;
	bcm2835_spi_transfern(&data_buffer[0], 10);			//data_buffer used for tx and rx
	
	//Return SPI pins to default inputs state
	//bcm2835_spi_end();
}
#endif

    int  loop_index = 0;
    char buf[10];   
    char out[10];
    float x, y, z;
    
void  bcm_initialize (void)
{   
    
    if (!bcm2835_spi_begin())
    {
        printf ("bcm2835_spi_begin() failed... Are you running as root?\n");
        bcm2835_close();
        exit (1);
    }
    
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536); // The default
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
   
  
    
    // Enter power saving state
    buf[0] = 0x2D;          // Reg
    buf[1] = 0x00;          // Data
    bcm2835_spi_transfern (buf, 2);

    // Set data rate to 100 Hz
    buf[0] = 0x2C;          // Reg
    buf[1] = 0x0A;          // Data
    bcm2835_spi_transfern (buf, 2);    

    // Set data format
    buf[0] = 0x31;          // Reg
    buf[1] = 0x10;          // Data
    bcm2835_spi_transfern (buf, 2);    
    
    // Enable Measurement
    buf[0] = 0x2D;          // Reg
    buf[1] = 0x08;          // Data
    bcm2835_spi_transfern (buf, 2);    
}


    
void  bcm_read_adxl345 (void)
{    
    // Read (x,y,z) data bytes
    buf[0] = 0xF2;          // Reg
    buf[1] = 0x00;          // Data
    buf[2] = 0x00;          // Data
    buf[3] = 0x00;          // Data
    buf[4] = 0x00;          // Data
    buf[5] = 0x00;          // Data
    buf[6] = 0x00;          // Data    
    bcm2835_spi_transfern (buf, 7);    
    
    // Format x-axis
    out[0] = buf[1] | (buf[2] << 8);
    out[1] = buf[3] | (buf[4] << 8);
    out[2] = buf[5] | (buf[6] << 8);    
    
    if (out[0] & (1<<16 - 1 ))          // Take care of twos-complement issues
    {
        out[0] = out[0] - (1<<16);
    }
    
    x = out[0] * 0.004; //#* 9.82     # Units of g
    
    
    // Format y-axis
    if (out[1] & (1<<16 - 1 ))
    {
        out[1] = out[1] - (1<<16);
    }
    
    y = out[1] * 0.004; //#* 9.82     # Units of g
    
    
    // Format z-axis
    if (out[2] & (1<<16 - 1 ))
    {
        out[2] = out[2] - (1<<16);
    }
    
    z = out[2] * 0.004; //#* 9.82     # Units of g
}

    
    
#if 1
int main(int argc, char **argv)
{
    // Initialize the SPI interface
    if (!bcm2835_init())
    {
        printf ("bcm2835_init() failed... Are you running as root?\n");
//      bcd2835_close();
        exit (1);
    }
    
    // Initialize the SPI interface
    bcm_initialize();    
    
    
    // Read DeviceID of ADXL345
    // ========================    
    //  char tbuf[] = { 0x0B, 0x00, 0x00}; // Read DeviceID of ADXL345
    char tbuf[] = { 0x80, 0x00 }; // Read DeviceID of ADXL345
    char rbuf[] = { 0xA5, 0x5A, 0xC0}; // Read DeviceID of ADXL345
    bcm2835_spi_transfernb (tbuf, rbuf, sizeof(tbuf));
    // buf will now be filled with the data that was read from the slave
    printf("Device(1) ID: %02X \n", rbuf[1]);
    
    // Read DeviceID of ADXL345
    // ========================
    buf[0] = 0x80;          // Read DeviceID of ADXL345
    buf[1] = 0x00;
    bcm2835_spi_transfern (buf, 2);
    // buf will now be filled with the data that was read from the slave
    printf("Device(2) ID: %02X \n", buf[0]);
    
    // Read some accelerometer (x,y,z) data samples 
    while(1)
    {
        // Read accelerometer (x,y,z) sample
        bcm_read_adxl345();
        
        // Print out result
        printf ("X = %2.4f    Y = %2.4f    Z = %2.4f\n", x, y, z);
        usleep (500000);
        loop_index++;
        if (loop_index > 9)
        {
            bcm2835_spi_end();
            bcm2835_close();
            exit(0);
        }
    }
    
    
    
    
#if 0    
    char buf1[] = {0x0A, 0x1F, 0x52}; // Soft Reset
    bcm2835_spi_transfern(buf1, sizeof(buf1));

    delay(1000);

    char buf2[] = {0x0A, 0x2D, 0x02}; // Setup for Measure
    bcm2835_spi_transfern(buf2, sizeof(buf2));

    delay(1000);

    char buf3[] = {0x0B, 0x0E, 0x00, 0x00}; // Read X
    bcm2835_spi_transfern(buf3, sizeof(buf3));
    printf("X Axis is: %02X %02X \n", buf3[3], buf3[2]);

    delay(1000);

    char buf4[] = {0x0B, 0x0E, 0x00, 0x00}; // Read X
    bcm2835_spi_transfern(buf4, sizeof(buf4));
    printf("Y Axis is: %02X %02X \n", buf4[3], buf4[2]);

    delay(1000);

    char buf5[] = {0x0B, 0x0E, 0x00, 0x00}; // Read X
    bcm2835_spi_transfern(buf5, sizeof(buf5));
    printf("Z Axis is: %02X %02X \n", buf5[3], buf5[2]);
#endif

    bcm2835_spi_end();
    bcm2835_close();    
    return 0;
}
#endif
