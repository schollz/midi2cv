#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
//
#include "lib/midi.h"

// https://github.com/pidd-wbiz/midivel/blob/main/midivel.c#L82
#define UART_ID uart0
#define BAUD_RATE 31250
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define GPIO_FUNC_UART 2

// https://github.com/raspberrypi/pico-examples/blob/master/i2c/bus_scan/bus_scan.c
bool reserved_addr(uint8_t addr) {
  return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

#define I2C_SDA0_PIN 4
#define I2C_SCL0_PIN 5
#define I2C_SDA1_PIN 10
#define I2C_SCL1_PIN 11

void test_mcp4728() {
  printf("\nI2C Bus Scan\n");
  printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

  for (int addr = 0; addr < (1 << 7); ++addr) {
    if (addr % 16 == 0) {
      printf("%02x ", addr);
    }

    int ret;
    uint8_t rxdata;
    if (reserved_addr(addr))
      ret = PICO_ERROR_GENERIC;
    else
      ret = i2c_read_timeout_us(i2c0, addr, &rxdata, 1, false, 20000);

    printf(ret < 0 ? "." : "@");
    printf(addr % 16 == 15 ? "\n" : "  ");
  }
  printf("Done.\n");

  // setup array of bytes
  uint8_t data[3] = {0b01000110, 0b00000000, 0b10111000};
  printf("writing data\n");
  printf("wrote %d bytes\n", i2c_write_blocking(i2c0, 0x60, data, 3, false));
  printf("wrote %d bytes\n", i2c_write_blocking(i2c0, 0x60, data, 3, false));
  while (1) {
    // printf("Hello, world!\n");
    sleep_ms(1000);
  }
}

void set_voltage(uint8_t ch, float voltage) {
  // convert voltage float to 12-bit value
  uint16_t value = (uint16_t)round(voltage * 4095.0 / 5.0);
  // http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  // page 41
  // setup array of bytes
  uint8_t data[3];
  uint8_t ch2 = ch;
  if (ch2 > 3) ch2 = ch2 - 4;
  if (ch2 == 0) {
    data[0] = 0b01011110;
  } else if (ch == 1) {
    data[0] = 0b01011100;
  } else if (ch == 2) {
    data[0] = 0b01011010;
  } else if (ch == 3) {
    data[0] = 0b01011000;
  }
  data[1] = 0b00000000;
  // put the msb 4 bits of the 12-bit into the last 4 bits of the 2nd byte
  data[1] = data[1] | (value >> 8);
  // put the lsb 8 bits of the 12-bit into the 3rd byte
  data[2] = value & 0xff;
  printf("writing %2.3f volts %d data %x%x%x\n", voltage, value, data[0],
         data[1], data[2]);
  if (ch < 4) {
    printf("wrote %d bytes\n", i2c_write_blocking(i2c0, 0x60, data, 3, false));
  } else {
    printf("wrote %d bytes\n", i2c_write_blocking(i2c1, 0x60, data, 3, false));
  }
}

int main() {
  stdio_init_all();

  sleep_ms(1000);

  // This example will use i2c0 on the default SDA and SCL (pins 6, 7 on a Pico)
  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(I2C_SDA0_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL0_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA0_PIN);
  gpio_pull_up(I2C_SCL0_PIN);
  i2c_init(i2c1, 100 * 1000);
  gpio_set_function(I2C_SDA1_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL1_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA1_PIN);
  gpio_pull_up(I2C_SCL1_PIN);

  sleep_ms(1000);

  // set UART speed.
  uart_init(UART_ID, BAUD_RATE);

  // set UART Rx pin
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // enable Rx fifos on UART
  uart_set_fifo_enabled(UART_ID, true);

  // disable cr/lf conversion on Tx
  uart_set_translate_crlf(UART_ID, false);

  // setup midi parser
  midi_parser mp;

  // endless loop
  while (true) {
    unsigned char midichar = uart_getc(UART_ID);
    midi_message msg = midi_read(&mp, midichar);
    switch (msg.status & 0xf0) {
      case MIDI_NOTE_ON:
        if (msg.data[1])
          printf("note on: %d\n", msg.data[0]);
        else
          printf("note off: %d\n", msg.data[0]);
        break;
      case MIDI_NOTE_OFF:
        printf("note off: %d\n", msg.data[0]);
        break;
      case MIDI_CONTROL_CHANGE:
        printf("control change: %d %d\n", msg.data[0], msg.data[1]);
        if (msg.data[0] == 104) {
          set_voltage(4, msg.data[1] / 127.0 * 5.0);
        }
        break;
    }
  }
  return 0;
}
