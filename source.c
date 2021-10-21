#include "source.h"

#define UART_ID uart0
#define BAUD_RATE 9600

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        // Can we send it back?
        //if (uart_is_writable(UART_ID)) {
            //uart_putc(UART_ID, ch);
            putc(ch, stdout);
        //}
        //chars_rxed++;
    }
}

int main() {
  stdio_init_all();

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

/*
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }*/

    return 0;
}