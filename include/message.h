#pragma once

#include <cstdint>
#include <variant>

enum unit_t { unit };   // FIXME: define elsewhere!

enum message_tag_t { button_press, encoder_delta, encoder_press, aux_data };

typedef std::variant<uint8_t, int16_t, unit_t, uint32_t> message_t;

