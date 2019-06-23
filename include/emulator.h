#pragma once

#include <stdio.h>
#include <SDL2/SDL.h>

using namespace color;

template<unsigned SCREEN_WIDTH, unsigned SCREEN_HEIGHT>
class display_t
{
public:
    static void initialize(uint8_t scale = 1)
    {
        printf("starting display emulator!\n");

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            throw "could not initialize SD2";

        if (!(m_window = SDL_CreateWindow
            ( "Hello World!"
            , SDL_WINDOWPOS_UNDEFINED
            , SDL_WINDOWPOS_UNDEFINED
	        , SCREEN_WIDTH * scale
            , SCREEN_HEIGHT * scale
            , SDL_WINDOW_SHOWN
            )))
            throw "could not create window";

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

        SDL_SetRenderDrawColor(m_renderer, 0xff, 0xff, 0x0, 0xff);
        SDL_RenderPresent(m_renderer);

        m_texture = SDL_CreateTexture
            ( m_renderer
            , SDL_PIXELFORMAT_RGB888
            , SDL_TEXTUREACCESS_TARGET
            , SCREEN_WIDTH
            , SCREEN_HEIGHT
            );

        SDL_SetRenderTarget(m_renderer, m_texture);
        SDL_StartTextInput();
    }

    static void shutdown()
    {
        SDL_DestroyTexture(m_texture);
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    static void render()
    {
        SDL_SetRenderTarget(m_renderer, 0);
        SDL_RenderCopy(m_renderer, m_texture, 0, 0);
        SDL_RenderPresent(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_texture);
    }

    static void clear(color_t x = 0)
    {
        SDL_SetRenderDrawColor(m_renderer, R(x), G(x), B(x), 0xff);
        for (unsigned c = 0; c < SCREEN_WIDTH; ++c)
            for (unsigned r = 0; r < SCREEN_HEIGHT; ++r)
                SDL_RenderDrawPoint(m_renderer, c, r);
        render();
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
        start();
        write(c);
    }

    static void set_pixels_h(uint16_t x, uint16_t y, uint16_t n, color_t c)
    {
        set_col_addr(x, x + n - 1);
        set_row_addr(y, y);
        start();
        for (uint16_t i = 0; i < n; ++i)
            write(c);
    }

    static void set_pixels_v(uint16_t x, uint16_t y, uint16_t n, color_t c)
    {
        set_col_addr(x, x);
        set_row_addr(y, y + n - 1);
        start();
        for (uint16_t i = 0; i < n; ++i)
            write(c);
    }

private:
    static SDL_Window *m_window;
    static SDL_Renderer *m_renderer;
    static SDL_Texture *m_texture;

    static inline uint8_t R(uint16_t x) { return (((x >> 11) & 0x1f) << 3) | 0x7; }
    static inline uint8_t G(uint16_t x) { return (((x >> 5) & 0x2f) << 2) | 0x3; }
    static inline uint8_t B(uint16_t x) { return ((x & 0x1f) << 3) | 0x7; }
    static uint16_t m_c0, m_cn, m_r0, m_rn, m_ci, m_ri;
};

template<unsigned W, unsigned H> SDL_Window *display_t<W, H>::m_window = 0;
template<unsigned W, unsigned H> SDL_Renderer *display_t<W, H>::m_renderer = 0;
template<unsigned W, unsigned H> SDL_Texture *display_t<W, H>::m_texture = 0;

template<unsigned W, unsigned H> uint16_t display_t<W, H>::m_c0 = 0;
template<unsigned W, unsigned H> uint16_t display_t<W, H>::m_cn = 0;
template<unsigned W, unsigned H> uint16_t display_t<W, H>::m_r0 = 0;
template<unsigned W, unsigned H> uint16_t display_t<W, H>::m_rn = 0;
template<unsigned W, unsigned H> uint16_t display_t<W, H>::m_ci = 0;
template<unsigned W, unsigned H> uint16_t display_t<W, H>::m_ri = 0;

bool keyboard_poll(char& c)
{
    SDL_Event e;

    if (!SDL_PollEvent(&e))
        return false;

    switch (e.type)
    {
    case SDL_QUIT:
        c = 0;
        return true;
    case SDL_TEXTINPUT:
        {
            uint16_t uc = e.text.text[0];

            if (uc < 0x80)
            {
                c = uc;
                return true;
            }
        }
        break;
    case SDL_KEYDOWN:
        switch (e.key.keysym.scancode)
        {
        case SDL_SCANCODE_RETURN:
            c = '\r';
            return true;
        default:
            ;
        }
    default:
        return false;
    }
}

