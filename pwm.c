#include "source.h"
#include "pwm.h"

void setup_pwm() {
  gpio_set_function(10, GPIO_FUNC_PWM);
  gpio_set_function(11, GPIO_FUNC_PWM);
  gpio_set_function(13, GPIO_FUNC_PWM);

  uint slice_num1 = pwm_gpio_to_slice_num(10);
  pwm_set_wrap(slice_num1, 1024);
  uint slice_num2 = pwm_gpio_to_slice_num(13);
  pwm_set_wrap(slice_num2, 1024);

  pwm_set_gpio_level(10, 1024);   // red
  pwm_set_gpio_level(11, 1024);   // green
  pwm_set_gpio_level(13, 0);      // blue    // 0= full brightness
 
  pwm_set_enabled(slice_num1, true);
  pwm_set_enabled(slice_num2, true);
} 

void set_led1(struct Colour led) {
  pwm_set_gpio_level(10, 1024 - led.red);
  pwm_set_gpio_level(11, 1024 - led.green);
  pwm_set_gpio_level(13, 1024 - led.blue);
}

void fade_between(struct Colour from, struct Colour to, int steps, int step) {
  int red_diff = to.red - from.red;
  int green_diff = to.green - from.green;
  int blue_diff = to.blue - from.blue;

  double red_step = (double)red_diff/steps;
  double green_step = (double)green_diff/steps;
  double blue_step = (double)blue_diff/steps;

  if(step < steps) {
    struct Colour new;
    new.red = from.red + (int)(red_step*step);
    new.green = from.green + (int)(green_step*step);
    new.blue = from.blue + (int)(blue_step*step);
    //printf("r/g/b %d/%d/%d\n", new.red, new.green, new.blue);
    set_led1(new);
  } else {
    set_led1(to);
  }

}