#include "source.h"
#include "pwm.h"

// led 1
// r 10, g 11, b 13

void set_slice(int pin, int limit) {
  uint slice_num = pwm_gpio_to_slice_num(pin);
  pwm_set_wrap(slice_num, limit);
  pwm_set_gpio_level(pin, limit);
  pwm_set_enabled(slice_num, true);
}
 
void setup_pwm(struct Led led, int limit) {
  gpio_set_function(led.redpin, GPIO_FUNC_PWM);
  gpio_set_function(led.greenpin, GPIO_FUNC_PWM);
  gpio_set_function(led.bluepin, GPIO_FUNC_PWM);

  /*uint slice_num1 = pwm_gpio_to_slice_num(10);
  pwm_set_wrap(slice_num1, 1024);
  uint slice_num2 = pwm_gpio_to_slice_num(13);
  pwm_set_wrap(slice_num2, 1024);*/

  /*
  pwm_set_gpio_level(10, 1024);   // red
  pwm_set_gpio_level(11, 1024);   // green
  pwm_set_gpio_level(13, 0);      // blue    // 0= full brightness
  */

  set_slice(led.redpin, limit);
  set_slice(led.greenpin, limit);
  set_slice(led.bluepin, limit);
 
  //pwm_set_enabled(slice_num1, true);
  //pwm_set_enabled(slice_num2, true);
} 

void set_led(struct Colour col, struct Led led, int limit) {
  pwm_set_gpio_level(led.redpin, limit - col.red);
  pwm_set_gpio_level(led.greenpin, limit - col.green);
  pwm_set_gpio_level(led.bluepin, limit - col.blue);
}

void fade_between(struct Led led, struct Colour from, struct Colour to, int steps, int step, int limit) {
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
    set_led(new, led, limit);
  } else {
    set_led(to, led, limit);
  }

}