#ifndef GRAMMAR_HPP_
#define GRAMMAR_HPP_

#include <algorithm>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "Ast.hpp"

namespace pluma {

struct Action {
    enum class ActionType {
        ERROR,
        GOTO,
        PUSH_STACK,
        REDUCE,
        ACCEPT,
    } actionType;
    size_t state;
    Terminal lookahead;

    Action(ActionType type = ActionType::ERROR, size_t state = (size_t)(-1),
           Terminal lookahead =
               Terminal{
                   Token{
                       std::string(""),
                       TokenType::ERROR,
                   },
               })
        : actionType(type), state(state), lookahead(lookahead) {}

    bool operator<(const Action &rhs) const {
        if (this->actionType == rhs.actionType) {
            return this->state < rhs.state;
        }
        return this->actionType < rhs.actionType;
    }
};

struct Grammar {
    std::map<Nonterminal, std::vector<SymList>> PRODUCE_RULES;
    std::vector<Rule> ORIGIN_PRODUCE_RULES;

    // Nonterminal for augmented grammar.
    Nonterminal S;

    std::set<Terminal> P_TERMINAL_SET;
    std::set<Nonterminal> P_NONTERMINAL_SET;

    std::map<size_t, std::map<Sym, std::set<Action>, SymLess>> LR1_Table;

    using TerminalSet = std::map<Sym, std::set<Terminal>, pluma::SymLess>;

    TerminalSet FIRST_SET;
    TerminalSet FOLLOW_SET;

    std::set<Terminal> &FIRST(const Sym &sym) {
        if (std::holds_alternative<Terminal>(sym)) {
            if (FIRST_SET.find(std::get<Terminal>(sym)) == FIRST_SET.end()) {
                FIRST_SET[sym] = {std::get<Terminal>(sym)};
            }
        }
        return FIRST_SET[sym];
    }

    std::set<Terminal> &FOLLOW(const Sym &sym) { return FOLLOW_SET[sym]; }

    struct LR1_Item {
        // 文法的产生式
        Rule rule;

        // 项的点的位置
        // 例: A -> ·aBb , currPos == 0
        //     A -> a·Bb , currPos == 1
        //     A -> aBb· , currPos == 3
        size_t currPos;

        Terminal lookahead;

        bool operator<(const LR1_Item &rhs) const {
            if (this->rule.first == rhs.rule.first) {
                if (this->rule.second == rhs.rule.second) {
                    if (this->currPos == rhs.currPos) {
                        return this->lookahead < rhs.lookahead;
                    } else {
                        return this->currPos < rhs.currPos;
                    }
                } else {
                    return this->rule.second < rhs.rule.second;
                }
            }
            return this->rule.first < rhs.rule.first;
        }
        bool operator==(const LR1_Item &rhs) const {
            return this->rule == rhs.rule && this->currPos == rhs.currPos &&
                   this->lookahead == rhs.lookahead;
        }
        bool operator!=(const LR1_Item &rhs) const { return !(*this == rhs); }

        inline bool isCurrPosAtEnd() const { return this->currPos >= this->rule.second.size(); }

        friend std::ostream &operator<<(std::ostream &os, const LR1_Item &lr1_item) {
            os << lr1_item.rule.first << " -> ";
            for (size_t i = 0; i < lr1_item.rule.second.size(); ++i) {
                if (lr1_item.currPos == i) {
                    os << " . ";
                }
                os << " " << lr1_item.rule.second[i] << " ";
            }
            if (lr1_item.currPos == lr1_item.rule.second.size()) {
                os << " . ";
            }
            os << ", " << lr1_item.lookahead;
            os << std::endl;
            return os;
        }
    };

    std::map<std::set<LR1_Item>, std::set<LR1_Item>> CLOSURE_SET;
    std::vector<std::set<LR1_Item>> C_SET;

