
typedef enum
{
	I2C_MASTER,
	I2C_SLAVE
} i2c_mode;

extern bool i2c_init(int clock, i2c_mode mode);
extern int i2c_read(int i2c_address, unsigned char* command, size_t c_size, unsigned char* data, size_t r_size);
extern int i2c_write(unsigned char i2c_address, unsigned char address, unsigned char* data, size_t size);
