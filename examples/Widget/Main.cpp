#include <timer.h>
#include <fifo.h>
#include <button.h>
#include "../display.h"
#include "gui.h"

typedef hal::timer::timer_t<6> aux;
typedef hal::timer::encoder_t<3, PA6, PA7> enc;
typedef button_t<PB6> btn0;
typedef button_t<PA11> btn1;
typedef button_t<PC9> btn2;
typedef button_t<PC7> btn3;
typedef fifo_t<message_t, 0, 8> mq;

template<> void handler<interrupt::TIM6_DAC>()
{
    aux::clear_uif();

    static int16_t enc_last_count = 0;
    int16_t c = static_cast<int16_t>(enc::count()) >> 1;

    if (c != enc_last_count)
    {
        int16_t n = c - enc_last_count;

        // hack around weird glitch

        if (n == 64)
            n = -1;
        else if (n == -64)
            n = 1;

        mq::put(message_t().emplace<encoder_delta>(n));
        enc_last_count = c;
    }

    if (btn0::update_read())
        mq::put(message_t().emplace<encoder_press>(unit));
    if (btn1::update_read())
        mq::put(message_t().emplace<button_press>(1));
    if (btn2::update_read())
        mq::put(message_t().emplace<button_press>(2));
    if (btn3::update_read())
        mq::put(message_t().emplace<button_press>(3));
}

int main()
{
    enc::setup<pull_up>(1 + (64 << 1));
    btn0::setup<pull_up>();
    btn1::setup<pull_up>();
    btn2::setup<pull_up>();
    btn3::setup<pull_up>();
    aux::setup(48-1, 1000-1); // 1kHz
    aux::update_interrupt_enable();
    hal::nvic<interrupt::TIM6_DAC>::enable();
    display::setup<display_spi_prescale>(dark_red);

    theme_t theme = { white, slate_gray, blue, yellow, orange_red, fontlib::cmunss_20, false };

    static gui_t<display> gui(theme);

    window_manager wm(&gui);
    message_t m;
    static uint8_t i = 0;

    for (;;)
    {
        if (mq::get(m))
            wm.handle_message(m);
        gui.f2 = ++i;
    }
}

