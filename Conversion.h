void bcd_to_ascii (unsigned char value) ;
char decimal_to_bcd (unsigned char value) ;
char bcd_to_decimal (unsigned char value) ;

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