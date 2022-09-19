#include "Grammar.hpp"

namespace pluma {

Action::Action(ActionType type = ActionType::ERROR, size_t state = (size_t)(-1),
               Terminal lookahead =
                   Terminal{
                       Token{
                           std::string(""),
                           TokenType::ERROR,
                       },
                   })
    : actionType(type), state(state), lookahead(lookahead) {}

bool Action::operator<(const Action &rhs) const {
    if (this->actionType == rhs.actionType) {
        return this->state < rhs.state;
    }
    return this->actionType < rhs.actionType;
}

std::set<Terminal> &Grammar::FIRST(const Sym &sym) {
    if (std::holds_alternative<Terminal>(sym)) {
        if (FIRST_SET.find(std::get<Terminal>(sym)) == FIRST_SET.end()) {
            FIRST_SET[sym] = {std::get<Terminal>(sym)};
        }
    }
    return FIRST_SET[sym];
}

std::set<Terminal> &Grammar::FOLLOW(const Sym &sym) { return FOLLOW_SET[sym]; }

std::set<LR1_Item> &Grammar::CLOSURE(const std::set<LR1_Item> &i) {
    auto result = CLOSURE_SET.find(i);
    if (result != CLOSURE_SET.end()) {
        return result->second;
    }

    std::set<LR1_Item> j = i;

    int prevAddNum = 0, currAddNum = 0;
    do {
        prevAddNum = currAddNum;
        std::set<LR1_Item> added{};
        for (auto &item : j) {
            if (!item.isCurrPosAtEnd()) {
                auto &next = item.rule.second[item.currPos];
                if (std::holds_alternative<Nonterminal>(next)) {
                    // only for non-terminal symbol
                    auto &nextNonterminal = std::get<Nonterminal>(next);
                    auto &prodRules = PRODUCE_RULES[nextNonterminal];
                    for (auto &p : prodRules) {
                        size_t currCurrPos = item.currPos;
                        bool hasEpsilonProduction = false;
                        do {
                            hasEpsilonProduction = false;
                            ++currCurrPos;
                            if (currCurrPos < item.rule.second.size()) {
                                std::set<Terminal> beta_FIRST_Set =
                                    FIRST(item.rule.second[currCurrPos]);
                                for (auto &beta : beta_FIRST_Set) {
                                    if (beta.token.tokenType == TokenType::NIL) {
                                        hasEpsilonProduction = true;
                                    } else {
                                        added.insert(LR1_Item{Rule{nextNonterminal, p}, 0, beta});
                                    }
                                }
                            } else {
                                added.insert(LR1_Item{Rule{nextNonterminal, p}, 0, item.lookahead});
                            }
                        } while (hasEpsilonProduction);
                    }
                }
            }
        }
        j.insert(added.begin(), added.end());
        currAddNum = j.size();
    } while (currAddNum != prevAddNum);

    // logger << "i:";
    // for (auto &nt : i) {
    //     logger << nt << ' ';
    // }
    // logger << std::endl;

    // logger << "closure(i) : ";
    // for (auto &nt : j) {
    //     logger << nt << ' ';
    // }
    // logger << std::endl;

    return CLOSURE_SET[i] = j;
}

std::set<LR1_Item> &Grammar::LR1_GOTO(const std::set<LR1_Item> &i, const Sym &sym) {
    std::set<LR1_Item> j;
    for (auto &item : i) {
        if (!item.isCurrPosAtEnd() && item.rule.second[item.currPos] == sym) {
            j.insert(LR1_Item{item.rule, item.currPos + 1, item.lookahead});
        }
    }
    return CLOSURE(j);
}

Action Grammar::LR1_Table_Read(const size_t &state, const Sym &sym) {
    auto &actionSet = LR1_Table[state][sym];
    if (actionSet.size() == 0) {
        // the default constructor makes an error action
        actionSet.insert(Action{});
    } else if (actionSet.size() == 2) {
        for (auto &action : actionSet) {
            if (action.actionType == Action::ActionType::PUSH_STACK) {
                return action;
            }
        }
    }
    return *(actionSet.begin());
}

Grammar::Grammar(const std::vector<Rule> &pRule) {
    // 构造所有产生式
    ORIGIN_PRODUCE_RULES = pRule;
    if (pRule.size()) {
        S = pRule[0].first;
        for (auto &pl : pRule) {
            PRODUCE_RULES[pl.first].emplace_back(pl.second);
            for (auto &sym : pl.second) {
                if (std::holds_alternative<Terminal>(sym)) {
                    P_TERMINAL_SET.insert(std::get<Terminal>(sym));
                } else {
                    P_NONTERMINAL_SET.insert(std::get<Nonterminal>(sym));
                }
            }
        }
    }
    P_TERMINAL_SET.insert(Terminal{
        Token{
            "",
            TokenType::TK_EOF,
        },
    });

    size_t symIndex = 0;

    for (auto &terminal : P_TERMINAL_SET) {
        symVec.push_back(terminal);
        symIndexMap.insert(std::pair{Sym(terminal), symIndex++});
    }

    for (auto &nonterminal : P_NONTERMINAL_SET) {
        symVec.push_back(nonterminal);
        symIndexMap.insert(std::pair{Sym(nonterminal), symIndex++});
    }

#ifdef HAS_OPENSSL
    const std::string grammarFile = "../../src/c/CParser.cpp";
    const std::string hashFile = "../../data/hash.txt";

    std::string hash = utils::fileHash(grammarFile);
    std::string hashFromFile;

    if (utils::loadHashFromFile(hashFile, hashFromFile)) {
        if (hash == hashFromFile) {
            this->LR1_Table.clear();
            bool readLR1TableSuccess = this->readLR1TableFromFile("../../data/lr1_table_cache.txt");
            if (readLR1TableSuccess) {
                return;
            } else {
                goto failed_to_read;
            }
        }
    }

failed_to_read:
    // Failed to generate hash or read from lr1_table cachefile.
    this->genLR1Table(pRule);
    this->writeLR1TableToFile("../../data/lr1_table_cache.txt");
    utils::saveHashToFile(hashFile, utils::fileHash(grammarFile));

#else
    this->genLR1Table(pRule);
#endif
}

void Grammar::genLR1Table(const std::vector<Rule> &pRule) {
    // 构造FIRST_SET
    int LAST_FIRST_SIZE = 0;
    int CURR_FIRST_SIZE = 0;
    auto CALC_FIRST_SET_COUNT = [&] {
        size_t SUM = 0;
        for (auto &fpair : FIRST_SET) {
            SUM += fpair.second.size();
        }
        return SUM;
    };
    do {
        LAST_FIRST_SIZE = CURR_FIRST_SIZE;
        for (auto &rule : pRule) {
            auto &leftSym = rule.first;
            auto &rightSyms = rule.second;
            for (size_t pIndex = 0; pIndex < rightSyms.size(); ++pIndex) {
                auto &CURR_FIRST = FIRST(rightSyms[pIndex]);
                bool hasEpsilonProduction = false;
                for (auto &sym : CURR_FIRST) {
                    if (sym.token.tokenType != TokenType::NIL) {
                        // No epsilon production
                        FIRST_SET[leftSym].insert(Terminal{sym.token});
                    } else {
                        hasEpsilonProduction = true;
                    }
                }
                if (!hasEpsilonProduction) {
                    break;
                }
                if (pIndex + 1 == rightSyms.size()) {
                    FIRST_SET[leftSym].insert(Terminal{
                        Token{
                            "nil",
                            TokenType::NIL,
                        },
                    });
                }
            }
        }
        CURR_FIRST_SIZE = CALC_FIRST_SET_COUNT();
    } while (LAST_FIRST_SIZE != CURR_FIRST_SIZE);

    logger << "first set:\n";
    for (auto &first : FIRST_SET) {
        logger << "FIRST(" << first.first << ") = ";
        for (auto &firstSyms : first.second) {
            logger << " " << firstSyms << " ";
        }
        logger << std::endl;
    }
    logger << std::endl;

    // 构造FOLLOW_SET
    FOLLOW_SET[S].insert(Terminal{
        Token{
            "",
            TokenType::TK_EOF,
        },
    });
    int LAST_FOLLOW_SIZE = 0;
    int CURR_FOLLOW_SIZE = 0;
    auto CALC_FOLLOW_SET_COUNT = [&] {
        size_t SUM = 0;
        for (auto &fpair : FOLLOW_SET) {
            SUM += fpair.second.size();
        }
        return SUM;
    };
    do {
        LAST_FOLLOW_SIZE = CURR_FOLLOW_SIZE;
        for (auto &rule : pRule) {
            auto &leftSym = rule.first;
            auto &rightSyms = rule.second;
            for (int pIndex = 0; pIndex < rightSyms.size(); ++pIndex) {
                const Sym &B = rightSyms[pIndex];
                if (std::holds_alternative<Nonterminal>(B)) {
                    // B is Nonterminal
                    auto &B_FOLLOW_SET = FOLLOW_SET[B];
                    if (pIndex == rightSyms.size() - 1) {
                        // A -> aB
                        auto &A_FOLLOW_SET = FOLLOW_SET[leftSym];
                        B_FOLLOW_SET.insert(A_FOLLOW_SET.begin(), A_FOLLOW_SET.end());
                    } else {
                        // A -> aBb
                        // succ : B 的后继
                        const Sym &succ = rightSyms[pIndex + 1];
                        auto &succ_FIRST_SET = FIRST(succ);

                        bool hasEpsilonProduction = false;
                        for (auto &terminal : succ_FIRST_SET) {
                            if (terminal.token.tokenType == TokenType::NIL) {
                                // b ->* NIL
                                hasEpsilonProduction = true;
                            } else {
                                // b is not NIL
                                B_FOLLOW_SET.insert(terminal);
                            }
                        }
                        if (hasEpsilonProduction) {
                            if (pIndex == rightSyms.size() - 2) {
                                // b 是最后一个
                                auto &A_FOLLOW_SET = FOLLOW_SET[leftSym];
                                B_FOLLOW_SET.insert(A_FOLLOW_SET.begin(), A_FOLLOW_SET.end());

                            } else {
                                // b 并不是最后一个, 找 b 后面的
                                auto &succ_succ_FOLLOW_SET = FOLLOW_SET[rightSyms[pIndex + 2]];
                                B_FOLLOW_SET.insert(succ_succ_FOLLOW_SET.begin(),
                                                    succ_succ_FOLLOW_SET.end());
                            }
                        }
                    }
                }
            }
        }
        CURR_FOLLOW_SIZE = CALC_FOLLOW_SET_COUNT();
    } while (LAST_FOLLOW_SIZE != CURR_FOLLOW_SIZE);

    logger << "follow set:\n";
    for (auto &follow : FOLLOW_SET) {
        logger << "FOLLOW(" << follow.first << ") = ";
        for (auto &followSyms : follow.second) {
            logger << " " << followSyms << " ";
        }
        logger << std::endl;
    }
    logger << std::endl;

    // 构造LR(1)_Item
    auto LR1_Item_Set_Equal = [](const std::set<LR1_Item> &s1,
                                 const std::set<LR1_Item> &s2) -> bool {
        if (s1.size() != s2.size()) {
            return false;
        }
        for (auto s1Iter = s1.begin(), s2Iter = s2.begin();
             s1Iter != s1.end() && s2Iter != s2.end(); ++s1Iter, ++s2Iter) {
            if ((*s1Iter) != (*s2Iter)) {
                return false;
            }
        }
        return true;
    };
    // [S' -> S, $]
    auto beginState = CLOSURE({LR1_Item{
        pRule[0],
        0,
        Token{
            "",
            TokenType::TK_EOF,
        },
    }});
    std::set<std::set<LR1_Item>> C{beginState};
    bool increased = false;
    do {
        increased = false;

        for (const std::set<LR1_Item> &itemSet : C) {
            std::vector<std::set<LR1_Item>> newAddToC;
            std::set<Sym> nextSym;
            for (const LR1_Item &item : itemSet) {
                if (!item.isCurrPosAtEnd()) {
                    nextSym.insert(item.rule.second[item.currPos]);
                }
            }
            for (const Sym &x : nextSym) {
                auto &iset = LR1_GOTO(itemSet, x);

                if (!iset.empty() && C.find(iset) == C.end()) {
                    newAddToC.push_back(iset);
                    increased = true;
                }
            }
            for (auto &newSet : newAddToC) {
                C.insert(newSet);
            }
        }

    } while (increased);
    C_SET = C;

    // this->display_LR1_C_SET();

    // 构造LR(1)的语法分析表
    std::map<size_t, std::map<Sym, std::set<Action>, SymLess>> &lr1_Table = LR1_Table;
    for (size_t statei = 0; auto &state : C) {
        std::set<Sym> nextSet;
        for (const LR1_Item &prod : state) {
            if (prod.isCurrPosAtEnd()) {
                if (prod.rule.first == pRule[0].first) {
                    // S' -> S, 完成
                    lr1_Table[statei][Terminal{
                                          Token{
                                              "",
                                              TokenType::TK_EOF,
                                          },
                                      }]
                        .insert(Action{Action::ActionType::ACCEPT, (size_t)10000, prod.lookahead});
                } else {
                    size_t originIndex = (size_t)(std::find(ORIGIN_PRODUCE_RULES.begin(),
                                                            ORIGIN_PRODUCE_RULES.end(), prod.rule) -
                                                  ORIGIN_PRODUCE_RULES.begin());
                    for (auto &pTerminal : P_TERMINAL_SET) {
                        if (pTerminal == prod.lookahead) {
                            lr1_Table[statei][pTerminal].insert(
                                Action{Action::ActionType::REDUCE, originIndex, prod.lookahead});
                        }
                    }
                }
            } else {
                nextSet.insert(prod.rule.second[prod.currPos]);
            }
        }
        for (auto &next : nextSet) {
            const auto &nextState = LR1_GOTO(state, next);
            for (size_t j = 0; auto &IJ : C) {
                if (IJ == nextState) {
                    if (std::holds_alternative<Nonterminal>(next)) {
                        LR1_Table[statei][next].insert(Action{Action::ActionType::GOTO, j,
                                                              Terminal{
                                                                  Token{
                                                                      "nil",
                                                                      TokenType::NIL,
                                                                  },
                                                              }});
                    } else {
                        LR1_Table[statei][next].insert(Action{Action::ActionType::PUSH_STACK, j,
                                                              Terminal{
                                                                  Token{
                                                                      "nil",
                                                                      TokenType::NIL,
                                                                  },
                                                              }});
                    }
                }
                ++j;
            }
        }
        ++statei;
    }
    for (size_t statei = 0; auto &state : C_SET) {
        if (state == beginState) {
            beginStateIndex = statei;
        }
        ++statei;
    }
}

void Grammar::writeLR1TableToFile(std::string filename) {
    // Open the file or create it if not exist.
    std::fstream file;
    file.open(filename, std::ios::out);
    if (!file.is_open()) {
        file.clear();
        file.open(filename, std::ios::out);
        file.close();
        file.open(filename);
    }
    file.sync_with_stdio(false);

    for (auto &statePair : LR1_Table) {
        file << statePair.first << '\n';
        for (auto &symPair : statePair.second) {
            file << symIndexMap[symPair.first] << ' ';
            for (auto &action : symPair.second) {
                file << (size_t)action.actionType << ' ' << action.state << ' ';
            }
            file << -1 << ' ' << -1 << '\n';
        }
        file << -1 << '\n';
    }
    file << -2 << '\n';

    file << this->beginStateIndex << '\n';

    // Close the file.
    file.close();
}

bool Grammar::readLR1TableFromFile(std::string filename) {
    std::ifstream file;
    file.open(filename, std::ios::in);
    if (!file.is_open()) {
        logger << "Failed to read from cache file.\n";
        return false;
    }
    file.sync_with_stdio(false);
    while (!file.eof()) {
        int32_t state_i32 = 0;
        file >> state_i32;
        while (state_i32 != -2) {
            size_t state = (size_t)state_i32;
            int32_t symIndex_i32 = 0;
            file >> symIndex_i32;
            while (symIndex_i32 != -1) {
                size_t symIndex = (size_t)symIndex_i32;
                int32_t actionType_i32, toState_i32;
                file >> actionType_i32 >> toState_i32;
                while (actionType_i32 != -1) {
                    size_t actionType = (size_t)actionType_i32;
                    size_t toState = (size_t)toState_i32;
                    this->LR1_Table[state][symVec[symIndex]].insert(Action{
                        (Action::ActionType)actionType,
                        toState,
                    });
                    file >> actionType_i32 >> toState_i32;
                }
                file >> symIndex_i32;
            }
            file >> state_i32;
        }
        if (state_i32 == -2) {
            break;
        }
    }

    file >> this->beginStateIndex;

    file.close();
    return true;
}

Ast Grammar::gen(std::vector<Sym> str) {
    if (str.empty()) {
        logger << "\nsource file is empty\n\n";
        return Ast(nullptr);
    }
    std::vector<size_t> stateStack;
    std::vector<AstNode *> nodeStack;
    size_t strPos = 0;
    stateStack.push_back(beginStateIndex);
    while (1) {
        Sym currSym = str[strPos];
        size_t state = stateStack.back();

        const auto &action = LR1_Table_Read(state, currSym);
        const auto &epsilonAction = LR1_Table_Read(state, Terminal{Token{"nil", TokenType::NIL}});
        if (action.actionType != Action::ActionType::ERROR ||
            epsilonAction.actionType != Action::ActionType::ERROR) {
            bool isRuleEpsilon = false;
            if (action.actionType == Action::ActionType::ERROR) {
                isRuleEpsilon = true;
            }
            auto &takenAction = isRuleEpsilon ? epsilonAction : action;
            logger << "Current state : " << state << ", symbol : " << currSym << std::endl;
            logger << "Current action : " << (int)takenAction.actionType << " " << takenAction.state
                   << std::endl;
            switch (takenAction.actionType) {
                case Action::ActionType::REDUCE: {
                    auto &rule = ORIGIN_PRODUCE_RULES[action.state];
                    // Left symbol
                    auto leftSymNode = (AstNode *)new AstNode(rule.first);
                    std::vector<AstNode *> rev;
                    for (size_t i = 0; i < rule.second.size(); ++i) {
                        stateStack.pop_back();
                        rev.push_back(nodeStack.back());
                        nodeStack.pop_back();
                    }
                    for (size_t i = 0; i < rule.second.size(); ++i) {
                        leftSymNode->appendSon(rev.back());
                        rev.pop_back();
                    }

                    // GOTO
                    const auto &gotoAction = LR1_Table_Read(stateStack.back(), rule.first);
                    if (gotoAction.actionType == Action::ActionType::GOTO) {
                        stateStack.push_back(gotoAction.state);
                    } else {
                        goto error;
                    }
                    nodeStack.push_back(leftSymNode);

                    logger << rule;
                    break;
                }
                case Action::ActionType::PUSH_STACK: {
                    stateStack.push_back(takenAction.state);
                    if (isRuleEpsilon) {
                        nodeStack.push_back(nullptr);
                    } else {
                        nodeStack.push_back((AstNode *)new AstNode(str[strPos++]));
                    }
                    break;
                }
                case Action::ActionType::GOTO: {
                    // This branch shouldn't be accessed.
                    break;
                }
                case Action::ActionType::ACCEPT: {
                    logger << "\nFinished parse procedure.\n";
                    AstNode *headPtr = nodeStack.back();
                    nodeStack.pop_back();
                    if (nodeStack.size()) {
                        // Not an AST-tree.
                        logger << "\nNot an ast-tree!\n";
                        goto err_failed_to_recover;
                    }
                    return Ast(headPtr);
                }
                case Action::ActionType::ERROR: {
                    goto error;
                }
            }
        } else {
        error:
            logger << "ERROR: state " << state << ", symbol " << currSym
                   << " have an error action.\n";
            logger << "At line " << std::get<Terminal>(currSym).token.line << ":";
            std::vector<Terminal> shouldBe;
            if (LR1_Table.find(state) != LR1_Table.end()) {
                for (auto &actionList : LR1_Table.at(state)) {
                    if (std::holds_alternative<Terminal>(actionList.first) &&
                        actionList.second.size() == 1 &&
                        actionList.second.begin()->actionType != Action::ActionType::ERROR) {
                        shouldBe.push_back(std::get<Terminal>(actionList.first));
                    }
                }
            }
            if (shouldBe.size()) {
                for (auto &expectedTerminal : shouldBe) {
                    logger << expectedTerminal << " expected.\n";
                    break;
                }
            }
            // TODO: error recovery
            goto err_failed_to_recover;
        }
    }

err_failed_to_recover:
    return Ast(nullptr);
}

bool Grammar::checkLR1() {
    bool result = false;
    for (auto &state : LR1_Table) {
        size_t stateIndex = state.first;
        for (auto &symCol : state.second) {
            const Sym &sym = symCol.first;
            std::set<Action> &actionSet = symCol.second;
            if (actionSet.size() >= 2) {
                if (!result) {
                    logger << "LR1 has grammatical conflict:\n\n\n";
                    result = true;
                }
                logger << "In state " << stateIndex << ", symbol " << sym << std::endl;
                for (auto &action : actionSet) {
                    if (action.actionType == Action::ActionType::REDUCE)
                        logger << "R" << action.state << std::endl;
                    else if (action.actionType == Action::ActionType::PUSH_STACK)
                        logger << "S" << action.state << std::endl;
                }
                logger << std::endl << std::endl;
            }
        }
    }
    return result;
}

void Grammar::display_LR1_Table() {
    for (auto &state : LR1_Table) {
        size_t stateIndex = state.first;
        logger << std::setw(12) << "";
        for (auto &symCol : state.second) {
            logger << std::setw(12) << symCol.first;
        }
        logger << std::endl;
        logger << std::setw(12) << stateIndex;
        for (auto &symCol : state.second) {
            std::set<Action> &actionSet = symCol.second;
            for (auto &action : actionSet) {
                switch (action.actionType) {
                    case Action::ActionType::PUSH_STACK: {
                        logger << std::setw(12)
                               << (std::string("S") + std::to_string(action.state));
                        break;
                    }
                    case Action::ActionType::REDUCE: {
                        logger << std::setw(12)
                               << (std::string("R") + std::to_string(action.state));
                        break;
                    }
                    case Action::ActionType::ERROR: {
                        logger << std::setw(12) << "E";
                        break;
                    }
                    case Action::ActionType::GOTO: {
                        logger << std::setw(12)
                               << (std::string("G") + std::to_string(action.state));
                        break;
                    }
                    case Action::ActionType::ACCEPT: {
                        logger << std::setw(12) << "A";
                        break;
                    }
                }
            }
        }
        logger << std::endl << std::endl;
    }
}

std::ostream &operator<<(std::ostream &os, const std::set<LR1_Item> itemSet) {
    for (auto &item : itemSet) {
        os << item;
    }
    return os;
}

void Grammar::display_LR1_C_SET() {
    for (size_t i = 0; auto &state : C_SET) {
        logger << "===============\n";
        logger << "state " << i << ":\n";
        logger << "---------------\n";
        logger << state;
        logger << "===============\n";
        ++i;
    }
}

void Grammar::displayAllRule() {
    logger << "\nrules:\n\n";
    for (size_t i = 0; i < ORIGIN_PRODUCE_RULES.size(); ++i) {
        logger << i << ":" << ORIGIN_PRODUCE_RULES[i];
    }
    logger << "\n\n";
    return;
}

}  // namespace pluma
