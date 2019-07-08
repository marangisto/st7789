#pragma once

#include <draw.h>
#include <algorithm>

namespace graphics
{

template<typename DISPLAY>
class xy_plot_t
{
public:
    typedef color::color_t color_t;

    void setup(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color_t gc, color_t bg)
    {
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;
        m_gc = gc;
        m_bg = bg;
    }
 
    void viewport(float x0, float y0, float x1, float y1)
    {
        m_xb = (m_w - 1) / (x1 - x0);
        m_xa = m_x - m_xb * x0;
        m_yb = (m_h - 1) / (y0 - y1);
        m_ya = m_y - m_yb * y1;
    }

    void clear()
    {
        pen_t<DISPLAY>(m_bg).fill_rectangle(m_x, m_y, m_w, m_h);
        pen_t<DISPLAY> pen(m_gc);

        pen.move_to(m_x, std::min<uint16_t>(std::max<uint16_t>(fy(0), m_y), m_y + m_h - 1));
        pen.rel_line_to(m_w - 1, 0);
        pen.move_to(std::min<uint16_t>(std::max<uint16_t>(fx(0), m_x), m_x + m_w - 1), m_y);
        pen.rel_line_to(0, m_h - 1);
    }

    void line_plot(const float *xs, const float *ys, uint16_t n, color_t c)
    {
        if (n < 2)
            return;

        pen_t<DISPLAY> pen(c);

        pen.move_to(fx(xs[0]), fy(ys[0]));

        for (uint16_t i = 1; i < n; ++i)
            pen.line_to(fx(xs[i]), fy(ys[i]));
    }

private:
    inline uint16_t fx(float x) { return m_xa + m_xb * x; }
    inline uint16_t fy(float y) { return m_ya + m_yb * y; }

    uint16_t  m_x, m_y, m_w, m_h;
    float m_xa, m_xb, m_ya, m_yb;
    color_t m_gc, m_bg;
};

} // namespace graphics

