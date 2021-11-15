#include "source.h"
#include "core1.h"
#include "pwm.h"

struct Colour colours[] = {
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

  // setup pwm
  setup_pwm();
  //struct Colour cur = purple;
  //led1 = purple;
  //set_led1(cur);

  while(true) {
    /*if(cur.blue != led1.blue || cur.green != led1.green || cur.red != led1.red) {
      set_led1(led1);
      cur = led1;
    }*/
    fade_between(from, to, 50, step);
    //printf("s: %d\n", step);
    step++;
    sleep_ms(20);
    //tight_loop_contents();
  }
}