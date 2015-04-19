unsigned long temp;
char uart_rd,tempF;
unsigned short MS_Byte, LS_Byte;
signed short tempC;



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
  }
}
}