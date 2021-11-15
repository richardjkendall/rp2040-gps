#include "source.h"
#include "gps.h"
#include "core1.h"

// UART details
#define UART_ID uart0
#define BAUD_RATE 9600
// pins that GPS module UART is connected to
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// masks for time maths
const uint32_t MINSEC_MASK = 63;
const uint32_t HR_MASK = 31;

// UART buffer
char buffer[100] = {0};
int pos = 0;

/*
hour 0 - 23
min  0 - 59
sec  0 - 59

om parsing 8:31:6
Encoded time as uint32_t 34758

1000 011111 000110

*/

void process_time() {
  char *field[20];

  struct GpsTime time;
  int result = nmea_sentence_to_time(buffer, &time);
  if(result) {
    //printf("Time from parsing %d:%d:%d\r\n", time.hour, time.minute, time.second);
    uint32_t data = 0;
    data = (time.hour & HR_MASK) << 12;
    data |= (time.minute & MINSEC_MASK) << 6;
    data |= (time.second & MINSEC_MASK);
    printf("Encoded time as uint32_t %d\n", data);
    multicore_fifo_push_blocking(data);
  } 
}

// RX interrupt handler
void on_uart_rx() {
  while (uart_is_readable(UART_ID)) {
    uint8_t ch = uart_getc(UART_ID);
    if(ch == '\n') {
      // we got a newline, so stop here and process it
      buffer[pos] = '\0';
      pos = 0;
      process_time();
    } else {
      if(pos > 99) {
        pos = 0;
      }
      buffer[pos] = ch;
      pos++;
    }
  }
}

int main() {
  stdio_init_all();

  // init core 1
  multicore_launch_core1(core1_entry);

  // setup UART
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // IRQ setup
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);
  uart_set_irq_enables(UART_ID, true, false);

  // main loop
  while(true) {
    tight_loop_contents();
  }

  return 0;
}