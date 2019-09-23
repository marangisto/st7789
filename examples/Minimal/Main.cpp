////
//
//      Demo ST7789-based TFT display
//
////

#include <spi.h>
#include <gpio.h>
#include <st7789.h>
#include <draw.h>

using namespace hal;
using namespace gpio;
using namespace st7789;
using namespace graphics;

#if defined(STM32F051)
typedef st7789_t<1, PA5, PA7, PC5, PC4> display;    // STM32F051-DISCOVERY
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_2;
#elif defined(STM32G431)
typedef st7789_t<1, PA5, PA7, PC7, PB6> display;    // NUCLEO-STM32G431
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_8;
#elif defined(STM32F103)
typedef st7789_t<1, PA5, PA7, PA4, PA3> display;    // BLUE-PILL
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_32;
#endif

int main()
{
    display::setup<display_spi_prescale>(dark_red);
    pen_t<display>(orange).circle(119, 119, 100);

    for (;;)
        ;
}

