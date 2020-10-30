#pragma once

////
//
//      Text rendering
//
////

#include <algorithm>
#include <fontlib.h>
#include "color.h"

namespace text
{

using namespace color;

template<typename DISPLAY>
class text_renderer_t
{
public:
    text_renderer_t
        ( const fontlib::font_t& font
        , color_t fg = color::white
        , color_t bg = color::black
        , bool pad = false
        )
    {
        setup(&font, fg, bg, pad);
    }

    text_renderer_t(): m_font(0) {}

    void setup
        ( const fontlib::font_t *font
        , color_t fg = color::white
        , color_t bg = color::black
        , bool pad = false
        )
    {
        m_font = font;
        m_fg = fg;
        m_bg = bg;
        m_c = 0;
        m_r = m_font->line_spacing() - 1;
        m_scroll = 0;
        m_pad = pad;
    }

    void bounding_box(const char *s, uint16_t& w, uint16_t& h) const
    {
        char c;

        w = 0;

        while ((c = *s++) != 0)
        {
            const fontlib::glyph_t *g = fontlib::get_glyph(*m_font, c);

            w += g->width + std::max<int16_t>(0, g->offset_h);
        }

        h = 1 + m_font->max_y - m_font->min_y;
    }

    uint16_t text_height() const
    {
        return m_font->height;
    }

    uint16_t line_spacing() const
    {
        return m_font->line_spacing();
    }

    uint16_t width_of(char ch) const
    {
        const fontlib::glyph_t *g = fontlib::get_glyph(*m_font, ch);

        return g->offset_h + g->width;
    }

    uint16_t width_of(const char *s) const
    {
        uint16_t w = 0;

        while (*s)
            w += width_of(*s++);
        return w;
    }

    void set_pos(uint16_t x, uint16_t y)
    {
        m_c = x;
        m_r = y;
    }

    void get_pos(uint16_t& x, uint16_t& y)
    {
        x = m_c;
        y = m_r;
    }

    void clear_line()
    {
        uint16_t w = DISPLAY::width();
        uint16_t h = m_font->height, extra = 3;

        DISPLAY::set_col_addr(0, w - 1);
        DISPLAY::set_row_addr(m_r - (h - 1), m_r + extra);
        DISPLAY::start();

        uint16_t n = (h + extra) * w;

        for (uint16_t i = 0; i < n; ++i)
            DISPLAY::write(m_bg);
    }

    void write(char ch)
    {
        const fontlib::glyph_t *g = fontlib::get_glyph(*m_font, ch);

        if (!g)         // bail out if we don't have a glyph
            return;

        uint16_t w = g->width, h = g->height, n = w * h;
        int16_t c0 = m_c + std::max<int16_t>(0, g->offset_h);   // box start and end columns
        int16_t c1 = c0 + w;
        int16_t r0 = m_r + g->offset_v, r1 = r0 + h;            // box start and end rows

        if (c1 >= DISPLAY::width())
            return;                                             // truncate long lines

        DISPLAY::set_col_addr(c0, c1 - 1);
        DISPLAY::set_row_addr(r0, r1 - 1);
        DISPLAY::start();

        for (uint16_t i = 0; i < n; ++i)
            DISPLAY::write(interpolate_color(m_bg, m_fg, g->bitmap[i]));

        if (m_pad)
        {
            DISPLAY::set_row_addr(m_r + m_font->min_y, r0 - 1);
            DISPLAY::start();
            int16_t nr = g->offset_v - m_font->min_y;
            int16_t n = w * nr;
            for (uint16_t i = 0; i < n; ++i)
                DISPLAY::write(m_bg);

            DISPLAY::set_row_addr(r1, m_r + m_font->max_y);
            DISPLAY::start();
            int16_t mr = m_font->max_y - g->offset_v - h + 1;
            int16_t m = w * mr;
            for (uint16_t i = 0; i < m; ++i)
                DISPLAY::write(m_bg);

            DISPLAY::set_col_addr(m_c, m_c + g->offset_h - 1);
            DISPLAY::set_row_addr(m_r + m_font->min_y, m_r + m_font->max_y);
            DISPLAY::start();
            int16_t o = std::max<int16_t>(0, g->offset_h) * (1 + m_font->max_y - m_font->min_y);
            for (uint16_t i = 0; i < o; ++i)
                DISPLAY::write(m_bg);
        }

        m_c = c1;
    }

    void write(const char *s)
    {
        while (*s)
            write(*s++);
    }

    void writeln(const char *s)
    {
        write(s);
        m_c = 0;

        uint16_t ls = m_font->line_spacing();

        if (m_r + ls < DISPLAY::height())
            m_r += ls;
        else
        {
            if (m_scroll + ls < DISPLAY::height())
                m_scroll += ls;
            else
                m_scroll = 0;

            DISPLAY::scroll(m_scroll);
        }
    }

private:
    const fontlib::font_t   *m_font;
    color_t                 m_fg;
    color_t                 m_bg;
    uint16_t                m_c;
    uint16_t                m_r;
    uint16_t                m_scroll;
    bool                    m_pad;
};

// quick and dirty static version for use with printf<WRITER>()

template<typename DISPLAY>
class console_t
{
public:
    static void setup(const fontlib::font_t& font, color_t fg, color_t bg)
    {
        m_bg = bg;
        m_font = &font;
        m_txr.setup(m_font, fg, bg, true);
        DISPLAY::set_scroll_area(0, DISPLAY::height());
    };

    static void clear()
    {
        DISPLAY::clear(m_bg);
        set_pos(0, 0);
    }

    static void set_pos(uint16_t r, uint16_t c)
    {
        m_txr.set_pos(c * m_font->max_w, (r + 1) * m_font->line_spacing());
    }

    static void get_pos(uint16_t& r, uint16_t& c)
    {
        m_txr.get_pos(c, r);
        c = c / m_font->max_w;
        r = (r / m_font->line_spacing()) - 1;
    }

    static void write(const char *s)
    {
        while (*s)
            write(*s++);
    }

    static void write(char c)
    {
        if (c == '\n')
            m_txr.writeln("");
        else
            m_txr.write(c);
    }

private:
    static color_t                  m_bg;
    static const fontlib::font_t    *m_font;
    static text_renderer_t<DISPLAY> m_txr;
};

template<typename DISPLAY>
color_t console_t<DISPLAY>::m_bg;

template<typename DISPLAY>
const fontlib::font_t *console_t<DISPLAY>::m_font = 0;

template<typename DISPLAY>
text_renderer_t<DISPLAY> console_t<DISPLAY>::m_txr;

} // namespace text

