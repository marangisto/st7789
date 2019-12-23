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

struct iwidget
{
    typedef color::color_t color_t;

    virtual void fgbg(color_t fg, color_t bg) = 0;
    virtual void edit(int i) = 0;
};

struct ilayout
{
    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax) = 0;
    virtual void layout(pixel_t x, pixel_t y) = 0;
    virtual void render() = 0;
};

template<typename DISPLAY, typename T>
class widget_t: public iwidget, ilayout
{
public:
    typedef color::color_t color_t;
    typedef fontlib::font_t font_t;
    typedef std::function<const char*(T)> show_t;
    typedef std::function<void(volatile T&, int i)> edit_t;

    widget_t
        ( const font_t& font
        , color_t fg
        , color_t bg
        , uint16_t x
        , uint16_t y
        , uint16_t w
        , uint16_t h
        , show_t show
        , edit_t edit = 0
        , bool initial_render = false
        )
    {
        setup(font, fg, bg, x, y, w, h, show, edit, initial_render);
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
        , edit_t edit = 0
        , bool initial_render = false
        )
    {
        m_value = T(0);
        m_font = &font;
        m_fg = fg;
        m_bg = bg;
        m_x = x;
        m_y = y;
        m_w = w;
        m_h = h;
        m_show = show;
        m_edit = edit;

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

    void _render() const
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

    virtual void fgbg(color_t fg, color_t bg)
    {
        m_fg = fg;
        m_bg = bg;
        render();
    }

    virtual void edit(int i)
    {
        if (m_edit)
        {
            m_edit(m_value, i);
            render();
        }
    }

    // ilayout

    virtual dims_t constrain(pixel_t wmin, pixel_t wmax, pixel_t hmin, pixel_t hmax)
    {
        m_w = wmax;
        m_h = std::min(hmax, m_h);            // FIXME: use font-height to choose
        return dims_t(m_w, m_h);
    }

    virtual void layout(pixel_t x, pixel_t y)
    {
        m_x = x;
        m_y = y;
    }

    virtual void render()
    {
        _render();
    }

private:
    volatile T      m_value;
    uint16_t        m_x, m_y, m_w, m_h;
    show_t          m_show;
    edit_t          m_edit;
    const font_t    *m_font;
    color_t         m_fg;
    color_t         m_bg;
};

template<typename DISPLAY>
class label_t: public ilayout
{
public:
    typedef color::color_t color_t;
    typedef fontlib::font_t font_t;

    void setup
        ( const char *text
        , const font_t& font
        , color_t fg
        , color_t bg
        )
    {
        m_text = text;
        m_font = &font;
        m_fg = fg;
        m_bg = bg;
    }

    /*
    operator T() const { return m_value; }

    T operator=(const T& x)
    {
        m_value = x;
        render();
        return m_value;
    }
    */

    /*
    virtual void fgbg(color_t fg, color_t bg)
    {
        m_fg = fg;
        m_bg = bg;
        render();
    }

    virtual void edit(int i)
    {
        if (m_edit)
        {
            m_edit(m_value, i);
            render();
        }
    }
    */

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
        const char *s = m_text;

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
    }

protected:
    const char      *m_text;
    rect_t          m_rect;
    const font_t    *m_font;
    color_t         m_fg;
    color_t         m_bg;
};

template<typename T>
struct read_only
{
    static void edit(volatile T&, int) {}
};

template<typename DISPLAY, typename SHOW, typename EDIT = read_only<typename SHOW::T> >
class valuebox_t: public ilayout
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
        )
    {
        m_font = &font;
        m_fg = fg;
        m_bg = bg;
        m_value = value;
    }

    operator T() const { return m_value; }

    T operator=(const T& x)
    {
        m_value = x;
        render();
        return m_value;
    }

    /*
    virtual void fgbg(color_t fg, color_t bg)
    {
        m_fg = fg;
        m_bg = bg;
        render();
    }

    virtual void edit(int i)
    {
        if (m_edit)
        {
            m_edit(m_value, i);
            render();
        }
    }
    */

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
    }

private:
    volatile T      m_value;
    rect_t          m_rect;
    const font_t    *m_font;
    color_t         m_fg;
    color_t         m_bg;
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

