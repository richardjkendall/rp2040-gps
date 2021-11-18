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

const struct Colour colours[] = {
  { .red = 1024, .green = 0, .blue = 0 },     // red
  { .red = 1024, .green = 1024, .blue = 0 },     // yellow
  { .red = 0, .green = 1024, .blue = 0 },     // green
  { .red = 0, .green = 1024, .blue = 1024 },  // turq
  { .red = 1024, .green = 1024, .blue = 1024 },     // white
  { .red = 0, .green = 0, .blue = 1024 },     // blue
  { .red = 1024, .green = 0, .blue = 1024 }  // purple
}; 

struct Colour from;
struct Colour to;

int step = 0;
int col = 0;

// Core 1 interrupt Handler
void core1_interrupt_handler() {
  while (multicore_fifo_rvalid()){
    uint32_t raw = multicore_fifo_pop_blocking();
    //printf("Got encoded time in core1: %d\n", raw);   
    uint32_t sec = raw & 63;
    uint32_t min = (raw >> 6) & 63;
    uint32_t hr = (raw >> 12) & 31;
    //printf("Recovered time %d:%d:%d\n", hr, min, sec);  
    if(sec % 2 == 0) {
      if(col == 6) {
        col = 0;
        from = colours[6];
        to = colours[0];
        step = 0;
      } else {
        col++;
        from = colours[col-1];
        to = colours[col];
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
    // handle led1
    fade_between(led1, from, to, 50, step, MAX_PWM);
    // handle led2
    fade_between(led2, to, from, 50, step, MAX_PWM);
    step++;
    sleep_ms(20);
    //tight_loop_contents();
  }
}