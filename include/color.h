#pragma once

namespace color
{

typedef uint32_t color_t;

static inline constexpr color_t to_color(uint8_t r, uint8_t g, uint8_t b)
{
    return (static_cast<uint32_t>(r) << 16)
         | (static_cast<uint32_t>(g) << 8)
         | static_cast<uint32_t>(b)
         ;
}

static inline void to_rgb(color_t c, uint16_t& r, uint16_t& g, uint16_t& b)
{
    r = c >> 16;
    g = (c >> 8) & 0xff;
    b = c & 0xff;
}

// swap byte order for 16-bit color scheme
__attribute__((always_inline))
static inline uint16_t swap_bytes(uint16_t x)
{
    uint16_t l = x & 0xff;

    return (l << 8) | (x >> 8);
}

// rgb color using conventional 0..255 ranges
__attribute__((always_inline))
static inline uint16_t st7899_from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

// convert color_t to st7780 color encoding
__attribute__((always_inline))
static inline uint16_t color2st7789(color_t c)
{
    return swap_bytes(st7899_from_rgb((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff));
}

static inline color_t interpolate_color(color_t bg, color_t fg, uint16_t x)
{
    uint16_t rb, gb, bb, rf, gf, bf;

    to_rgb(bg, rb, gb, bb);
    to_rgb(fg, rf, gf, bf);

    return to_color
        ( (rb * (256 - x) + rf * x) >> 8
        , (gb * (256 - x) + gf * x) >> 8
        , (bb * (256 - x) + bf * x) >> 8
        );
}

static const color_t black = to_color(0, 0, 0);
static const color_t red = to_color(255, 0, 0);
static const color_t green = to_color(0, 255, 0);
static const color_t blue = to_color(0, 0, 255);
static const color_t magenta = to_color(255, 0, 255);
static const color_t cyan = to_color(0, 255, 255);
static const color_t yellow = to_color(255, 255, 0);
static const color_t white = to_color(255, 255, 255);

} // color

