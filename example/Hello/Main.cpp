////
//
//      Demo ST7789-based TFT display
//
////

#include <spi.h>
#include <gpio.h>
#include <st7789.h>

using namespace stm32f0;
using namespace gpio;
using namespace st7789;

typedef st7789_t<1, PA5, PA7, PC5, PC4> display;
typedef output_t<PC8> led_a;
typedef output_t<PC9> led_b;
typedef output_t<PB12> probe;

void loop();

int main()
{
    display::setup();
    led_a::setup();
    led_b::setup();
    probe::setup();

    for (;;)
        loop();
}

void loop()
{
    static uint8_t i = 0;

    if (!(i & 0xf))
    {
        led_a::toggle();
        if (led_a::read())
            led_b::toggle();
    }

    for (size_t k = 0; k < 1; ++k)
    {
        uint16_t pixel;

        switch (k)
        {
            case 0: pixel = from_rgb(127, 127, 127); break;
            case 1: pixel = from_rgb(255,   0,   0); break;
            case 2: pixel = from_rgb(  0, 255,   0); break;
            case 3: pixel = from_rgb(  0,   0, 255); break;
            case 4: pixel = from_rgb(255,   0, 255); break;
            case 5: pixel = from_rgb(  0, 255, 255); break;
            case 6: pixel = from_rgb(255, 255,   0); break;
            case 7: pixel = from_rgb(255, 255, 255); break;
        }

        pixel = swap_bytes(pixel);

        display::clear(swap_bytes(from_rgb(255, 0, 0)));
        probe::clear();

        //sys_tick::delay_ms(250);
    }

    display::set_col_addr(50, 59);
    display::set_row_addr(50, 59);
    display::start();

    for (uint16_t i = 0; i < 100; ++i)
    {
        probe::set();                                   // to measure frame rate
        display::write(0);
        probe::clear();
        sys_tick::delay_ms(1);
    }

    sys_tick::delay_ms(1000);
}

