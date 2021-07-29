/******************************************************************************
 *                                                                            *
 * Exercise 5                                                                 *
 *                                                                            *
 * Task 1: Step By Step                    X /  6 Points                      *
 * Task 2: One Step Back                   X /  2 Points                      *
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
 *                   (S1) -->|P4.5         P4.6|--> (LED1)                    *
 *                   (S2) -->|P1.1         P1.0|--> (LED2)                    *
 *                           |                 |                              *
 *                            -----------------                               *
 *                                                                            *
 ******************************************************************************/

#include <msp430fr5969.h>
#include <stdint.h>



volatile struct                         // State variables of the FSMs:
{
    uint8_t Main : 2;                   // FSM of main routine:
                                        // 0:   Idle
                                        // 1:   LED1
                                        // 2:   LED2
    uint8_t Led1 : 1;                   // FSM of LED 1:
                                        // 0:   Off
                                        // 1:   On
    uint8_t Led2 : 1;                   // FSM of LED 2:
                                        // 0:   Off
                                        // 1:   On
    uint8_t But1 : 1;                   // Event at button S1:
                                        // 0:   No button event
                                        // 1:   Button S1 has been pushed
    uint8_t But2 : 1;                   // Event at button S2:
                                        // 0:   No button event
                                        // 1:   Button S2 has been pushed
    uint8_t Time : 1;                   // Timeout event:
                                        // 0:   No timeout event
                                        // 1:   Timeout event released
} States;

typedef enum                            // State values of main FSM:
{
    MAIN_IDLE,                          // 0:   Idle
    MAIN_LED1,                          // 1:   LED1
    MAIN_LED2                           // 2:   LED2
} STATES_MAIN;


/* TIMEOUT TIMER */

void start_timeout(void);               // Initialize timeout watchdog timer.
void reset_timeout(void);               // Reset timeout watchdog timer.
void stop_timeout(void);                // Stop timeout watchdog timer.

/* BLINKING LEDS */

void blink_led1(void);                  // Blink LED1 considering its status.
void blink_led2(void);                  // Blink LED2 considering its status.



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
    P1IES |= BIT1;                      // P1.1 - falling edge detection.
    P1IE |= BIT1;                       //   Port interrupt enabled.
    P4IES |= BIT5;                      // P4.5 - falling edge detection.
    P4IE |= BIT5;                       //   Port interrupt enabled.

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    // Clear interrupt flags that have been raised due to high-impedance settings.
    P1IFG &= ~BIT1;
    P4IFG &= ~BIT5;


    /* Initialize status bits */

    States.Main = MAIN_IDLE;
    States.Led1 = 0;
    States.Led2 = 0;
    States.But1 = 0;
    States.But2 = 0;
    States.Time = 0;


    // Enable interrupts globally.
    __bis_SR_register(GIE);


    /* MAIN LOOP */
    while(1)
    {

        /* TODO FSM MAIN */

        switch(States.Main)
        {

        /* IDLE */
        case MAIN_IDLE:

            // Button S1 -> go to MAIN_LED1
            if(States.But1)             // Button S1 event
            {
                States.But1 = 0;        // Reset button S1 event.
                start_timeout();        // Start timeout timer.
                States.Main = MAIN_LED1;// Go to state of LED 1 configuration.
            }

            // EXECUTION: IDLE

            break;

        /* CONFIGURE LED1 */
        case MAIN_LED1:

            // Button S1 -> go to MAIN_LED2
            if(States.But1)             // Button S1 event
            {
                States.But1 = 0;        // Reset button S1 event.
                reset_timeout();        // Reset timeout timer.
                States.Main = MAIN_LED2;// Go to state of LED 2 configuration.
            }

            // Button S2 -> toggle LED1
            if(States.But2)             // Button S2 event
            {
                States.But2 = 0;        // Reset button S2 event.
                reset_timeout();        // Reset timeout timer.
                P4OUT ^= BIT6;          // Toggle LED1.
                States.Led1 = !States.Led1;  // Toggle Led1 state
            }

            // Timeout -> go to MAIN_IDLE
            if(States.Time)             // Timeout event
            {
                States.Time = 0;        // Reset Timeout event.
                stop_timeout();         // Stop timeout timer.
                States.Main = MAIN_IDLE;// Go to state of IDLE configuration.
            }

            // EXECUTION: LED1

            blink_led1();               // Play configuration active blinking.

            break;

        /* CONFIGURE LED2 */
        case MAIN_LED2:

            // Button S1 -> go to MAIN_LED1
            if(States.But1)             // Button S1 event
            {
                States.But1 = 0;        // Reset button S1 event.
                reset_timeout();        // Reset timeout timer.
                States.Main = MAIN_LED1;// Go to state of LED 1 configuration.
            }

            // Button S2 -> toggle LED2
            if(States.But2)             // Button S2 event
            {
                States.But2 = 0;        // Reset button S2 event.
                reset_timeout();        // Reset timeout timer.
                P1OUT ^= BIT0;              // Toggle LED2.
                States.Led2 = !States.Led2;  // Toggle Led2 state
            }

            // Timeout -> go to MAIN_IDLE
            if(States.Time)             // Timeout event
            {
                States.Time = 0;        // Reset Timeout event.
                stop_timeout();         // Stop timeout timer.
                States.Main = MAIN_IDLE;// Go to state of IDLE configuration.
            }

            // EXECUTION: LED2

            blink_led2();               // Play configuration active blinking.

            break;
        }
    }
}


