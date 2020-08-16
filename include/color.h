#pragma once

typedef uint32_t color_t;

static inline constexpr color_t to_color(uint8_t r, uint8_t g, uint8_t b)
{
    return (static_cast<uint32_t>(r) << 16)
         | (static_cast<uint32_t>(g) << 8)
         | static_cast<uint32_t>(b)
         ;
}

static inline void to_rgb(color_t c, uint8_t& r, uint8_t& g, uint8_t& b)
{
    r = c >> 16;
    g = (c >> 8) & 0xff;
    b = c & 0xff;
}

static inline color_t interpolate_color(color_t bg, color_t fg, uint16_t x)
{
    uint8_t rb, gb, bb, rf, gf, bf;

    to_rgb(bg, rb, gb, bb);
    to_rgb(fg, rf, gf, bf);

    return to_color
        ( (rb * (256 - x) + rf * x) >> 8
        , (gb * (256 - x) + gf * x) >> 8
        , (bb * (256 - x) + bf * x) >> 8
        );
}

static constexpr color_t grey(int brightness)
{
    return to_color(brightness, brightness, brightness);
}

namespace color
{
static const color_t alice_blue             = 0xf0f8ff;
static const color_t antique_white          = 0xfaebd7;
static const color_t aqua                   = 0x00ffff;
static const color_t aquamarine             = 0x7fffd4;
static const color_t azure                  = 0xf0ffff;
static const color_t beige                  = 0xf5f5dc;
static const color_t bisque                 = 0xffe4c4;
static const color_t black                  = 0x000000;
static const color_t blanched_almond        = 0xffebcd;
static const color_t blue                   = 0x0000ff;
static const color_t blue_violet            = 0x8a2be2;
static const color_t brown                  = 0xa52a2a;
static const color_t burlywood              = 0xdeb887;
static const color_t cadet_blue             = 0x5f9ea0;
static const color_t chartreuse             = 0x7fff00;
static const color_t chocolate              = 0xd2691e;
static const color_t coral                  = 0xff7f50;
static const color_t cornflower_blue        = 0x6495ed;
static const color_t cornsilk               = 0xfff8dc;
static const color_t crimson                = 0xdc143c;
static const color_t cyan                   = 0x00ffff;
static const color_t dark_blue              = 0x00008b;
static const color_t dark_cyan              = 0x008b8b;
static const color_t dark_goldenrod         = 0xb8860b;
static const color_t dark_gray              = 0xa9a9a9;
static const color_t dark_green             = 0x006400;
static const color_t dark_khaki             = 0xbdb76b;
static const color_t dark_magenta           = 0x8b008b;
static const color_t dark_olive_green       = 0x556b2f;
static const color_t dark_orange            = 0xff8c00;
static const color_t dark_orchid            = 0x9932cc;
static const color_t dark_red               = 0x8b0000;
static const color_t dark_salmon            = 0xe9967a;
static const color_t dark_sea_green         = 0x8fbc8f;
static const color_t dark_slate_blue        = 0x483d8b;
static const color_t dark_slate_gray        = 0x2f4f4f;
static const color_t dark_turquoise         = 0x00ced1;
static const color_t dark_violet            = 0x9400d3;
static const color_t deep_pink              = 0xff1493;
static const color_t deep_sky_blue          = 0x00bfff;
static const color_t dim_gray               = 0x696969;
static const color_t dodger_blue            = 0x1e90ff;
static const color_t firebrick              = 0xb22222;
static const color_t floral_white           = 0xfffaf0;
static const color_t forest_green           = 0x228b22;
static const color_t fuchsia                = 0xff00ff;
static const color_t gainsboro              = 0xdcdcdc;
static const color_t ghost_white            = 0xf8f8ff;
static const color_t gold                   = 0xffd700;
static const color_t goldenrod              = 0xdaa520;
static const color_t gray                   = 0xbebebe;
static const color_t web_gray               = 0x808080;
static const color_t green                  = 0x00ff00;
static const color_t web_green              = 0x008000;
static const color_t green_yellow           = 0xadff2f;
static const color_t honeydew               = 0xf0fff0;
static const color_t hot_pink               = 0xff69b4;
static const color_t indian_red             = 0xcd5c5c;
static const color_t indigo                 = 0x4b0082;
static const color_t ivory                  = 0xfffff0;
static const color_t khaki                  = 0xf0e68c;
static const color_t lavender               = 0xe6e6fa;
static const color_t lavender_blush         = 0xfff0f5;
static const color_t lawn_green             = 0x7cfc00;
static const color_t lemon_chiffon          = 0xfffacd;
static const color_t light_blue             = 0xadd8e6;
static const color_t light_coral            = 0xf08080;
static const color_t light_cyan             = 0xe0ffff;
static const color_t light_goldenrod        = 0xfafad2;
static const color_t light_gray             = 0xd3d3d3;
static const color_t light_green            = 0x90ee90;
static const color_t light_pink             = 0xffb6c1;
static const color_t light_salmon           = 0xffa07a;
static const color_t light_sea_green        = 0x20b2aa;
static const color_t light_sky_blue         = 0x87cefa;
static const color_t light_slate_gray       = 0x778899;
static const color_t light_steel_blue       = 0xb0c4de;
static const color_t light_yellow           = 0xffffe0;
static const color_t lime                   = 0x00ff00;
static const color_t lime_green             = 0x32cd32;
static const color_t linen                  = 0xfaf0e6;
static const color_t magenta                = 0xff00ff;
static const color_t maroon                 = 0xb03060;
static const color_t web_maroon             = 0x800000;
static const color_t medium_aquamarine      = 0x66cdaa;
static const color_t medium_blue            = 0x0000cd;
static const color_t medium_orchid          = 0xba55d3;
static const color_t medium_purple          = 0x9370db;
static const color_t medium_sea_green       = 0x3cb371;
static const color_t medium_slate_blue      = 0x7b68ee;
static const color_t medium_spring_green    = 0x00fa9a;
static const color_t medium_turquoise       = 0x48d1cc;
static const color_t medium_violet_red      = 0xc71585;
static const color_t midnight_blue          = 0x191970;
static const color_t mint_cream             = 0xf5fffa;
static const color_t misty_rose             = 0xffe4e1;
static const color_t moccasin               = 0xffe4b5;
static const color_t navajo_white           = 0xffdead;
static const color_t navy_blue              = 0x000080;
static const color_t old_lace               = 0xfdf5e6;
static const color_t olive                  = 0x808000;
static const color_t olive_drab             = 0x6b8e23;
static const color_t orange                 = 0xffa500;
static const color_t orange_red             = 0xff4500;
static const color_t orchid                 = 0xda70d6;
static const color_t pale_goldenrod         = 0xeee8aa;
static const color_t pale_green             = 0x98fb98;
static const color_t pale_turquoise         = 0xafeeee;
static const color_t pale_violet_red        = 0xdb7093;
static const color_t papaya_whip            = 0xffefd5;
static const color_t peach_puff             = 0xffdab9;
static const color_t peru                   = 0xcd853f;
static const color_t pink                   = 0xffc0cb;
static const color_t plum                   = 0xdda0dd;
static const color_t powder_blue            = 0xb0e0e6;
static const color_t purple                 = 0xa020f0;
static const color_t web_purple             = 0x800080;
static const color_t rebecca_purple         = 0x663399;
static const color_t red                    = 0xff0000;
static const color_t rosy_brown             = 0xbc8f8f;
static const color_t royal_blue             = 0x4169e1;
static const color_t saddle_brown           = 0x8b4513;
static const color_t salmon                 = 0xfa8072;
static const color_t sandy_brown            = 0xf4a460;
static const color_t sea_green              = 0x2e8b57;
static const color_t seashell               = 0xfff5ee;
static const color_t sienna                 = 0xa0522d;
static const color_t silver                 = 0xc0c0c0;
static const color_t sky_blue               = 0x87ceeb;
static const color_t slate_blue             = 0x6a5acd;
static const color_t slate_gray             = 0x708090;
static const color_t snow                   = 0xfffafa;
static const color_t spring_green           = 0x00ff7f;
static const color_t steel_blue             = 0x4682b4;
static const color_t tan                    = 0xd2b48c;
static const color_t teal                   = 0x008080;
static const color_t thistle                = 0xd8bfd8;
static const color_t tomato                 = 0xff6347;
static const color_t turquoise              = 0x40e0d0;
static const color_t violet                 = 0xee82ee;
static const color_t wheat                  = 0xf5deb3;
static const color_t white                  = 0xffffff;
static const color_t white_smoke            = 0xf5f5f5;
static const color_t yellow                 = 0xffff00;
static const color_t yellow_green           = 0x9acd32;
} // namespace color

