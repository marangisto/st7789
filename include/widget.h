#pragma once

#include <message.h>
#include <text.h>
#include <fontlib.h>
#include <draw.h>
#include <list.h>
#include <functional>
#include <variant>

struct theme_t
{
    using color_t = color::color_t;
    using font_t = fontlib::font_t;

    color_t normal_fg;
    color_t normal_bg;
    color_t border_color;
    color_t normal_cursor;
    color_t active_cursor;
    const font_t font;
    bool quiet;             // don't render on propery update
};

typedef uint8_t pixel_t;    // FIXME: traits to determine smallest word for DISPLAY dimensions!

struct __attribute__((__packed__)) rect_t
{
    rect_t(): x(0), y(0), w(0), h(0) {}
    rect_t(pixel_t _x, pixel_t _y, pixel_t _w, pixel_t _h): x(_x), y(_y), w(_w), h(_h) {}

    pixel_t x, y, w, h;
};

struct iwidget
{
    virtual void size(pixel_t& w, bool& fill_h, pixel_t& h, bool& fill_v) = 0;
    virtual void size(pixel_t w, pixel_t h) = 0;
    virtual void place(pixel_t x, pixel_t y) = 0;
    virtual void render() = 0;
};

struct ifocus
{
    typedef color::color_t color_t;

    virtual void focus(color_t c) = 0;
    virtual void defocus() = 0;
    virtual void edit(int i) = 0;
};

enum action_tag_t { no_action, push_window, pop_window, pop_window_message };

struct iwindow;

typedef std::variant<unit_t, iwindow*, unsigned, message_t> action_t;

struct iwindow
{
    virtual void render() = 0;
    virtual action_t handle_message(const message_t& m) = 0;
};

template<typename T>
struct read_only
{
    static void edit(volatile T&, int) {}
};

template<typename DISPLAY, typename SHOW, typename EDIT = read_only<typename SHOW::T> >
class valuebox_t: public iwidget, public ifocus
{
public:
    typedef typename SHOW::T T;
    typedef color::color_t color_t;
    typedef fontlib::font_t font_t;

    valuebox_t
        ( const theme_t& theme
        , const T& value = T()
        ): m_value(value), m_theme(theme), m_frame(m_theme.normal_bg)
    {}

    operator T() const { return m_value; }

    T operator=(const T& x)
    {
        m_value = x;
        if (!m_theme.quiet)
            render();
        return m_value;
    }

    const volatile T* ptr() const { return &m_value; }

    // iwidget

    virtual void size(pixel_t& w, bool& fill_h, pixel_t& h, bool& fill_v)
    {
        w = h = m_theme.font.line_spacing();
        fill_h = true;
        fill_v = false;
    }

    virtual void size(pixel_t w, pixel_t h)
    {
        m_rect.w = w;
        m_rect.h = h;
    }

    virtual void place(pixel_t x, pixel_t y)
    {
        m_rect.x = x;
        m_rect.y = y;
    }

    virtual void render()
    {
        const char *s = SHOW::show(m_value);

        text::text_renderer_t<DISPLAY> tr(m_theme.font, m_theme.normal_fg, m_theme.normal_bg, true);
        graphics::pen_t<DISPLAY> pen(m_theme.normal_bg);
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

        tr.set_pos(m_rect.x + lpad, m_rect.y + tpad - m_theme.font.min_y);
        tr.write(s);

        if (m_frame != m_theme.normal_bg)
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
        m_frame = m_theme.normal_bg;
        render();
    }

    virtual void edit(int i)
    {
        EDIT::edit(m_value, i);
        render();
    }

private:
    volatile T      m_value;
    const theme_t&  m_theme;
    color_t         m_frame;
    rect_t          m_rect;
};

template<typename DISPLAY>
class border_t: public iwidget
{
public:
    typedef color::color_t color_t;

    border_t(iwidget *child, color_t color = color::black, pixel_t thickness = 1)
    {
        m_child = child;
        m_color = color;
        m_thickness = thickness;
    }

    // iwidget

    virtual void size(pixel_t& w, bool& fill_h, pixel_t& h, bool& fill_v)
    {
        pixel_t dp = m_thickness << 1;  // both sides of border

        m_child->size(w, fill_h, h, fill_v);
        w += dp;
        h += dp;
    }

