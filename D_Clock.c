
#include <xc.h>
#include <pic16f877a.h>
#define _XTAL_FREQ 20000000

#pragma config FOSC = EXTRC     // Oscillator Selection bits (RC oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define RS PORTBbits.RB1   // Register select , when RS = 1 , data register is selected 
#define RW PORTBbits.RB2   // RW read write . when RW = 0 , write is enable . when RW = 1 , read is enable 
#define EN PORTBbits.RB3    // EN, Enable the functioning of LCD
#define button_exit PORTBbits.RB6   // Assign inpt
#define button_up PORTBbits.RB4   // Assign
#define button_down PORTBbits.RB5   // Assign
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
void DS1307_read (char slave_address, char register_address);
void bcd_to_ascii (unsigned char value) ;
char decimal_to_bcd (unsigned char value) ;
void lcd_command (unsigned char cmd) ;
void lcd_data (unsigned char data) ;
void lcd_init () ;
void lcd_string (const unsigned char *str, unsigned char num) ;
void set_timing () ;
void interrupt external () ;

unsigned char __second, __minute, __hour, __day, __date, __month, __year, __con ;
unsigned char timing_flag = 0;


void main() {
    INTCONbits.GIE = 1;     // Interrupt Control Register bit Global interrupt Enable is set 
    INTCONbits.PEIE = 1;    // Interrupt Control Register bit PEriperal interrupt Enable is set 
    INTCONbits.INTE = 1;    // Interrupt Control Register bit External Interrupt RB0 pin is Enable    
    OPTION_REGbits.INTEDG =1;  // Option Register bit Edge triggering is enable 
    
    SSPADD = 49 ;     // clock = fosc/(4*(SSPADD+1))   , I2C communication clock speed is determined by the SSPADD value
    SSPCON = 0x28 ;  /// Enable I2C communication and select mode of I2C as master mode, 
    
    TRISC = 0xff;
    TRISD =0x00;
    PORTD = 0x00;
    TRISBbits.TRISB0 =1;
    TRISBbits.TRISB1 =0;
    TRISBbits.TRISB2 =0;
    TRISBbits.TRISB3 =0;
    TRISBbits.TRISB4 =1;
    TRISBbits.TRISB5 =1;
    TRISBbits.TRISB6 =1;
    
    
    lcd_init();
    lcd_command (0x80);
    lcd_string ("CLOCK:",6);
    lcd_command (0xc0) ;
    lcd_string ("DATE:", 5) ;
    __delay_ms(100);
    
    while(1)
    {
        __delay_ms(20);
        DS1307_read (DS1307_address, 0);
        if(timing_flag > 0 ) {
        set_timing ();   
       }
    }
    
    return ;
}
/////* Functions *//////////////////////////////////////////////


void lcd_init(){
    lcd_command (0x38);
    lcd_command (0x06);
    lcd_command (0x0c);
    lcd_command (0x01);
}

void lcd_data (unsigned char data){
    PORTD = data;
    RS = 1;
    RW = 0;
    EN =1;
    __delay_ms(5);
    EN =0 ;
}

void lcd_command (unsigned char cmd){
    PORTD = cmd;
    RS = 0;
    RW = 0;
    EN =1;
    __delay_ms(5);
    EN =0 ;
}

void lcd_string (const unsigned char *str, unsigned char num) {
    unsigned char i ;
    for (i=0; i<num; i++)     // i will be incremented until it become less than the num value
    {
        lcd_data(str[i]);      // for example, if str = "SYSTEMS",   str[0]="S", str[1]="Y",...
    }
}

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

void bcd_to_ascii (unsigned char value) 
{
    unsigned char bcd ;
    bcd = value ;
    bcd = bcd & 0xf0 ;
    bcd = bcd >> 4 ;
    bcd = bcd | 0x30 ;
    lcd_data (bcd) ;
    
    bcd = value ;
    bcd = bcd & 0x0f ;
    bcd = bcd | 0x30 ;
    lcd_data (bcd) ;    
}

char bcd_to_decimal (unsigned char value)   // if vale eceived fom DS137 is 99, bcd fomat is 1001 1001
{
    unsigned char dec ;      
    dec = value ;       // now dec = 1001 1001
    dec = dec & 0xf0 ;   //  now dec = 1001 0000
    dec = dec >> 4 ;     //  now dec = ob 0000 1001  = 9
    dec = dec * 10 ;     // now dec = 90
    dec = dec + (value & 0x0f) ;   //  now dec = 99  
    return (dec) ; 
}

