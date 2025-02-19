#define byte unsigned char
#define size_t int // 互換性のため
#define HIGH (1)
#define LOW (0)

// Timer
#define PIO_T1 (0x00) // PA0
#define PIO_T2 (0x01) // PA1
#define PIO_T3 (0x06) // PA6
#define PIO_T4 (0x07) // PA7
#define PIO_T5 (0x10) // PB0
#define PIO_T6 (0x11) // PB1
// DAC
#define PIO_DAC (0x04) // PA4
// AD
#define PIO_AD1 (0x20) // PC0
#define PIO_AD2 (0x21) // PC1
#define PIO_AD3 (0x22) // PC2
#define PIO_AD4 (0x23) // PC3
#define PIO_VDD (0x24) // PC4
//SW
#define PIO_SW1 (0x2E) // PC14
#define PIO_SW2 (0x2F) // PC15
// LED
#define PIO_LED1 (0x1D) // PB13(Red LED)
#define PIO_LED2 (0x1C) // PB12(Green LED)
// I2C
#define PIO_I2C_SCL (0x18) // PB8
#define PIO_I2C_SDA (0x19) // PB9
// SPI
#define PIO_SPI_MOSI (0x15) // PB5
#define PIO_SPI_MISO (0x14) // PB4
#define PIO_SPI_SCK (0x13) // PB3
#define PIO_SPI_NSS (0x0F) // PA15
// SIO
#define PIO_SIO1_TX (0x1A) // PB10
#define PIO_SIO1_RX (0x1B) // PB11
#define PIO_SIO2_TX (0x2A) // PC10
#define PIO_SIO2_RX (0x2B) // PC11
#define PIO_SIO3_TX (0x2C) // PC12
#define PIO_SIO3_RX (0x32) // PD2
#define PIO_SIO4_TX (0x26) // PC6
#define PIO_SIO4_RX (0x27) // PC7

// things omitted in SDK:
#define PIO_SET_IN (0x10) // dummy value
#define PIO_SET_OUT (0x20) // dummy value

extern bool pio_init(int port, int direction);
extern int pio_read(int port);
extern int pio_write(int port, int value);
