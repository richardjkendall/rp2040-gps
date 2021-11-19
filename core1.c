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
const struct Colour dimwhite = { .red = 256, .green = 256, .blue = 256 };
const struct Colour blue = { .red = 0, .green = 0, .blue = 1024 };
const struct Colour purple = { .red = 1024, .green = 0, .blue = 1024 };
const struct Colour orange = { .red = 1024, .green = 256, .blue = 0 };
const struct Colour black = { .red = 0, .green = 0, .blue = 0 };
const struct Colour dimgreen = { .red = 0, .green = 512, .blue = 0 };
const struct Colour dimred = { .red = 512, .green = 0, .blue = 0 };
const struct Colour dimblue = { .red = 0, .green = 0, .blue = 256 };




const struct Pattern patterns[] = {
  /* 00 */ { .steps = 2, .stepdelay = 2, .fade = true, .led1colours = { red, green }, .led2colours = { green, red } },
  /* 01 */ { .steps = 3, .stepdelay = 1, .fade = true, .led1colours = { white, black, turq }, .led2colours = { turq, white, black } },
  /* 02 */ { .steps = 2, .stepdelay = 1, .fade = true, .led1colours = { blue, black }, .led2colours = { black, blue } },
  /* 03 */ { .steps = 6, .stepdelay = 2, .fade = true, .led1colours = { red, orange, yellow, green, turq, blue }, .led2colours = { red, orange, yellow, green, turq, blue } },
  /* 04 */ { .steps = 3, .stepdelay = 3, .fade = true, .led1colours = { white, black, black }, .led2colours = { white, black, black } },
  /* 05 */ { .steps = 3, .stepdelay = 3, .fade = true, .led1colours = { white, blue, black }, .led2colours = { white, blue, black } },
  /* 06 */ { .steps = 4, .stepdelay = 1, .fade = true, .led1colours = { blue, red, green, yellow }, .led2colours = { red, blue, yellow, green } },
  /* 07 */ { .steps = 2, .stepdelay = 1, .fade = true, .led1colours = { white, dimwhite }, .led2colours = { dimwhite, white } },
  /* 08 */ { .steps = 2, .stepdelay = 1, .fade = true, .led1colours = { blue, turq }, .led2colours = { blue, turq } },
  /* 09 */ { .steps = 3, .stepdelay = 4, .fade = true, .led1colours = { black, orange, black }, .led2colours = { black, orange, black } },
  /* 10 */ { .steps = 2, .stepdelay = 1, .fade = true, .led1colours = { red, blue }, .led2colours = { blue, red} },
  /* 11 */ { .steps = 2, .stepdelay = 1, .fade = false, .led1colours = { white, black }, .led2colours = { white, black } },
  /* 12 */ { .steps = 4, .stepdelay = 1, .fade = false, .led1colours = { blue, red, green, yellow }, .led2colours = { red, blue, yellow, green } },
  /* 13 */ { .steps = 2, .stepdelay = 1, .fade = false, .led1colours = { purple, orange }, .led2colours = { orange, purple } },
  /* 14 */ { .steps = 2, .stepdelay = 3, .fade = true, .led1colours = { red, black }, .led2colours = { red, black } },
  /* 15 */ { .steps = 3, .stepdelay = 3, .fade = true, .led1colours = { green, black, black }, .led2colours = { green, black, black } },
  /* 16 */ { .steps = 4, .stepdelay = 2, .fade = true, .led1colours = { white, turq, green, blue }, .led2colours = { blue, green, turq, white } },
  /* 17 */ { .steps = 3, .stepdelay = 3, .fade = true, .led1colours = { purple, black, black }, .led2colours = { purple, black, black } },
  /* 18 */ { .steps = 3, .stepdelay = 4, .fade = true, .led1colours = { purple, black, black }, .led2colours = { black, black, blue } },
  /* 19 */ { .steps = 2, .stepdelay = 2, .fade = false, .led1colours = { dimred, dimgreen }, .led2colours = { dimred, dimgreen } },
  /* 20 */ { .steps = 2, .stepdelay = 1, .fade = true, .led1colours = { dimwhite, orange }, .led2colours = { orange, dimwhite } },
  /* 21 */ { .steps = 7, .stepdelay = 2, .fade = false, .led1colours = { red, orange, yellow, green, turq, blue, purple }, .led2colours = { red, orange, yellow, green, turq, blue, purple } },
  /* 22 */ { .steps = 3, .stepdelay = 5, .fade = true, .led1colours = { dimwhite, black, black }, .led2colours = { dimwhite, black, black } },
  /* 23 */ { .steps = 3, .stepdelay = 5, .fade = true, .led1colours = { dimblue, black, black }, .led2colours = { dimblue, black, black } },
  /* 24 */ { .steps = 5, .stepdelay = 1, .fade = false, .led1colours = { yellow, black, yellow, black, black  }, .led2colours = { yellow, black, yellow, black, black } },
};

struct Colour from_led1;
struct Colour to_led1;
struct Colour from_led2;
struct Colour to_led2;

int step = 0;
int col = 0;
int pattern = 24;       // start with pattern 24 during startup
int pattern_step = 0;

int last_hour = -1;

// Core 1 interrupt Handler
void core1_interrupt_handler() {
  while (multicore_fifo_rvalid()){
    uint32_t raw = multicore_fifo_pop_blocking();
    //printf("Got encoded time in core1: %d\n", raw);   
    uint32_t sec = raw & 63;
    uint32_t min = (raw >> 6) & 63;
    uint32_t hr = (raw >> 12) & 31;
    printf("Recovered time %d:%d:%d\n", hr, min, sec);  

    // set pattern based on hour
    // check time is not all 0
    if(hr + min + sec != 0) {
      if(hr != last_hour) {
        printf("Setting pattern %d\n", hr);
        pattern = hr;
        pattern_step = 0;
        step = 0;
        last_hour = hr;
      }
    } 

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
    bool fade = patterns[pattern].fade;

    // handle led1
    fade_between(led1, from_led1, to_led1, maxstep, fade ? step : maxstep, MAX_PWM);

    // handle led2
    fade_between(led2, from_led2, to_led2, maxstep, fade ? step : maxstep, MAX_PWM);

    step++;
    sleep_ms(20);
    //tight_loop_contents();
  }
}