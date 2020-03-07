#pragma once

#include <spi.h>
#include <st7789.h>

using namespace hal;
using namespace spi;
using namespace st7789;

#if defined(STM32F051)
//typedef st7789_t<1, PA5, PA7, PC5, PC4> display;    // STM32F051-DISCOVERY
typedef st7789_t<1, PB3, PB5, PB4, PC12> display;   // Marangisto Beats
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_2;
#elif defined(STM32G431)
typedef st7789_t<1, PA5, PA7, PC7, PB6> display;    // NUCLEO-STM32G431
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_8;
#elif defined(STM32F103)
typedef st7789_t<1, PA5, PA7, PA4, PA3> display;    // BLUE-PILL
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_4;
#endif

