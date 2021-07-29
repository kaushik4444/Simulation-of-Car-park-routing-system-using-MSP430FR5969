/******************************************************************************
 *                                                                            *
 * Exercise 2                                                                 *
 *                                                                            *
 * Task 1: Push Da Button                   X /  4 Points                     *
 * Task 2: Advanced Button Handling         X /  4 Points                     *
 * Comprehension Questions                  X /  2 Points                     *
 *                                        ----------------                    *
 *                                         XX / 10 Points                     *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * name:                    Kaushik Goud Chandapet                                         *
 * matriculation number:    1536199                                         *
 * e-mail:                  kaushik.chandapet@student.uni-siegen.de                   *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * Hardware Setup                                                             *
 *                                                                            *
 *                               MSP430FR5969                                 *
 *                            -----------------                               *
 *                           |                 |                              *
 *                   (S1) -->|P4.5         P4.6|--> (LED1)                    *
 *                   (S2) -->|P1.1         P1.0|--> (LED2)                    *
 *                           |                 |                              *
 *                            -----------------                               *
 *                                                                            *
 ******************************************************************************/

// Select the task you are working on:
#define TASK_1
//#define TASK_2

#include <msp430fr5969.h>



#ifdef TASK_1
/*******************************************************************************
** Task 1
*******************************************************************************/

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

    /* TODO INITIALIZATION */
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

       P1DIR &= ~BIT1;                      // P1.1 - input for Button.
       P1REN |= BIT1;                       // Enable Resistor
       P1OUT |= BIT1;                       // Select Pull up Resistor

       // Initialize port 4:
       P4DIR |= BIT6;                      // P4.6 - output for LED1, off.
       P4OUT &= ~BIT6;

       P4DIR &= ~BIT5;                      // P4.5 - input for Button.
       P4REN |= BIT5;                       // Enable Resistor
       P4OUT |= BIT5;                       // Select Pull up Resistor

       // Initialize port 2:
       // Select Tx and Rx functionality of eUSCI0 for hardware UART.
       // P2.0 - UART Tx (UCA0TXD).
       // P2.1 - UART Rx (UCA0RXD).
       P2SEL0 = ~(BIT1 | BIT0);
       P2SEL1 = BIT1 | BIT0;

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    /* MAIN LOOP */
    while(1)
    {
        /* TODO POLLING */
        if(!(P1IN & BIT1) && !(P1OUT & BIT0))    // Validate Button pushed and LED2 Off condition
        {
            P1OUT |= BIT0;          // Turn LED2 on.
        }
        else if((P1IN & BIT1) && (P1OUT & BIT0))
        {
            P1OUT &= ~BIT0;         // Turn LED2 off.
        }

        if(!(P4IN & BIT5) && !(P4OUT & BIT6))    // Validate Button pushed and LED1 Off condition
        {
            P4OUT |= BIT6;          // Turn LED1 on.
        }
        else if((P4IN & BIT5) && (P4OUT & BIT6))
        {
            P4OUT &= ~BIT6;         // Turn LED1 off.
        }
    }
}

/******************************************************************************/
#endif /* TASK_1 */



#ifdef TASK_2
/*******************************************************************************
** Task 2
*******************************************************************************/

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

    /* TODO INITIALIZATION */
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

       P1DIR &= ~BIT1;                      // P1.1 - input for Button.
       P1REN |= BIT1;                       // Enable Resistor
       P1OUT |= BIT1;                       // Select Pull up Resistor

       // Initialize port 4:
       P4DIR |= BIT6;                      // P4.6 - output for LED1, off.
       P4OUT &= ~BIT6;

       P4DIR &= ~BIT5;                      // P4.5 - input for Button.
       P4REN |= BIT5;                       // Enable Resistor
       P4OUT |= BIT5;                       // Select Pull up Resistor

       // Initialize port 2:
       // Select Tx and Rx functionality of eUSCI0 for hardware UART.
       // P2.0 - UART Tx (UCA0TXD).
       // P2.1 - UART Rx (UCA0RXD).
       P2SEL0 = ~(BIT1 | BIT0);
       P2SEL1 = BIT1 | BIT0;

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    P1IE |= BIT1;                    // P1.3 interrupt enabled
    P1IES |= BIT1;                  // P1.3 High/low edge

    P4IE |= BIT5;                    // P1.3 interrupt enabled
    P4IES |= BIT5;                  // P1.3 High/low edge

    // Clear interrupt flags that have been raised due to high-impedance settings.
    P1IFG &= ~BIT1;
    P4IFG &= ~BIT5;

    // Enable interrupts globally.
    __bis_SR_register(GIE);

    /* MAIN LOOP */
    while(1)
    {

    }
}

/* ISR PORT 1 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    switch(P1IV)
    {
    case P1IV_P1IFG0:                   // P1.0
        break;
    case P1IV_P1IFG1:                   // P1.1

        /* TODO PORT ISR */
        __delay_cycles(25000);         // Delay period 0.025 s.
        P1IFG &= ~BIT1;                // Clear raised Flag P1.1 IFG
        if(!(P1IN & BIT1))             // Validate button state (Pushed - 0)
        {
            P1OUT ^= BIT0;                  // Toggle P1.0 on validated interrupt.
        }
        break;
    case P1IV_P1IFG2:                   // P1.2
        break;
    case P1IV_P1IFG3:                   // P1.3
        break;
    case P1IV_P1IFG4:                   // P1.4
        break;
    case P1IV_P1IFG5:                   // P1.5
        break;
    case P1IV_P1IFG6:                   // P1.6
        break;
    case P1IV_P1IFG7:                   // P1.7
        break;
    }
}

/* ISR PORT 4 */
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    switch(P4IV)
    {
    case P4IV_P4IFG0:                   // P4.0
        break;
    case P4IV_P4IFG1:                   // P4.1
        break;
    case P4IV_P4IFG2:                   // P4.2
        break;
    case P4IV_P4IFG3:                   // P4.3
        break;
    case P4IV_P4IFG4:                   // P4.4
        break;
    case P4IV_P4IFG5:                   // P4.5

        /* TODO PORT ISR */
        __delay_cycles(25000);         // Delay period 0.025 s.
        P4IFG &= ~BIT5;                // Clear raised Flag P4.5 IFG
        if(!(P4IN & BIT5))             // Validate button state (Pushed - 0)
        {
            P4OUT ^= BIT6;                  // Toggle P4.6 on validated interrupt.
        }
        break;
    case P4IV_P4IFG6:                   // P4.6
        break;
    case P4IV_P4IFG7:                   // P4.7
        break;
    }
}

/******************************************************************************/
#endif /* TASK_2 */
