unsigned long tlong;
char uart_rd, procent[3], temp[5];
unsigned int adc_var;
signed short tempC;


   void ADC0 () {
     // Configure ADCON0 for channel 0
//    ADCON0.CHS0 = 0;    //1
//    ADCON0.CHS1 = 0;     //0
//    ADCON0.CHS2 = 0;    //1
//    ADCON0.CHS3 = 0;     //1
//    ADCON0.CHS4 = 0;     //1
//    ADCON0.ADON = 1;  // enable A/D converter
      ANSELA.B0  = 1;
      adc_var = ADC_Read(0);
      tlong = (long)adc_var; // Convert the result in millivolts
      procent[2] = 48+(tlong*100 / 1023)%10;
      procent[1] = 48+(tlong*10 / 1023)%10;
      procent[0] = 48+(tlong / 1023)%10;

      Delay_ms(1);

    Delay_ms(50);
  }
  void TempSensor () {
    TSEN_bit=1;      //temp sensor enable
    TSRNG_bit=1;        //temp sensor range  0=small range 1=extended
    adc_var = ADC_Read(29);
    tlong = (long)(adc_var*10); // Convert the result deg +20 // form orig e (2*adc_read-400)/20), pe care am simplificat-o in (adc_read/10-20)
      temp[4] = 48+(tlong/1)%10;
      temp[3] = 48+(tlong/10)%10;
      temp[2] = 48+(tlong/100)%10;
      temp[1] = 48+(tlong/1000)%10;
      temp[0] = 48+(tlong/10000)%10;
    // Configure ADCON0 for int temp sensor
  //  ADCON0.CHS0 = 1;    //1
  //  ADCON0.CHS1 = 0;     //0
  //  ADCON0.CHS2 = 1;    //1
  //  ADCON0.CHS3 = 1;     //1
  //  ADCON0.CHS4 = 1;     //1
  //  ADCON0.ADON = 1;  // enable A/D converter
  //  Delay_ms(50);
  }
   void readADC (char channel) {

    // Configure ADCON1
    ADCON1.ADPREF0 = 0; // Vref+ is VCC
    ADCON1.ADPREF1 = 0;
    ADCON1.ADCS0   = 0; // Use conversion clock, Fosc/32
    ADCON1.ADCS1   = 1; // Fosc = 500 KHz
    ADCON1.ADCS2   = 0;
    ADCON1.ADFM    = 1;  // result is right Justified
    if (channel == 0) ADC0 ();
    else  TempSensor();
//    ADCON0.F1 = 1;     // start conversion, GO/DONE = 1
//    while (ADCON0.F1); // wait for conversion
//    MS_Byte = ADRESH;
//    LS_Byte = ADRESL;


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
  TRISA0_bit=1;
  TRISA2_bit=0;
  ra2_bit=0;
  while (1) {                     // Endless loop
    if (UART1_Data_Ready()) {     // If data is received,
      uart_rd = UART1_Read();
      UART1_Write(uart_rd);       // and send data via UART
    }
    readADC(0);
    readADC(29);
    UART1_Write_Text(procent); //3 bytes ASCII numbers 000-100
    UART1_Write_Text(temp);    //3 bytes ASCII numbers 00000-99999  -- temperatura citita + 20, in centiGrade xxx,xx
    //UART1_Write(13);                  // ASCII carriage return
    UART1_Write(13);
    ra2_bit=~ra2_bit;
    Delay_ms(500);
  }
}