#pragma once

#include <draw.h>

namespace graphics
{

template<typename DISPLAY>
class xy_plot_t
{
public:
    void setup(uint16_t x, uint16_t y, uint16_t w, uint16_t h, color::color_t bg)
    {
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;
        m_bg = bg;

        pen_t<DISPLAY> pen(m_bg);

        pen.fill_rectangle(m_x, m_y, m_w, m_h);
    }
 
    void viewport(float x0, float y0, float x1, float y1)
    {
        m_xb = (m_w - 1) / (x1 - x0);
        m_xa = m_x - m_xb * x0;
        m_yb = (m_h - 1) / (y0 - y1);
        m_ya = m_y - m_yb * y1;
        printf("%f, %f\n", m_xa, m_xb);
        printf("%f, %f\n", m_ya, m_yb);
    }

    void line_plot(const float *xs, const float *ys, uint16_t n, color::color_t c)
    {
        if (n < 2)
            return;

        pen_t<DISPLAY> pen(c);

        printf("%d, %d\n", fx(xs[0]), fy(ys[0]));
        pen.move_to(fx(xs[0]), fy(ys[0]));

        for (uint16_t i = 1; i < n; ++i)
        {
            printf("%d, %d\n", fx(xs[i]), fy(ys[i]));
            pen.line_to(fx(xs[i]), fy(ys[i]));
        }
    }

private:
    inline uint16_t fx(float x) { return m_xa + m_xb * x; }
    inline uint16_t fy(float y) { return m_ya + m_yb * y; }

    uint16_t  m_x, m_y, m_w, m_h;
    float m_xa, m_xb, m_ya, m_yb;
    color::color_t m_bg;
};

} // namespace graphics

