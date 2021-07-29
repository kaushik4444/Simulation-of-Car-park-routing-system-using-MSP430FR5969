/******************************************************************************
 *                                                                            *
 * Exercise 3                                                                 *
 *                                                                            *
 * Task 1: Wavin' Flag                      X /  3 Points                     *
 * Task 2: Take Your Time                   X /  5 Points                     *
 * Comprehension Questions                  X /  2 Points                     *
 *                                        ----------------                    *
 *                                         XX / 10 Points                     *
 *                                                                            *
 ******************************************************************************
 *                                                                            *
 * name:                    Kaushik Goud Chandapet                            *
 * matriculation number:    1536199                                           *
 * e-mail:                  kaushik.chandapet@student.uni-siegen.de           *
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

// Select the exercise you are working on:
#define TASK_1
//#define TASK_2

#include <msp430fr5969.h>
//#include <stdint.h>


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
    P1DIR &= ~BIT1;                     // P1.1 - input for S2, pullup.
    P1REN |= BIT1;
    P1OUT |= BIT1;
    // Initialize port 4:
    P4DIR |= BIT6;                      // P4.6 - output for LED1, off.
    P4OUT &= ~BIT6;
    P4DIR &= ~BIT5;                     // P4.5 - input for S1, pullup.
    P4REN |= BIT5;
    P4OUT |= BIT5;

    // Initialize port interrupts:
    P1IE |= BIT1;                       // P1.1 - port interrupt enabled.
    P1IES |= BIT1;                      //   Falling edge detection.
    P4IE |= BIT5;                       // P4.5 - port interrupt enabled.
    P4IES |= BIT5;                      //   Falling edge detection.

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

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
    switch(__even_in_range(P1IV, P1IV_P1IFG7))
    {
    case P1IV_P1IFG0:                   // P1.0
        break;
    case P1IV_P1IFG1:                   // P1.1

        /* TODO TIMER WAIT */
        TA0CTL = TASSEL_2 + ID_0 + MC_1;    // Source_SMCLK + Prescaler_1 + Up_mode
        TA0CCR0 = 25000;                    // 25ms offset value
        while(!(TA0CCTL0 & CCIFG))          // Delay loop
        {

        }
        TA0CTL = MC_0 + TACLR;              // Stop Timer and Reset
        TA0CCTL0 &= ~CCIFG;                 // Drop TA0CCR0_CCIFG Flag

        P1IFG &= ~BIT1;                 // Drop P1.1 flag.

        if(!(P1IN & BIT1))              // Validate button S2 state: pushed.
        {
            P1OUT ^= BIT0;              // Toggle LED2.
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
    switch(__even_in_range(P4IV, P4IV_P4IFG7))
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

        /* TODO TIMER WAIT */
        TA0CTL = TASSEL_2 + ID_0 + MC_1;    // Source_SMCLK + Prescaler_1 + Up_mode
        TA0CCR0 = 25000;                    // 25ms offset value
        while(!(TA0CCTL0 & CCIFG))          // Delay loop
        {

        }
        TA0CTL = MC_0 + TACLR;              // Stop Timer and Reset
        TA0CCTL0 &= ~CCIFG;                 // Drop TA0CCR0_CCIFG Flag

        P4IFG &= ~BIT5;                 // Drop P4.5 flag.

        if(!(P4IN & BIT5))              // Validate button S1 state: pushed.
        {
            P4OUT ^= BIT6;              // Toggle LED1.
        }

        break;
    case P4IV_P4IFG6:                   // P4.6
        break;
    case P4IV_P4IFG7:                   // P4.7
        break;
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
    P1DIR &= ~BIT1;                     // P1.1 - input for S2, pullup.
    P1REN |= BIT1;
    P1OUT |= BIT1;
    // Initialize port 4:
    P4DIR |= BIT6;                      // P4.6 - output for LED1, off.
    P4OUT &= ~BIT6;
    P4DIR &= ~BIT5;                     // P4.5 - input for S1, pullup.
    P4REN |= BIT5;
    P4OUT |= BIT5;

    // Initialize port interrupts:
    P1IE |= BIT1;                       // P1.1 - port interrupt enabled.
    P1IES |= BIT1;                      //   Falling edge detection.
    P4IE |= BIT5;                       // P4.5 - port interrupt enabled.
    P4IES |= BIT5;                      //   Falling edge detection.

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    // Clear interrupt flags that have been raised due to high-impedance settings.
    P1IFG &= ~BIT1;
    P4IFG &= ~BIT5;

    /* TODO TIMER INITIALIZATION */
    TA0CTL = TASSEL_2 + ID_0 + MC_2;    // Source_SMCLK + Prescaler_1 + Continuous_mode

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
    switch(__even_in_range(P1IV, P1IV_P1IFG7))
    {
    case P1IV_P1IFG0:                   // P1.0
        break;
    case P1IV_P1IFG1:                   // P1.1

        /* TODO TIMER START */
        TA0CCTL2 |= CCIE;                       // Enable TA0CCR2_Timer Interrupt
        TA0CCTL2 |= CAP + CM_3 + CCIS_2 + SCS;  // Capture_mode + Rise_&_Fall + Default_GND + Synchronous
        TA0CCTL2 |= CCIS0;                      // Capture value by switching the CCIS potential
        TA0CCTL2 &= ~CAP;                       // Change mode to COMPARE
        TA0CCTL2 &= ~CCIFG;                     // Drop TA0CCR2_CCIFG flag
        TA0CCR2 = TA0CCR2 + 25000;              // Add offset 25ms
        P1IE &= ~BIT1;                       // P1.1 - port interrupt disabled.

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
    switch(__even_in_range(P4IV, P4IV_P4IFG7))
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

        /* TODO TIMER START */
        TA0CCTL1 |= CCIE;                       // Enable TA0CCR1_Timer Interrupt
        TA0CCTL1 |= CAP + CM_3 + CCIS_2 + SCS;  // Capture_mode + Rise_&_Fall + Default_GND + Synchronous
        TA0CCTL1 |= CCIS0;                      // Capture value by switching the CCIS potential
        TA0CCTL1 &= ~CAP;                       // Change mode to COMPARE
        TA0CCTL1 &= ~CCIFG;                     // Drop TA0CCR1_CCIFG flag
        TA0CCR1 = TA0CCR1 + 25000;              // Add offset 25ms
        P4IE &= ~BIT5;                       // P4.5 - port interrupt disabled.

        break;
    case P4IV_P4IFG6:                   // P4.6
        break;
    case P4IV_P4IFG7:                   // P4.7
        break;
    }
}