char decimal_to_bcd (unsigned char value) 
{
    unsigned char msb ;
    unsigned char lsb ;
    unsigned char hex ;
    
    msb = value / 10 ;
    lsb = value % 10 ;
    hex = ((msb<< 4) + lsb ) ;
    return (hex) ;           
}

void set_timing () 
{
unsigned char decimal_val_second ;
unsigned char bcd_val_second ;
unsigned char decimal_val_minute ;
unsigned char bcd_val_minute ;
unsigned char decimal_val_hour ;
unsigned char bcd_val_hour ;
unsigned char decimal_val_date ;
unsigned char bcd_val_date ;
unsigned char decimal_val_month ;
unsigned char bcd_val_month ;
unsigned char decimal_val_year ;
unsigned char bcd_val_year ;

        decimal_val_second = bcd_to_decimal (__second) ;
        bcd_val_second =__second ;
        decimal_val_minute = bcd_to_decimal (__minute) ;
        bcd_val_minute =__minute ;
        decimal_val_hour = bcd_to_decimal (__hour) ;
        bcd_val_hour =__hour ;
        decimal_val_date = bcd_to_decimal (__date) ;
        bcd_val_date =__date ;
        decimal_val_month = bcd_to_decimal (__month) ;
        bcd_val_month =__month ;
        decimal_val_year = bcd_to_decimal (__year) ;
        bcd_val_year =__year ;
        
        lcd_init ();
        
         while (button_exit == 0)
            {
//////////////////////////////////////////////////*  1   *//////////////////////////////////////////////////////////////////////////             
                if (timing_flag == 1)
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("SEC.:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_second) ;
                    __delay_ms(100);
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_second ++ ;
                           if(decimal_val_second == 60)
                           {
                             decimal_val_second =0 ;
                           }
                           __delay_ms(100);                           
                           //lcd_command (0x83);
                           //lcd_string ("SEC.:",5);
                           lcd_command (0x89) ;
                           bcd_val_second = decimal_to_bcd (decimal_val_second);
                           bcd_to_ascii (bcd_val_second) ;
                           lcd_init ();
                        }
                        if(button_down == 1)
                        {                       
                           while(button_down == 1 );
                           decimal_val_second -- ;
                           if(decimal_val_second == 0)
                           {
                              decimal_val_second = 59 ;
                           }
                           __delay_ms(100);                           
                           //lcd_command (0x83);
                           //lcd_string ("SEC.:",5);
                           lcd_command (0x89) ;
                           bcd_val_second = decimal_to_bcd (decimal_val_second);
                           bcd_to_ascii (bcd_val_second) ;
                           lcd_init ();
                        }
                    
                }
////////////////////////////////////////////////////////* 2  *//////////////////////////////////////////////////////////////////////////                
                if (timing_flag == 2)
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("MIN.:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_minute) ;
                    __delay_ms(100);
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_minute ++ ;
                           if(decimal_val_minute == 60)
                           {
                             decimal_val_minute =0 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("MIN.:",5);
                           lcd_command (0x89) ;
                           bcd_val_minute = decimal_to_bcd (decimal_val_minute);
                           bcd_to_ascii (bcd_val_minute) ;
                           lcd_init ();
                        }
                        if(button_down == 1)
                        {                       
                           while(button_down == 1 );
                           decimal_val_minute -- ;
                           if(decimal_val_minute == 0)
                           {
                              decimal_val_minute = 59 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("MIN.:",5);
                           lcd_command (0x89) ;
                           bcd_val_minute = decimal_to_bcd (decimal_val_minute);
                           bcd_to_ascii (bcd_val_minute) ;
                           lcd_init ();
                        }
                }
//////////////////////////////////////////////////////*  3  *////////////////////////////////////////////////////////////////////////
                if (timing_flag == 3)
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("hour:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_hour) ;
                    __delay_ms(100);
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_hour ++ ;
                           if(decimal_val_hour == 60)
                           {
                             decimal_val_hour =0 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("SEC.:",5);
                           lcd_command (0x89) ;
                           bcd_val_hour = decimal_to_bcd (decimal_val_hour);
                           bcd_to_ascii (bcd_val_hour) ;
                           lcd_init ();
                        }
                        if(button_down == 1)
                        {                       
                           while(button_down == 1 );
                           decimal_val_hour -- ;
                           if(decimal_val_hour == 0)
                           {
                              decimal_val_hour = 59 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("SEC.:",5);
                           lcd_command (0x89) ;
                           bcd_val_hour = decimal_to_bcd (decimal_val_hour);
                           bcd_to_ascii (bcd_val_hour) ;
                           lcd_init ();
                        }
                }
////////////////////////////////////////////////////*  4  *////////////////////////////////////////////////////////////////////////////                
                if (timing_flag == 4 )
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("DATE:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_date) ;
                    __delay_ms(100);
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_date ++ ;
                           if(decimal_val_date == 32)
                           {
                             decimal_val_date =0 ;
                           }
                           __delay_ms(100);                          
                           lcd_command (0x83);
                           lcd_string ("DATE:",5);
                           lcd_command (0x89) ;
                           bcd_val_date = decimal_to_bcd (decimal_val_date);
                           bcd_to_ascii (bcd_val_date) ;
                           lcd_init ();
                        }
                        if(button_down == 1)
                        {                       
                           while(button_down == 1 );
                           decimal_val_date -- ;
                           if(decimal_val_date == 0)
                           {
                              decimal_val_date = 31 ;
                           }
                           __delay_ms(100);                          
                           lcd_command (0x83);
                           lcd_string ("DATE:",5);
                           lcd_command (0x89) ;
                           bcd_val_date = decimal_to_bcd (decimal_val_date);
                           bcd_to_ascii (bcd_val_date) ;
                           lcd_init ();
                        }
                }
/////////////////////////////////////////////////*  5   */////////////////////////////////////////////////////////////////////////////
                if (timing_flag == 5)
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("MON.:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_month) ;
                    __delay_ms(100);
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_month ++ ;
                           if(decimal_val_month == 60)
                           {
                             decimal_val_month =0 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("SEC.:",5);
                           lcd_command (0x89) ;
                           bcd_val_month = decimal_to_bcd (decimal_val_month);
                           bcd_to_ascii (bcd_val_month) ;
                           lcd_init ();
                        }
                        if(button_down == 1)
                        {                       
                           while(button_down == 1 );
                           decimal_val_month -- ;
                           if(decimal_val_month == 0)
                           {
                              decimal_val_month = 59 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("SEC.:",5);
                           lcd_command (0x89) ;
                           bcd_val_month = decimal_to_bcd (decimal_val_month);
                           bcd_to_ascii (bcd_val_month) ;
                           lcd_init ();
                        }
                }
///////////////////////////////////////////////*  6   *///////////////////////////////////////////////////////////////////////////////
                if (timing_flag == 6)
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("yer.:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_year) ;
                    __delay_ms(100);
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_year ++ ;
                           if(decimal_val_year == 60)
                           {
                             decimal_val_year =0 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("yer.:",5);
                           lcd_command (0x89) ;
                           bcd_val_year = decimal_to_bcd (decimal_val_year);
                           bcd_to_ascii (bcd_val_year) ;
                           lcd_init ();
                        }
                        if(button_down == 1)
                        {                       
                           while(button_down == 1 );
                           decimal_val_year -- ;
                           if(decimal_val_year == 0)
                           {
                              decimal_val_year = 59 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("yer.:",5);
                           lcd_command (0x89) ;
                           bcd_val_year = decimal_to_bcd (decimal_val_year);
                           bcd_to_ascii (bcd_val_year) ;
                           lcd_init ();
                        }
                }                
                 
            
            
    }        

    lcd_init ();              ///////Befo
    lcd_command (0x80);
    lcd_string ("CLOCK:",6);
    lcd_command (0xc0) ;
    lcd_string ("DATE:", 5) ;
    lcd_command (0xc7) ;  
    timing_flag = 0 ;
    __delay_ms(300) ;
    DS1307_write (decimal_val_second, decimal_val_minute, decimal_val_hour,1, decimal_val_date, decimal_val_month, decimal_val_year);
}
void interrupt external () 
{                                  

     if (INTCONbits.INTF == 1) {
        INTCONbits.INTF = 0;
        lcd_init();
        timing_flag = 1 + timing_flag ;
        if (timing_flag == 7 ) {
            timing_flag = 1; // Set the flag when i is incremented
        }
    }
} 