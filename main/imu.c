#include "esp_idf_version.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "mpu6050.h"
#include "bsp/esp_bsp_devkit.h"
#include "driver/i2c_master.h"

static const char *TAG = "imu";

static complimentary_angle_t complimentary_angle;
static mpu6050_handle_t mpu6050_dev;
static mpu6050_acce_value_t acce;
static mpu6050_gyro_value_t gyro;

static void mpu6050_init()
{
    mpu6050_dev = mpu6050_create(BSP_I2C_NUM, MPU6050_I2C_ADDRESS);
    mpu6050_config(mpu6050_dev, ACCE_FS_4G, GYRO_FS_500DPS);
    mpu6050_wake_up(mpu6050_dev);
}

static void mpu6050_read(void *pvParameters)
{
    static int cnt;
    mpu6050_get_acce(mpu6050_dev, &acce);
    mpu6050_get_gyro(mpu6050_dev, &gyro);
    mpu6050_complimentory_filter(mpu6050_dev, &acce, &gyro, &complimentary_angle);
    if (cnt++ % 200 == 0) {
      ESP_LOGI(TAG, "acce_x:%.2f, acce_y:%.2f, acce_z:%.2f", acce.acce_x, acce.acce_y, acce.acce_z);
      ESP_LOGI(TAG, "gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
      ESP_LOGI(TAG, "roll: %.2f, pitch: %.2f", complimentary_angle.roll, complimentary_angle.pitch);
    }
}

void app_main(void)
{
    bsp_i2c_init();
    mpu6050_init();

    // In order to get accurate calculation of complimentary angle we need fast reading (5ms)
    // FreeRTOS resolution is 10ms, so esp_timer is used
    const esp_timer_create_args_t cal_timer_config = {
        .callback = mpu6050_read,
        .arg = NULL,
        .name = "MPU6050 timer",
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
        .skip_unhandled_events = true,
#endif
        .dispatch_method = ESP_TIMER_TASK
    };
    esp_timer_handle_t cal_timer = NULL;
    esp_timer_create(&cal_timer_config, &cal_timer);
    esp_timer_start_periodic(cal_timer, 5000); // 5ms
}
