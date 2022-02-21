/* ============================================================================
micro:			PIC18F4580
oscillatore:	interno, RA6/7 in/out digitali
=============================================================================*/

// CONFIGURATION BITS
#pragma config OSC = IRCIO67 	// Internal clock
#pragma config WDT = OFF 		// Watchdog Timer (WDT) disabled
#pragma config PWRT = ON 		// Power-up Timer enabled
#pragma config MCLRE = ON 		// MCLR/VPP pin function is MCLR
#pragma config PBADEN = OFF 	// PORTB<0,1,4> digital al power on
#pragma config DEBUG = ON  		// Debug ON
#pragma config LVP = OFF 		// Low-Voltage Programming disabled
#pragma config STVREN = ON 		// Stack Over/Underflow Reset enabled
#pragma config BOREN = OFF 		// Brown-out Reset disabled

/* ==========================================================================*/
//  DEFINIZIONI DI SISTEMA
/* ==========================================================================*/
#include <xc.h>
#define _XTAL_FREQ 4000000		//deve coincidere con il valore di frequenza
								//impostata con il registro <OSCCON>
