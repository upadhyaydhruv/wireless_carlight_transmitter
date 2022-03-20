#include "driver/i2c.h"

#define I2C_SCL_GPIO "TODO"
#define I2C_SDA_GPIO "TODO"

#define I2C_MASTER_NUM                                                         \
  0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces   \
       available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

#define MPU9250_SENSOR_ADDR 0x68 /*!< Slave address of the MPU9250 sensor */
#define MPU9250_WHO_AM_I_REG_ADDR                                              \
  0x75 /*!< Register addresses of the "who am I" register */

#define MPU9250_PWR_MGMT_1_REG_ADDR                                            \
  0x6B /*!< Register addresses of the power managment register */
#define MPU9250_RESET_BIT 7

static esp_err_t i2c_master_init(void)

void sensor_reset(i2c_config_t sensor);

void sensor_config(i2c_config_t sensor);

static esp_err_t write_reg(uint8_t reg_addr, uint8_t data);
static esp_err_t read_reg(uint8_t reg_addr, uint8_t *data, size_t len);


