#include <math.h>

#include "freertos/FreeRTOS.h"
#include "hal/ledc_types.h"
#include "iot_button.h"
#include "iot_servo.h"
#include "driver/gpio.h"

int SERVO_GPIO = 4;

void slew_to(int channel, float *cur_angle, float target_angle, float deg_per_sec) {
    float max_delta_angle = deg_per_sec * 0.02;
    float delta_angle = 1;
    while (fabs(delta_angle) > 0e-3) {
	delta_angle = target_angle - *cur_angle;
	if (fabs(delta_angle) > max_delta_angle) {
	    delta_angle = copysign(max_delta_angle, delta_angle);
	}
	*cur_angle += delta_angle;
	iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, channel, *cur_angle);
	vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    servo_config_t servo_cfg = {
        .max_angle = 270,
        .min_width_us = 500,
        .max_width_us = 2500,
        .freq = 50,
        .timer_number = LEDC_TIMER_0,
        .channels = {
            .servo_pin = {
                SERVO_GPIO,
            },
            .ch = {
                LEDC_CHANNEL_0,
            },
	},
        .channel_number = 1,
    };

    iot_servo_init(LEDC_HIGH_SPEED_MODE, &servo_cfg);
    int angles[] = {45, 225};
    int cnt = 0;

    float angle = angles[0];
    iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, 0, angle);

    while (1) {
      if (gpio_get_level(0) == 0) {  // boot button pressed
	slew_to(0, &angle, angles[cnt++ % 2], 30);
	vTaskDelay(pdMS_TO_TICKS(2000));
      }
      vTaskDelay(pdMS_TO_TICKS(400));
    }

 //    while (1) {
	// slew_to(0, &angle, 180, 30);
	// vTaskDelay(pdMS_TO_TICKS(1000));
	// slew_to(0, &angle, 0, 60);
	// vTaskDelay(pdMS_TO_TICKS(1000));
	// slew_to(0, &angle, 180, 90);
	// vTaskDelay(pdMS_TO_TICKS(1000));
	// slew_to(0, &angle, 0, 120);
	// vTaskDelay(pdMS_TO_TICKS(1000));
 //    }
}
