
_readIntTemp:

;UART.c,6 :: 		void readIntTemp () {
;UART.c,7 :: 		TSEN_bit=1;      //temp sensor enable
	BSF        TSEN_bit+0, BitPos(TSEN_bit+0)
;UART.c,8 :: 		TSRNG_bit=1;        //temp sensor range
	BSF        TSRNG_bit+0, BitPos(TSRNG_bit+0)
;UART.c,10 :: 		ADCON1.ADPREF0 = 0; // Vref+ is 2.048v
	BCF        ADCON1+0, 0
;UART.c,11 :: 		ADCON1.ADPREF1 = 0;
	BCF        ADCON1+0, 1
;UART.c,12 :: 		ADCON1.ADCS0   = 0; // Use conversion clock, Fosc/32
	BCF        ADCON1+0, 4
;UART.c,13 :: 		ADCON1.ADCS1   = 1; // Fosc = 500 KHz
	BSF        ADCON1+0, 5
;UART.c,14 :: 		ADCON1.ADCS2   = 0;
	BCF        ADCON1+0, 6
;UART.c,15 :: 		ADCON1.ADFM    = 1;  // result is right Justified
	BSF        ADCON1+0, 7
;UART.c,17 :: 		ADCON0.CHS0 = 1;    //1
	BSF        ADCON0+0, 2
;UART.c,18 :: 		ADCON0.CHS1 = 0;     //0
	BCF        ADCON0+0, 3
;UART.c,19 :: 		ADCON0.CHS2 = 1;    //1
	BSF        ADCON0+0, 4
;UART.c,20 :: 		ADCON0.CHS3 = 1;     //1
	BSF        ADCON0+0, 5
;UART.c,21 :: 		ADCON0.CHS4 = 1;     //1
	BSF        ADCON0+0, 6
;UART.c,22 :: 		ADCON0.ADON = 1;  // enable A/D converter
	BSF        ADCON0+0, 0
;UART.c,23 :: 		ADC_Init();
	CALL       _ADC_Init+0
;UART.c,24 :: 		ADCON0.F1 = 1;     // start conversion, GO/DONE = 1
	BSF        ADCON0+0, 1
;UART.c,25 :: 		while (ADCON0.F1); // wait for conversion
L_readIntTemp0:
	BTFSS      ADCON0+0, 1
	GOTO       L_readIntTemp1
	GOTO       L_readIntTemp0
L_readIntTemp1:
;UART.c,26 :: 		MS_Byte = ADRESH;
	MOVF       ADRESH+0, 0
	MOVWF      _MS_Byte+0
;UART.c,27 :: 		LS_Byte = ADRESL;
	MOVF       ADRESL+0, 0
	MOVWF      _LS_Byte+0
;UART.c,28 :: 		temp = MS_Byte*256 + LS_Byte;
	CLRF       _temp+3
	MOVF       _MS_Byte+1, 0
	MOVWF      _temp+2
	MOVF       _MS_Byte+0, 0
	MOVWF      _temp+1
	CLRF       _temp+0
	MOVF       _LS_Byte+0, 0
	ADDWF      _temp+0, 1
	MOVLW      0
	ADDWFC     _temp+1, 1
	MOVLW      0
	BTFSC      _temp+1, 7
	MOVLW      255
	MOVWF      _temp+2
	MOVWF      _temp+3
;UART.c,29 :: 		Delay_ms(200);
	MOVLW      5
	MOVWF      R11
	MOVLW      15
	MOVWF      R12
	MOVLW      241
	MOVWF      R13
L_readIntTemp2:
	DECFSZ     R13, 1
	GOTO       L_readIntTemp2
	DECFSZ     R12, 1
	GOTO       L_readIntTemp2
	DECFSZ     R11, 1
	GOTO       L_readIntTemp2
;UART.c,31 :: 		TSEN_bit=0;
	BCF        TSEN_bit+0, BitPos(TSEN_bit+0)
;UART.c,32 :: 		ADCON0.ADON = 0;
	BCF        ADCON0+0, 0
;UART.c,33 :: 		}
L_end_readIntTemp:
	RETURN
