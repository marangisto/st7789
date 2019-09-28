////
//
//      Demo ST7789-based TFT display
//
////

#include "../display.h"
#include <draw.h>

using namespace graphics;

int main()
{
    display::setup<display_spi_prescale>(dark_red);
    pen_t<display>(orange).circle(119, 119, 100);

    for (;;)
        ;
}

