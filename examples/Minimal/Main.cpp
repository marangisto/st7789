////
//
//      Demo ST7789-based TFT display
//
////

#include <gpio.h>
#include <draw.h>
#include "../display.h"

int main()
{
    display::setup<display_spi_prescale>(color::dark_red);
    pen_t<display>(color::orange).circle(119, 119, 100);

    for (;;)
        ;
}

