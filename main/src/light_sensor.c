/*
Driver for LTR329-ALS 
*/

#define I2C_MASTER_SCL_IO         CONFIG_I2C_MASTER_SCL /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO         CONFIG_I2C_MASTER_SDA /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM            0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces   \
       available will depend on the chip */
#define I2C_MASTER_FREQ_HZ        400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS     1000

#define LTR329_SLAVE_ADDR         0x29 // Write/Read bits are appended by IDF API
#define ALS_CONTR_REG             0x80
#define ALS_MEAS_RATE_REG         0x85 
#define ALS_DATA_CH1_LOW          0x88 
#define ALS_DATA_CH1_HIGH         0x89 

#define ALS_DATA_CH0_LOW          0x8A
#define ALS_DATA_CH0_HIGH         0x8B

#define LTR329_PFACTOR            1 // Factor for calculating Lux
#define GAIN                      8
#define INT_RATE                  2 // twice default rate of 100ms (200ms)

#include "driver/i2c.h"
#include "config.h"

/*
Process for getting values from sensor

1.) Reset via ALS_CONTR_REG
2.) Configure sensor range in Lux (8000 chosen)
3.) Configure sensor measurement and integration rate via ALS_MEAS_RATE
4.) Set sensor to active mode
5.) Measure via ALS_DATA_CH1_LOW, ALS_DATA_CH1_HIGH, and same with CH0
6.) Use gain based equation to get value in Lux
7.) Rinse and repeat
*/

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void) {
       int i2c_master_port = I2C_MASTER_NUM;

       i2c_config_t conf = {
       .mode = I2C_MODE_MASTER,
       .sda_io_num = I2C_MASTER_SDA_IO,
       .scl_io_num = I2C_MASTER_SCL_IO,
       .sda_pullup_en = GPIO_PULLUP_DISABLE,
       .scl_pullup_en = GPIO_PULLUP_DISABLE,
       .master.clk_speed = I2C_MASTER_FREQ_HZ,
       };

       i2c_param_config(i2c_master_port, &conf);

       return i2c_driver_install(i2c_master_port, conf.mode,
                            I2C_MASTER_RX_BUF_DISABLE,
                            I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * @brief Write a byte to a LTR329-ALS sensor register
 */
static esp_err_t write_reg(uint8_t reg_addr, uint8_t data) {
       int ret;
       uint8_t write_buf[2] = {reg_addr, data};

       ret = i2c_master_write_to_device(I2C_MASTER_NUM, LTR329_SLAVE_ADDR,
                                   write_buf, sizeof(write_buf),
                                   I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

       return ret;
}

/**
 * @brief Read a sequence of bytes from a LTR329-ALS sensor registers
 */
static esp_err_t read_reg(uint16_t reg_addr, uint8_t *data, size_t len) {
       return i2c_master_write_read_device(
              I2C_MASTER_NUM, LTR329_SLAVE_ADDR, &reg_addr, 1, data, len,
              I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void sensor_set_gain() {
       // Need to write ALS Gain to 0111 (0.125 to 8000 lux) in bits 4:2
       // Need to write SW Reset = 1 in bit 1 (initial startup procedure started)
       // Set ALS mode to active (1 in bit 0)
       // data -> 0b00001111 == 0x0F
       uint8_t data = 0x0F;
       esp_err_t err = write_reg(ALS_CONTR_REG, data);
}

void sensor_set_rate() {
       // Need to set integration rate of every 200 ms and meeasuring rate as default (every 500 ms)
       // Set bits 5:3 to 010, bits 2:0 to 011
       // data -> 00010011 --> 0x13
       uint8_t data = 0x13;
       esp_err_t err = write_reg(ALS_MEAS_RATE_REG, data);
}

float measure_data() {
       // Read register data from both channels in order prescribed in datasheet
       // Use Appendix A to convert datasheet values into output in lux
       uint8_t* ch1_low_data;
       uint8_t* ch1_high_data;
       uint8_t* ch0_low_data;
       uint8_t* ch0_high_data;
       esp_err_t err1 = read_reg(ALS_DATA_CH1_LOW, ch1_0_data);
       esp_err_t err2 = read_reg(ALS_DATA_CH1_HIGH, ch1_1_data);

       esp_err_t err3 = read_reg(ALS_DATA_CH0_LOW, ch0_0_data);
       esp_err_t err4 = read_reg(ALS_DATA_CH0_HIGH, ch0_1_data); // reading this register prompts a reset of the values

       uint16_t ch1_data = ((*ch1_low_data) << 8) || (*ch1_high_data);
       uint16_t ch0_data = ((*ch0_low_data) << 8) || (*ch0_high_data);

       // Apply formulae given in datasheet to get intensity in lux

       if (ch1_data + ch0_data == 0) {
              return 0; // to avoid divide by zero errors
       }

       float lux = 0;
       uint32_t ratio = (ch1_data) / (ch1_data + ch0_data);
       float pFactor = LTR329_PFACTOR;

       uint8_t divider = 16; // For lux range of 8000, integration time of 200ms
       uint8_t multiplier = 1; 

       if (ratio < 0.45) {
              lux = (1.773 * ch0_data + 1.1059 * ch1_data) / GAIN / INT_RATE;
       }

       else if (ratio < 0.64 && ratio >= 0.45) {
              lux = (4.2785 * ch0_data - 1.9548 * ch1_data) / GAIN / INT_RATE;
       }

       else if (ratio < 0.85 && ratio >= 0.64) {
              lux = (0.5926 * ch0_data + 0.1185 * ch1_data) / GAIN / INT_RATE;
       }

       else {
              lux = 0;
       }

       return lux;
} 

void i2c_poll(void) {
       i2c_master_init();
       sensor_set_gain();
       sensor_set_rate();

       while (1) {
              ambient_light = measure_data();
              vTaskDelay(400 / portTICK_PERIOD_MS); // update every 400ms
       }
}
