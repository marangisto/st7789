#pragma once

////
//
//      Text rendering
//
////

#include <fontlib.h>

namespace stm32f0
{

namespace st7789
{

template<typename DISPLAY>
class text_renderer_t
{
public:
    text_renderer_t(const fontlib::font_t& font, color_t fg = color::white, color_t bg = color::black)
        : m_font(font), m_fg(fg), m_bg(bg), m_c(0), m_r(m_font.height - 1), m_scroll(0) {}

    uint16_t text_height() const
    {
        return m_font.height;
    }

    uint16_t width_of(char ch) const
    {
        const fontlib::glyph_t *g = fontlib::get_glyph(m_font, ch);

        return g->offset_h + g->width;
    }

    uint16_t width_of(const char *s) const
    {
        uint16_t w = 0;

        while (*s)
            w += width_of(*s++);
        return w;
    }

    void set_pos(uint16_t c, uint16_t r)
    {
        m_c = c;
        m_r = r;
    }

    void clear_line()
    {
        uint16_t w = DISPLAY::width();
        uint16_t h = m_font.height, extra = 3;

        DISPLAY::set_col_addr(0, w - 1);
        DISPLAY::set_row_addr(m_r - (h - 1), m_r + extra);
        DISPLAY::start();

        uint16_t n = (h + extra) * w;

        for (uint16_t i = 0; i < n; ++i)
            DISPLAY::write(color2st7789(m_bg));
    }

    void write(char ch)
    {
        const fontlib::glyph_t *g = fontlib::get_glyph(m_font, ch);

        if (!g)         // bail out if we don't have a glyph
            return;

        uint16_t w = g->width, h = g->height, n = w * h;
        uint16_t c0 = m_c + g->offset_h, c1 = c0 + w;       // box start and end columns
        int16_t r0 = m_r + g->offset_v, r1 = r0 + h;        // box start and end rows

        if (c1 >= DISPLAY::width())
            return;                                         // truncate long lines

        DISPLAY::set_col_addr(c0, c1 - 1);
        DISPLAY::set_row_addr(r0, r1 - 1);
        DISPLAY::start();

        for (uint16_t i = 0; i < n; ++i)
            DISPLAY::write(color2st7789(interpolate_color(m_bg, m_fg, g->bitmap[i])));

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

            m_r += m_font.height;
            if (m_r > DISPLAY::height())
            {
                if (m_scroll + m_font.height < 320)
                    m_scroll += m_font.height;
                else
                {
                    m_scroll = 0;
                    m_r = m_font.height - 1;
                    DISPLAY::clear(m_bg);
                }
                DISPLAY::scroll(m_scroll);
            }
    }

private:
    const fontlib::font_t&  m_font;
    color_t                 m_fg;
    color_t                 m_bg;
    uint16_t                m_c;
    uint16_t                m_r;
    uint16_t                m_scroll;
};
    
} // namespace st7789

} // namespace stm32f0

