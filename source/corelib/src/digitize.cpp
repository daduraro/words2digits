#include "core/digitize.h"

#include "core/grammar.h"
#include "core/token_stream.h"

#include <algorithm>
#include <iostream>
#include <locale>

namespace {
    using namespace core;

    bool has_newline(const token_view_t& token) noexcept
    {
        if (!token.is_space()) return false;
        const auto& str = token.raw_str();
        return std::find( str.begin(), str.end(), '\n') != str.end();
    }
}

namespace core {

    void convert(std::istream& is, std::ostream& os) noexcept
    {
        std::locale loc("en_US.UTF-8");
        is.imbue(loc);
        token_stream_t stream(is);

        input_token_iterator_t it = stream.begin();
        while (stream) {
            auto fwd_it = it.look_ahead();
            auto m = match_cardinal_number(fwd_it);
            if (m) {
                bool write_nl = false;
                for (auto i = 0u; i < m.size; ++i) {
                    if (has_newline(*fwd_it)) {
                        write_nl = true;
                        break;
                    }
                }
                if (write_nl) os << "\n";
                os << std::to_string(m.num);
                it += m.size;
            }
            else {
                os << it->raw_str();
                ++it;
            }
        }
    }

}