/* ### TIMEOUT TIMER ### */

void start_timeout(void)                // Start the timeout counter.
{
    /* TODO TIMEOUT START */
    SFRIE1 |= WDTIE;
    //WDTCTL = WDTPW | WDTTMSEL | WDTSSEL_1 | WDTIS_4 | WDTCNTCL;
    WDTCTL = WDT_ADLY_1000;             // Unlock + Interval_mode + Timer Interval_32k + ACLK Clock source + Timer Clear
}

void reset_timeout(void)                // Reset the timeout counter.
{
    /* TODO TIMEOUT RESET */
    //WDTCTL = WDTPW | WDTTMSEL | WDTSSEL_1 | WDTIS_4 | WDTCNTCL;
    WDTCTL = WDT_ADLY_1000;             // Unlock + Interval_mode + Timer Interval_32k + ACLK Clock source + Timer Clear
}

void stop_timeout(void)                 // Stop the timeout counter.
{
    /* TODO TIMEOUT STOP */
    WDTCTL = WDTPW | WDTHOLD;             // Unlock + Stop Timer
}

/* ISR WATCHDOG */
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    // Raise status bit of timeout event.
    States.Time = 1;
}


/* ### BLINKING LEDS ### */

void blink_led1(void)
{
    // Play configuration active blinking.
    if(States.Led1)
    {
        P4OUT &= ~BIT6;
        __delay_cycles(40000);
        P4OUT |= BIT6;
        __delay_cycles(260000);
    }
    else
    {
        P4OUT |= BIT6;
        __delay_cycles(20000);
        P4OUT &= ~BIT6;
        __delay_cycles(280000);
    }
}

void blink_led2(void)
{
    // Play configuration active blinking.
    if(States.Led2)
    {
        P1OUT &= ~BIT0;
        __delay_cycles(40000);
        P1OUT |= BIT0;
        __delay_cycles(260000);
    }
    else
    {
        P1OUT |= BIT0;
        __delay_cycles(20000);
        P1OUT &= ~BIT0;
        __delay_cycles(280000);
    }
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

        /* Start Debouncing Button S2 */

        P1IE &= ~BIT1;                  // Disable port interrupt during delay.

        /* Timer-based delay of 25 ms */
        // Initialize timer A0 for a delay of 25 ms:
        // 25 ms = 25,000 cycles @ 1 MHz
        // 25,000 / 4 / 5 = 1250
        TA0CCR1 = 1250;                 // Compare value of 1250 with CCR1.
        TA0EX0 = TAIDEX_4;              // Secondary input divider stage of 5.
        TA0CTL = TASSEL__SMCLK |        // Select clock source SMCLK, 1 MHz.
                 ID__4 |                // First input divider stage of 4.
                 MC__CONTINUOUS;        // Start timer in continuous mode.
        TA0CCTL1 |= CCIE;               // Enable interrupt capability.

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

        /* Start Debouncing Button S1 */

        P4IE &= ~BIT5;                  // Disable port interrupt during delay.

        /* Timer-based delay of 25 ms */
        // Initialize timer A0 for a delay of 25 ms:
        // 25 ms = 25,000 cycles @ 1 MHz
        // 25,000 / 4 / 5 = 1250
        TA0CCR2 = 1250;                 // Compare value of 1250 with CCR2.
        TA0EX0 = TAIDEX_4;              // Secondary input divider stage of 5.
        TA0CTL = TASSEL__SMCLK |        // Select clock source SMCLK, 1 MHz.
                 ID__4 |                // First input divider stage of 4.
                 MC__CONTINUOUS;        // Start timer in continuous mode.
        TA0CCTL2 |= CCIE;               // Enable interrupt capability.

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
{                                       // TA0 CCR0

}

/* ISR TIMER A0 - CCR1, CCR2 AND TAIFG */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    switch(__even_in_range(TA0IV, TA0IV_TA0IFG))
    {
    case TA0IV_TA0CCR1:                 // TA0 CCR1

        /* Complete Debouncing Button S2 */

        TA0CTL = TACLR;                 // Stop and clear the timer.
        TA0CCTL1 &= ~CCIE;              // Disable interrupt capability.

        if(!(P1IN & BIT1))              // Validate button S2 state: pushed.
        {
            /* TODO VALIDATE FSM S2 */

            // TODO Validate current state of main FSM:
            if(States.Main != MAIN_IDLE)        // Main state is either LED1 or LED2
                {
                    States.But2 = 1;            // Raise button S2 event flag.
                }

        }

        P1IFG &= ~BIT1;                 // Drop meanwhile raised flag.
        P1IE |= BIT1;                   // Enable port interrupt again.

        break;
    case TA0IV_TA0CCR2:                 // TA0 CCR2

        /* Complete Debouncing Button S1 */

        TA0CTL = TACLR;                 // Stop and clear the timer.
        TA0CCTL2 &= ~CCIE;              // Disable interrupt capability.

        if(!(P4IN & BIT5))              // Validate button S1 state: pushed.
        {
            /* TODO VALIDATE FSM S1 */
            // No specific validation as S1 event should raise in all 3 states
            // TODO Validate current state of main FSM:
            States.But1 = 1;            // Raise button S1 event flag.
        }

        P4IFG &= ~BIT5;                 // Drop meanwhile raised flag.
        P4IE |= BIT5;                   // Enable port interrupt again.

        break;
    case TA0IV_TA0IFG:                  // TA0 TAIFG

        break;
    }
}
