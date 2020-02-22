#include "core/digitize.h"

#include "core/grammar.h"
#include "core/token_stream.h"

#include <iostream>
#include <locale>

namespace core {

    void convert(std::istream& is, std::ostream& os) noexcept
    {
        std::locale loc("en_US.UTF-8");
        is.imbue(loc);
        token_stream_t stream(is);

        while (stream) {
            token_sequence_t seq = stream.new_sequence();
            auto m = match_cardinal_number(seq);
            if (m) {
                stream.replace(m->seq, std::to_string(m->num));
                stream.commit(m->seq, os);
            }
            else {
                stream.commit(seq, os);
            }
        }
    }

}
