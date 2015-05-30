
// slave cu adresa 100
unsigned long tlong;
char string[3],rs485_rcv[8];
unsigned int adc_var;
signed short tempC;
sbit RS485_rxtx_pin at RA1_bit;     // transmit/receive control set to PORTA.B1
sbit RS485_rxtx_pin_direction at TRISA1_bit;   // RxTx pin direction set as output

void interrupt() {
  if(RCIF_bit) {
    RS485Slave_Receive(rs485_rcv);
    RCIF_bit=0;
  }
}

void ADC0 () {
    // Configure ADCON0 for channel 0
    ADCON0=0b00000001;
    Delay_ms(10);
    ADCON0.F1 = 1;     // start conversion, GO/DONE = 1
    while (ADCON0.F1); // wait for conversion
    adc_var=256*ADRESH + ADRESL;
    tlong = (long)adc_var; // Convert the result
    string[2] = 48+(tlong*100 / 100)%10;
    string[1] = 48+(tlong*10 / 100)%10;
    string[0] = 48+(tlong / 100)%10;
}
/*void TempSensor () {

  // Configure ADCON0 for int temp sensor
  //  ADCON0.CHS0 = 1;    //1
  //  ADCON0.CHS1 = 0;     //0
  //  ADCON0.CHS2 = 1;    //1
  //  ADCON0.CHS3 = 1;     //1
  //  ADCON0.CHS4 = 1;     //1
  //  ADCON0.ADON = 1;  // enable A/D converter
  ADCON0=0b01110101;
  Delay_ms(10);
  ADCON0.F1 = 1;     // start conversion, GO/DONE = 1
  while (ADCON0.F1); // wait for conversion
  adc_var=256*ADRESH + ADRESL;
  tlong=(long)(((0.863-(1.25*(1-(adc_var-570)/1023)))/0.00132)-40);
  string[4] = 48+(tlong/1)%10;
  string[3] = 48+(tlong/10)%10;
  //temp[0] = 48+(tlong1/100)%10;

}*/
   void readADC (char channel) {

    // Configure ADCON1
    ADCON1.ADPREF0 = 0; // Vref+ is VCC
    ADCON1.ADPREF1 = 0;
    ADCON1.ADCS0   = 0; // Use conversion clock, Fosc/32
    ADCON1.ADCS1   = 1; // Fosc = 500 KHz
    ADCON1.ADCS2   = 0;
    ADCON1.ADFM    = 1;  // result is right Justified
    if (channel == 0) ADC0 ();
   // else  TempSensor();
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
  RS485Slave_Init(100);                // intialize MCU as a Slave for RS-485 communication with address 100

  RCIE_bit=1;                              // RX interrupt active
  PEIE_bit=1;
  GIE_bit=1;                               // general interrupt enable

  //TSEN_bit=1;      //temp sensor enable
  //TSRNG_bit=1;        //temp sensor range  0=small range 1=extended
  
  TRISA0_bit=1;
  ANSELA.B0  = 1;
  TRISA1_bit=0;
  TRISA2_bit=0;
  ra1_bit=0;
  ra2_bit=0;
  
  
  while (1) {                     // Endless loop
  readADC(0);
  UART1_Write_Text(string); //3 bytes ASCII numbers 000-100
  UART1_Write(13);
  if (rs485_rcv[4]) {                    // upon completed valid message receive
      rs485_rcv[4] = 0;                    //   data[4] is set to 0
      if(rs485_rcv[0]==1 && rs485_rcv[1] == 0 && rs485_rcv[2] == 0 ) {   //if the incoming bytes match the address, reply
         RS485Slave_Send(string,1);        //   and send the procent back to master
      }
      Delay_ms(1);

    }

  ra2_bit=~ra2_bit;
  Delay_ms(500);
  }
}