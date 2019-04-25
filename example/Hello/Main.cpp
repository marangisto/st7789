#include <spi.h>
#include <gpio.h>

using namespace stm32f0;
using namespace gpio;
//using namespace spi;
using namespace device;
typedef spi1_t _;

//typedef spi_t<1, PA5, PA7> display;
typedef output_t<PA2> latch;
typedef output_t<PC8> led_a;
typedef output_t<PC9> led_b;

void loop();

int main()
{
    //display::setup();
    latch::setup();
    led_a::setup();
    led_b::setup();

    GPIOA.MODER |= (0x2 << gpioa_t::MODER_MODER5) | (0x2 << gpioa_t::MODER_MODER7);
    GPIOA.AFRL = 0;

    RCC.APB2ENR |= BV(rcc_t::APB2ENR_SPI1EN);
    SPI1.CR1 |= BV(_::CR1_MSTR) | (0x1 << _::CR1_BR);
    SPI1.CR2 |= BV(_::CR2_SSOE);
    SPI1.CR2 |= 0x7 << _::CR2_DS;
    //SPI1.CR2 |= BV(_::CR2_FRF);     // TI mode (as opposed to Motorola)
    SPI1.CR1 |= BV(_::CR1_LSBFIRST);
    SPI1.CR1 |= BV(_::CR1_SPE);

    for (;;)
        loop();
}

void loop()
{
    static uint8_t i = 0;

    led_a::toggle();

    if (led_a::read())
        led_b::toggle();

    *reinterpret_cast<volatile uint8_t*>(&SPI1.DR) = i++;

    sys_tick::delay_ms(1);
    latch::set();
    sys_tick::delay_ms(1);
    latch::clear();

    //display::write(i++);

    sys_tick::delay_ms(250);
}

