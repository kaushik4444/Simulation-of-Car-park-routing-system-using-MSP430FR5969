/******************************************************************************
 *                                                                            *
 * Final Project                                                              *
 *                                                                            *
 * Parking Lot                                                                *
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
 *                   (S2) -->|P4.5         P4.6|--> (LED1)                    *
 *               UART Tx> <--|P2.0             |                              *
 *               <UART Rx -->|P2.1             |                              *
 *                           |                 |                              *
 *                           | ACLK = 32768 Hz |                              *
 *                            -----------------                               *
 ******************************************************************************/

#include <msp430fr5969.h>
#include <stdint.h>
#include "string.h"

volatile struct                         // State variables of the FSMs:
{
    uint8_t Parser : 6;                 // FSM of parser:
                                        // 0:   Idle
                                        // 1:   'C'
                                        // 2:   'CP'
                                        // 3:   'CPR'
                                        // 4:   'CPRS'
                                        // 5:   'CPRS_'
                                        // 6:   'CPRS_R'
                                        // 7:   'CPRS_RE'
                                        // 8:   'CPRS_RES'
                                        // 9:   'CPRS_D'
                                        // 10:   'CPRS_DI'
                                        // 11:   'CPRS_DIR'
                                        // 12:   'CPRS_DIR1'
                                        // 13:   'CPRS_DIR2'
                                        // 14:   'CPRS_C'
                                        // 15:   'CPRS_CL'
                                        // 16:   'CPRS_CLS'
                                        // 17:   'CPRS_CN'
                                        // 18:   'CPRS_CNT'
                                        // 19:   'CPRS_O'
                                        // 20:   'CPRS_OP'
                                        // 21:   'CPRS_OPN'
                                        // 22:   'CPRS_M'
                                        // 23:   'CPRS_MA'
                                        // 24:   'CPRS_MAX'
                                        // 25:   'CPRS_MAX1'
                                        // 26:   'CPRS_MAX12'
                                        // 27:   'CPRS_MAX123'
                                        // 28:   'CPRS_S'
                                        // 29:   'CPRS_SP'
                                        // 30:   'CPRS_SPD'
                                        // 31:   'CPRS_CLK'
                                        // 32:   'CPRS_CLKX'
                                        // 33:   'CPRS_CLKXX'
                                        // 34:   'CPRS_CLKXX:'
                                        // 35:   'CPRS_CLKXX:X'
                                        // 36:   'CPRS_CLKXX:XX'
                                        // 37:   'CPRS_CLSX'
                                        // 38:   'CPRS_CLSXX'
                                        // 39:   'CPRS_CLSXX:'
                                        // 40:   'CPRS_CLSXX:X'
                                        // 41:   'CPRS_CLSXX:XX'
                                        // 42:   'CPRS_OPNX'
                                        // 43:   'CPRS_OPNXX'
                                        // 44:   'CPRS_OPNXX:'
                                        // 45:   'CPRS_OPNXX:X'
                                        // 46:   'CPRS_OPNXX:XX'
    uint8_t Tout : 1;                   // Timeout event:
                                        // 0:   No timeout event
                                        // 1:   Timeout event released
} States;

typedef enum                            // State values of FSM:
{
    PARSE_IDLE,                         // 0:   Idle
    PARSE_C,                            // 1:   'C'
    PARSE_P,                            // 2:   'P'
    PARSE_R,                            // 3:   'R'
    PARSE_S,                            // 4:   'S'
    PARSE__,                            // 5:   '_'
    PARSE__R,                           // 6:   'R'
    PARSE__RE,                          // 7:   'E'
    PARSE__RES,                         // 8:   'S'
    PARSE__D,                           // 9:   'D'
    PARSE__DI,                          // 10:   'I'
    PARSE__DIR,                         // 11:   'R'
    PARSE__DIR1,                        // 12:   '1'
    PARSE__DIR2,                        // 13:   '2'
    PARSE__C,                           // 14:   'C'
    PARSE__CL,                          // 15:   'L'
    PARSE__CLS,                         // 16:   'S'
    PARSE__CN,                          // 17:   'N'
    PARSE__CNT,                         // 18:   'T'
    PARSE__O,                           // 19:   'O'
    PARSE__OP,                          // 20:   'P'
    PARSE__OPN,                         // 21:   'N'
    PARSE__M,                           // 22:   'M'
    PARSE__MA,                          // 23:   'A'
    PARSE__MAX,                         // 24:   'X'
    PARSE__MAX1,                        // 25:   '1'
    PARSE__MAX2,                        // 26:   '2'
    PARSE__MAX3,                        // 27:   '3'
    PARSE__S,                           // 28:   'S'
    PARSE__SP,                          // 29:   'P'
    PARSE__SPD,                         // 30:   'D'
    PARSE__CLK,                         // 31:   'K'
    PARSE__CLKX,                        // 32:   'X'
    PARSE__CLKXX,                       // 33:   'X'
    PARSE__CLKXX_,                      // 34:   ':'
    PARSE__CLKXX_X,                     // 35:   'X'
    PARSE__CLKXX_XX,                    // 36:   'X'
    PARSE__CLSX,                        // 37:   'X'
    PARSE__CLSXX,                       // 38:   'X'
    PARSE__CLSXX_,                      // 39:   ':'
    PARSE__CLSXX_X,                     // 40:   'X'
    PARSE__CLSXX_XX,                    // 41:   'X'
    PARSE__OPNX,                        // 42:   'X'
    PARSE__OPNXX,                       // 43:   'X'
    PARSE__OPNXX_,                      // 44:   ':'
    PARSE__OPNXX_X,                     // 45:   'X'
    PARSE__OPNXX_XX                     // 46:   'X'
} STATES_MAIN;

