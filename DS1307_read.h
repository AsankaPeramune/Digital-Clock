#define DS1307_address 0xd0 
#define second 0x00
#define minute 0x01
#define hour 0x02
#define day 0x03
#define date 0x04
#define month 0x05
#define year 0x06
#define control_reg 0x07

void DS1307_read (char slave_address, char register_address);

unsigned char __second, __minute, __hour, __day, __date, __month, __year, __con ;

void DS1307_read (char slave_address, char register_address)
{
    ///* Start Bit *///////
    SSPCON2bits.SEN = 1;    /// initiate start condition for I2C communication on SDA and SCL pins
    while (SEN);
    PIR1bits.SSPIF =0 ;
    
    ////* Writing Slave Address *///////////////
    SSPBUF = slave_address ;
    while (!SSPIF) ;    //////// SSPIF is set at every 9th clock cycle
    PIR1bits.SSPIF = 0 ;
    if (SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while (PEN);
        return ;   ///////Exit the DS1307_write function 
    }
    
    ////* Writing Register Address *///////////////
    SSPBUF = register_address ;
    while (!SSPIF) ;    //////// SSPIF is set at every 9th clock cycle
    PIR1bits.SSPIF = 0 ;
    if (SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while (PEN);
        return ;   ///////Exit the DS1307_write function 
    }
    
    ///* Restart Bit *///////
    SSPCON2bits.RSEN = 1;    /// initiate star condition for I2C communication on SDA and SCL pins
    while (RSEN);
    PIR1bits.SSPIF =0 ;
    
    ////* Reading from First Internal Register Address *///////////////
    SSPBUF = (slave_address + 1) ;     //// write slave address for reading from slave
    while (!SSPIF) ;    //////// SSPIF is set at every 9th clock cycle
    PIR1bits.SSPIF = 0 ;
    if (SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while (PEN);
        return ;   ///////Exit the DS1307_write function 
    }
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __second = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __minute = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __hour = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __day = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __date = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __month = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __year = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 0 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    SSPCON2bits.RCEN = 1 ;   ////////Initiate receive conditions of I2C on SDA and SCL pins
    while (!SSPSTATbits.BF);
    __con = SSPBUF ;     ///// Register address 0x00 is loaded to SSPBUF and it is saved in variable __second
    
    SSPCON2bits.ACKDT = 1 ;   //// Acknowledge data bit , 0 = ack, 1 = not ack
    SSPCON2bits.ACKEN = 1 ; //// Initiate ACK conditions of I2C on SDA and SCL pins
    while (ACKEN) ;
    
    ////* Stop Bit */////////////
    SSPCON2bits.PEN = 1 ;
    while (PEN);
    
    lcd_command (0x88) ;
    bcd_to_ascii (__hour) ;
    lcd_data (':');
    bcd_to_ascii (__minute);
    lcd_data (':');
    bcd_to_ascii (__second);
    
    lcd_command (0xc7) ;
    bcd_to_ascii (__date) ;
    lcd_data ('/');
    bcd_to_ascii (__month);
    lcd_data ('/');
    bcd_to_ascii (__year);
}