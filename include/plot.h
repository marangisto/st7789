#pragma once

#include <draw.h>
#include <widget.h>
#include <algorithm>

/*
template<typename DISPLAY>
class border_t: public iwidget
{
public:
    typedef color::color_t color_t;

    void setup(iwidget *child, color_t color = color::black, pixel_t thickness = 1)
    {
        m_child = child;
        m_color = color;
        m_thickness = thickness;
    }

    // iwidget

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        pixel_t dp = m_thickness << 1;  // both sides of border
        dims_t inner = m_child->constrain(wmin - dp, wmax - dp, hmin - dp, hmax - dp);
        m_rect.w = inner.first + dp;
        m_rect.h = inner.second + dp;
        return dims_t(m_rect.w, m_rect.h);
    }

    virtual void place(pixel_t x, pixel_t y)
    {
        m_rect.x = x;
        m_rect.y = y;
        m_child->place(x + m_thickness, y + m_thickness);
    }

    virtual void render()
    {
        graphics::pen_t<DISPLAY> pen(m_color);

        for (pixel_t i = 0; i < m_thickness; ++i)
            pen.rectangle(m_rect.x + i, m_rect.y + i, m_rect.w - (i << 1), m_rect.h - (i << 1));
        m_child->render();
    }

private:
    iwidget     *m_child;
    rect_t      m_rect;
    pixel_t     m_thickness;
    color_t     m_color;
};
*/

template<typename DISPLAY>
class xy_plot_t: public iwidget
{
public:
    typedef color::color_t color_t;

    void setup(const theme_t& t)
    {
        m_gc = t.normal_fg;
        m_bg = t.normal_bg;
    }
 
    void viewport(float x0, float y0, float x1, float y1)
    {
        m_xb = (m_rect.w - 1) / (x1 - x0);
        m_xa = m_rect.x - m_xb * x0;
        m_yb = (m_rect.h - 1) / (y0 - y1);
        m_ya = m_rect.y - m_yb * y1;
    }

    void clear()
    {
        graphics::pen_t<DISPLAY>(m_bg).fill_rectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
        graphics::pen_t<DISPLAY> pen(m_gc);

        pen.move_to(m_rect.x, std::min<uint16_t>(std::max<uint16_t>(fy(0), m_rect.y), m_rect.y + m_rect.h - 1));
        pen.rel_line_to(m_rect.w - 1, 0);
        pen.move_to(std::min<uint16_t>(std::max<uint16_t>(fx(0), m_rect.x), m_rect.x + m_rect.w - 1), m_rect.y);
        pen.rel_line_to(0, m_rect.h - 1);
    }

    void line_plot(const float *xs, const float *ys, uint16_t n, color_t c)
    {
        if (n < 2)
            return;

        graphics::pen_t<DISPLAY> pen(c);

        pen.move_to(fx(xs[0]), fy(ys[0]));

        for (uint16_t i = 1; i < n; ++i)
            pen.line_to(fx(xs[i]), fy(ys[i]));
    }

    // iwidget

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        return dims_t(m_rect.w = wmax, m_rect.h = hmax);
    }

    virtual void place(pixel_t x, pixel_t y)
    {
        m_rect.x = x;
        m_rect.y = y;
    }

    virtual void render()
    {
        clear();
    }

private:
    inline uint16_t fx(float x) { return m_xa + m_xb * x; }
    inline uint16_t fy(float y) { return m_ya + m_yb * y; }

    rect_t          m_rect;
    float           m_xa, m_xb, m_ya, m_yb;
    color::color_t  m_gc, m_bg;
};

