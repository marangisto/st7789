#pragma once

#include <text.h>
#include <fontlib.h>
#include <draw.h>
#include <functional>

typedef uint16_t pixel_t;

typedef std::pair<pixel_t, pixel_t> dims_t;

struct rect_t
{
    rect_t(): x(0), y(0), w(0), h(0) {}

    pixel_t x, y, w, h;
};

struct ilayout
{
    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax) = 0;
    virtual void layout(pixel_t x, pixel_t y) = 0;
    virtual void render() = 0;
};

struct ifocus
{
    typedef color::color_t color_t;

    virtual void focus(color_t c) = 0;
    virtual void defocus() = 0;
    virtual void edit(int i) = 0;
};

template<typename T>
struct read_only
{
    static void edit(volatile T&, int) {}
};

template<typename DISPLAY, typename SHOW, typename EDIT = read_only<typename SHOW::T> >
class valuebox_t: public ilayout, public ifocus
{
public:
    typedef typename SHOW::T T;
    typedef color::color_t color_t;
    typedef fontlib::font_t font_t;

    void setup
        ( const font_t& font
        , color_t fg
        , color_t bg
        , const T& value = T()
        , const bool *quiet = 0
        )
    {
        m_font = &font;
        m_fg = fg;
        m_bg = m_frame = bg;
        m_value = value;
        m_quiet = quiet;
    }

    operator T() const { return m_value; }

    T operator=(const T& x)
    {
        m_value = x;
        if (m_quiet && !*m_quiet)
            render();
        return m_value;
    }

    // ilayout

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        m_rect.w = wmax;
        m_rect.h = std::min(hmax, m_font->line_spacing());
        return dims_t(m_rect.w, m_rect.h);
    }

    virtual void layout(pixel_t x, pixel_t y)
    {
        m_rect.x = x;
        m_rect.y = y;
    }

    virtual void render()
    {
        const char *s = SHOW::show(m_value);

        text::text_renderer_t<DISPLAY> tr(*m_font, m_fg, m_bg, true);
        graphics::pen_t<DISPLAY> pen(m_bg);
        uint16_t tw, th;

        tr.bounding_box(s, tw, th);

        uint16_t rpad = tw < m_rect.w ? (m_rect.w - tw) >> 1 : 0;
        uint16_t lpad = tw < m_rect.w ? m_rect.w - tw - rpad : 0;

        if (lpad)
            pen.fill_rectangle(m_rect.x, m_rect.y, lpad, m_rect.h);
        if (rpad)
            pen.fill_rectangle(m_rect.x + lpad + tw, m_rect.y, rpad, m_rect.h);

        uint16_t bpad = th < m_rect.h ? (m_rect.h - th) >> 1 : 0;
        uint16_t tpad = th < m_rect.h ? m_rect.h - th - bpad : 0;

        if (tpad)
            pen.fill_rectangle(m_rect.x + lpad, m_rect.y, tw, tpad);
        if (bpad)
            pen.fill_rectangle(m_rect.x + lpad, m_rect.y + th + tpad, tw, bpad);

        tr.set_pos(m_rect.x + lpad, m_rect.y + tpad - m_font->min_y);
        tr.write(s);

        if (m_frame != m_bg)
            graphics::pen_t<DISPLAY>(m_frame).rectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    }

    // ifocus

    virtual void focus(color_t c)
    {
        m_frame = c;
        render();
    }

    virtual void defocus()
    {
        m_frame = m_bg;
        render();
    }

    virtual void edit(int i)
    {
        EDIT::edit(m_value, i);
        render();
    }

private:
    volatile T      m_value;
    rect_t          m_rect;
    const font_t    *m_font;
    color_t         m_fg;
    color_t         m_bg;
    color_t         m_frame;
    const bool      *m_quiet;
};

template<typename DISPLAY>
class border_t: public ilayout
{
public:
    typedef color::color_t color_t;

    void setup(ilayout *child, color_t color = color::black, pixel_t thickness = 1)
    {
        m_child = child;
        m_color = color;
        m_thickness = thickness;
    }

    // ilayout

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        pixel_t dp = m_thickness << 1;  // both sides of border
        dims_t inner = m_child->constrain(wmin - dp, wmax - dp, hmin - dp, hmax - dp);
        m_rect.w = inner.first + dp;
        m_rect.h = inner.second + dp;
        return dims_t(m_rect.w, m_rect.h);
    }

    virtual void layout(pixel_t x, pixel_t y)
    {
        m_rect.x = x;
        m_rect.y = y;
        m_child->layout(x + m_thickness, y + m_thickness);
    }

    virtual void render()
    {
        graphics::pen_t<DISPLAY> pen(m_color);

        for (pixel_t i = 0; i < m_thickness; ++i)
            pen.rectangle(m_rect.x + i, m_rect.y + i, m_rect.w - (i << 1), m_rect.h - (i << 1));
        m_child->render();
    }

private:
    ilayout     *m_child;
    rect_t      m_rect;
    pixel_t     m_thickness;
    color_t     m_color;
};

template<typename DISPLAY>
class vertical_t: public ilayout
{
public:
    static constexpr uint8_t max_children = 8;

    void setup()
    {
        m_count = 0;
    }

    void append(ilayout *child)
    {
        if (m_count < max_children)
            m_child[m_count++] = child;
    }

    // ilayout

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        dims_t outer(0, 0);

        for (uint8_t i = 0; i < m_count; ++i)
        {
            dims_t inner = m_child[i]->constrain(wmin, wmax, hmin / m_count, hmax / m_count);
            outer.first = std::max(inner.first, outer.first);
            outer.second += m_size[i] = inner.second;
        }
        return outer;
    }

    virtual void layout(pixel_t x, pixel_t y)
    {
        for (uint8_t i = 0; i < m_count; ++i)
        {
            m_child[i]->layout(x, y);
            y += m_size[i];
        }
    }

    virtual void render()
    {
        for (uint8_t i = 0; i < m_count; ++i)
            m_child[i]->render();
    }

protected:
    ilayout     *m_child[max_children];
    pixel_t     m_size[max_children];
    uint8_t     m_count;
};

template<typename DISPLAY>
class horizontal_t: public vertical_t<DISPLAY>
{
public:
    // ilayout overrides

    typedef vertical_t<DISPLAY> base;

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        dims_t outer(0, 0);

        for (uint8_t i = 0; i < base::m_count; ++i)
        {
            dims_t inner = base::m_child[i]->constrain(wmin / base::m_count, wmax / base::m_count, hmin, hmax);
            outer.second = std::max(inner.second, outer.second);
            outer.first += base::m_size[i] = inner.first;
        }
        return outer;
    }

    virtual void layout(pixel_t x, pixel_t y)
    {
        for (uint8_t i = 0; i < base::m_count; ++i)
        {
            base::m_child[i]->layout(x, y);
            x += base::m_size[i];
        }
    }
};

