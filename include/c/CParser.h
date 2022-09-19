#include "../Parser.h"

namespace pluma {

struct CParser : Parser {
    CParser();
    CParser(const CParser &other) = delete;
    CParser(CParser &&other);
    CParser &operator=(const CParser &rhs) = delete;
    CParser &operator=(CParser &&rhs);

    void genGrammar() override;
};

}  // namespace pluma
