/******************************************************************************
 *                                                                            *
 * Exercise 4                                                                 *
 *                                                                            *
 * Task 1: Time to Wonder                  X /  6 Points                      *
 * Task 2: Perfect Wave                    X /  2 Points                      *
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
 *                           |             P4.6|--> (LED1)                    *
 *                           |             P1.0|--> (LED2)                    *
 *                           |                 |                              *
 *                            -----------------                               *
 *                                                                            *
 ******************************************************************************/

// Select the task you are working on:
//#define LINEAR                      // Example for linear dimming.
//#define EXPONENTIAL                 // Your solution for exponential dimming.
#define SINUSOIDAL                  // Your solution for sinusoidal dimming.

#include <msp430fr5969.h>
#include <stdint.h>



#if defined EXPONENTIAL || defined SINUSOIDAL
/* LOOKUP TABLE: 16-BIT EXPONENTIAL */
// Lookup table for a linearized dimming of the LEDs. Because the human
// perception scales logarithmic, an exponential increase is applied to
// map 256 steps of perceived brightness to 16 bit of linear PWM.
const uint16_t EXP_16[256] =
{
        0,     1,     1,     1,     1,     1,     1,     1,
        1,     2,     2,     2,     2,     2,     2,     2,
        2,     2,     2,     2,     2,     3,     3,     3,
        3,     3,     3,     3,     4,     4,     4,     4,
        4,     4,     5,     5,     5,     5,     5,     6,
        6,     6,     6,     7,     7,     7,     8,     8,
        8,     9,     9,    10,    10,    10,    11,    11,
       12,    12,    13,    13,    14,    15,    15,    16,
       17,    17,    18,    19,    20,    21,    22,    23,
       24,    25,    26,    27,    28,    29,    31,    32,
       33,    35,    36,    38,    40,    41,    43,    45,
       47,    49,    52,    54,    56,    59,    61,    64,
       67,    70,    73,    76,    79,    83,    87,    91,
       95,    99,   103,   108,   112,   117,   123,   128,
      134,   140,   146,   152,   159,   166,   173,   181,
      189,   197,   206,   215,   225,   235,   245,   256,
      267,   279,   292,   304,   318,   332,   347,   362,
      378,   395,   412,   431,   450,   470,   490,   512,
      535,   558,   583,   609,   636,   664,   693,   724,
      756,   790,   825,   861,   899,   939,   981,  1024,
     1069,  1117,  1166,  1218,  1272,  1328,  1387,  1448,
     1512,  1579,  1649,  1722,  1798,  1878,  1961,  2048,
     2139,  2233,  2332,  2435,  2543,  2656,  2773,  2896,
     3025,  3158,  3298,  3444,  3597,  3756,  3922,  4096,
     4277,  4467,  4664,  4871,  5087,  5312,  5547,  5793,
     6049,  6317,  6596,  6889,  7194,  7512,  7845,  8192,
     8555,  8933,  9329,  9742, 10173, 10624, 11094, 11585,
    12098, 12634, 13193, 13777, 14387, 15024, 15689, 16384,
    17109, 17867, 18658, 19484, 20346, 21247, 22188, 23170,
    24196, 25267, 26386, 27554, 28774, 30048, 31378, 32768,
    34218, 35733, 37315, 38967, 40693, 42494, 44376, 46340,
    48392, 50534, 52772, 55108, 57548, 60096, 62757, 65535,
};
#endif /* EXPONENTIAL OR SINUSOIDAL */

#if defined SINUSOIDAL
/* LOOKUP TABLE: 8-BIT SINUSOIDAL */
// Lookup table containing one full period of a sine wave with 8 bit temporal
// and 8 bit amplitude resolution, resulting in 256 samples.
const uint16_t SIN_8[256] =
{
    128, 131, 134, 137, 140, 143, 146, 149,
    152, 155, 158, 162, 165, 167, 170, 173,
    176, 179, 182, 185, 188, 190, 193, 196,
    198, 201, 203, 206, 208, 211, 213, 215,
    218, 220, 222, 224, 226, 228, 230, 232,
    234, 235, 237, 238, 240, 241, 243, 244,
    245, 246, 248, 249, 250, 250, 251, 252,
    253, 253, 254, 254, 254, 255, 255, 255,
    255, 255, 255, 255, 254, 254, 254, 253,
    253, 252, 251, 250, 250, 249, 248, 246,
    245, 244, 243, 241, 240, 238, 237, 235,
    234, 232, 230, 228, 226, 224, 222, 220,
    218, 215, 213, 211, 208, 206, 203, 201,
    198, 196, 193, 190, 188, 185, 182, 179,
    176, 173, 170, 167, 165, 162, 158, 155,
    152, 149, 146, 143, 140, 137, 134, 131,
    128, 124, 121, 118, 115, 112, 109, 106,
    103, 100, 97, 93, 90, 88, 85, 82,
    79, 76, 73, 70, 67, 65, 62, 59,
    57, 54, 52, 49, 47, 44, 42, 40,
    37, 35, 33, 31, 29, 27, 25, 23,
    21, 20, 18, 17, 15, 14, 12, 11,
    10, 9, 7, 6, 5, 5, 4, 3,
    2, 2, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 2,
    2, 3, 4, 5, 5, 6, 7, 9,
    10, 11, 12, 14, 15, 17, 18, 20,
    21, 23, 25, 27, 29, 31, 33, 35,
    37, 40, 42, 44, 47, 49, 52, 54,
    57, 59, 62, 65, 67, 70, 73, 76,
    79, 82, 85, 88, 90, 93, 97, 100,
    103, 106, 109, 112, 115, 118, 121, 124,
};
#endif /* SINUSOIDAL */



