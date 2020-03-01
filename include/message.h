#pragma once

#include <cstdint>
#include <variant>

enum unit_t { unit };   // FIXME: define elsewhere!

enum message_tag_t { button_press, encoder_delta, encoder_press };

typedef std::variant<uint8_t, int16_t, unit_t> message_t;

