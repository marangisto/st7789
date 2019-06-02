#include <stdio.h>
#include <SDL2/SDL.h>
#include <fontlib.h>
#include <text.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

using namespace fontlib;
using namespace color;

class sdl_display
{
public:
    static void set_renderer(SDL_Renderer *renderer) { m_renderer = renderer; }

    static void clear(color_t color = 0)
    {
    }

    static inline void start()
    {
        m_ci = m_c0;
        m_ri = m_r0;
        printf("start: %d %d\n", m_ci, m_ri);
    }

    static inline void write(uint16_t x)
    {
        printf("write %x ", x);

        if (m_ri > m_rn)
            throw("attempt to write outside bounding box");

        x = swap_bytes(x);  // because of reasons
        printf("(%d, %d, %d)\n", R(x), G(x), B(x));
        SDL_SetRenderDrawColor(m_renderer, R(x), G(x), B(x), 0xff);
        SDL_RenderDrawPoint(m_renderer, m_ci, m_ri);

        if (++m_ci > m_cn)
        {
            m_ci = m_c0;
            ++m_ri;
        }
    }

    static constexpr uint16_t width() { return SCREEN_WIDTH; }
    static constexpr uint16_t height() { return SCREEN_HEIGHT; }

    static void set_col_addr(uint16_t c0, uint16_t cn)
    {
        m_c0 = c0;
        m_cn = cn;
        printf("set_col: %d %d\n", m_c0, m_cn);
    }

    static void set_row_addr(uint16_t r0, uint16_t rn)
    {
        m_r0 = r0;
        m_rn = rn;
        printf("set_row: %d %d\n", m_r0, m_rn);
    }

    static void scroll(uint16_t lines)
    {
    }

    static void set_pixel(uint16_t x, uint16_t y, color_t c)
    {
        set_col_addr(x, x);
        set_row_addr(y, y);
        write(c);
    }

    static void set_pixels_h(uint16_t x, uint16_t y, uint16_t n, color_t c)
    {
        set_col_addr(x, x + n - 1);
        set_row_addr(y, y);
        for (uint16_t i = 0; i < n; ++i)
            write(c);
    }

    static void set_pixels_v(uint16_t x, uint16_t y, uint16_t n, color_t c)
    {
        set_col_addr(x, x);
        set_row_addr(y, y + n - 1);
        for (uint16_t i = 0; i < n; ++i)
            write(c);
    }

private:
    static inline uint8_t R(uint16_t x) { return (((x >> 11) & 0x1f) << 3) | 0x7; }
    static inline uint8_t G(uint16_t x) { return (((x >> 5) & 0x2f) << 2) | 0x3; }
    static inline uint8_t B(uint16_t x) { return ((x & 0x1f) << 3) | 0x7; }
    static SDL_Renderer *m_renderer;
    static uint16_t m_c0, m_cn, m_r0, m_rn, m_ci, m_ri;
};

SDL_Renderer *sdl_display::m_renderer = 0;

uint16_t sdl_display::m_c0 = 0;
uint16_t sdl_display::m_cn = 0;
uint16_t sdl_display::m_r0 = 0;
uint16_t sdl_display::m_rn = 0;
uint16_t sdl_display::m_ci = 0;
uint16_t sdl_display::m_ri = 0;

void run()
{
    printf("hello world!\n");
    SDL_Window *window = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw "could not initialize SD2";

    if (!(window = SDL_CreateWindow
        ( "Hello World!"
        , SDL_WINDOWPOS_UNDEFINED
        , SDL_WINDOWPOS_UNDEFINED
	    , SCREEN_WIDTH * 4
        , SCREEN_HEIGHT * 4
        , SDL_WINDOW_SHOWN
        )))
        throw "could not create window";

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0x0, 0xff);
    SDL_RenderPresent(renderer);

    sdl_display::set_renderer(renderer);

    bool quit = false;
    font_t font = fontlib::cmunrm_48;
    stm32f0::st7789::text_renderer_t<sdl_display> tr(font, white, black, true);

    SDL_Texture *texture = SDL_CreateTexture
        ( renderer
        , SDL_PIXELFORMAT_RGB888
        , SDL_TEXTUREACCESS_TARGET
        , SCREEN_WIDTH
        , SCREEN_HEIGHT
        );

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xff);
    SDL_RenderClear(renderer);

    SDL_StartTextInput();

    uint16_t r = font.start_row();

    tr.set_pos(0, r);

    while (!quit)
    {
        SDL_Event e;

        if (SDL_PollEvent(&e))
            switch (e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_TEXTINPUT:
                {
                    uint16_t uc = e.text.text[0];

                    if (uc < 0x80)
                    {
                        tr.write((char) uc);
                        SDL_SetRenderTarget(renderer, 0);
                        SDL_RenderCopy(renderer, texture, 0, 0);
                        SDL_RenderPresent(renderer);
                        SDL_SetRenderTarget(renderer, texture);
                    }
                }
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.scancode)
                {
                case SDL_SCANCODE_RETURN:
                    r += font.line_spacing();
                    tr.set_pos(0, r);
                    break;
                default:
                    ;
                }
                break;
            default:
                ;
            }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
