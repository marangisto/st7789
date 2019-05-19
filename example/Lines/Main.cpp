////
//
//      Line graphics demo
//
////

#include <spi.h>
#include <gpio.h>
#include <st7789.h>
#include <draw.h>

using namespace stm32f0;
using namespace gpio;
using namespace st7789;

typedef st7789_t<1, PA5, PA7, PC5, PC4> display;

static inline int16_t abs(int16_t x) { return x < 0 ? -x : x; }

template<typename DISPLAY>
struct pen_t
{
public:
    pen_t(color_t c): m_c(c), m_x(0), m_y(0) {}

    inline void move_to(int16_t x, int16_t y)
    {
        m_x = x;
        m_y = y;
    }

    inline void line_to(int16_t x1, int16_t y1)
    {
        int16_t x0 = m_x, y0 = m_y;
        int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int16_t dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int16_t e = (dx > dy ? dx : -dy) / 2;

        for (;;)
        {
            DISPLAY::set_pixel(x0, y0, m_c);
            if (x0 == x1 && y0 == y1)
                break;

            int16_t e2 = e;

            if (e2 > -dx)
            {
                e -= dy;
                x0 += sx;
            }

            if (e2 < dy)
            {
                e += dx;
                y0 += sy;
            }
        }

        m_x = x1;
        m_y = y1;
    }

    inline void hline_to(int16_t x1)
    {
        if (x1 >= m_x) 
            DISPLAY::set_pixels_h(m_x, m_y, x1 - m_x + 1, m_c);
        else
            DISPLAY::set_pixels_h(x1, m_y, m_x - x1 + 1, m_c);
        m_x = x1;
    }

    inline void vline_to(int16_t y1)
    {
        if (y1 >= m_y) 
            DISPLAY::set_pixels_v(m_x, m_y, y1 - m_y + 1, m_c);
        else
            DISPLAY::set_pixels_v(m_x, y1, m_y - y1 + 1, m_c);
        m_y = y1;
    }

    inline void rel_move_to(int16_t x, int16_t y)
    {
        m_x += x;
        m_y += y;
    }

    inline void rel_line_to(int16_t x1, int16_t y1)
    {
        line_to(m_x + x1, m_y + y1);
    }

    inline void rectangle(int16_t x, int16_t y, int16_t w, int16_t h)
    {
        move_to(x, y);
        hline_to(x + w);
        vline_to(y + h);
        hline_to(x);
        vline_to(y);
    }

    inline void circle(int16_t xc, int16_t yc, int16_t r)
    {
        int16_t y = r, p = 3 - (r << 1);

        circle_pixel(xc, yc, 0, y);

        for (int16_t x = 1; x < y; ++x)
        {
            if (p < 0)
                p += 4 * x + 6;
            else
            {
                --y;
                p += 4 * (x - y) + 10;
            }
            circle_pixel(xc, yc, x, y);
        }
    }

private:
    inline void circle_pixel(int16_t xc, int16_t yc, int16_t x, int16_t y)
    {
        DISPLAY::set_pixel(xc + x, yc + y, m_c);
        DISPLAY::set_pixel(xc + x, yc - y, m_c);
        DISPLAY::set_pixel(xc - x, yc + y, m_c);
        DISPLAY::set_pixel(xc - x, yc - y, m_c);
        DISPLAY::set_pixel(xc + y, yc + x, m_c);
        DISPLAY::set_pixel(xc + y, yc - x, m_c);
        DISPLAY::set_pixel(xc - y, yc + x, m_c);
        DISPLAY::set_pixel(xc - y, yc - x, m_c);
    }

    color_t     m_c;
    int16_t     m_x, m_y;
};

static void squares(color_t bg, color_t fg)
{
    display::clear(bg);
    pen_t<display> pen(fg);

    int16_t x1 = display::width() - 1;
    int16_t y1 = display::height() - 1;
    int16_t xc = x1 >> 1, yc = y1 >> 1;

    for (int i = 0; i <= y1 >> 1; i += 8)
        pen.rectangle(xc - i, yc - i, i << 1, i << 1);
}

static void circles(color_t bg, color_t fg)
{
    display::clear(bg);
    pen_t<display> pen(fg);

    int16_t x1 = display::width() - 1;
    int16_t y1 = display::height() - 1;
    int16_t xc = x1 >> 1, yc = y1 >> 1;

    for (int r = 0; r <= y1 >> 1; r += 8)
        pen.circle(xc, yc, r);
}

int main()
{
    display::setup();

    for (;;)
    {
        squares(color::black, color::white);
        sys_tick::delay_ms(1000);
        circles(color::white, color::blue);
        sys_tick::delay_ms(1000);
        squares(color::blue, color::yellow);
        sys_tick::delay_ms(1000);
        circles(color::yellow, color::black);
        sys_tick::delay_ms(1000);
    }

    for (;;);   // never return
}