typedef enum                            // Parking Lot Status
{
    PL_CLOSE,                           // 0:   parking lot closed
    PL_OPEN                             // 1:   parking lot open
} PARKING_LOT_STATUS;


/* SERIAL INTERFACE */
#define RXBUF 32                        // Size of circular receiver buffer.

volatile uint8_t rxBuf[RXBUF];          // The circular buffer.
volatile uint8_t rxBufS = 0;            // Start: head position.
volatile uint8_t rxBufE = 0;            // End: tail position.
uint8_t rxBufErr = 0;                   // Overflow error counter.
static uint8_t rxChar = 0;              // Currently parsing Character

/* UART methods */
void uartTx(uint8_t *data);             // transmit to monitor screen
char ASCIIstring[5];                    // String or character array
void ConvertInt16toACSIIstring (unsigned int);  // Convert integer to string


/* PARSER ROUTINES */

//FSM of the parser.
void parse_main(uint8_t *data);

// Parking Lot parameters
volatile uint8_t button1 = 0;            // Counting register for sensor1 (button1).
volatile uint8_t button2 = 0;            // Counting register for sensor2 (button2).
static uint8_t vehicle_count = 0;        // Counting register for number of vehicles.
volatile uint8_t max_limit = 255;        // Maximum vehicles(Default).
static uint16_t avg_speed = 0;           // Average speed of all vehicles inside the garage (KMPH)
volatile uint8_t direction = 0;          // Direction: Forward 0(default), Reverse 1.
volatile uint8_t rtc_status = 0;         // RTC alarm event status: Open 0(default), Close 1.
volatile uint8_t PL_Status = PL_OPEN;    // Parking Lot Status: Open(Default).

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
    PJSEL0 |= BIT4;                // Enable Crystal mode on LFXIN-Pin
    PJSEL1 &= ~BIT4;

    CSCTL0_H    = CSKEY_H;              // Unlock CS registers.
    CSCTL1      = DCOFSEL_0;            // Set DCO to 1 MHz, DCORSEL for
                                        //   high speed mode not enabled.
    CSCTL2      = SELA__LFXTCLK |       // Set ACLK = LFXTCLK = 32768 Hz.
                  SELS__DCOCLK |        //   Set SMCLK = DCOCLK.
                  SELM__DCOCLK;         //   Set MCLK = DCOCLK.
                                        // SMCLK = MCLK = DCOCLK = 1 MHz.
    CSCTL3      = DIVA__1 |             //   Set ACLK divider to 1.
                  DIVS__1 |             //   Set SMCLK divider to 1.
                  DIVM__1;              //   Set MCLK divider to 1.
                                        // Set all dividers to 1.
    CSCTL4      |= LFXTDRIVE_0;
    CSCTL4      &= ~LFXTOFF;            // Enable LFXT

    do
    {
        CSCTL5 &= ~LFXTOFFG;            // Clear LFXT fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1&OFIFG);             // Test oscillator fault flag

    CSCTL0_H    = 0;                    // Lock CS registers.

    // Configure Real time clock
    RTCCTL01 |= RTCTEVIE + RTCBCD + RTCRDYIE + RTCHOLD + RTCAIE;
    // BCD mode; Hold RTC for setting; Enable RTC ready interrupt, Alarm interrupt and Timer Event interrupt
    RTCHOUR = 0x08;                     // Initialise RTC to 08:00 (Default)
    RTCMIN = 0x00;
    RTCCTL01 &= ~RTCHOLD; // Release RTChold and begin counting

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

    P1DIR &= ~BIT1;                     // P1.1 - input for Button.
    P1REN |= BIT1;                      // Enable Resistor
    P1OUT |= BIT1;                      // Select Pull up Resistor

    // Initialize port 4:
    P4DIR |= BIT6;                      // P4.6 - output for LED1, off.
    P4OUT &= ~BIT6;

    P4DIR &= ~BIT5;                     // P4.5 - input for Button.
    P4REN |= BIT5;                      // Enable Resistor
    P4OUT |= BIT5;                      // Select Pull up Resistor

    // Initialize port interrupts:
    P1IE |= BIT1;                       // P1.1 - port interrupt enabled.
    P1IES |= BIT1;                      //   Falling edge detection.
    P4IE |= BIT5;                       // P4.5 - port interrupt enabled.
    P4IES |= BIT5;                      //   Falling edge detection.

    // Initialize port 2:
    // Select Tx and Rx functionality of eUSCI0 for hardware UART.
    // P2.0 - UART Tx (UCA0TXD).
    // P2.1 - UART Rx (UCA0RXD).
    P2SEL0 &= ~(BIT1 | BIT0);
    P2SEL1 |= BIT1 | BIT0;

    // Disable the GPIO power-on default high-impedance mode to activate the
    // previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;

    /* Initialization of the serial UART interface */
    UCA0CTLW0 = UCSWRST;               // Enable software reset.
    UCA0CTLW0 |= UCSSEL__ACLK;         // Select clock source ACLK = 32768 Hz.
    // Set Baud rate of 9600 Bd.
    UCA0BRW = 3;                       // Clock prescaler of the
                                       //   Baud rate generator.
    UCA0MCTLW = UCBRS0 |
                UCBRS3 |
                UCBRS6;                // Second modulation stage. (0x92)
    UCA0CTLW0 &= ~UCSWRST;             // Disable software reset and start
                                       //   eUSCI state machine.
    UCA0IE |= UCRXIE;                  // Enable UART receiver Interrupt

    // Clear interrupt flags that have been raised due to high-impedance settings.
    P1IFG &= ~BIT1;
    P4IFG &= ~BIT5;

    /* Initialize status bits */
    States.Parser = PARSE_IDLE;
    States.Tout = 0;

    // Enable interrupts globally.
    __bis_SR_register(GIE);

    /* MAIN LOOP */
    while(1)
    {
        /* Overflow handling */
         while(rxBufErr > 0)            // Buffer overflow occurred?
        {
            uartTx("Error: Buffer overflow!\r\n");
            rxBufErr--;                 // Decrement overflow error counter.
        }

        /* Input handling */
        if(rxBufS != rxBufE)            // Received characters available?
        {
            // Get received character from circular buffer.
            rxChar  = rxBuf[rxBufE++];
            rxBufE = rxBufE & (RXBUF - 1);
            parse_main(&rxChar);        // Parse the character.
        }
        else                            // Parsing completed
        {
            __bis_SR_register(LPM3_bits);       // Enter LPM3
            /* TODO SLEEP */
            // Mode: LPM3
            // Reason: Only ACLK is required further
            _no_operation();            // for debugger.
        }

        if(States.Tout)
        {
            States.Tout = 0;            // Reset timeout status bit.
            stop_timeout();             // Stop timeout watchdog timer.

            // Reset parser FSM states.
            States.Parser = PARSE_IDLE;

            // Send timeout notification.
            uartTx("Error: Timeout!\r\n");
        }
    }
}

