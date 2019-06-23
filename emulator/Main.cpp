#include <text.h>
#include <fontlib.h>
#include <emulator.h>

using namespace fontlib;
using namespace color;

typedef display_t<240, 240> display;

void run()
{
    display::initialize(2);
    display::clear(blue);

    bool quit = false;
    font_t font = fontlib::cmunrm_48;
    st7789::text_renderer_t<display> tr(font, white, red, true);

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

