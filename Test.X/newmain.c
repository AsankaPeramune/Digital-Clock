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
 
void bcd_to_ascii (unsigned char value) ;
char decimal_to_bcd (unsigned char value) ;
char bcd_to_decimal (unsigned char value) ;
void lcd_command (unsigned char cmd) ;
void lcd_data (unsigned char data) ;
void lcd_init () ;
void lcd_string (const unsigned char *str, unsigned char num) ;
void interrupt external () ;
void set_timing () ;

 unsigned char __second, __minute, __hour, __day, __date, __month, __year, __con ;
 unsigned char timing_flag = 0;

void main() {
    INTCONbits.GIE = 1;     // Interrupt Control Register bit Global interrupt Enable is set 
    INTCONbits.PEIE = 1;    // Interrupt Control Register bit PEriperal interrupt Enable is set 
    INTCONbits.INTE = 1;    // Interrupt Control Register bit External Interrupt RB0 pin is Enable    
    OPTION_REGbits.INTEDG =1;  // Option Register bit Edge triggering is enable 
    
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
    
    __second = 53 ;            ////// this is bcd 35 vale 0011 0101
    __date =   33 ;         ///this is bcd 21 = 0010 0001
    timing_flag = 0;
    
    while(1)
    {
       lcd_command (0x88) ;
       bcd_to_ascii (__second) ;
       __delay_ms(300);
       lcd_command (0xc7) ;
       bcd_to_ascii (__date) ;
       __delay_ms(300);
       if(timing_flag > 0 ) {
        set_timing ();   
       }
    }
    
    return ;
}

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
    __delay_ms(2);
    EN =0 ;
}

void lcd_string (const unsigned char *str, unsigned char num) {
    unsigned char i ;
    for (i=0; i<num; i++)     // i will be incremented until it become less than the num value
    {
        lcd_data(str[i]);      // for example, if str = "SYSTEMS",   str[0]="S", str[1]="Y",...
    }
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
        decimal_val_second = bcd_to_decimal (__minute) ;
        bcd_val_minute =__minute ;
        decimal_val_second = bcd_to_decimal (__hour) ;
        bcd_val_hour =__hour ;
        decimal_val_date = bcd_to_decimal (__date) ;
        bcd_val_date =__date ;
        decimal_val_month = bcd_to_decimal (__month) ;
        bcd_val_month =__month ;
        decimal_val_year = bcd_to_decimal (__year) ;
        bcd_val_year =__year ;
        
        lcd_init ();
        
         while (timing_flag > 0)
            {
//////////////////////////////////////////////////*  1   *//////////////////////////////////////////////////////////////////////////             
                if (timing_flag == 1)
                {
                    lcd_command(0x01);
                    lcd_command (0x83);
                    lcd_string ("SEC.:",5);
                    lcd_command (0x89) ;
                    bcd_to_ascii (bcd_val_second) ;
                    
                        if(button_up == 1)
                        {
                           while(button_up == 1 );
                           decimal_val_second ++ ;
                           if(decimal_val_second == 60)
                           {
                             decimal_val_second =0 ;
                           }
                           __delay_ms(100);                           
                           lcd_command (0x83);
                           lcd_string ("SEC.:",5);
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
                           lcd_command (0x83);
                           lcd_string ("SEC.:",5);
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
    __second = bcd_val_second  ;
    __minute = bcd_val_minute  ;
    __hour = bcd_val_hour  ;
    __date = bcd_val_date ;
    __month = bcd_val_month ;
    __year = bcd_val_year ;
    __delay_ms(300) ;
}
void interrupt external () 
{                                  

     if (INTCONbits.INTF == 1) {
        INTCONbits.INTF = 0;
        lcd_init();
        timing_flag = 1 + timing_flag ;
        if (timing_flag == 7 ) {
            timing_flag = 0; // Set the flag when i is incremented
        }
       // else {
       //  timing_flag = 0;   
       // }
        
    }
} 