/* FSM Implementation */
void parse_main(uint8_t *data)
{
    /* Active */
    static uint8_t digit1 = 0;  //MAX1
    static uint8_t digit2 = 0;  //MAX12
    static uint8_t digit3 = 0;  //MAX123
    static uint8_t digit4 = 0;  //HourX
    static uint8_t digit5 = 0;  //HourXX
    static uint8_t digit6 = 0;  //MinutesX
    static uint8_t digit7 = 0;  //MinutesXX

    switch(States.Parser)
    {
    case PARSE_IDLE:                    // IDLE
        if(*data == 'C')
        {
            start_timeout();            // Start timeout watchdog timer.
            States.Parser = PARSE_C;    // Go to C state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
        }
        break;
    case PARSE_C:                       // C
        if(*data == 'P')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE_P;    // Go to P state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE_P:                       // P
        if(*data == 'R')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE_R;    // Go to R state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE_R:                       // R
        if(*data == 'S')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE_S;    // Go to S state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE_S:                       // S
        if(*data == '_')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__;    // Go to _ state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__:                       // _
        if(*data == 'R')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__R;   // Go to _R state.
        }
        else if(*data == 'D')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__D;   // Go to _D state.
        }
        else if(*data == 'C')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__C;   // Go to _C state.
        }
        else if(*data == 'O')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__O;   // Go to _O state.
        }
        else if(*data == 'M')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__M;   // Go to _M state.
        }
        else if(*data == 'S')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__S;   // Go to _S state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__R:                      // _R
        if(*data == 'E')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__RE;  // Go to _RE state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__RE:                     // _RE
        if(*data == 'S')
        {
            reset_timeout();            // Clear timeout watchdog timer.
            States.Parser = PARSE__RES; // Go to _RES state.
        }
        else
        {
            stop_timeout();             // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE; // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__RES:                    // _RES
        stop_timeout();                 // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;     // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            button1 = 0;                // Reset Counting register for sensor1 (button1).
            button2 = 0;                // Reset Counting register for sensor2 (button2).
            vehicle_count = 0;          // Reset Counting register for number of vehicles.
            avg_speed = 0;              // Reset Average speed value.
            direction = 0;              // Reset Direction.
            max_limit = 255;            // Reset Maximum vehicle limit.
            PL_Status = PL_OPEN;        // RESET Parking lot status to Open

            // Blink Both LED once for 400ms to acknowledge successful RESET
            TA1CTL = TACLR;                     // Stop and clear the timer.
            TA1CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
            TA1CCTL2 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
            TA1CCTL2 |= CCIS_3;                 // Capture value by switching the CCIS potential to Vcc
            P1OUT |= BIT0;                      // Turn LED2 On.
            P4OUT |= BIT6;                      // Turn LED1 On.
            TA1CCTL2 &= ~CAP;                   // Change mode to COMPARE
            TA1CCTL2 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
            TA1CCTL2 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
            TA1CCR2 = TA1CCR2 + 6554;           // Add offset 400ms

            // Send acknowledgement.
            uartTx("All parameters reset to default!\r\n");
        }
        break;
    case PARSE__D:                       // _D
        if(*data == 'I')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__DI;   // Go to _DI state.
        }
        else
        {
            stop_timeout();              // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;  // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__DI:                      // _DI
        if(*data == 'R')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__DIR;  // Go to _DIR state.
        }
        else
        {
            stop_timeout();              // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;  // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__DIR:                     // _DIR
        if(*data == '1')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__DIR1; // Go to _DIR1 state.
        }
        else if(*data == '2')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__DIR2; // Go to _DIR2 state.
        }
        else
        {
            stop_timeout();              // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;  // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__DIR1:                    // _DIR1
        stop_timeout();                  // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;      // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            direction = 0;
            // Send acknowledgement.
            uartTx("Direction 1 Set!\r\n");
        }
        break;
    case PARSE__DIR2:                    // _DIR2
        //TODO FSM_E
        stop_timeout();                  // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;      // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            direction = 1;
            // Send acknowledgement.
            uartTx("Direction 2 Set!\r\n");
        }
        break;
    case PARSE__C:                       // _C
        if(*data == 'L')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__CL;   // Go to _CL state.
        }
        else if(*data == 'N')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__CN;   // Go to _CN state.
        }
        else
        {
            stop_timeout();              // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;  // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CL:                      // _CL
        if(*data == 'S')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__CLS;  // Go to _CLS state.
        }
        else if(*data == 'K')
        {
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__CLK;  // Go to _CLK state.
        }
        else
        {
            stop_timeout();              // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;  // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLS:                     // _CLS
        if(*data >= '0' && *data <= '2')
        {
            digit4 = *data - 48;         // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();             // Clear timeout watchdog timer.
            States.Parser = PARSE__CLSX; // Go to _CLSX state.
        }
        else if(*data == '\r' || *data == '\n')
        {
            PL_Status = PL_CLOSE;

            // Blink Both LED once for 400ms to acknowledge successful CLOSE
            TA1CTL = TACLR;                     // Stop and clear the timer.
            TA1CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
            TA1CCTL2 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
            TA1CCTL2 |= CCIS_3;                 // Capture value by switching the CCIS potential to Vcc
            P1OUT |= BIT0;                      // Turn LED2 On.
            P4OUT |= BIT6;                      // Turn LED1 On.
            TA1CCTL2 &= ~CAP;                   // Change mode to COMPARE
            TA1CCTL2 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
            TA1CCTL2 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
            TA1CCR2 = TA1CCR2 + 6554;           // Add offset 400ms

            stop_timeout();                     // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;         // Go back to idle state.
            // Send acknowledgement.
            uartTx("Parking lot Closed!\r\n");
        }
        else
        {
            stop_timeout();                     // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;         // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLSX:                       // _CLSX
        if(*data >= '0' && *data <= '9')
        {
            digit5 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLSXX;   // Go to _CLSXX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLSXX:                      // _CLSXX
        if(*data == ':')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLSXX_;  // Go to _CLSXX_ state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLSXX_:                     // _CLSXX_
        if(*data >= '0' && *data <= '5')
        {
            digit6 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLSXX_X; // Go to _CLSXX_X state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLSXX_X:                    // _CLSXX_X
        if(*data >= '0' && *data <= '9')
        {
            digit7 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLSXX_XX;// Go to _CLSXX_XX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLSXX_XX:                   // _CLSXX_XX
        stop_timeout();                     // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;         // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            rtc_status = 1;                 // Close at alarm event
            // Disable Alarm bits
            RTCAHOUR &= ~RTCAE;
            RTCAMIN &= ~RTCAE;
            // CLear all Alarm registers
            RTCADOWDAY = 0x00;
            RTCADAY = 0x00;
            RTCAHOUR = 0x00;
            RTCAMIN = 0x00;
            // New alarm time
            RTCAHOUR = 10*digit4 + digit5;
            RTCAMIN = 10*digit6 + digit7;
            // Enable alarm bits
            RTCAHOUR |= RTCAE;
            RTCAMIN |= RTCAE;
            // Send acknowledgement.
            uartTx("Parking lot Closing time set!\r\n");
        }
        break;
    case PARSE__CLK:                        // _CLK
        if(*data >= '0' && *data <= '2')
        {
            digit4 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLKX;    // Go to _CLKX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLKX:                       // _CLKX
        if(*data >= '0' && *data <= '9')
        {
            digit5 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLKXX;   // Go to _CLKXX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLKXX:                      // _CLKXX
        if(*data == ':')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLKXX_;  // Go to _CLKXX_ state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLKXX_:                     // _CLKXX_
        if(*data >= '0' && *data <= '5')
        {
            digit6 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLKXX_X; // Go to _CLKXX_X state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLKXX_X:                    // _CLKXX_X
        if(*data >= '0' && *data <= '9')
        {
            digit7 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CLKXX_XX;// Go to _CLKXX_XX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CLKXX_XX:                   // _CLKXX_XX
        stop_timeout();                     // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;         // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            // Disable Alarm interrupts
            RTCCTL01 &= ~RTCAIE;
            RTCCTL01 &= ~RTCAIFG;
            // Clear all alarm registers
            RTCADOWDAY = 0x00;
            RTCADAY = 0x00;
            RTCAHOUR = 0x00;
            RTCAMIN = 0x00;

            RTCCTL01 |= RTCHOLD;            // Hold RTC for setting
            // New initial time
            RTCHOUR = 10*digit4 + digit5;
            RTCMIN = 10*digit6 + digit7;
            RTCCTL01 &= ~RTCHOLD;           // Release RTChold and begin counting
            RTCCTL01 |= RTCAIE;             // Enable Alarm interrupts
            // Send acknowledgement.
            uartTx("RTC initial value set!\r\n");
        }
        break;
    case PARSE__CN:                         // _CN
        if(*data == 'T')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__CNT;     // Go to _CNT state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__CNT:                        // _CNT
        stop_timeout();                     // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;         // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            ConvertInt16toACSIIstring(vehicle_count);
            // Send acknowledgement.
            uartTx("No. of vehicles:");
            uartTx(ASCIIstring);            // Counter value
            uartTx("!\r\n");
        }
        break;
    case PARSE__O:                          // _O
        if(*data == 'P')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OP;      // Go to _OP state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OP:                         // _OP
        if(*data == 'N')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OPN;     // Go to _OPN state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OPN:                        // _OPN
        if(*data >= '0' && *data <= '2')
        {
            digit4 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OPNX;    // Go to _OPNX state.
        }
        else if(*data == '\r' || *data == '\n')
        {
            PL_Status = PL_OPEN;

            // Blink Both LED once for 400ms to acknowledge successful OPEN
            TA1CTL = TACLR;                 // Stop and clear the timer.
            TA1CTL = TASSEL_1 + ID_1 + MC_2;// Source_ACLK + Prescaler_2 + Continuous_mode
            TA1CCTL2 = CAP + CM_1 + CCIS_2; // Capture_mode + Rise_Edge + Default_GND
            TA1CCTL2 |= CCIS_3;             // Capture value by switching the CCIS potential to Vcc
            P1OUT |= BIT0;                  // Turn LED2 On.
            P4OUT |= BIT6;                  // Turn LED1 on.
            TA1CCTL2 &= ~CAP;               // Change mode to COMPARE
            TA1CCTL2 &= ~CCIFG;             // Drop TA0CCR1_CCIFG flag
            TA1CCTL2 |= CCIE;               // Enable TA0CCR1_Timer Interrupt
            TA1CCR2 = TA1CCR2 + 6554;       // Add offset 400ms

            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Parking lot Opened!\r\n");
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OPNX:                       // _OPNX
        if(*data >= '0' && *data <= '9')
        {
            digit5 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OPNXX;   // Go to _OPNXX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OPNXX:                      // _OPNXX
        if(*data == ':')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OPNXX_;  // Go to _OPNXX_ state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OPNXX_:                     // _OPNXX_
        if(*data >= '0' && *data <= '5')
        {
            digit6 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OPNXX_X; // Go to _OPNXX_X state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OPNXX_X:                    // _OPNXX_X
        if(*data >= '0' && *data <= '9')
        {
            digit7 = *data - 48;            // ASCII value conversion '0' - 48 to '9' - 57
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__OPNXX_XX;// Go to _OPNXX_XX state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__OPNXX_XX:                   // _OPNXX_XX
        stop_timeout();                     // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;         // Go back to idle state.
        if(*data == '\r' || *data == '\n')
        {
            rtc_status = 0;                 // Open at alarm event
            // Disable Alarm bits
            RTCAHOUR &= ~RTCAE;
            RTCAMIN &= ~RTCAE;
            // CLear all Alarm registers
            RTCADOWDAY = 0x00;
            RTCADAY = 0x00;
            RTCAHOUR = 0x00;
            RTCAMIN = 0x00;
            // New alarm time
            RTCAHOUR = 10*digit4 + digit5;
            RTCAMIN = 10*digit6 + digit7;
            // Enable alarm bits
            RTCAHOUR |= RTCAE;
            RTCAMIN |= RTCAE;
            // Send acknowledgement.
            uartTx("Parking lot Opening time set!\r\n");
        }
        break;
    case PARSE__M:                          // _M
        if(*data == 'A')
        {
            reset_timeout();                // Clear timeout watchdog timer.
            States.Parser = PARSE__MA;      // Go to _MA state.
        }
        else
        {
            stop_timeout();                 // Stop timeout watchdog timer.
            States.Parser = PARSE_IDLE;     // Go back to idle state.
            // Send acknowledgement.
            uartTx("Invalid command!\r\n");
        }
        break;
    case PARSE__MA:                         // _MA
         if(*data == 'X')
         {
             reset_timeout();               // Clear timeout watchdog timer.
             States.Parser = PARSE__MAX;    // Go to _MAX state.
         }
         else
         {
             stop_timeout();                // Stop timeout watchdog timer.
             States.Parser = PARSE_IDLE;    // Go back to idle state.
             // Send acknowledgement.
             uartTx("Invalid command!\r\n");
         }
         break;
    case PARSE__MAX:                        // _MAX
         if(*data >= '0' && *data <= '9')
         {
             digit1 = *data - 48;           // ASCII value conversion '0' - 48 to '9' - 57
             reset_timeout();               // Clear timeout watchdog timer.
             States.Parser = PARSE__MAX1;   // Go to _MAX1 state.
         }
         else
         {
             stop_timeout();                // Stop timeout watchdog timer.
             States.Parser = PARSE_IDLE;    // Go back to idle state.
             // Send acknowledgement.
             uartTx("Invalid command!\r\n");
         }
         break;
    case PARSE__MAX1:                       // _MAX1
         if(*data >= '0' && *data <= '9')
         {
             digit2 = *data - 48;           // ASCII value conversion '0' - 48 to '9' - 57
             reset_timeout();               // Clear timeout watchdog timer.
             States.Parser = PARSE__MAX2;   // Go to _MAX2 state.
         }
         else
         {
             stop_timeout();                // Stop timeout watchdog timer.
             States.Parser = PARSE_IDLE;    // Go back to idle state.
             if(*data == '\r' || *data == '\n')
             {
                 max_limit = digit1;
                 // Send acknowledgement.
                 uartTx("New maximum vehicle limit value set!\r\n");
             }
         }
         break;
    case PARSE__MAX2:                       // _MAX2
         if(*data >= '0' && *data <= '9')
         {
             digit3 = *data - 48;           // ASCII value conversion '0' - 48 to '9' - 57
             reset_timeout();               // Clear timeout watchdog timer.
             States.Parser = PARSE__MAX3;   // Go to _MAX3 state.
         }
         else
         {
             stop_timeout();                // Stop timeout watchdog timer.
             States.Parser = PARSE_IDLE;    // Go back to idle state.
             if(*data == '\r' || *data == '\n')
             {
                 max_limit = 10*digit1 + digit2;
                 // Send acknowledgement.
                 uartTx("New maximum vehicle limit value set!\r\n");
             }
         }
         break;
    case PARSE__MAX3:                       // _MAX3
        stop_timeout();                     // Stop timeout watchdog timer.
        States.Parser = PARSE_IDLE;         // Go back to idle state.
        if(*data == '\r' || *data == '\n')
         {
             max_limit = 100*digit1 + 10*digit2 + digit3;
             // Send acknowledgement.
             uartTx("New maximum vehicle limit value set!\r\n");
         }
         break;
    case PARSE__S:                          // _S
         if(*data == 'P')
         {
             reset_timeout();               // Clear timeout watchdog timer.
             States.Parser = PARSE__SP;     // Go to _SP state.
         }
         else
         {
             stop_timeout();                // Stop timeout watchdog timer.
             States.Parser = PARSE_IDLE;    // Go back to idle state.
             // Send acknowledgement.
             uartTx("Invalid command!\r\n");
         }
         break;
    case PARSE__SP:                         // _SP
         if(*data == 'D')
         {
             reset_timeout();               // Clear timeout watchdog timer.
             States.Parser = PARSE__SPD;    // Go to _SPD state.
         }
         else
         {
             stop_timeout();                // Stop timeout watchdog timer.
             States.Parser = PARSE_IDLE;    // Go back to idle state.
             // Send acknowledgement.
             uartTx("Invalid command!\r\n");
         }
         break;
    case PARSE__SPD:                        // _SPD
       stop_timeout();                      // Stop timeout watchdog timer.
       States.Parser = PARSE_IDLE;          // Go back to idle state.
       if(*data == '\r' || *data == '\n')
        {
           ConvertInt16toACSIIstring(avg_speed);
           // Send acknowledgement.
           uartTx("Average speed of vehicles in parking lot:");
           uartTx(ASCIIstring);             // Average speed value
           uartTx(" KMPH!\r\n");
        }
        break;
    }
}


/* ### SERIAL INTERFACE ### */
void uartTx(uint8_t *data)
{
    /* Active  */
    // Iterate through array, look for null pointer at end of string.
    uint16_t i = 0;
    while(data[i])
    {
        while((UCA0STATW & UCBUSY));    // Wait while module is busy with data.
        UCA0TXBUF = data[i];            // Transmit element i of data array.
        i++;                            // Increment variable for array address.
    }
}

void ConvertInt16toACSIIstring (unsigned int Value)
{
    /* Active  */
    unsigned char c = 5;
    memcpy(ASCIIstring, "    0", 5);
    //  convert integer to a character array
    while (c > 0 && Value != 0)
    {
        ASCIIstring[--c] = Value % 10 + '0';
        Value /= 10;
    }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE:                     // No interrupts
        break;
    case USCI_UART_UCRXIFG:             // Received data
        /* LPM3 or Active*/
        rxBuf[rxBufS++] = UCA0RXBUF;    // push the character to circular buffer
        rxBufS = rxBufS & (RXBUF - 1);
        if(rxBufS == rxBufE)            // Maximum characters waiting to be parsed
            {
                rxBufErr++;             // Buffer full flag
            }
        __bic_SR_register_on_exit(LPM3_bits);   // Exit LPM3
        /* TODO SLEEP */
        // Mode: Active
        // Reason: Go back to Active mode as AClck and CPU are required
        break;
    case USCI_UART_UCTXIFG:             // Transmit data
        break;
    case USCI_UART_UCSTTIFG:            //
        break;
    case USCI_UART_UCTXCPTIFG:          //
        break;
    }
}

/* ### TIMEOUT TIMER ### */
void start_timeout(void)                // Initialize timeout watchdog timer.
{
    /* Active  */
    // Timeout interrupt after interval of 1 s using ACLK.
    WDTCTL = WDTPW |                    // The watchdog timer password (0x5A).
             WDTSSEL__ACLK |            // Select ACLK = 32768 Hz as clock source.
             WDTTMSEL |                 // Interval timer mode.
             WDTIS__32K;                // Set interrupt prescaler: 2^15
                                        // 1 / (32768 Hz / 32768) = 1 s
    SFRIE1 |= WDTIE;                    // Enable watchdog timer interrupt.

}

void reset_timeout(void)                // Reset timeout watchdog timer.
{
    /* Active  */
    WDTCTL = (WDTCTL & 0x00FF) |
             WDTPW |                    // The watchdog timer password (0x5A).
             WDTCNTCL;                  // Clear watchdog timer.
}

void stop_timeout(void)                 // Stop timeout watchdog timer.
{
    /* Active  */
    WDTCTL = (WDTCTL & 0x00FF) |
             WDTPW |                    // The watchdog timer password (0x5A).
             WDTCNTCL |                 // Clear watchdog timer.
             WDTHOLD;                   // Stop watchdog timer.
    SFRIE1 &= ~WDTIE;                   // Disable watchdog timer interrupt.

}

/* ISR WATCHDOG */
#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    /* Active  */
    // Raise status bit of timeout event.
    States.Tout = 1;
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
        if(PL_Status == PL_OPEN)        // check if Parking lot is Open
        {
            /* LPM3  */
            /* Start Debouncing of Button S2 */
            P1IE &= ~BIT1;                      // Disable port interrupt during delay.

            // Initialize timer TA0 for button debouncing with a delay of 25 ms:
            TA0CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
            TA0CCTL2 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
            TA0CCTL2 |= CCIS_3;                 // Capture value by switching the CCIS potential to Vcc
            TA0CCTL2 &= ~CAP;                   // Change mode to COMPARE
            TA0CCTL2 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
            TA0CCTL2 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
            TA0CCR2 = TA0CCR2 + 410;            // Add offset 25ms
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

        if(PL_Status == PL_OPEN)
        {
            /* LPM3 */
            /* Start Debouncing of Button S2 */
            P4IE &= ~BIT5;                      // Disable port interrupt during delay.

            // Initialize timer TA0 for button debouncing with a delay of 25 ms:
            TA0CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
            TA0CCTL1 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
            TA0CCTL1 |= CCIS_3;                 // Capture value by switching the CCIS potential to Vcc
            TA0CCTL1 &= ~CAP;                   // Change mode to COMPARE
            TA0CCTL1 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
            TA0CCTL1 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
            TA0CCR1 = TA0CCR1 + 410;            // Add offset 25ms
        }
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
    static uint8_t speed = 0;
    switch(__even_in_range(TA0IV, TA0IV_TA0IFG))
    {
    case TA0IV_TA0CCR1:                 // TA0 CCR1
        /* LPM3 */
        /* Complete Debouncing of Button S2 */
        TA0CTL = TACLR;                 // Stop and clear the timer.
        TA0CCTL1 &= ~CCIE;              // Disable interrupt capability.

        if(!(P4IN & BIT5))              // Validate button S2 state: pushed.
        {
            if(button2 > 0)             // check if button2 pushed earlier to this event
            {
                if(direction == 0)      // check the direction
                {
                    if(vehicle_count > 0)               // check if parking lot is empty
                    {
                        TA1CCTL2 |= CCIS_3;             // Capture value by switching the CCIS potential to Vcc
                        vehicle_count--;                // Decrement counter
                        speed = 36000/TA1CCR2;          // speed = distance / time; (Minimum speed = 2 KMPH)
                                                        // distance = 1 mtr = 0.001 km (For practical speed values)
                                                        // time = (TA1CCR/10,000)sec .... (from ACLCK 10KHz - 1 sec)
                                                        // time = (TA1CCR/(10,000*3600))hr
                        if(vehicle_count == 0)
                            avg_speed = 0;
                        else
                            avg_speed = (avg_speed*(vehicle_count+1)-speed)/vehicle_count;

                        P4OUT |= BIT6;                      // Turn LED1 On.
                        TA1CCTL2 &= ~CAP;                   // Change mode to COMPARE
                        TA1CCTL2 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
                        P1IE &= ~BIT1;                      // Disable port interrupt during LED active.
                        P4IE &= ~BIT5;                      // Disable port interrupt during LED active.
                        TA1CCTL2 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
                        TA1CCR2 = TA1CCR2 + 3277;           // Add offset 200ms
                    }
                }
                else
                {
                    if(vehicle_count < max_limit)       // check if parking lot is full
                    {
                        TA1CCTL2 |= CCIS_3;             // Capture value by switching the CCIS potential to Vcc
                        vehicle_count++;                // Increment counter
                        speed = 36000/TA1CCR2;          // speed = distance / time; (Minimum speed = 2 KMPH)
                                                        // distance = 1 mtr = 0.001 km (For practical speed values)
                                                        // time = (TA1CCR/10,000)sec .... (from ACLCK 10KHz - 1 sec)
                                                        // time = (TA1CCR/(10,000*3600))hr
                        avg_speed = (avg_speed*(vehicle_count-1)+speed)/vehicle_count;

                        P1OUT |= BIT0;                      // Turn LED2 On.
                        TA1CCTL2 &= ~CAP;                   // Change mode to COMPARE
                        TA1CCTL2 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
                        P1IE &= ~BIT1;                      // Disable port interrupt during LED active.
                        P4IE &= ~BIT5;                      // Disable port interrupt during LED active.
                        TA1CCTL2 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
                        TA1CCR2 = TA1CCR2 + 3277;           // Add offset 200ms
                    }
                }
                button1 = 0;    // reset button1 event
                button2 = 0;    // reset button2 event
            }
            else
            {
                button1 = 1;                        // Increment counter variable.
                TA1CTL = TACLR;                     // Stop and clear the timer.
                TA1CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
                TA1CCTL2 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
            }
        }
        P4IFG &= ~BIT5;                 // Drop meanwhile raised flag.
        P4IE |= BIT5;                   // Enable port interrupt again.
        break;
    case TA0IV_TA0CCR2:                 // TA0 CCR2
        /* LPM3 */
        /* Complete Debouncing of Button S2 */
        TA0CTL = TACLR;                 // Stop and clear the timer.
        TA0CCTL2 &= ~CCIE;              // Disable interrupt capability.

        if(!(P1IN & BIT1))              // Validate button S2 state: pushed.
        {
            /* Start or Reset Timeout */

            if(button1 > 0)             // check if button1 pushed earlier to this event
            {
                if(direction == 0)      // check the direction
                {
                    if(vehicle_count < max_limit)       // check if parking lot is full
                    {
                        TA1CCTL2 |= CCIS_3;             // Capture value by switching the CCIS potential to Vcc
                        vehicle_count++;                // Increment counter
                        speed = 36000/TA1CCR2;          // speed = distance / time; (Minimum speed = 2 KMPH)
                                                        // distance = 1 mtr = 0.001 km (For practical speed values)
                                                        // time = (TA1CCR/10,000)sec .... (from ACLCK 10KHz - 1 sec)
                                                        // time = (TA1CCR/(10,000*3600))hr
                        avg_speed = (avg_speed*(vehicle_count-1)+speed)/vehicle_count;

                        P1OUT |= BIT0;                          // Turn LED2 On.
                        TA1CCTL2 &= ~CAP;                       // Change mode to COMPARE
                        TA1CCTL2 &= ~CCIFG;                     // Drop TA0CCR1_CCIFG flag
                        P1IE &= ~BIT1;                          // Disable port interrupt during LED active.
                        P4IE &= ~BIT5;                          // Disable port interrupt during LED active.
                        TA1CCTL2 |= CCIE;                       // Enable TA0CCR1_Timer Interrupt
                        TA1CCR2 = TA1CCR2 + 3277;               // Add offset 200ms
                    }
                }
                else
                {
                    if(vehicle_count > 0)   // check if parking lot is empty
                    {
                        TA1CCTL2 |= CCIS_3;             // Capture value by switching the CCIS potential to Vcc
                        vehicle_count--;                // Decrement counter
                        speed = 36000/TA1CCR2;          // speed = distance / time; (Minimum speed = 2 KMPH)
                                                        // distance = 1 mtr = 0.001 km (For practical speed values)
                                                        // time = (TA1CCR/10,000)sec .... (from ACLCK 10KHz - 1 sec)
                                                        // time = (TA1CCR/(10,000*3600))hr
                        if(vehicle_count == 0)
                            avg_speed = 0;
                        else
                            avg_speed = (avg_speed*(vehicle_count+1)-speed)/vehicle_count;

                        P4OUT |= BIT6;                          // Turn LED1 On.
                        TA1CCTL2 &= ~CAP;                       // Change mode to COMPARE
                        TA1CCTL2 &= ~CCIFG;                     // Drop TA0CCR1_CCIFG flag
                        P1IE &= ~BIT1;                          // Disable port interrupt during LED active.
                        P4IE &= ~BIT5;                          // Disable port interrupt during LED active.
                        TA1CCTL2 |= CCIE;                       // Enable TA0CCR1_Timer Interrupt
                        TA1CCR2 = TA1CCR2 + 3277;               // Add offset 200ms
                    }
                }
                button1 = 0;    // reset button1 event
                button2 = 0;    // reset button2 event
            }
            else
            {
                button2 = 1;                        // Increment counter variable.
                TA1CTL = TACLR;                     // Stop and clear the timer.
                TA1CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
                TA1CCTL2 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
            }
        }
        P1IFG &= ~BIT1;                 // Drop meanwhile raised flag.
        P1IE |= BIT1;                   // Enable port interrupt again.
        break;
    case TA0IV_TA0IFG:                  // TA0 TAIFG
        break;
    }
}

/* ISR TIMER A1 - CCR0 */
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR (void)
{                                       // TA1 CCR0

}

/* ISR TIMER A1 - CCR1, CCR2 AND TAIFG */
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer1_A1_ISR (void)
{
    switch(__even_in_range(TA1IV, TA1IV_TA1IFG))
    {
    case TA1IV_TA1CCR1:                 // TA1 CCR1
        break;
    case TA1IV_TA1CCR2:                 // TA1 CCR2
        /* Active or LPM3 */
        TA1CTL = TACLR;                 // Stop and clear the timer.
        TA1CCTL2 &= ~CCIE;              // Disable interrupt capability.
        P1IE |= BIT1;                   // Enable port interrupt.
        P4IE |= BIT5;                   // Enable port interrupt.
        P1OUT &= ~BIT0;                 // Turn LED2 Off.
        P4OUT &= ~BIT6;                 // Turn LED1 Off.
        break;
    case TA1IV_TA1IFG:                  // TA1 TAIFG
        break;
    }
}

/* ISR RTC_B */
#pragma vector = RTC_VECTOR
__interrupt void rtc_isr(void)
{
    switch(__even_in_range(RTCIV,0x12))
    {
    case RTCIV_NONE:                    // No interrupt
        break;
    case RTCIV_RTCRDYIFG:               // RTC ready
        break;
    case RTCIV_RTCTEVIFG:               // RTC interval timer
        break;
    case RTCIV_RTCAIFG:                 // RTC user alarm
        /* Active*/
        // Blink Both LED once for 400ms to acknowledge successful OPEN/CLOSE
        TA1CTL = TACLR;                     // Stop and clear the timer.
        TA1CTL = TASSEL_1 + ID_1 + MC_2;    // Source_ACLK + Prescaler_2 + Continuous_mode
        TA1CCTL2 = CAP + CM_1 + CCIS_2;     // Capture_mode + Rise_Edge + Default_GND
        TA1CCTL2 |= CCIS_3;                 // Capture value by switching the CCIS potential to Vcc
        P1OUT |= BIT0;                      // Turn LED2 On.
        P4OUT |= BIT6;                      // Turn LED1 On.
        TA1CCTL2 &= ~CAP;                   // Change mode to COMPARE
        TA1CCTL2 &= ~CCIFG;                 // Drop TA0CCR1_CCIFG flag
        TA1CCTL2 |= CCIE;                   // Enable TA0CCR1_Timer Interrupt
        TA1CCR2 = TA1CCR2 + 6554;           // Add offset 400ms

        if(rtc_status == 0)                 // RTC OPEN alarm event
            PL_Status = PL_OPEN;            // Parking lot status: OPEN
        else if (rtc_status == 1)           // RTC CLOSE alarm event
            PL_Status = PL_CLOSE;           // Parking lot status: CLOSE
        break;
    case RTCIV_RT0PSIFG:                // RTC prescaler 0
        break;
    case RTCIV_RT1PSIFG:                // RTC prescaler 1
        break;
    case RTCIV_RTCOFIFG:                // RTC oscillator fault
        break;
    }
}
