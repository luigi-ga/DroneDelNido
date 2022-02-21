
#include "config_word_18F4580.h"

unsigned short numero_overflow;

float distanza_in_metri;
int distanza_in_centimetri;
int valore_partenza;
short int stato_display;

float velocita;

int centinaiaV;
int decineV;
int unitaV;
int resto_divisione;

int i;

int Ttx;
float tempo;
int valore_TMR0L;

#define DCD LATC1                //Display Centinaia distanza
#define DDD LATC2                //Display Decine distanza
#define DUD LATC3                //Display Unità distanza
#define burst LATC0              //pin su cui genero il burst


/*array contenente la codifica per i display*/

unsigned short CODIFICA [10] =
{
    //afbgc.de
    0b11101011,         //0
    0b00101000,         //1
    0b10110011,         //2
    0b10111010,         //3
    0b01111000,         //4
    0b11011010,         //5
    0b11011011,         //6
    0b10101000,         //7
    0b11111011,         //8
    0b11111010          //9
};

/*subroutine che gestisce la visualizzazione, adoperando la tecnica del multiplexing*/

void visualizzazione (void)
{
    switch(stato_display)
    {
        case 0:                                  //display centinaia
            DUD=0;                               //spengo il display precedentemente attivo
            LATA = CODIFICA[centinaiaV];         //codifico il numero
            LATA2=1;                             //accendo il dot
            DCD=1;                               //attivo il display
            stato_display=1;                     //cambio stato
            break;

        case 1:                                  //display decine
            DCD=0;                               //spendo il display precedentemente attivo
            LATA2=0;                             //spengo il dot
            LATA = CODIFICA[decineV];            //codifico il display
            DDD=1;                               //attivo il display
            stato_display=2;                     //cambio stato
            break;

        case 2:
            DDD=0;
            LATA = CODIFICA[unitaV];
            DUD=1;
            stato_display=0;
            break;
    }
}


/*subroutine che assegna i valori da visualizzare sui display*/
void valori_display (void)
{
    centinaiaV = distanza_in_centimetri/100;
    resto_divisione = distanza_in_centimetri%100;
    decineV = resto_divisione/10;
    unitaV = resto_divisione%10;
}


void high_priority interrupt irs1 (void)
{
    if(TMR2IF==1)
    {
        TMR2ON=0;
        burst=!burst;                   //commuto il pin
        TMR2=250;                       //reimposto il valore di partenza del ti
        numero_overflow++;              //incremento la variabilemer
        if(numero_overflow==16)         //dopo 16 overflow
        {
            numero_overflow=0;          //resetto la variabile
            TMR2ON=0;                   //disabilito il timer
        }
        TMR2IF=0;                       //resetto il bit flag        
        TMR2=241;
        TMR2ON=1;
    }
}



void low_priority interrupt irs2 (void)
{
    if(TMR1IF==1)
    {
        TMR1H=valore_partenza/256;          //reimposto i...
        TMR1L=valore_partenza%256;          //valori di partenza

        TMR2=241;                           //reimposto il valore di partenza del timer2

        TMR0H=0;                            //reimposto il valore
        TMR0L=0;                            //di partenza del timer0
        TMR0ON=1;                           //avvio il conteggio del timer0

        TMR1IF=0;                           //resetto il bit flag
        TMR2ON=1;                           //riabilito il timer2
    }

    if(TMR3IF==1)
    {
        valori_display();                   //assegno i valori
        visualizzazione();                  //visualizzo i valori
        TMR3H=0xF0;                         //riconfiguro il valore di
        TMR3L=0x60;                         //partenza del timer3
        TMR3IF=0;                           //resetto il bit flag
    }

    if(INT1IF==1)
    {
        TMR0ON=0;                                //disabilito il timer
        valore_TMR0L=TMR0L;
        Ttx=TMR0H*256+valore_TMR0L;
        if(Ttx>30000)
            Ttx=30000;
        tempo=Ttx/1000000;                       //riporto il tempo in secondi
        TMR0H=0;                                 //resetto il
        TMR0L=0;                                 //timer
        INT1IF=0;
    }
}


void main (void)
{    
    OSCCON = 0x60;	            //freq. int. =
    ADCON1 = 0b00001111;            //I/O digitali
    TRISA = 0b00000000;             //uscite display
    TRISC = 0b00000000;
    TRISB = 0b11111111;


    LATA=0;
    burst=0;

    velocita = 343.8;   

    numero_overflow=0;
    stato_display=0;


    //CONFIGURAZIONE TIMER

    T0CON = 0b00001000;             //Timer disabilitato, clock interno, NO PSA
    T1CON = 0b10010001;             //PSA 1:2, clock interno (FOSC/4), timer abilitato
    T2CON = 0b00000100;             //postscale 1:1, prescaler 1, timer abilitato
    T3CON = 0b10000001;             //prescaler 1:1, clock interno (FOSC/4), timer abilitato
    
    
    //CONFIGURO L'INTERRUPT

    IPEN=1;                         //abilito i livelli di priorità

    TMR1IF=0;                       //bit flag resettato
    TMR1IE=0;                       //interrupt da timer1 abilitato
    TMR1IP=0;                       //interrupt di priorità bassa
    
    TMR2IF=0;                       //bit flag resettato
    TMR2IE=1;                       //interrupt da timer2 abilitato
    TMR2IP=1;                       //interrupt di priorità alta

    TMR3IF=0;                       //bit flag resettato
    TMR3IE=0;                       //interrupt da timer3 abilitato
    TMR3IP=0;                       //interrupt di priorità bassa

    INT1IF=0;                       //bit flag resettato
    INT1IE=0;                       //interruzioni da INT1 abilitate
    INT1IP=0;                       //priorità bassa
    INTEDG1=0;                      //interrupt su fronti di discesa

    PEIE=1;                         //abilito l'interrupt
    GIEH=1;                         //abilito l'interrupt generale (HIGH)
    GIEL=1;                         //abilito l'interrupt generale (LOW)
    

    while(1);
    {
        distanza_in_metri=tempo*velocita;

        valore_partenza=65536-(50*distanza_in_centimetri);
        distanza_in_centimetri=distanza_in_metri*100;

        if(valore_partenza>60536)                      /*il periodo di generazione del 
                                                        *burst si ferma ad un minimo di 10ms*/
            valore_partenza=60536;
    }  
}