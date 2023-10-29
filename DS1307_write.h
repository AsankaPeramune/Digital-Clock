#define DS1307_address 0xd0 
#define second 0x00
#define minute 0x01
#define hour 0x02
#define day 0x03
#define date 0x04
#define month 0x05
#define year 0x06
#define control_reg 0x07

void DS1307_write (char _second, char _minute, char _hour, char _day, char _date, char _month, char _year) ;

void DS1307_write (char _second, char _minute, char _hour, char _day, char _date, char _month, char _year)
{
    ///* Start Bit *///////
    SSPCON2bits.SEN = 1;    /// initiate star condition for I2C communication on SDA and SCL pins
    while (SEN);
    PIR1bits.SSPIF =0 ;

    ////* Writing Slave Address *///////////////
    SSPBUF = DS1307_address ;
    while (!SSPIF) ;    //////// SSPIF is set at every 9th clock cycle
    PIR1bits.SSPIF = 0 ;
    if (SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while (PEN);
        return ;   ///////Exit the DS1307_write function 
    }
    
    ////* Write Internal Register Address *//////////////////
    SSPBUF = second ;
    while (!SSPIF);
    PIR1bits.SSPIF = 0 ;
    if (SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while (PEN);
        return ;   ///////Exit the DS1307_write function 
    }
    
    SSPBUF = decimal_to_bcd (_second) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = decimal_to_bcd (_minute) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = decimal_to_bcd (_hour) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = decimal_to_bcd (_day) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = decimal_to_bcd (_date) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = decimal_to_bcd (_month) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = decimal_to_bcd (_year) ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    SSPBUF = 0x00 ;
    while(!SSPIF);   ///// Wait till SDA line is pulling low through out the 9th clk cycle
    PIR1bits.SSPIF = 0 ;
    
    ///* Stop Bit *//////////////////
    SSPCON2bits.PEN = 1 ;
    while(PEN) ;
    PIR1bits.SSPIF = 0 ;   
}