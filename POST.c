/* 
 * File:   MAIN10.c
 * Author: ANGELA
 *
 * Created on 10 de mayo de 2023, 20:44
 */

//LIBRERIAS
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//DIRECTIVAS DEL COMPILADOR
#define _XTAL_FREQ 4000000

// VARIABLES

unsigned char FLAG = 0x00;
unsigned char FLAG1 = 0x00;
unsigned char tempRX;
unsigned char CONT;
int select;
unsigned char contint;
unsigned char unidad;
unsigned char decena;
unsigned char centena;
char var, con;
int full;

//PROTOTIPOS DE FUNCIONES
void setup(void);
void decimal(uint8_t var);

//FUNCION PARA ISR
void __interrupt() isr(void){
    if(INTCONbits.RBIF == 1){
        if(PORTBbits.RB0 == 0){
            FLAG = 1;}
        else{
            if(FLAG == 1){
                FLAG = 0;
                contint++;
            }
        }
        if(PORTBbits.RB1 == 0){
            FLAG1 = 1;}
        else{
            if(FLAG1 == 1){
                FLAG1 = 0;
                contint--;
            }    
        }
        INTCONbits.RBIF = 0;
    }
    if(PIR1bits.RCIF == 1){
        tempRX = RCREG;
        if(tempRX == 0x00){
            if(PIR1bits.TXIF == 1){
                TXREG = cen;
                __delay_ms(1);
                }
            if(PIR1bits.TXIF == 1){
                TXREG = dec;
                __delay_ms(1);
                }
            if(PIR1bits.TXIF == 1){
                TXREG = un;
                __delay_ms(1);
                }
        }    
            PIR1bits.RCIF = 0;
            
            if(tempRX == 0xff){
                while(PIR1bits.RCIF == 0){    
                }
                centena = RCREG;
                PIR1bits.RCIF = 0;
                
                while(PIR1bits.RCIF == 0){}
                decena = RCREG;
                PIR1bits.RCIF = 0;
                
                while(PIR1bits.RCIF == 0){}
                unidad = RCREG;
                PIR1bits.RCIF = 0;
                
                centena = centena - 48;
                decena = decena - 48;
                unidad = unidad - 48;
                
                PORTD = ((centena*100)+(decena*10)+unidad);
            }
            PIR1bits.RCIF = 0;
        
    }
}
//LOOP PRINCIPAL
void setup(void){
    //configuracion de los puertos
    ANSEL = 0X00;
    ANSELH = 0X00;
    
    TRISA = 0X00;
    TRISB = 0B00000011;
    TRISD = 0X00;
    TRISE = 0X00;
    
    PORTA = 0X00;
    PORTB = 0X00;
    PORTC = 0X00;
    PORTD = 0X00; 
    PORTE = 0X00;
    
    //configuracion del oscilador
    OSCCONbits.SCS = 1;
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    
    //weak pull up
    IOCB = 0B00000011;
    WPUB = 0B00000011;
    
    //Config de los special register
    OPTION_REGbits.nRBPU = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    
    //configuracion UART
    PIR1bits.RCIF = 0;
    PIR1bits.TXIF = 0;
    PIE1bits.RCIE = 1;
    PIE1bits.TXIE = 0;
    TXSTAbits.TX9 = 0;
    TXSTAbits.TXEN = 1;
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    RCSTAbits.SPEN = 1;
    
    //generador de bauidos del UART
    BAUDCTLbits.BRG16 = 0;
    SPBRG = 25;
    SPBRGH = 1;
    
    }
//Funciones
void main(void){
    setup();
    while(1){
        decimal(contint);
        __delay_ms(200);
    }
}

//FUNCIONES
void putch(char info){
    while (TXIF == 0);
    TXREG = info;
}

void decimal(uint8_t var){
    uint8_t VAL;
    VAL = var;
    cen = (VAL/100);
    VAL = (VAL - (cen*100));
    dec = (VAL/10);
    VAL = (VAL - (dec*10));
    un = (VAL);
    
    cen = cen + 48;
    dec = dec + 48;
    un = un + 48;
  
}
