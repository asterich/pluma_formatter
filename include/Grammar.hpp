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
#include "Logger.h"
#include "utils/Hash.h"

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

    Action(ActionType type, size_t state, Terminal lookahead);

    bool operator<(const Action &rhs) const;
};

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

struct Grammar {
   private:
    std::map<Nonterminal, std::vector<SymList>> PRODUCE_RULES;
    std::vector<Rule> ORIGIN_PRODUCE_RULES;

   private:
    // Nonterminal for augmented grammar.
    Nonterminal S;

   private:
    std::set<Terminal> P_TERMINAL_SET;
    std::set<Nonterminal> P_NONTERMINAL_SET;

    // Used for cache.
   private:
    std::vector<Sym> symVec;
    std::map<Sym, size_t> symIndexMap;

   private:
    std::map<size_t, std::map<Sym, std::set<Action>, SymLess>> LR1_Table;

    using TerminalSet = std::map<Sym, std::set<Terminal>, pluma::SymLess>;

   private:
    TerminalSet FIRST_SET;
    TerminalSet FOLLOW_SET;

   private:
    std::set<Terminal> &FIRST(const Sym &sym);
    std::set<Terminal> &FOLLOW(const Sym &sym);

   private:
    std::map<std::set<LR1_Item>, std::set<LR1_Item>> CLOSURE_SET;
    std::set<std::set<LR1_Item>> C_SET;
    size_t beginStateIndex;

   private:
    std::set<LR1_Item> &CLOSURE(const std::set<LR1_Item> &i);
    std::set<LR1_Item> &LR1_GOTO(const std::set<LR1_Item> &i, const Sym &sym);

   private:
    Action LR1_Table_Read(const size_t &state, const Sym &sym);

   private:
    // Default constructor is deleted; please use a vector of Rule to initialize
    // it.
    Grammar() = delete;

   public:
    Grammar(const std::string &grammarFile, const std::string &hashFile,
            const std::vector<Rule> &pRule);

   private:
    void genLR1Table(const std::vector<Rule> &pRule);

   private:
    void writeLR1TableToFile(std::string filename);
    bool readLR1TableFromFile(std::string filename);

   public:
    Ast gen(std::vector<Sym> str);

   public:
    friend std::ostream &operator<<(std::ostream &os, const std::set<LR1_Item> itemSet);

   public:
    bool checkLR1();
    void display_LR1_Table();
    void display_LR1_C_SET();
    void displayAllRule();
};

}  // namespace pluma
#endif