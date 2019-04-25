#include <spi.h>
#include <gpio.h>

using namespace stm32f0;
using namespace gpio;
using namespace spi;

typedef spi_t<1, PA5, PA7> display;
typedef output_t<PC8> led_a;
typedef output_t<PC9> led_b;

void loop();

int main()
{
    display::setup();
    led_a::setup();
    led_b::setup();

    for (;;)
        loop();
}

void loop()
{
    led_a::toggle();

    if (led_a::read())
        led_b::toggle();

    sys_tick::delay_ms(250);
}

