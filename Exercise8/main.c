/******************************************************************************
 *                                                                            *
 * Exercise 8                                                                 *
 *                                                                            *
 * Task 1: Twinkle, Twinkle Little Star    X /  8 Points                      *
 * Comprehension Questions                 X /  2 Points                      *
 *                                        ----------------                    *
 *                                        XX / 10 Points                      *
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
 *                   (S2) -->|P1.1         P1.0|--> (LED2)                    *
 *                           |                 |                              *
 *                           | SMCLK = 1 MHz   |                              *
 *                           | ACLK = 10 kHz   |                              *
 *                            -----------------                               *
 *                                                                            *
 ******************************************************************************/

#include <msp430fr5969.h>
#include <stdint.h>



// Place the following marker above the initiation of the desired sleep modes:
/* TODO SLEEP */
// Mode: LMPx
// Reason: Single-sentence explanation about your decision.

volatile uint8_t Cnt = 0;               // Counting register for button events.


/* TIMEOUT TIMER */

void start_timeout(void);               // Initialize timeout watchdog timer.
void reset_timeout(void);               // Reset timeout watchdog timer.
void stop_timeout(void);                // Stop timeout watchdog timer.


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

    // Initialize port interrupt:
    P1IE |= BIT1;                       // P1.1 - port interrupt enabled.
    P1IES |= BIT1;                      //   Falling edge detection.


    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    // Clear interrupt flag that arose due to high-impedance settings.
    P1IFG &= ~BIT1;


    // Enable interrupts globally.
    __bis_SR_register(GIE);


    /* MAIN LOOP */
    while(1)
    {
        if(Cnt > 0)                     // REPLAY
        {
            P1OUT |= BIT0;              // Turn LED2 on.

            /* TODO REPLAY 50MS */

            // Initialise timer TA1 for replaying with 50 ms on:
            TA1CTL = TACLR;              // Reset Timer
            TA1CTL = TASSEL_1 + ID_1 + MC_1;    // Source_ACLK + Prescaler_2 + Up_mode
            TA1CCR0 = 250;                    // 50ms offset value
            while(!(TA1CTL & TAIFG))          // Delay loop
            {

            }

            P1OUT &= ~BIT0;             // Turn LED2 off.

            /* TODO REPLAY 200MS */

            // Initialize timer TA1 for replaying with 200 ms off:
            TA1CTL = TACLR;              // Stop Timer and Reset
            TA1CTL = TASSEL_1 + ID_1 + MC_1;    // Source_ACLK + Prescaler_2 + Up_mode
            TA1CCR0 = 1000;                    // 200ms offset value
            while(!(TA1CTL & TAIFG))          // Delay loop
            {

            }

            // Decrease counter variable.
            Cnt--;
        }
        else                            // STOP REPLAY
        {
            /* TODO REPLAY STOP */

            // Stop and turn off timer TA1:
            TA1CTL = MC_0 + TACLR;              // Stop Timer and Reset
            P1IE |= BIT1;               // Enable port interrupt again.
            __bis_SR_register(LPM4_bits);   // Enter LPM4
            /* TODO SLEEP */
            // Mode: LMP4
            // Reason: CPU and All clocks can be disabled when no processing is performed till Button pushed
        }
    }
}


/* ### REPLAY TIMER ### */

/* ISR TIMER TA1 - CCR0 */
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR (void)
{                                       // TA1 CCR0

}

/* ISR TIMER TA1 - CCR1, CCR2 AND TAIFG */
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR (void)
{
    switch(__even_in_range(TA1IV, TA1IV_TA1IFG))
    {
    case TA1IV_TA1CCR1:                 // TA1 CCR1

        break;
    case TA1IV_TA1CCR2:                 // TA1 CCR2

        break;
    case TA1IV_TA1IFG:                  // TA1 TAIFG

        break;
    }
}


/* ### TIMEOUT TIMER ### */

