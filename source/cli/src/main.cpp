#include "grammar.h"
#include "token_stream.h"

#include <fstream>

void convert(std::istream& is, std::ostream& os)
{
    std::locale loc("en_US.UTF-8");
    is.imbue(loc);
    token_stream_t stream(is);

    while (stream) {
        token_sequence_t seq = stream.new_sequence();
        auto m = match_cardinal_number(seq);
        if (m) {
            stream.replace(m.seq, std::to_string(m.num));
            stream.commit(m.seq.curr_id(), os);
        }
        else {
            stream.commit(seq.curr_id(), os);
        }
    }
}

int main(int argc, char** argv) {
    //*/
    std::ifstream in("test.txt", std::ios::binary);
    std::ofstream out("test-out.txt", std::ios::binary);
    convert(in, out);
    /*/

    char c1, c2, c3;
    std::istringstream iss("a b c");
    convert(iss, std::cout);

    //*/

    return 0;
}