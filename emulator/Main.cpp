#include <text.h>
#include <fontlib.h>
#include <draw.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;

void text_box
    ( uint16_t      x
    , uint16_t      y
    , uint16_t      w
    , uint16_t      h
    , const font_t& font
    , color_t       fg
    , color_t       bg
    , const char    *s
    )
{
    text_renderer_t<display> tr(font, fg, bg, true);
    pen_t<display> pen(white);
    uint16_t tw, th;
 
    tr.bounding_box(s, tw, th);
    pen_t<display>(black).rectangle(x-1, y-1, tw+2, th+2);
    tr.set_pos(x, y - font.min_y);
    tr.write(s);
    display::render();
}

void run()
{
    display::initialize(3);
    display::clear(slate_gray);
    pen_t<display> pen(white);

    pen.rectangle(0, 0, display::width(), display::height());
    display::render();

    text_box(100,  50, 100, 25, fontlib::cmunss_20, yellow, dark_red, "-");
    text_box(100, 100, 100, 25, fontlib::cmunss_20, yellow, rebecca_purple, "Hello World!");
    text_box(100, 150, 100, 25, fontlib::cmunss_20, yellow, teal, "j-g");
    text_box(150, 150, 100, 25, fontlib::cmunss_20, yellow, olive_drab, "H j-g");
    text_box(100, 200, 100, 25, fontlib::cmunss_20, yellow, lime_green, "//");
    text_box(150, 200, 100, 25, fontlib::cmunss_20, yellow, orange_red, "l//g");

    bool quit = false;
    font_t font = fontlib::cmunrm_48;
    text_renderer_t<display> tr(font, white, red, true);

    uint16_t r = font.start_row();

    tr.set_pos(0, r);

    while (!quit)
    {
        char c;

        if (keyboard_poll(c))
            switch (c)
            {
            case 0:
                quit = true;
                break;
            case '\r':
                r += font.line_spacing();
                tr.set_pos(0, r);
                break;
            default:
                tr.write(c);
                display::render();
            }
    }

    display::shutdown();
}

int main()
{
    try
    {
        run();
    }
    catch (const char *s)
    {
        printf("exception: %s\n", s);
    }
}