    virtual void size(pixel_t w, pixel_t h)
    {
        pixel_t dp = m_thickness << 1;  // both sides of border

        m_rect.w = w;
        m_rect.h = h;
        m_child->size(w - dp, h - dp);
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

template<typename DISPLAY>
class vertical_t: public iwidget
{
public:
    static constexpr uint8_t max_children = 10;

    vertical_t
        ( iwidget *c0 = 0
        , iwidget *c1 = 0
        , iwidget *c2 = 0
        , iwidget *c3 = 0
        , iwidget *c4 = 0
        , iwidget *c5 = 0
        , iwidget *c6 = 0
        , iwidget *c7 = 0
        , iwidget *c8 = 0
        , iwidget *c9 = 0
        )
    {
        m_count = 0;
        if (c0) m_child[m_count++] = c0;
        if (c1) m_child[m_count++] = c1;
        if (c2) m_child[m_count++] = c2;
        if (c3) m_child[m_count++] = c3;
        if (c4) m_child[m_count++] = c4;
        if (c5) m_child[m_count++] = c5;
        if (c6) m_child[m_count++] = c6;
        if (c7) m_child[m_count++] = c7;
        if (c8) m_child[m_count++] = c8;
        if (c9) m_child[m_count++] = c9;
    }

    // iwidget

    virtual void size(pixel_t& w, bool& fill_h, pixel_t& h, bool& fill_v)
    {
        w = h = 0;
        fill_h = fill_v = false;

        for (uint8_t i = 0; i < m_count; ++i)
        {
            pixel_t cw, ch;
            bool fh, fv;

            m_child[i]->size(cw, fh, ch, fv);
            w = std::max(w, cw);
            h += ch;
            fill_h = fill_h || fh;
            fill_v = fill_v || fv;
            m_size[i] = fv ? 0 : ch;
        }
    }

    virtual void size(pixel_t w, pixel_t h)
    {
        make_fills(h);
        for (unsigned i = 0; i < m_count; ++i)
            m_child[i]->size(w, m_size[i]);
    }

    virtual void place(pixel_t x, pixel_t y)
    {
        for (uint8_t i = 0; i < m_count; ++i)
        {
            m_child[i]->place(x, y);
            y += m_size[i];
        }
    }

    virtual void render()
    {
        for (uint8_t i = 0; i < m_count; ++i)
            m_child[i]->render();
    }

protected:
    void make_fills(pixel_t s)
    {
        unsigned fills = 0;
        pixel_t used = 0;

        for (unsigned i = 0; i < m_count; ++i)
            if (m_size[i] == 0)
                ++fills;
            else
                used += m_size[i];

        if (fills > 0)  // FIXME: account for requested minimum sizes!
        {
            pixel_t each = (s - used) / fills;
            pixel_t stub = (s - used) - fills * each;

            for (unsigned i = 0; i < m_count; ++i)
                if (m_size[i] == 0)
                {
                    m_size[i] = each + stub;
                    stub = 0;
                }
        }
    }

    iwidget     *m_child[max_children];
    pixel_t     m_size[max_children];
    uint8_t     m_count;
};

template<typename DISPLAY>
class horizontal_t: public vertical_t<DISPLAY>
{
public:
    horizontal_t
        ( iwidget *c0 = 0
        , iwidget *c1 = 0
        , iwidget *c2 = 0
        , iwidget *c3 = 0
        , iwidget *c4 = 0
        , iwidget *c5 = 0
        , iwidget *c6 = 0
        , iwidget *c7 = 0
        , iwidget *c8 = 0
        , iwidget *c9 = 0
        ) : vertical_t<DISPLAY>(c0, c1, c2, c3, c4, c5, c6, c7, c8, c9)
    {}

    // iwidget overrides

    typedef vertical_t<DISPLAY> base;

    virtual void size(pixel_t& w, bool& fill_h, pixel_t& h, bool& fill_v)
    {
        w = h = 0;
        fill_h = fill_v = false;

        for (uint8_t i = 0; i < base::m_count; ++i)
        {
            pixel_t cw, ch;
            bool fh, fv;

            base::m_child[i]->size(cw, fh, ch, fv);
            h = std::max(h, ch);
            w += cw;
            fill_h = fill_h || fh;
            fill_v = fill_v || fv;
            base::m_size[i] = fh ? 0 : cw;
        }
    }

    virtual void size(pixel_t w, pixel_t h)
    {
        base::make_fills(w);
        for (unsigned i = 0; i < base::m_count; ++i)
            base::m_child[i]->size(base::m_size[i], h);
    }

    virtual void place(pixel_t x, pixel_t y)
    {
        for (uint8_t i = 0; i < base::m_count; ++i)
        {
            base::m_child[i]->place(x, y);
            x += base::m_size[i];
        }
    }
};

template<typename DISPLAY>
class scroll_region_t: public vertical_t<DISPLAY>
{
public:
    scroll_region_t(const theme_t& theme): m_theme(theme), m_scroll(0) {}

    // iwidget

    virtual void size(pixel_t& w, bool& fill_h, pixel_t& h, bool& fill_v)
    {
        w = DISPLAY::width();
        h = 1;                          // whatever we can get
        fill_h = false;
        fill_v = true;
    }

    virtual void size(pixel_t w, pixel_t h)
    {
        m_rect.w = w;
        m_rect.h = h;
    }

    virtual void place(pixel_t x, pixel_t y)
    {
        m_rect.x = x;
        m_rect.y = y;
    }

    virtual void render()
    {
        graphics::pen_t<DISPLAY> pen(m_theme.normal_bg);

        pen.fill_rectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
        DISPLAY::set_scroll_area(m_rect.y, m_rect.h);
    }

    // programming interface

    const rect_t& rect() { return m_rect; }

    uint32_t scroll(int n)
    {
        m_scroll += n;

        uint32_t s = m_rect.y + m_scroll % m_rect.h;

        DISPLAY::scroll(s);
        return s;
    }

    void scroll_off()
    {
        graphics::pen_t<DISPLAY> pen(m_theme.normal_bg);

        pen.fill_rectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
        m_scroll = 0;
        DISPLAY::scroll(0);
        DISPLAY::set_scroll_area(0, DISPLAY::height());
    }


private:
    const theme_t&  m_theme;
    rect_t          m_rect;
    uint32_t        m_scroll;
};

template<typename DISPLAY>
class window_t: public iwindow
{
public:
    typedef color::color_t color_t;

    static rect_t full_size() { return rect_t(0, 0, DISPLAY::width(), DISPLAY::height()); }

    window_t() {}

    window_t(iwidget *widget, list<ifocus*>& navigation, const theme_t& theme, const rect_t& r = full_size())
    {
        setup(widget, navigation, theme, r);
    }

    void setup(iwidget *widget, list<ifocus*>& navigation, const theme_t& theme, const rect_t& r = full_size())
    {
        m_normal = theme.normal_cursor;
        m_active = theme.active_cursor;
        m_widget = widget;

        pixel_t w, h;
        bool fill_h, fill_v;

        m_widget->size(w, fill_h, h, fill_v);
        m_widget->size(std::max(w, r.w), std::max(h, r.h));

        m_widget->place(r.x, r.y);
        m_navigation.splice(m_navigation.end(), navigation);
        m_focus = m_navigation.begin();
        if (!m_navigation.empty())
            (*m_focus)->focus(m_normal);
    }

    virtual void render()
    {
        m_widget->render();
    }

    virtual action_t handle_message(const message_t& m)
    {
        switch (m.index())
        {
        case encoder_press:
            m_state = m_state == navigating ? editing : navigating;
            if (!m_navigation.empty())
                (*m_focus)->focus(m_state == editing ? m_active : m_normal);
            break;
        case encoder_delta:
            if (m_navigation.empty())
                break;
            if (m_state == navigating)
            {
                int dir = std::get<encoder_delta>(m);

                (*m_focus)->defocus();
                if (dir > 0 && ++m_focus == m_navigation.end())
                    m_focus = m_navigation.begin();
                else if (dir < 0 && --m_focus == m_navigation.end())
                    --m_focus;
                (*m_focus)->focus(m_normal);
            }
            else
                (*m_focus)->edit(std::get<encoder_delta>(m));
            break;
        default:
            ;
        }
        return action_t().emplace<no_action>(unit);
    }

private:
    enum state_t { navigating, editing };

    iwidget                 *m_widget;
    list<ifocus*>           m_navigation;
    list_iterator<ifocus*>  m_focus;
    state_t                 m_state;
    color_t                 m_normal;
    color_t                 m_active;
};

class window_manager
{
public:
    window_manager(iwindow *w): m_top(w)
    {
        m_wstack.push_front(w);
        (*m_wstack.begin())->render();
    }

    void handle_message(const message_t& m)
    {
        action_t a = (*m_wstack.begin())->handle_message(m);
        switch (a.index())
        {
        case no_action:
            break;
        case push_window:
            m_wstack.push_front(std::get<push_window>(a));
            (*m_wstack.begin())->render();
            break;
        case pop_window:
            if (*m_wstack.begin() != m_top)
                m_wstack.pop_front();
            (*m_wstack.begin())->render();
            break;
        case pop_window_message:
            if (*m_wstack.begin() != m_top)
                m_wstack.pop_front();
            (*m_wstack.begin())->render();
            handle_message(std::get<pop_window_message>(a));    // how deep does this go?
            break;
        default: ;      // FIXME: illegal action!
        }
    }

private:
    iwindow         *m_top;
    list<iwindow*>  m_wstack;
};

static char tmp_buf[256];   // FIXME: think of a more principled way to share this!

struct show_str
{
    typedef const char *T;
    static const char *show(T x) { return x; }
};

struct show_int
{
    typedef int T;
    static const char *show(T x) { sprintf(tmp_buf, "%d", x); return tmp_buf; }
};

struct edit_int
{
    static void edit(volatile int& x, int i) { x += i; }
};

struct show_unsigned
{
    typedef int T;
    static const char *show(T x) { sprintf(tmp_buf, "%u", x); return tmp_buf; }
};

struct edit_unsigned
{
    static void edit(volatile int& x, int i) { if (x + i >= 0) x += i; }
};

template<int DECIMALS>
struct show_float
{
    typedef float T;
    static const char *show(T x) { sprintf(tmp_buf, "%.*f", DECIMALS, x); return tmp_buf; }
};

template<int DIVISOR>
struct edit_float
{
    static void edit(volatile float& x, int i) { x += static_cast<float>(i) / DIVISOR; }
};

template<int DECIMALS>
struct show_percent
{
    typedef float T;
    static const char *show(T x) { sprintf(tmp_buf, "%.*f%%", DECIMALS, x * 100.0f); return tmp_buf; }
};

struct show_bool
{
    typedef bool T;
    static const char *show(T x) { sprintf(tmp_buf, "%s", x ? "true" : "false"); return tmp_buf; }
};

struct edit_bool
{
    static void edit(volatile bool& x, int i) { if (i) x = !x; }
};