void start_timeout(void)                // Initialize timeout watchdog timer.
{
    /* TODO START TIMEOUT */

    // Timeout interrupt after interval of 0.8 s using ACLK.
    WDTCTL = WDTPW |                    // The watchdog timer password (0x5A).
             WDTSSEL__ACLK |            // Select ACLK = 10 kHz as clock source.
             WDTTMSEL |                 // Interval timer mode.
             WDTIS__8192;                // Set interrupt prescaler: 2^13
                                        // 1 / (10,000 Hz / 8192) = 0.8192 s
                                        // Interrupt after 0.8 s.
    SFRIE1 |= WDTIE;                    // Enable watchdog timer interrupt.

}

void reset_timeout(void)                // Reset timeout watchdog timer.
{
    /* TODO RESET TIMEOUT */
    WDTCTL = (WDTCTL & 0x00FF) |        // Carry lower byte configuration.
             WDTPW |                    // The watchdog timer password (0x5A).
             WDTCNTCL;                  // Clear watchdog timer.
}

void stop_timeout(void)                 // Stop timeout watchdog timer.
{
    /* TODO STOP TIMEOUT */
    WDTCTL = (WDTCTL & 0x00FF) |        // Carry lower byte configuration.
             WDTPW |                    // The watchdog timer password (0x5A).
             WDTCNTCL |                 // Clear watchdog timer.
             WDTHOLD;                   // Stop watchdog timer.
    SFRIE1 &= ~WDTIE;                   // Disable watchdog timer interrupt.

}

/* ISR WATCHDOG */
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    stop_timeout();                     // Stop timeout timer.
    P1IE &= ~BIT1;                      // Disable port interrupt during replay.
    __bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3
    /* TODO SLEEP */
    // Mode: Active
    // Reason: Go back to Active mode as AClck and CPU are required
}


/* ### BUTTON DEBOUNCING ### */

/* ISR PORT 1 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    switch(__even_in_range(P1IV, P1IV_P1IFG7))
    {
    case P1IV_P1IFG0:                   // P1.0
        break;
    case P1IV_P1IFG1:                   // P1.1

        /* Start Debouncing of Button S2 */
        __bis_SR_register_on_exit(LPM3_bits);   // Enter LPM3
        /* TODO SLEEP */
        // Mode: LPM3
        // Reason: AClck is required but CPU can be disabled
        P1IE &= ~BIT1;                  // Disable port interrupt during delay.

        /* TODO DEBOUNCE */

        // Initialize timer TA0 for button debouncing with a delay of 25 ms:
        TA0CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
        TA0CCTL1 = CAP + CM_1 + CCIS_2;  // Capture_mode + Rise_Edge + Default_GND
        TA0CCTL1 |= CCIS_3;                      // Capture value by switching the CCIS potential to Vcc
        TA0CCTL1 &= ~CAP;                       // Change mode to COMPARE
        TA0CCTL1 &= ~CCIFG;                     // Drop TA0CCR1_CCIFG flag
        TA0CCTL1 |= CCIE;                       // Enable TA0CCR1_Timer Interrupt
        TA0CCR1 = TA0CCR1 + 125;              // Add offset 25ms

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

/* ISR TIMER A0 - CCR0 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{                                       // TA0 CCR0

}

/* ISR TIMER A0 - CCR1, CCR2 AND TAIFG */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    switch(__even_in_range(TA0IV, TA0IV_TA0IFG))
    {
    case TA0IV_TA0CCR1:                 // TA0 CCR1

        /* Complete Debouncing of Button S2 */

        TA0CTL = TACLR;                 // Stop and clear the timer.
        TA0CCTL1 &= ~CCIE;              // Disable interrupt capability.

        if(!(P1IN & BIT1))              // Validate button S2 state: pushed.
        {
            /* Start or Reset Timeout */

            if(Cnt > 0)
                reset_timeout();        // Reset timeout timer.
            else
                start_timeout();        // Start timeout timer.

            Cnt++;                      // Increment counter variable.
        }

        P1IFG &= ~BIT1;                 // Drop meanwhile raised flag.
        P1IE |= BIT1;                   // Enable port interrupt again.

        break;
    case TA0IV_TA0CCR2:                 // TA0 CCR2

        break;
    case TA0IV_TA0IFG:                  // TA0 TAIFG

        break;
    }
}
