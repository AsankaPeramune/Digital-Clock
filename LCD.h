#define RS PORTBbits.RB1   // Register select , when RS = 1 , data register is selected 
#define RW PORTBbits.RB2   // RW read write . when RW = 0 , write is enable . when RW = 1 , read is enable 
#define EN PORTBbits.RB3    // EN, Enable the functioning of LCD


void lcd_command (unsigned char cmd) ;
void lcd_data (unsigned char data) ;
void lcd_init () ;
void lcd_string (const unsigned char *str, unsigned char num) ;

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

 
    
