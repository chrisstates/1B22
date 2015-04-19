unsigned long temp;
char uart_rd,tempF;
unsigned short MS_Byte, LS_Byte;
signed short tempC;

  void readIntTemp () {
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
    ADC_Init();
    ADCON0.F1 = 1;     // start conversion, GO/DONE = 1
    while (ADCON0.F1); // wait for conversion
    MS_Byte = ADRESH;
    LS_Byte = ADRESL;
    temp = MS_Byte*256 + LS_Byte;
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


  UART1_Init(9600);                // Initialize UART module at 9600 bps
  Delay_ms(100);                    // Wait for UART module to stabilize
  
   // Configure FVR to 2.048 V for ADC
  FVRCON = 0b11000010 ;
  
  while (1) {                     // Endless loop
    if (UART1_Data_Ready()) {     // If data is received,
      uart_rd = UART1_Read();
      UART1_Write(uart_rd);       // and send data via UART
    }
    readIntTemp();
    UART1_Write(MS_Byte);
    Delay_ms(50);
    UART1_Write(LS_Byte);
    UART1_Write(13);                  // ASCII carriage return
    Delay_ms(500);
  }
}
