#include <stdio.h>

#include <nuclei_sdk_soc.h>
#include <gd32vf103.h>


int main(void) {
  // Initialize standard I/O (configures UART for printf)
  // board_init();     
  printf("Longan Nano initialized via Nuclei SDK!\r\n");
  printf("Hello, World from Nuclei RISC-V!\n");

  while (1) {
    // Toggle an LED or add your logic here
    delay_1ms(500);
  }
  return 0;
}

// https://github.com/tuupola/hagl

/* #include <nuclei_sdk_hal.h> */
/* #include <hagl_hal.h> */
/* #include <hagl.h> */
/* #include <font6x9.h> */
/* void main() */
/* { */
/*     color_t red = hagl_color(255, 0, 0); */
/*     color_t green = hagl_color(0, 255, 0); */
/*     color_t blue = hagl_color(0, 0, 255); */
/*  */
/*     hagl_init(); */
/*     hagl_clear_screen(); */
/*  */
/*     while (1) { */
/*         hagl_put_text(L"Hello world!", 48, 32, red, font6x9); */
/*         delay_1ms(100); */
/*  */
/*         hagl_put_text(L"Hello world!", 48, 32, green, font6x9); */
/*         delay_1ms(100); */
/*  */
/*         hagl_put_text(L"Hello world!", 48, 32, blue, font6x9); */
/*         delay_1ms(100); */
/*     }; */
/* } */
