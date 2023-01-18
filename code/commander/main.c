#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include <tusb.h>
#include "pico/multicore.h"
#include "arducam/arducam.h"
#include "lib/st7735.h"
#include "lib/fonts.h"
#include "string.h"
#include "hardware/uart.h"
#include "math.h"
#include "hardware/pio.h"
#include "hardware/uart.h"
// #include "uart_rx.pio.h"

uint8_t image_buf[324*324];
uint8_t displayBuf[80*160*2];
uint8_t header[2] = {0x55,0xAA};

int frames=0, freq=250000, t0, t1;

void core1_entry() {
        int frame = 0;

	gpio_init(PIN_LED);
	gpio_set_dir(PIN_LED, GPIO_OUT);

	ST7735_Init();
	ST7735_DrawImage(0, 0, 80, 160, arducam_logo);

	struct arducam_config config;
	config.sccb = i2c0;
	config.sccb_mode = I2C_MODE_16_8;
	config.sensor_address = 0x24;
	config.pin_sioc = PIN_CAM_SIOC;
	config.pin_siod = PIN_CAM_SIOD;
	config.pin_resetb = PIN_CAM_RESETB;
	config.pin_xclk = PIN_CAM_XCLK;
	config.pin_vsync = PIN_CAM_VSYNC;
	config.pin_y2_pio_base = PIN_CAM_Y2_PIO_BASE;

	config.pio = pio0;
	config.pio_sm = 0;

	config.dma_channel = 0;
	config.image_buf = image_buf;
	config.image_buf_size = sizeof(image_buf);

	arducam_init(&config);
	while (true) {
	  gpio_put(PIN_LED, !gpio_get(PIN_LED));
	  arducam_capture_frame(&config);
          ++frame;
          if (frame==3)  t0=time_us_32();
          else if (frame-frames==3) {
            t1=time_us_32();
            printf("%.2ffps\n", 1000000.0*frames/(t1-t0));
          }

	  uint16_t index = 0;
	  for (int y = 0; y < 160; y++) {
	    for (int x = 0; x < 80; x++) {
              uint8_t c = image_buf[(2+320-2*y)*324+(2+40+2*x)];
              uint16_t imageRGB   = ST7735_COLOR565(c, c, c);
              displayBuf[index++] = (uint8_t)(imageRGB >> 8) & 0xFF;
              displayBuf[index++] = (uint8_t)(imageRGB)&0xFF;
            }
	  }
    // printf(" The length of int array is : %d ", sizeof(displayBuf) / sizeof(displayBuf[0]));
    int sum = 0;
    int i;
    int decimalNumber;

    #define UART_ID uart0
    #define BAUD_RATE 115200

    // We are using pins 0 and 1, but see the GPIO function select table in the
    // datasheet for information on which other pins can be used.
    #define UART_TX_PIN 0
    #define UART_RX_PIN 1

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    // Set up the state machine we're going to use to receive them.
    // PIO pio = pio0;
    // uint sm = 0;
    // uint offset = pio_add_program(pio, &uart_rx_program);
    // uart_rx_program_init(pio, sm, offset, PIO_RX_PIN, SERIAL_BAUD);


    for (i = 0; i < 25600; ++i) {
      sum += displayBuf[i];
    }
    int avg;
    avg = sum / 25600;
    // sscanf(displayBuf[1],"%x",&decimalNumber);
    printf("The average pixel value is %d\n",avg);

    if (avg < 100) {
      printf("I am turing right!\n");
      uart_rx_program_putc(UART_ID, 0x72);
    } else {
      printf("I am going straight!\n");
      uart_rx_program_putc(UART_ID, 0x66);
    }

    // Stop moving forward
    if (avg > 150) {
      printf("Stop\n");
      // uart_rx_program_putc(UART_ID, 0xff);
    }

	  ST7735_DrawImage(0, 0, 80, 160, displayBuf);
	}
}

#include "hardware/vreg.h"

#define L 8
unsigned char str[L+1];

unsigned char *readLine() {
  int u; unsigned char *p;
  for(p=str, u=getchar_timeout_us(100000); u!='\r' && p-str<L; u=getchar_timeout_us(10000))  if (u>=0)  putchar(*p++=u);
  *p = 0;  putchar('\n'); return str;
}

int main() {
  int loops=20;
  stdio_init_all();
  while (!tud_cdc_connected()) { sleep_ms(100); if (--loops==0) break;  }

  printf("tud_cdc_connected(%d)\n", tud_cdc_connected()?1:0);

  if (tud_cdc_connected()) {
    getchar_timeout_us(100000);     // without 1st atoi(readLine()) returns 0
 
    printf("clock speed [KHz]: ");            freq=atoi(readLine());
    printf("fps frames: ");                   frames=atoi(readLine());
  }

  vreg_set_voltage(VREG_VOLTAGE_1_30);
  sleep_ms(1000);
  set_sys_clock_khz(freq, true);

  multicore_launch_core1(core1_entry);

  while (1)
    tight_loop_contents();
}
