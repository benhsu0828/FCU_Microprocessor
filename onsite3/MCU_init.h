//Define Clock source
#define MCU_CLOCK_SOURCE      
#define MCU_CLOCK_SOURCE_HXT  // HXT, LXT, HIRC, LIRC
#define MCU_CLOCK_FREQUENCY   50000000  //Hz

//Define MCU Interfaces

// Timer 0
#define MCU_INTERFACE_TMR0
#define TMR0_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, EXT, LIRC, HIRC
#define TMR0_CLOCK_DIVIDER    1
#define TMR0_OPERATING_MODE   TIMER_PERIODIC_MODE // ONESHOT, PERIODIC, TOGGLE, CONTINUOUS
#define TMR0_OPERATING_FREQ   1 //Hz
// Timer 1
#define MCU_INTERFACE_TMR1
#define TMR1_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, EXT, LIRC, HIRC
#define TMR1_CLOCK_DIVIDER    1
#define TMR1_OPERATING_MODE   TIMER_PERIODIC_MODE // ONESHOT, PERIODIC, TOGGLE, CONTINUOUS
#define TMR1_OPERATING_FREQ   1 //Hz
// Timer 2
#define MCU_INTERFACE_TMR2
#define TMR2_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, EXT, LIRC, HIRC
#define TMR2_CLOCK_DIVIDER    1
#define TMR2_OPERATING_MODE   TIMER_PERIODIC_MODE // ONESHOT, PERIODIC, TOGGLE, CONTINUOUS
#define TMR2_OPERATING_FREQ   1 //Hz
// Timer 3
#define MCU_INTERFACE_TMR3
#define TMR3_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, EXT, LIRC, HIRC
#define TMR3_CLOCK_DIVIDER    1
#define TMR3_OPERATING_MODE   TIMER_PERIODIC_MODE // ONESHOT, PERIODIC, TOGGLE, CONTINUOUS
#define TMR3_OPERATING_FREQ   1 //Hz

// PB15
#define MCU_INTERFACE_INT1_PB15

//Define MCU Interfaces
// SPI3 for LCD
#define MCU_INTERFACE_SPI3
#define SPI3_CLOCK_SOURCE_HCLK // HCLK, PLL
#define PIN_SPI3_SS0_PD8
#define PIN_SPI3_SCLK_PD9
#define PIN_SPI3_MISO0_PD10
#define PIN_SPI3_MOSI0_PD11

//Define MCU Interfaces for ADC
#define MCU_INTERFACE_ADC
#define ADC_CLOCK_SOURCE_HXT  // HXT, LXT, PLL, HIRC, HCLK
#define ADC_CLOCK_DIVIDER     1
#define PIN_ADC7_PA7
#define ADC_CHANNEL_MASK      ADC_CH_7_MASK
#define ADC_INPUT_MODE        ADC_INPUT_MODE_SINGLE_END // SINGLE_END, DIFFERENTIAL
#define ADC_OPERATION_MODE    ADC_OPERATION_MODE_SINGLE_CYCLE // SINGLE, SINGLE_CYCLE, CONTINUOUS

//Define PWM1
#define MCU_INTERFACE_PWM1
#define PWM1_CH01_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, HIRC
#define PWM1_CH23_CLOCK_SOURCE_HXT // HXT, LXT, HCLK, HIRC maybe not used
#define PIN_PWM1_CH0_PB11

// Define UART0 - RX, TX
#define MCU_INTERFACE_UART0
#define UART_CLOCK_SOURCE_HXT  // HXT, PLL, HIRC
#define UART_CLOCK_DIVIDER     3
#define PIN_UART0_RX_PB0
#define PIN_UART0_TX_PB1