/* ISR TIMER A0 - CCR0 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{
                                        // TA0 CCR0
}

/* ISR TIMER A0 - CCR1, CCR2 AND TAIFG */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    switch(__even_in_range(TA0IV, TA0IV_TA0IFG))
    {
    case TA0IV_TA0CCR1:                 // TA0 CCR1

        /* TODO TIMER ISR */
        TA0CCTL1 &= ~CCIE;              // Disable TA0CCR1_Timer Interrupt
        TA0CCTL1 &= ~CCIFG;             // Drop TA0CCR1_CCIFG flag
        P4IFG &= ~BIT5;                 // Drop P4.5 flag.
        P4IE |= BIT5;                   // P4.5 - port interrupt enabled.

        if(!(P4IN & BIT5))              // Validate button S1 state: pushed.
        {
            P4OUT ^= BIT6;              // Toggle LED1.
        }


        break;
    case TA0IV_TA0CCR2:                 // TA0 CCR2

        /* TODO TIMER ISR */
        TA0CCTL2 &= ~CCIE;              // Disable TA0CCR2_Timer Interrupt
        TA0CCTL2 &= ~CCIFG;             // Drop TA0CCR2_CCIFG flag
        P1IFG &= ~BIT1;                 // Drop P1.1 flag.
        P1IE |= BIT1;                   // P1.1 - port interrupt enabled.

        if(!(P1IN & BIT1))              // Validate button S2 state: pushed.
        {
            P1OUT ^= BIT0;              // Toggle LED2.
        }

        break;
    case TA0IV_TA0IFG:                  // TA0 TAIFG

        break;
    }
}

/******************************************************************************/
#endif /* TASK_2 */