; end of _readIntTemp

_main:

;UART.c,35 :: 		void main() {
;UART.c,36 :: 		OSCCON = 0b01111011 ; // INTERNAL OSC 16MHz
	MOVLW      123
	MOVWF      OSCCON+0
;UART.c,38 :: 		ANSELA  = 0;
	CLRF       ANSELA+0
;UART.c,39 :: 		CM1CON0 = 0x00;   // Disable comparators
	CLRF       CM1CON0+0
;UART.c,41 :: 		RXDTSEL_bit = 1;
	BSF        RXDTSEL_bit+0, BitPos(RXDTSEL_bit+0)
;UART.c,42 :: 		TXCKSEL_bit = 1;    //alternate pins RX=RA4, TX=RA5
	BSF        TXCKSEL_bit+0, BitPos(TXCKSEL_bit+0)
;UART.c,45 :: 		UART1_Init(9600);                // Initialize UART module at 9600 bps
	BSF        BAUDCON+0, 3
	MOVLW      160
	MOVWF      SPBRG+0
	MOVLW      1
	MOVWF      SPBRG+1
	BSF        TXSTA+0, 2
	CALL       _UART1_Init+0
;UART.c,46 :: 		Delay_ms(100);                    // Wait for UART module to stabilize
	MOVLW      3
	MOVWF      R11
	MOVLW      8
	MOVWF      R12
	MOVLW      119
	MOVWF      R13
L_main3:
	DECFSZ     R13, 1
	GOTO       L_main3
	DECFSZ     R12, 1
	GOTO       L_main3
	DECFSZ     R11, 1
	GOTO       L_main3
;UART.c,49 :: 		FVRCON = 0b11000010 ;
	MOVLW      194
	MOVWF      FVRCON+0
;UART.c,53 :: 		while (1) {                     // Endless loop
L_main4:
;UART.c,54 :: 		if (UART1_Data_Ready()) {     // If data is received,
	CALL       _UART1_Data_Ready+0
	MOVF       R0, 0
	BTFSC      STATUS+0, 2
	GOTO       L_main6
;UART.c,55 :: 		uart_rd = UART1_Read();
	CALL       _UART1_Read+0
	MOVF       R0, 0
	MOVWF      _uart_rd+0
;UART.c,56 :: 		UART1_Write(uart_rd);       // and send data via UART
	MOVF       R0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;UART.c,57 :: 		}
L_main6:
;UART.c,58 :: 		readIntTemp();
	CALL       _readIntTemp+0
;UART.c,59 :: 		UART1_Write(MS_Byte);
	MOVF       _MS_Byte+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;UART.c,60 :: 		Delay_ms(50);
	MOVLW      2
	MOVWF      R11
	MOVLW      4
	MOVWF      R12
	MOVLW      186
	MOVWF      R13
L_main7:
	DECFSZ     R13, 1
	GOTO       L_main7
	DECFSZ     R12, 1
	GOTO       L_main7
	DECFSZ     R11, 1
	GOTO       L_main7
	NOP
;UART.c,61 :: 		UART1_Write(LS_Byte);
	MOVF       _LS_Byte+0, 0
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;UART.c,62 :: 		UART1_Write(13);                  // ASCII carriage return
	MOVLW      13
	MOVWF      FARG_UART1_Write_data_+0
	CALL       _UART1_Write+0
;UART.c,63 :: 		Delay_ms(500);
	MOVLW      11
	MOVWF      R11
	MOVLW      38
	MOVWF      R12
	MOVLW      93
	MOVWF      R13
L_main8:
	DECFSZ     R13, 1
	GOTO       L_main8
	DECFSZ     R12, 1
	GOTO       L_main8
	DECFSZ     R11, 1
	GOTO       L_main8
	NOP
	NOP
;UART.c,64 :: 		}
	GOTO       L_main4
;UART.c,65 :: 		}
L_end_main:
	GOTO       $+0
; end of _main