    std::set<LR1_Item> &CLOSURE(const std::set<LR1_Item> &i) {
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
                                            added.insert(
                                                LR1_Item{Rule{nextNonterminal, p}, 0, beta});
                                        }
                                    }
                                } else {
                                    added.insert(
                                        LR1_Item{Rule{nextNonterminal, p}, 0, item.lookahead});
                                }
                            } while (hasEpsilonProduction);
                        }
                    }
                }
            }
            j.insert(added.begin(), added.end());
            currAddNum = j.size();
        } while (currAddNum != prevAddNum);

        // std::cout << "i:";
        // for (auto &nt : i) {
        //     std::cout << nt << ' ';
        // }
        // std::cout << std::endl;

        // std::cout << "closure(i) : ";
        // for (auto &nt : j) {
        //     std::cout << nt << ' ';
        // }
        // std::cout << std::endl;

        return CLOSURE_SET[i] = j;
    }

    std::set<LR1_Item> &LR1_GOTO(const std::set<LR1_Item> &i, const Sym &sym) {
        std::set<LR1_Item> j;
        for (auto &item : i) {
            if (!item.isCurrPosAtEnd() && item.rule.second[item.currPos] == sym) {
                j.insert(LR1_Item{item.rule, item.currPos + 1, item.lookahead});
            }
        }
        return CLOSURE(j);
    }

    Action LR1_Table_Read(const size_t &state, const Sym &sym) {
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

    // Default constructor is deleted; please use a vector of Rule to initialize
    // it.
    Grammar() = delete;

    Grammar(const std::vector<Rule> &pRule) {
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

        std::cout << "first set:\n";
        for (auto &first : FIRST_SET) {
            std::cout << "FIRST(" << first.first << ") = ";
            for (auto &firstSyms : first.second) {
                std::cout << " " << firstSyms << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

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

        std::cout << "follow set:\n";
        for (auto &follow : FOLLOW_SET) {
            std::cout << "FOLLOW(" << follow.first << ") = ";
            for (auto &followSyms : follow.second) {
                std::cout << " " << followSyms << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

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
        std::vector<std::set<LR1_Item>> C{CLOSURE({LR1_Item{
            pRule[0],
            0,
            Token{
                "",
                TokenType::TK_EOF,
            },
        }})};
        bool increased = false;
        do {
            increased = false;

            for (std::set<LR1_Item> &itemSet : C) {
                std::vector<std::set<LR1_Item>> newAddToC;
                std::set<Sym> nextSym;
                for (const LR1_Item &item : itemSet) {
                    if (!item.isCurrPosAtEnd()) {
                        nextSym.insert(item.rule.second[item.currPos]);
                    }
                }
                for (const Sym &x : nextSym) {
                    auto &iset = LR1_GOTO(itemSet, x);

                    if (!iset.empty() &&
                        std::find_if(C.begin(), C.end(), [&](std::set<LR1_Item> &aim) {
                            return LR1_Item_Set_Equal(iset, aim);
                        }) == C.end()) {
                        newAddToC.push_back(iset);
                        increased = true;
                    }
                }
                C.insert(C.end(), newAddToC.begin(), newAddToC.end());
            }

        } while (increased);
        C_SET = C;

        // 构造LR(1)的语法分析表
        std::map<size_t, std::map<Sym, std::set<Action>, SymLess>> &lr1_Table = LR1_Table;
        for (size_t statei = 0; statei < C.size(); ++statei) {
            std::set<Sym> nextSet;
            for (const LR1_Item &prod : C[statei]) {
                if (prod.isCurrPosAtEnd()) {
                    if (prod.rule.first == pRule[0].first) {
                        // S' -> S, 完成
                        lr1_Table[statei][Terminal{
                                              Token{
                                                  "",
                                                  TokenType::TK_EOF,
                                              },
                                          }]
                            .insert(Action{Action::ActionType::ACCEPT, (size_t)-1, prod.lookahead});
                    } else {
                        size_t originIndex =
                            (size_t)(std::find(ORIGIN_PRODUCE_RULES.begin(),
                                               ORIGIN_PRODUCE_RULES.end(), prod.rule) -
                                     ORIGIN_PRODUCE_RULES.begin());
                        for (auto &pTerminal : P_TERMINAL_SET) {
                            if (pTerminal == prod.lookahead) {
                                lr1_Table[statei][pTerminal].insert(Action{
                                    Action::ActionType::REDUCE, originIndex, prod.lookahead});
                            }
                        }
                    }
                } else {
                    nextSet.insert(prod.rule.second[prod.currPos]);
                }
            }
            for (auto &next : nextSet) {
                const auto &nextState = LR1_GOTO(C[statei], next);
                for (size_t j = 0; j < C.size(); j++) {
                    auto &IJ = C[j];
                    if (LR1_Item_Set_Equal(IJ, nextState)) {
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
                }
            }
        }
    }

    Ast gen(std::vector<Sym> str) {
        if (str.empty()) {
            std::cout << "\nsource file is empty\n\n";
            return Ast(nullptr);
        }
        std::vector<size_t> stateStack;
        std::vector<AstNode *> nodeStack;
        size_t strPos = 0;
        stateStack.push_back(0);
        while (1) {
            Sym currSym = str[strPos];
            size_t state = stateStack.back();

            const auto &action = LR1_Table_Read(state, currSym);
            const auto &epsilonAction =
                LR1_Table_Read(state, Terminal{Token{"nil", TokenType::NIL}});
            if (action.actionType != Action::ActionType::ERROR ||
                epsilonAction.actionType != Action::ActionType::ERROR) {
                bool isRuleEpsilon = false;
                if (action.actionType == Action::ActionType::ERROR) {
                    isRuleEpsilon = true;
                }
                auto &takenAction = isRuleEpsilon ? epsilonAction : action;
                std::cout << "Current state : " << state << ", symbol : " << currSym << std::endl;
                std::cout << "Current action : " << (int)takenAction.actionType << " "
                          << takenAction.state << std::endl;
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

                        std::cout << rule;
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
                        std::cout << "\nFinished parse procedure.\n";
                        AstNode *headPtr = nodeStack.back();
                        nodeStack.pop_back();
                        if (nodeStack.size()) {
                            // Not an AST-tree.
                            std::cout << "\nNot an ast-tree!\n";
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
                std::cout << "ERROR: state " << state << ", symbol " << currSym
                          << " have an error action.\n";
                std::cout << "At line " << std::get<Terminal>(currSym).token.line << ":";
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
                        std::cout << expectedTerminal << " expected.\n";
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

    bool checkLR1() {
        bool result = false;
        for (auto &state : LR1_Table) {
            size_t stateIndex = state.first;
            for (auto &symCol : state.second) {
                const Sym &sym = symCol.first;
                std::set<Action> &actionSet = symCol.second;
                if (actionSet.size() >= 2) {
                    if (!result) {
                        std::cout << "LR1 has grammatical conflict:\n\n\n";
                        result = true;
                    }
                    std::cout << "In state " << stateIndex << ", symbol " << sym << std::endl;
                    for (auto &action : actionSet) {
                        if (action.actionType == Action::ActionType::REDUCE)
                            std::cout << "R" << action.state << std::endl;
                        else if (action.actionType == Action::ActionType::PUSH_STACK)
                            std::cout << "S" << action.state << std::endl;
                    }
                    std::cout << std::endl << std::endl;
                }
            }
        }
        return result;
    }

    void display_LR1_Table() {
        for (auto &state : LR1_Table) {
            size_t stateIndex = state.first;
            std::cout << std::setw(12) << "";
            for (auto &symCol : state.second) {
                std::cout << std::setw(12) << symCol.first;
            }
            std::cout << std::endl;
            std::cout << std::setw(12) << stateIndex;
            for (auto &symCol : state.second) {
                std::set<Action> &actionSet = symCol.second;
                for (auto &action : actionSet) {
                    switch (action.actionType) {
                        case Action::ActionType::PUSH_STACK: {
                            std::cout << std::setw(12)
                                      << (std::string("S") + std::to_string(action.state));
                            break;
                        }
                        case Action::ActionType::REDUCE: {
                            std::cout << std::setw(12)
                                      << (std::string("R") + std::to_string(action.state));
                            break;
                        }
                        case Action::ActionType::ERROR: {
                            std::cout << std::setw(12) << "E";
                            break;
                        }
                        case Action::ActionType::GOTO: {
                            std::cout << std::setw(12)
                                      << (std::string("G") + std::to_string(action.state));
                            break;
                        }
                        case Action::ActionType::ACCEPT: {
                            std::cout << std::setw(12) << "A";
                            break;
                        }
                    }
                }
            }
            std::cout << std::endl << std::endl;
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const std::set<LR1_Item> itemSet) {
        for (auto &item : itemSet) {
            os << item;
        }
        return os;
    }

    void display_LR1_C_SET() {
        for (size_t i = 0; i < C_SET.size(); ++i) {
            std::cout << "===============\n";
            std::cout << "state " << i << ":\n";
            std::cout << "---------------\n";
            std::cout << C_SET[i];
            std::cout << "===============\n";
        }
    }

    void displayAllRule() {
        std::cout << "\nrules:\n\n";
        for (size_t i = 0; i < ORIGIN_PRODUCE_RULES.size(); ++i) {
            std::cout << i << ":" << ORIGIN_PRODUCE_RULES[i];
        }
        std::cout << "\n\n";
        return;
    }
};

}  // namespace pluma
#endif