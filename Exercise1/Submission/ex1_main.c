/******************************************************************************
 *                                                                            *
 * Exercise 1                                                                 *
 *                                                                            *
 * Task 1: Hello World                                                        *
 * Task 2: Advanced Hello World                                               *
 * Task 3: Modified Hello World             X /  4 Points                     *
 * Task 4: United States of Hello World     X /  4 Points                     *
 * Comprehension Questions                  X /  2 Points                     *
 *                                        ----------------                    *
 *                                         XX / 10 Points                     *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * name:                    your name                                         *
 * matriculation number:    123456789                                         *
 * e-mail:                  your.name@student.uni-siegen.de                   *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * Hardware Setup                                                             *
 *                                                                            *
 *                               MSP430FR5969                                 *
 *                            -----------------                               *
 *                           |                 |                              *
 *                           |             P4.6|--> (LED1)                    *
 *                           |             P1.0|--> (LED2)                    *
 *                           |                 |                              *
 *                           |             P2.0|--> |UART Tx>                 *
 *                           |             P2.1|<-- <UART Rx|                 *
 *                            -----------------                               *
 *                                                                            *
 ******************************************************************************/

// Select the task you are working on:
//#define LOOP_1
#define LOOP_2

#include <msp430fr5969.h>

void uartTx(char *data);

/* MAIN PROGRAM */
void main(void)
{
    // Stop watchdog timer.

    WDTCTL = WDTPW | WDTHOLD;

    // Initialize the clock system to generate 1 MHz DCO clock.
    CSCTL0_H    = CSKEY_H;              // Unlock CS registers.
    CSCTL1      = DCOFSEL_0;            // Set DCO to 1 MHz, DCORSEL for
                                        //   high speed mode not enabled.
    CSCTL2      = SELA__VLOCLK |        // Set ACLK = VLOCLK = 10 kHz.
                  SELS__DCOCLK |        //   Set SMCLK = DCOCLK.
                  SELM__DCOCLK;         //   Set MCLK = DCOCLK.
                                        // SMCLK = MCLK = DCOCLK = 1 MHz.
    CSCTL3      = DIVA__1 |             //   Set ACLK divider to 1.
                  DIVS__1 |             //   Set SMCLK divider to 1.
                  DIVM__1;              //   Set MCLK divider to 1.
                                        // Set all dividers to 1.
    CSCTL0_H    = 0;                    // Lock CS registers.

    // Initialize unused GPIOs to minimize energy-consumption.
    // Port 1:
    P1DIR = 0xFF;
    P1OUT = 0x00;
    // Port 2:
    P2DIR = 0xFF;
    P2OUT = 0x00;
    // Port 3:
    P3DIR = 0xFF;
    P3OUT = 0x00;
    // Port 4:
    P4DIR = 0xFF;
    P4OUT = 0x00;
    // Port J:
    PJDIR = 0xFFFF;
    PJOUT = 0x0000;

    // Initialize port 1:
    P1DIR |= BIT0;                      // P1.0 - output for LED2, off.
    P1OUT &= ~BIT0;
    // Initialize port 4:
    P4DIR |= BIT6;                      // P4.6 - output for LED1, off.
    P4OUT &= ~BIT6;

    // Initialize port 2:
    // Select Tx and Rx functionality of eUSCI0 for hardware UART.
    // P2.0 - UART Tx (UCA0TXD).
    // P2.1 - UART Rx (UCA0RXD).
    P2SEL0 = ~(BIT1 | BIT0);
    P2SEL1 = BIT1 | BIT0;

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    /* Initialization of the serial UART interface */
    UCA0CTLW0 |= UCSSEL__SMCLK |        // Select clock source SMCLK = 1 MHz.
                 UCSWRST;               // Enable software reset.
    // Set Baud rate of 9600 Bd.
    // Recommended settings available in table 30-5, p. 765 of the User's Guide.
    UCA0BRW = 6;                        // Clock prescaler of the
                                        //   Baud rate generator.
    UCA0MCTLW = UCBRF_8 |               // First modulations stage.
                UCBRS5 |                // Second modulation stage.
                UCOS16;                 // Enable oversampling mode.
    UCA0CTLW0 &= ~UCSWRST;              // Disable software reset and start
                                        //   eUSCI state machine.


#ifdef LOOP_1
/*******************************************************************************
** Loop 1
*******************************************************************************/

    /* MAIN LOOP */
    while(1)
    {
        /* Blink LEDs in a sequence with a period time of 1 s. */

        /* LED1 red blink twice for half period. */
        P4OUT |= BIT6;                  // Turn LED1 on.
        __delay_cycles(125000);         // Delay period 0.125 s.
        P4OUT &= ~BIT6;                 // Turn LED1 off.
        __delay_cycles(125000);         // Delay period 0.125 s.
        P4OUT |= BIT6;                  // Turn LED1 on.
        __delay_cycles(125000);         // Delay period 0.125 s.
        P4OUT &= ~BIT6;                 // Turn LED1 off.
        __delay_cycles(125000);         // Delay period 0.125 s.
        /* Transmit Hello World message every half period. */
        uartTx("ItsMe_Kaushik!\r\n");

        /* LED2 green blink twice for half period. */
        P1OUT |= BIT0;                  // Turn LED2 on.
        __delay_cycles(125000);         // Delay period 0.125 s.
        P1OUT &= ~BIT0;                 // Turn LED2 off.
        __delay_cycles(125000);         // Delay period 0.125 s.
        P1OUT |= BIT0;                  // Turn LED2 on.
        __delay_cycles(125000);         // Delay period 0.125 s.
        P1OUT &= ~BIT0;                 // Turn LED2 off.
        __delay_cycles(125000);         // Delay period 0.125 s.
        /* Transmit Hello World message every half period. */
        uartTx("ItsMe_Kaushik!\r\n");
    }

/******************************************************************************/
#endif /* LOOP_1 */

#ifdef LOOP_2
/*******************************************************************************
** Loop 2
*******************************************************************************/

    /* MAIN LOOP */
    while(1)
    {
        /* Blink LEDs alternating with a period time of 1 s, but
         * reduce the LEDs' energy consumption by 50%. */

        /* LED1 red on for quarter period. */
        P4OUT |= BIT6;                  // Turn LED1 on.
        __delay_cycles(250000);         // Delay period 0.25 s.
        /* LED1 & LED2 off for quarter period to save Energy. */
        P4OUT &= ~BIT6;                 // Turn LED1 off.
        __delay_cycles(250000);         // Delay period 0.25 s.

        /* LED2 green on for quarter period. */
        P1OUT |= BIT0;                  // Turn LED2 on.
        __delay_cycles(250000);         // Delay period 0.25 s.
        /* LED1 & LED2 off for quarter period to save Energy. */
        P1OUT &= ~BIT0;                 // Turn LED2 off.
        __delay_cycles(250000);         // Delay period 0.25 s.

        /* Transmit Hello World message every full period. */
        uartTx("Hello World!\r\n");
    }

/******************************************************************************/
#endif /* LOOP_2 */

}

/**
 * Transmit data via the serial interface UART of the eUSCI module, for example
 * a string or another array of character data.
 *
 * Parameter:
 *   *data      Pointer on byte array of data that is transmitted.
 */
void uartTx(char *data)
{
    // Iterate through array, look for null pointer at end of string.
    unsigned int i = 0;
    while(data[i])
    {
        while((UCA0STATW & UCBUSY));    // Wait while module is busy with data.
        UCA0TXBUF = data[i];            // Transmit element i of data array.
        i++;                            // Increment variable for array address.
    }
}
