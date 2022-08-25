#ifndef AST_HPP_
#define AST_HPP_

#include <iostream>
#include <string>
#include <utility>

#include "Symbol.hpp"

namespace pluma {

struct AstNode {
    static constexpr size_t astIndentSize = 2;

    Sym sym;

    AstNode *parent;
    AstNode *brother;
    AstNode *son;

    // A tail pointer of list "brother" making append operation O(1), do not
    // apply any destruct operation on it.
    AstNode *lastBrother;

    AstNode()
        : sym(Terminal{Token{"", TokenType::NIL}}),
          parent(nullptr),
          brother(nullptr),
          son(nullptr),
          lastBrother(nullptr) {}

    // When a node is created with a sym that makes sense, it has no parent,
    // brother or son, but the "lastBrother" is just itself.
    AstNode(Sym _sym, AstNode *_parent = nullptr, AstNode *_brother = nullptr,
            AstNode *_son = nullptr)
        : sym(std::move(_sym)), parent(_parent), brother(_brother), son(_son), lastBrother(this) {
        // this->display();
    }

    // copy initialization is not allowed
    AstNode(const AstNode &) = delete;

    // move initialization
    AstNode(AstNode &&other)
        : sym(std::move(other.sym)),
          parent(other.parent),
          brother(std::move(other.brother)),
          son(std::move(other.son)),
          lastBrother(other.lastBrother) {
        other.parent = other.brother = other.son = other.lastBrother = nullptr;
    }

    ~AstNode() noexcept {
        this->lastBrother = this->parent = nullptr;
        if (this->brother != nullptr) {
            this->brother->~AstNode();
        }
        if (this->son != nullptr) {
            this->son->~AstNode();
        }
    }

    // copy assignment is not allowed
    AstNode &operator=(const AstNode &) = delete;

    // move assignment
    AstNode &operator=(AstNode &&other) noexcept {
        if (this != &other) {
            this->brother->~AstNode();
            this->son->~AstNode();
            this->sym = other.sym;
            this->parent = other.parent;
            this->brother = other.brother;
            this->son = other.son;
            this->lastBrother = other.lastBrother;
            other.brother = other.son = other.lastBrother = nullptr;
        }
        return *this;
    }

    void appendSon(AstNode *newSon) {
        if (newSon == nullptr) {
            return;
        }
        if (this->son == nullptr) {
            this->son = newSon;
            newSon->parent = this;
        } else {
            this->son->lastBrother->brother = newSon;
            this->son->lastBrother = newSon->lastBrother;
            newSon->parent = this;
        }
        return;
    }

    // 这个应该没什么用……
    void createNewSon(const Sym &sym) {
        AstNode *newSon = new AstNode(sym);
        this->appendSon(newSon);
    }

    void display(size_t indents = 0) {
        std::cout << std::string(indents * astIndentSize, ' ');
        std::cout << this->sym << std::endl;
        if (this->son != nullptr) {
            this->son->display(indents + 1);
        }
        if (this->brother != nullptr) {
            this->brother->display(indents);
        }
    }
};

struct Ast {
    AstNode *head;

    Ast() : head(nullptr) {}

    Ast(AstNode *_head) : head(_head) {}

    Ast(const Ast &) = delete;

    Ast(Ast &&other) : head(std::move(other.head)) {}

    Ast &operator=(const Ast &) = delete;

    Ast &operator=(Ast &&other) {
        if (this == &other) {
            return *this;
        }
        this->head = std::move(other.head);
        return *this;
    }

    ~Ast() {
        if (head != nullptr) head->~AstNode();
    }

    void display() {
        if (head != nullptr) head->display();
    }
};

}  // namespace pluma

#endif