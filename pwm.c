/*
 * File:   pwm.c
 * Author: Richard Kendall
 * Version: 1
 * -----------------------
 * Methods to manage PWM for RGB LEDs
 */

#include "source.h"
#include "pwm.h"

/*
 * Function: set_slice
 * -------------------
 * Sets up PWM on a particular pin with a particular limit
 * 
 * pin:   the numeric identifier of the pin where PWM should be enabled
 * limit: the counter wrap value (in other words, the value where the pin is constantly on)
 *
 * returns: nothing
 */
void set_slice(int pin, int limit) {
  uint slice_num = pwm_gpio_to_slice_num(pin);
  pwm_set_wrap(slice_num, limit);
  pwm_set_gpio_level(pin, limit);
  pwm_set_enabled(slice_num, true);
}

/*
 * Function: setup_pwm
 * -------------------
 * Sets up a given set of pins for an RGB LED
 * 
 * led:   a struct with the GPIO pins for the red, green and blue LED pins
 * limit: the counter wrap value (in other words, the value where the led is constantly on)
 * 
 * returns: nothing
 */
void setup_pwm(struct Led led, int limit) {
  gpio_set_function(led.redpin, GPIO_FUNC_PWM);
  gpio_set_function(led.greenpin, GPIO_FUNC_PWM);
  gpio_set_function(led.bluepin, GPIO_FUNC_PWM);

  set_slice(led.redpin, limit);
  set_slice(led.greenpin, limit);
  set_slice(led.bluepin, limit);
} 

/*
 * Function: setup_pwm
 * -------------------
 * Set a given RGB led to a given colour
 * 
 * col: a struct with the RGB colour to set the LED to
 * led: a struct with the GPIO pins for the red, green and blue LED pins
 * 
 * returns: nothing
 */
void set_led(struct Colour col, struct Led led, int limit) {
  pwm_set_gpio_level(led.redpin, limit - col.red);
  pwm_set_gpio_level(led.greenpin, limit - col.green);
  pwm_set_gpio_level(led.bluepin, limit - col.blue);
}

/*
 * Function: fade_between
 * ----------------------
 * Set an RGB LED to a calculated colour value between a start and end point
 * Value is determined by dividing the colour delta by steps and then using the current step value to
 * determine the change amount
 * 
 * led:   a struct with the GPIO pins for the red, green and blue LED pins
 * from:  the start colour
 * to:    the end colour
 * steps: the number of steps in the fading
 * step:  the current step number
 * limit: the max colour value used for PWM
 * 
 * returns: nothing
 */
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
    set_led(new, led, limit);
  } else {
    set_led(to, led, limit);
  }

}