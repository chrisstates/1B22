#include "i2c.h"
unsigned long temp;
char uart_rd, uart_rd1,tempF;
unsigned short MS_Byte, LS_Byte, MS_old, LS_old, avg_MS, avg_LS;
signed short tempC;
signed int tempOffset;
char output[11];
int i;

  void readIntTemp () {
       // get old temps...
       MS_old = MS_Byte;
       LS_old = LS_Byte;
    // Configure FVR to 2.048 V for ADC
    FVRCON = 0b11000010 ;
    TSEN_bit=1;      //temp sensor enable
    TSRNG_bit=1;        //temp sensor range
    // Configure ADCON1
    ADCON1.ADPREF0 = 0; // Vref+ is 2.048v
    ADCON1.ADPREF1 = 0;
    ADCON1.ADCS0   = 0; // Use conversion clock, Fosc/32
    ADCON1.ADCS1   = 1; // Fosc = 500 KHz
    ADCON1.ADCS2   = 0;
    ADCON1.ADFM    = 1;  // result is right Justified
    // Configure ADCON0 for channel TEMP
    ADCON0.CHS0 = 1;    //1
    ADCON0.CHS1 = 0;     //0
    ADCON0.CHS2 = 1;    //1
    ADCON0.CHS3 = 1;     //1
    ADCON0.CHS4 = 1;     //1
    ADCON0.ADON = 1;  // enable A/D converter

    ADCON0.F1 = 1;     // start conversion, GO/DONE = 1
    while (ADCON0.F1); // wait for conversion
    MS_Byte = ADRESH;
    LS_Byte = ADRESL;
    avg_MS=(MS_Byte+MS_old)/2;
    avg_LS=(LS_Byte+LS_old)/2;
    temp = avg_MS*256 + avg_LS;
    tempC = (2*temp - 400)/19.5+tempOffset;
    Delay_ms(200);

    TSEN_bit=0;
    ADCON0.ADON = 0;
  }

void main() {
  OSCCON = 0b01111011 ; // INTERNAL OSC 16MHz
  ANSELA  = 0;
  CM1CON0 = 0x00;   // Disable comparators

  RXDTSEL_bit = 1;
  TXCKSEL_bit = 1;    //alternate pins RX=RA4, TX=RA5

  UART1_Init(9600);                  // Initialize UART module at 9600 bps
  Delay_ms(100);                     // Wait for UART module to stabilize
  UART1_Write_Text("Press m for menu");
  ADC_Init();                        // init adc with default settings
  TRISA2_bit=0;                      //*
  RA2_bit=1;                         //* heartbeat init
   // i2c init
  SDOSEL_bit =0;     // i2c init bits --- RA0 and RA1
  SSSEL_bit =0;      //

  
  tempOffset=  EEPROM_Read(0x00);    // temperature offset is in EEPROM at 0x00
  if (tempOffset==0xFF)  {
     EEPROM_Write(0x00,0xFD);        //init temp offset
     tempOffset = 0xFD;
  }

  while (1) {                     // Endless loop
  i=0;
    while(i<11){
    output[i]=0x00;
    i++;
  }
    if (UART1_Data_Ready()) {     // If data is received,
      UART1_Read_Text(output, 0x77, 11); //reads text until '\n' is found
      /*uart_rd = UART1_Read();
      if (uart_rd==0x6F)          //char 'o'--offset
        while (UART1_Data_Ready()){
           uart_rd1= UART1_Read();
           EEPROM_Write(0x00,uart_rd1);
      UART1_Write(uart_rd);       // and send data via UART*/
    }
    UART1_Write_Text(output);
    RA2_bit=~RA2_bit; //heartbeat
    readIntTemp();
    UART1_Write(MS_Byte);
    Delay_ms(50);
    UART1_Write(LS_Byte);
    UART1_Write(tempC);
    UART1_Write(13);                  // ASCII carriage return
    Delay_ms(500);
  }
}