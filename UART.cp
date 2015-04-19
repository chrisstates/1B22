#line 1 "D:/_projects/PicProjects/12f1822/ADCSer/UART.c"
unsigned long temp;
char uart_rd,tempF;
unsigned short MS_Byte, LS_Byte;
signed short tempC;



void main() {
 OSCCON = 0b01111011 ;

 ANSELA = 0;
 CM1CON0 = 0x00;

 RXDTSEL_bit = 1;
 TXCKSEL_bit = 1;


 UART1_Init(9600);
 Delay_ms(100);


 FVRCON = 0b11000010 ;


 while (1) {
 if (UART1_Data_Ready()) {
 uart_rd = UART1_Read();
 UART1_Write(uart_rd);
 }
 UART1_Write(MS_Byte);
 Delay_ms(50);
 UART1_Write(LS_Byte);
 UART1_Write(13);
 Delay_ms(500);
 }
}
void readIntTemp () {
 TSEN_bit=1;
 TSRNG_bit=1;

 ADCON1.ADPREF0 = 0;
 ADCON1.ADPREF1 = 0;
 ADCON1.ADCS0 = 0;
 ADCON1.ADCS1 = 1;
 ADCON1.ADCS2 = 0;
 ADCON1.ADFM = 1;

 ADCON0.CHS0 = 1;
 ADCON0.CHS1 = 0;
 ADCON0.CHS2 = 1;
 ADCON0.CHS3 = 1;
 ADCON0.CHS4 = 1;
 ADCON0.ADON = 1;
 ADC_Init();
 ADCON0.F1 = 1;
 while (ADCON0.F1);
 MS_Byte = ADRESH;
 LS_Byte = ADRESL;
 temp = MS_Byte*256 + LS_Byte;
 Delay_ms(200);

 TSEN_bit=0;
 ADCON0.ADON = 0;
}
