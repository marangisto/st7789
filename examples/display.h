#pragma once

#include <spi.h>
#include <st7789.h>

#if defined(BEATS)
//typedef st7789_t<1, PA5, PA7, PC5, PC4> display;    // STM32F051-DISCOVERY
typedef st7789_t<1, PB3, PB5, PB4, PC12> display;   // Marangisto Beats
static const spi_clock_divider_t display_spi_prescale = fpclk_2;
#elif defined(WAVES)
typedef st7789_t<1, PA5, PA7, PB1, PB4> display;
static const spi_clock_divider_t display_spi_prescale = fpclk_8;
#elif defined(STM32G431)
typedef st7789_t<1, PA5, PA7, PC7, PB6> display;    // NUCLEO-STM32G431
static const spi_clock_divider_t display_spi_prescale = fpclk_8;
#elif defined(STM32F103)
typedef st7789_t<1, PA5, PA7, PA4, PA3> display;    // BLUE-PILL
static const spi_clock_divider_t display_spi_prescale = fpclk_4;
#endif

