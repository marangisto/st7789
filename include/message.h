#pragma once

#include <cstdint>
#include <variant>

enum message_tag_t { button_press, encoder_delta };

typedef std::variant<uint8_t, int16_t> message_t;

