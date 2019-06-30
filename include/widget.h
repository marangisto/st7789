#pragma once

#include <text.h>
#include <fontlib.h>
#include <draw.h>
#include <functional>

template<typename DISPLAY, typename T>
class widget_t
{
public:
    typedef color::color_t color_t;
    typedef fontlib::font_t font_t;
    typedef std::function<const char*(T)> show_t;

    widget_t
        ( const font_t& font
        , color_t fg
        , color_t bg
        , uint16_t x
        , uint16_t y
        , uint16_t w
        , uint16_t h
        , show_t show
        , bool initial_render = false
        )
    {
        setup(font, fg, bg, x, y, w, h, show, initial_render);
    }

    void setup
        ( const font_t& font
        , color_t fg
        , color_t bg
        , uint16_t x
        , uint16_t y
        , uint16_t w
        , uint16_t h
        , show_t show
        , bool initial_render = false
        )
    {
        m_font = &font;
        m_fg = fg;
        m_bg = bg;
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;
        m_show = show;

        if (initial_render)
            render();
    }

    operator T() const { return m_value; }

    T operator=(const T& x)
    {   
        m_value = x;
        render();
        return m_value;
    }

    void render() const
    {
        if (!m_show || !m_font)
            return;

        const char *s = m_show(m_value);

        text::text_renderer_t<DISPLAY> tr(*m_font, m_fg, m_bg, true);
        graphics::pen_t<DISPLAY> pen(m_bg);
        uint16_t tw, th;
 
        tr.bounding_box(s, tw, th);

        uint16_t rpad = tw < m_w ? (m_w - tw) >> 1 : 0;
        uint16_t lpad = tw < m_w ? m_w - tw - rpad : 0;

        if (lpad)
            pen.fill_rectangle(m_x, m_y, lpad, m_h);
        if (rpad)
            pen.fill_rectangle(m_x + lpad + tw, m_y, rpad, m_h);

        uint16_t bpad = th < m_h ? (m_h - th) >> 1 : 0;
        uint16_t tpad = th < m_h ? m_h - th - bpad : 0;

        if (tpad)
            pen.fill_rectangle(m_x + lpad, m_y, tw, tpad);
        if (bpad)
            pen.fill_rectangle(m_x + lpad, m_y + th + tpad, tw, bpad);

        graphics::pen_t<DISPLAY>(color::black).rectangle(m_x, m_y, m_w, m_h);    // FIXME: remove / optional?

        tr.set_pos(m_x + lpad, m_y + tpad - m_font->min_y);
        tr.write(s);
    }

private:
    T               m_value;
    uint16_t        m_x, m_y, m_w, m_h;
    show_t          m_show;
    const font_t    *m_font;
    color_t         m_fg;
    color_t         m_bg;
};

