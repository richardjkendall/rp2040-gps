#include "source.h"
#include "core1.h"
#include "pwm.h"

#define MAX_PWM 1024

#define LED1_RED 10
#define LED1_GRN 11
#define LED1_BLU 13
#define LED2_RED 19
#define LED2_GRN 17
#define LED2_BLU 15

const struct Led led1 = { .redpin = LED1_RED, .greenpin = LED1_GRN, .bluepin = LED1_BLU };
const struct Led led2 = { .redpin = LED2_RED, .greenpin = LED2_GRN, .bluepin = LED2_BLU };

const struct Colour red = { .red = 1024, .green = 0, .blue = 0 };
const struct Colour yellow = { .red = 1024, .green = 1024, .blue = 0 };
const struct Colour green = { .red = 0, .green = 1024, .blue = 0 };
const struct Colour turq = { .red = 0, .green = 1024, .blue = 1024 };
const struct Colour white = { .red = 1024, .green = 1024, .blue = 1024 };
const struct Colour blue = { .red = 0, .green = 0, .blue = 1024 };
const struct Colour purple = { .red = 1024, .green = 0, .blue = 1024 };
const struct Colour orange = { .red = 1024, .green = 512, .blue = 0 };
const struct Colour black = { .red = 0, .green = 0, .blue = 0 };

const struct Pattern patterns[] = {
  { .steps = 2, .stepdelay = 2, .led1colours = { red, green }, .led2colours = { green, red } }
};

struct Colour from_led1;
struct Colour to_led1;
struct Colour from_led2;
struct Colour to_led2;

int step = 0;
int col = 0;
int pattern = 0;
int pattern_step = 0;

// Core 1 interrupt Handler
void core1_interrupt_handler() {
  while (multicore_fifo_rvalid()){
    uint32_t raw = multicore_fifo_pop_blocking();
    //printf("Got encoded time in core1: %d\n", raw);   
    uint32_t sec = raw & 63;
    uint32_t min = (raw >> 6) & 63;
    uint32_t hr = (raw >> 12) & 31;
    printf("Recovered time %d:%d:%d\n", hr, min, sec);  

    if (sec % patterns[pattern].stepdelay == 0) {
      if(pattern_step == patterns[pattern].steps) {
        // need to reset the step count to 0
        to_led1 = patterns[pattern].led1colours[0];
        from_led1 = patterns[pattern].led1colours[patterns[pattern].steps - 1];

        to_led2 = patterns[pattern].led2colours[0];
        from_led2 = patterns[pattern].led2colours[patterns[pattern].steps - 1];

        pattern_step = 1;
        step = 0;
      } else {
        to_led1 = patterns[pattern].led1colours[pattern_step];
        from_led1 = patterns[pattern].led1colours[pattern_step - 1];

        to_led2 = patterns[pattern].led2colours[pattern_step];
        from_led2 = patterns[pattern].led2colours[pattern_step - 1];

        pattern_step++;
        step = 0;
      }
    }
    
  }
  multicore_fifo_clear_irq(); // Clear interrupt
}

// Core 1 Main Code
void core1_entry() {
  multicore_fifo_clear_irq();
  irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
  irq_set_enabled(SIO_IRQ_PROC1, true);

  // setup pwm for led 1
  setup_pwm(led1, MAX_PWM);

  // setup pwm for led 2
  setup_pwm(led2, MAX_PWM);

  while(true) {
    // get max step count
    int maxstep = patterns[pattern].stepdelay * 50;

    // handle led1
    fade_between(led1, from_led1, to_led1, maxstep, step, MAX_PWM);

    // handle led2
    fade_between(led2, from_led2, to_led2, maxstep, step, MAX_PWM);

    step++;
    sleep_ms(20);
    //tight_loop_contents();
  }
}