/* MAIN PROGRAM */
void main(void)
{
    // Stop watchdog timer.
    WDTCTL = WDTPW | WDTHOLD;

    /* TODO SYSTEM CLOCK INIT */

    // Before the clock system is set to run the microcontroller at 16 MHz, the
    // clock source for the FRAM memory needs to be prescaled using a wait state
    // for its operation with MCLK beyond 8 MHz.
    // TODO WAIT STATES
    FRCTL0      = FRCTLPW | NWAITS_1;   // Unlock FRAM and Increase FRAM Wait state number

    // TODO 16 MHZ CLOCK
    // Initialize the clock system to generate 1 MHz DCO clock.
    CSCTL0_H    = CSKEY_H;              // Unlock CS registers.
    CSCTL1      = DCOFSEL_4 | DCORSEL;  // Set DCO to 16 MHz, DCORSEL for
                                        //   high speed mode enabled.
    CSCTL2      = SELA__VLOCLK |        // Set ACLK = VLOCLK = 10 kHz.
                  SELS__DCOCLK |        //   Set SMCLK = DCOCLK.
                  SELM__DCOCLK;         //   Set MCLK = DCOCLK.
                                        // SMCLK = MCLK = DCOCLK = 16 MHz.
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

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    /* TODO TIMER INITIALIZATION */
    TA0CTL = TACLR;
    TA0CTL = TASSEL_2 | ID_2 | MC_2;    // Source_SMCLK + Prescaler_4 + Continuous_mode
    // Initialize Timer A0 for PWM at > 60 Hz:


    /* TODO HARDWARE PWM INIT */

    // Hardware PWM on P1.0:
    P1DIR |= BIT0;                      // P1.0 - Set Direction as Output .
    P1SEL0 |= BIT0;                     // P1.0 - Select for Primary module function.
    TA0CCTL1 = OUTMOD_7 | OUT;          // Connect OUT1 of CCR1 to P1.0 and Select PWM reset/set output mode

    /* TODO SOFTWARE PWM INIT */

    // Software PWM on P4.6:
    TA0CCTL0 |= CCIE;                       // Enable TA0CCR0_Timer Interrupt
    TA0CCTL2 |= CCIE;                       // Enable TA0CCR2_Timer Interrupt


    // Enable interrupts globally.
    __bis_SR_register(GIE);

    /* MAIN LOOP */
    while(1)
    {

#if defined LINEAR
        TA0CCR1 = 0x0000;
        TA0CCR2 = 0xFFFF;
        uint16_t i = 0;
        for(i=0x0000; i<0xFFFF; i++)
        {
            TA0CCR1++;
            TA0CCR2--;
            __delay_cycles(350);
        }
        TA0CCR1 = 0xFFFF;
        TA0CCR2 = 0x0000;
        for(i=0x0000; i<0xFFFF; i++)
        {
            TA0CCR1--;
            TA0CCR2++;
            __delay_cycles(350);
        }
#elif defined EXPONENTIAL
        uint8_t i = 0;
        for(i=0x00; i<0xFF; i++)
        {
            // TODO
            TA0CCR1 =  EXP_16[i];       // Increase Intensity by Iterating Lookup table from top to bottom
            TA0CCR2 =  EXP_16[255 - i]; // Decrease Intensity (DIM) by Iterating Lookup table from bottom to top
            __delay_cycles(120000);
        }
        for(i=0x00; i<0xFF; i++)
        {
            // TODO
            TA0CCR1 =  EXP_16[255 - i]; // Decrease Intensity (DIM) by Iterating Lookup table from bottom to top
            TA0CCR2 =  EXP_16[i];       // Increase Intensity by Iterating Lookup table from top to bottom
            __delay_cycles(120000);
        }
#elif defined SINUSOIDAL
        uint16_t i = 0;
        uint16_t j = 0;
        while(i < 256)
        {
            /* Cosine function */
            // TODO
            TA0CCR2 = EXP_16[SIN_8[i]]; // Fancy Dimming -- phase-delayed run of a cosine function

            /* Superposition of sine waves */
            // TODO
            TA0CCR1 = EXP_16[(SIN_8[i] + SIN_8[j]) / 2]; // Fancy Dimming -- two superimposed sine functions

            __delay_cycles(250000);
            i++;
            j += 3;        // second sine function of angular frequency multiplied by factor 3
            if(j > 255)    // condition to Keep value of j under 255
            {
                j = 0;
            }
        }
#endif

    }
}

/* ISR TIMER A0 - CCR0 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
{                                       // TA0 CCR0
    /* TODO SOFTWARE PWM CCR0 */
    TA0CCTL0 &= ~CCIFG;             // Drop TA0CCR0_CCIFG flag
    P4OUT |= BIT6;                  // Set the Pin (LED ON)
}

/* ISR TIMER A0 - CCR1, CCR2 AND TAIFG */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void)
{
    switch(__even_in_range(TA0IV, TA0IV_TA0IFG))
    {
    case TA0IV_TA0CCR1:                 // TA0 CCR1
        break;
    case TA0IV_TA0CCR2:                 // TA0 CCR2

        /* TODO SOFTWARE PWM CCR2 */
        TA0CCTL2 &= ~CCIFG;             // Drop TA0CCR2_CCIFG flag
        P4OUT &= ~BIT6;                 // Reset the Pin (LED OFF)

        break;
    case TA0IV_TA0IFG:                  // TA0 TAIFG
        break;
    }
}
