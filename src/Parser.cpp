#include "Parser.h"

pluma::Parser::Parser() {
#if 0
    // FIXME: grammar that causes infinite loop.
    grammarPtr = std::make_unique<Grammar>(std::vector<Rule>{
        /**
         *  S' -> translation-unit
         */
        Nonterminal{"S_AUG"} >> SymList{Nonterminal{"translation-unit"}},

        /**
         * translation-unit -> external-decls
         */
        Nonterminal{"translation-unit"} >> SymList{Nonterminal{"external-decls"}},

        /**
         * external-decls -> external-decls external-decl
         * external-decls -> NIL
         */
        Nonterminal{"external-decls"} >>
            SymList{Nonterminal{"external-decls"}, Nonterminal{"external-decl"}},
        Nonterminal{"external-decls"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * external-decl -> func-def
         * external-decl -> decl
         */
        Nonterminal{"external-decl"} >> SymList{Nonterminal{"func-def"}},
        Nonterminal{"external-decl"} >> SymList{Nonterminal{"decl"}},

        /**
         * func-def -> decl-specs declarator decls compound-stmt
         */
        Nonterminal{"func-def"} >> SymList{Nonterminal{"decl-specs"}, Nonterminal{"declarator"},
                                           Nonterminal{"decls"}, Nonterminal{"compound-stmt"}},

        /**
         * decl-specs -> decl-specs decl-spec
         * decl-specs -> NIL
         */
        Nonterminal{"decl-specs"} >> SymList{Nonterminal{"decl-specs"}, Nonterminal{"decl-spec"}},
        Nonterminal{"decl-specs"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * decls -> decls decl
         * decls -> NIL
         */
        Nonterminal{"decls"} >> SymList{Nonterminal{"decls"}, Nonterminal{"decl"}},
        Nonterminal{"decls"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * decl-spec -> storage-class-spec
         * decl-spec -> type-spec
         * decl-spec -> type-qualifier
         */
        Nonterminal{"decl-spec"} >> SymList{Nonterminal{"storage-class-spec"}},
        Nonterminal{"decl-spec"} >> SymList{Nonterminal{"type-spec"}},
        Nonterminal{"decl-spec"} >> SymList{Nonterminal{"type-qualifier"}},

        /**
         * storage-class-spec -> STATIC | EXTERN | TYPEDEF
         */
        Nonterminal{"storage-class-spec"} >> SymList{Terminal{Token{"static", TokenType::STATIC}}},
        Nonterminal{"storage-class-spec"} >> SymList{Terminal{Token{"extern", TokenType::EXTERN}}},
        Nonterminal{"storage-class-spec"} >>
            SymList{Terminal{Token{"typedef", TokenType::TYPEDEF}}},

        /**
         * type-spec -> VOID | CHAR | SHORT | INT | LONG | FLOAT
         *              | DOUBLE | SIGNED | UNSIGNED
         * type-spec -> struct-or-union-spec
         * type-spec -> enum-spec
         * type-spec -> typedef-name
         */
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"void", TokenType::VOID}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"char", TokenType::CHAR}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"short", TokenType::SHORT}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"int", TokenType::INT}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"long", TokenType::LONG}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"float", TokenType::FLOAT}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"double", TokenType::DOUBLE}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"signed", TokenType::SIGNED}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"unsigned", TokenType::UNSIGNED}}},
        Nonterminal{"type-spec"} >> SymList{Nonterminal{"struct-or-union-spec"}},
        Nonterminal{"type-spec"} >> SymList{Nonterminal{"enum-spec"}},
        Nonterminal{"type-spec"} >> SymList{Nonterminal{"typedef-name"}},

        /**
         * struct-or-union-spec -> struct-or-union IDENTIFIER '{' struct-decls '}'
         * struct-or-union-spec -> struct-or-union '{' struct-decls '}'
         * struct-or-union-spec -> struct-or-union IDENTIFIER
         */
        Nonterminal{"struct-or-union-spec"} >>
            SymList{Nonterminal{"struct-or-union"}, Terminal{Token{"id", TokenType::IDENTIFIER}},
                    Terminal{Token{"{", TokenType::LBRACE}}, Nonterminal{"struct-decls"},
                    Terminal{Token{"}", TokenType::RBRACE}}},
        Nonterminal{"struct-or-union-spec"} >>
            SymList{Nonterminal{"struct-or-union"}, Terminal{Token{"{", TokenType::LBRACE}},
                    Nonterminal{"struct-decls"}, Terminal{Token{"}", TokenType::RBRACE}}},
        Nonterminal{"struct-or-union-spec"} >>
            SymList{Nonterminal{"struct-or-union"}, Terminal{Token{"id", TokenType::IDENTIFIER}}},

        /**
         * struct-or-union -> STRUCT | UNION
         */
        Nonterminal{"struct-or-union"} >> SymList{Terminal{Token{"struct", TokenType::STRUCT}}},
        Nonterminal{"struct-or-union"} >> SymList{Terminal{Token{"union", TokenType::UNION}}},

        /**
         * struct-decls -> struct-decls struct-decl
         * struct-decls -> struct-decl
         */
        // TODO: Check it.
        Nonterminal{"struct-decls"} >>
            SymList{Nonterminal{"struct-decls"}, Nonterminal{"struct-decl"}},
        Nonterminal{"struct-decls"} >> SymList{Nonterminal{"struct-decl"}},

        /**
         * struct-decl -> spec-qualifiers struct-declarator-list
         */
        Nonterminal{"struct-decl"} >>
            SymList{Nonterminal{"spec-qualifiers"}, Nonterminal{"struct-declarator-list"}},

        /**
         * spec-qualifiers -> spec-qualifiers spec-qualifier
         * spec-qualifiers -> NIL
         */
        Nonterminal{"spec-qualifiers"} >>
            SymList{Nonterminal{"spec-qualifiers"}, Nonterminal{"spec-qualifier"}},
        Nonterminal{"spec-qualifiers"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * spec-qualifier -> type-spec
         * spec-qualifier -> type-qualifier
         */
        Nonterminal{"spec-qualifier"} >> SymList{Nonterminal{"type-spec"}},
        Nonterminal{"spec-qualifier"} >> SymList{Nonterminal{"type-qualifier"}},

        /**
         * struct-declarator-list -> struct-declarator
         * struct-declarator-list -> struct-declarator-list ',' struct-declarator
         */
        Nonterminal{"struct-declarator-list"} >> SymList{Nonterminal{"struct-declarator-list"}},
        Nonterminal{"struct-declarator-list"} >> SymList{Nonterminal{"struct-declarator-list"},
                                                         Terminal{Token{",", TokenType::COMMA}},
                                                         Nonterminal{"struct-declarator"}},

        /**
         * struct-declarator -> declarator
         * struct-declarator -> declarator ':' const-expr
         * struct-declarator -> ':' const-expr
         */
        Nonterminal{"struct-declarator"} >> SymList{Nonterminal{"declarator"}},
        Nonterminal{"struct-declarator"} >> SymList{Nonterminal{"declarator"},
                                                    Terminal{Token{":", TokenType::COLON}},
                                                    Nonterminal{"const-expr"}},
        Nonterminal{"struct-declarator"} >>
            SymList{Terminal{Token{":", TokenType::COLON}}, Nonterminal{"const-expr"}},

        /**
         * declarator -> ptr direct-declarator
         * declarator -> direct-declarator
         */
        Nonterminal{"declarator"} >> SymList{Nonterminal{"ptr"}, Nonterminal{"direct-declarator"}},
        Nonterminal{"declarator"} >> SymList{Nonterminal{"direct-declarator"}},

        /**
         * ptr -> * type-qualifiers ptr
         * ptr -> * type-qualifiers
         */
        // FIXME: Seems to be right-recursive, should be checked.
        Nonterminal{"ptr"} >> SymList{Terminal{Token{"*", TokenType::MUL}},
                                      Nonterminal{"type-qualifiers"}, Nonterminal{"ptr"}},
        Nonterminal{"ptr"} >>
            SymList{Terminal{Token{"*", TokenType::MUL}}, Nonterminal{"type-qualifiers"}},

        /**
         * type-qualifiers -> type-qualifiers type-qualifier
         * type-qualifiers -> NIL
         */
        Nonterminal{"type-qualifiers"} >>
            SymList{Nonterminal{"type-qualifiers"}, Nonterminal{"type-qualifier"}},
        Nonterminal{"type-qualifiers"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * type-qualifier -> CONST | VOLATILE
         */
        Nonterminal{"type-qualifier"} >> SymList{Terminal{Token{"const", TokenType::CONST}}},
        Nonterminal{"type-qualifier"} >> SymList{Terminal{Token{"volatile", TokenType::VOLATILE}}},

        /**
         * direct-declarator -> IDENTIFIER
         * direct-declarator -> '(' declarator ')'
         * direct-declarator -> direct-declarator '[' const-expr ']'
         * direct-declarator -> direct-declarator '[' ']'
         * direct-declarator -> direct-declarator '(' param-type-list ')'
         * direct-declarator -> direct-declarator '(' identifiers ')'
         */
        Nonterminal{"direct-declarator"} >> SymList{Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"direct-declarator"} >> SymList{Terminal{Token{"(", TokenType::LPAREN}},
                                                    Nonterminal{"declarator"},
                                                    Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"direct-declarator"} >>
            SymList{Nonterminal{"direct-declarator"}, Terminal{Token{"[", TokenType::LSBRACKET}},
                    Nonterminal{"const-expr"}, Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"direct-declarator"} >> SymList{Nonterminal{"direct-declarator"},
                                                    Terminal{Token{"[", TokenType::LSBRACKET}},
                                                    Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"direct-declarator"} >>
            SymList{Nonterminal{"direct-declarator"}, Terminal{Token{"(", TokenType::LPAREN}},
                    Nonterminal{"param-type-list"}, Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"direct-declarator"} >>
            SymList{Nonterminal{"direct-declarator"}, Terminal{Token{"(", TokenType::LPAREN}},
                    Nonterminal{"identifiers"}, Terminal{Token{")", TokenType::RPAREN}}},

        /**
         * identifiers -> identifiers IDENTIFIER
         * idnetifiers -> NIL
         */
        Nonterminal{"identifiers"} >>
            SymList{Nonterminal{"identifiers"}, Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"identifiers"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * const-expr -> cond-expr
         */
        Nonterminal{"const-expr"} >> SymList{Nonterminal{"cond-expr"}},

        /**
         * cond-expr -> logical-or-expr
         * cond-expr -> logical-or-expr '?' expr ':' cond-expr
         */
        Nonterminal{"cond-expr"} >> SymList{Nonterminal{"logical-or-expr"}},
        Nonterminal{"cond-expr"} >>
            SymList{Nonterminal{"logical-or-expr"}, Terminal{Token{"?", TokenType::QUESTION_MARK}},
                    Nonterminal{"expr"}, Terminal{Token{":", TokenType::COLON}},
                    Nonterminal{"cond-expr"}},

        /**
         * logical-or-expr -> logical-and-expr
         * logical-or-expr -> logical-or-expr '||' logical-and-expr
         */
        Nonterminal{"logical-or-expr"} >> SymList{Nonterminal{"logical-and-expr"}},
        Nonterminal{"logical-or-expr"} >> SymList{Nonterminal{"logical-or-expr"},
                                                  Terminal{Token{"||", TokenType::OR}},
                                                  Nonterminal{"logical-and-expr"}},

        /**
         * logical-and-expr -> inclusive-or-expr
         * logical-and-expr -> logical-and-expr '&&' inclusive-or-expr
         */
        Nonterminal{"logical-and-expr"} >> SymList{Nonterminal{"inclusive-or-expr"}},
        Nonterminal{"logical-and-expr"} >> SymList{Nonterminal{"logical-and-expr"},
                                                   Terminal{Token{"&&", TokenType::AND}},
                                                   Nonterminal{"inclusive-or-expr"}},

        /**
         * inclusive-or-expr -> exclusive-or-expr
         * inclusive-or-expr -> inclusive-or-expr '|' exclusive-or-expr
         */
        Nonterminal{"inclusive-or-expr"} >> SymList{Nonterminal{"exclusive-or-expr"}},
        Nonterminal{"inclusive-or-expr"} >> SymList{Nonterminal{"inclusive-or-expr"},
                                                    Terminal{Token{"|", TokenType::BITOR}},
                                                    Nonterminal{"exclusive-or-expr"}},

        /**
         * exclusive-or-expr -> and-expr
         * exclusive-or-expr -> exclusive-or-expr '^' and-expr
         */
        Nonterminal{"exclusive-or-expr"} >> SymList{Nonterminal{"and-expr"}},
        Nonterminal{"exclusive-or-expr"} >> SymList{Nonterminal{"exclusive-or-expr"},
                                                    Terminal{Token{"^", TokenType::BITXOR}},
                                                    Nonterminal{"and-expr"}},

        /**
         * and-expr -> eq-expr
         * and-expr -> and-expr '&' eq-expr
         */
        Nonterminal{"and-expr"} >> SymList{Nonterminal{"eq-expr"}},
        Nonterminal{"and-expr"} >> SymList{Nonterminal{"and-expr"},
                                           Terminal{Token{"&", TokenType::BITAND}},
                                           Nonterminal{"eq-expr"}},

        /**
         * eq-expr -> relational-expr
         * eq-expr -> eq-expr '==' relational-expr
         * eq-expr -> eq-expr '!=' relational-expr
         */
        Nonterminal{"eq-expr"} >> SymList{Nonterminal{"relational-expr"}},
        Nonterminal{"eq-expr"} >> SymList{Nonterminal{"eq-expr"},
                                          Terminal{Token{"==", TokenType::EQ}},
                                          Nonterminal{"relational-expr"}},
        Nonterminal{"eq-expr"} >> SymList{Nonterminal{"eq-expr"},
                                          Terminal{Token{"!=", TokenType::NEQ}},
                                          Nonterminal{"relational-expr"}},

        /**
         * relational-expr -> shift-expr
         * relational-expr -> relational-expr '<' shift-expr
         * relational-expr -> relational-expr '>' shift-expr
         * relational-expr -> relational-expr '<=' shift-expr
         * relational-expr -> relational-expr '>=' shift-expr
         */
        Nonterminal{"relational-expr"} >> SymList{Nonterminal{"shift-expr"}},
        Nonterminal{"relational-expr"} >> SymList{Nonterminal{"relational-expr"},
                                                  Terminal{Token{"<", TokenType::LT}},
                                                  Nonterminal{"shift-expr"}},
        Nonterminal{"relational-expr"} >> SymList{Nonterminal{"relational-expr"},
                                                  Terminal{Token{">", TokenType::GT}},
                                                  Nonterminal{"shift-expr"}},
        Nonterminal{"relational-expr"} >> SymList{Nonterminal{"relational-expr"},
                                                  Terminal{Token{"<=", TokenType::LE}},
                                                  Nonterminal{"shift-expr"}},
        Nonterminal{"relational-expr"} >> SymList{Nonterminal{"relational-expr"},
                                                  Terminal{Token{">=", TokenType::GE}},
                                                  Nonterminal{"shift-expr"}},

        /**
         * shift-expr -> add-expr
         * shift-expr -> shift-expr '<<' add-expr
         * shift-expr -> shift-expr '>>' add-expr
         */
        Nonterminal{"shift-expr"} >> SymList{Nonterminal{"add-expr"}},
        Nonterminal{"shift-expr"} >> SymList{Nonterminal{"shift-expr"},
                                             Terminal{Token{"<<", TokenType::LSHIFT}},
                                             Nonterminal{"add-expr"}},
        Nonterminal{"shift-expr"} >> SymList{Nonterminal{"shift-expr"},
                                             Terminal{Token{">>", TokenType::RSHIFT}},
                                             Nonterminal{"add-expr"}},

        /**
         * add-expr -> mul-expr
         * add-expr -> add-expr '+' mul-expr
         * add-expr -> add-expr '-' mul-expr
         */
        Nonterminal{"add-expr"} >> SymList{Nonterminal{"mul-expr"}},
        Nonterminal{"add-expr"} >> SymList{Nonterminal{"add-expr"},
                                           Terminal{Token{"+", TokenType::ADD}},
                                           Nonterminal{"mul-expr"}},
        Nonterminal{"add-expr"} >> SymList{Nonterminal{"add-expr"},
                                           Terminal{Token{"-", TokenType::SUB}},
                                           Nonterminal{"mul-expr"}},

        /**
         * mul-expr -> cast-expr
         * mul-expr -> mul-expr '*' cast-expr
         * mul-expr -> mul-expr '/' cast-expr
         * mul-expr -> mul-expr '%' cast-expr
         */
        Nonterminal{"mul-expr"} >> SymList{Nonterminal{"cast-expr"}},
        Nonterminal{"mul-expr"} >> SymList{Nonterminal{"mul-expr"},
                                           Terminal{Token{"*", TokenType::MUL}},
                                           Nonterminal{"cast-expr"}},
        Nonterminal{"mul-expr"} >> SymList{Nonterminal{"mul-expr"},
                                           Terminal{Token{"/", TokenType::DIV}},
                                           Nonterminal{"cast-expr"}},
        Nonterminal{"mul-expr"} >> SymList{Nonterminal{"mul-expr"},
                                           Terminal{Token{"%", TokenType::MOD}},
                                           Nonterminal{"cast-expr"}},

        /**
         * cast-expr -> unary-expr
         * cast-expr -> '(' type-name ')' cast-expr
         */
        Nonterminal{"cast-expr"} >> SymList{Nonterminal{"unary-expr"}},
        Nonterminal{"cast-expr"} >>
            SymList{Terminal{Token{"(", TokenType::LPAREN}}, Nonterminal{"type-name"},
                    Terminal{Token{")", TokenType::RPAREN}}, Nonterminal{"cast-expr"}},

        /**
         * unary-expr -> postfix-expr
         * unary-expr -> '++' unary-expr
         * unary-expr -> '--' unary-expr
         * unary-expr -> unary-op cast-expr
         * unary-expr -> SIZEOF unary-expr
         * unary-expr -> SIZEOF type-name
         */
        Nonterminal{"unary-expr"} >> SymList{Nonterminal{"postfix-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"++", TokenType::INCR}}, Nonterminal{"unary-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"--", TokenType::DECR}}, Nonterminal{"unary-expr"}},
        Nonterminal{"unary-expr"} >> SymList{Nonterminal{"unary-op"}, Nonterminal{"cast-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"sizeof", TokenType::SIZEOF}}, Nonterminal{"unary-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"sizeof", TokenType::SIZEOF}}, Nonterminal{"type-name"}},

        /**
         * postfix-expr -> primary-expr
         * postfix-expr -> postfix-expr '[' expr ']'
         * postfix-expr -> postfix-expr '(' assign-exprs ')'
         * postfix-expr -> postfix-expr '.' IDENTIFIER
         * postfix-expr -> postfix-expr '->' IDENTIFIER
         * postfix-expr -> postfix-expr '++'
         * postfix-expr -> postfix-expr '--'
         */
        Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"primary-expr"}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"[", TokenType::LSBRACKET}},
                    Nonterminal{"expr"}, Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"(", TokenType::LPAREN}},
                    Nonterminal{"assign-exprs"}, Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"postfix-expr"},
                                               Terminal{Token{".", TokenType::PERIOD}},
                                               Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"postfix-expr"},
                                               Terminal{Token{"->", TokenType::ARROW}},
                                               Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"++", TokenType::INCR}}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"--", TokenType::DECR}}},

        /**
         * assign-exprs -> assign-exprs assign-expr
         * assign-exprs -> NIL
         */
        Nonterminal{"assign-exprs"} >>
            SymList{Nonterminal{"assign-exprs"}, Nonterminal{"assign-expr"}},
        Nonterminal{"assign-exprs"} >> SymList{Terminal{Token{"", TokenType::NIL}}},

        /**
         * primary-expr -> IDENTIFIER
         * primary-expr -> const
         * primary-expr -> STRING_CONST
         * primary-expr -> '(' expr ')'
         */
        Nonterminal{"primary-expr"} >> SymList{Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"primary-expr"} >>
            SymList{Terminal{Token{"str-const", TokenType::STRING_CONST}}},
        Nonterminal{"primary-expr"} >> SymList{Nonterminal{"const"}},
        Nonterminal{"primary-expr"} >> SymList{Terminal{Token{"(", TokenType::LPAREN}},
                                               Nonterminal{"expr"},
                                               Terminal{Token{")", TokenType::RPAREN}}},

        /**
         * const -> INT_CONST | CHAR_CONST | FLOAT_CONST
         * const -> enum-const
         */
        Nonterminal{"const"} >> SymList{Terminal{Token{"int-const", TokenType::INT_CONST}}},
        Nonterminal{"const"} >> SymList{Terminal{Token{"char-const", TokenType::CHAR_CONST}}},
        Nonterminal{"const"} >> SymList{Terminal{Token{"float-const", TokenType::FLOAT_CONST}}},
        Nonterminal{"const"} >> SymList{Nonterminal{"enum-const"}},

        /**
         * enum-const -> IDENTIFIER
         * NOTE: the original BNF doesn't specify the "enum-const" non-terminal,
         * but it can be deduced that "enum-const" is an "IDENTIFIER".
         */
        Nonterminal{"enum-const"} >> SymList{Terminal{Token{"id", TokenType::IDENTIFIER}}},

        /**
         * expr -> assign-expr
         * expr -> expr ',' assign-expr
         */
        Nonterminal{"expr"} >> SymList{Nonterminal{"assign-expr"}},
        Nonterminal{"expr"} >> SymList{Nonterminal{"expr"}, Terminal{Token{",", TokenType::COMMA}},
                                       Nonterminal{"assign-expr"}},

        /**
         * assign-expr -> cond-expr
         * assign-expr -> unary-expr assign-op assign-expr
         */
        Nonterminal{"assign-expr"} >> SymList{Nonterminal{"cond-expr"}},
        Nonterminal{"assign-expr"} >> SymList{Nonterminal{"unary-expr"}, Nonterminal{"assign-op"},
                                              Nonterminal{"assign-expr"}},

        /**
         * assign-op -> '=' | '*=' | '/=' | '%=' | '+=' | '-='
         *              | '<<=' | '>>=' | '&=' | '^=' | '|='
         */
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"=", TokenType::ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"*=", TokenType::MUL_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"/=", TokenType::DIV_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"%=", TokenType::MOD_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"+=", TokenType::ADD_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"-=", TokenType::SUB_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"<<=", TokenType::LSHIFT_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{">>=", TokenType::RSHIFT_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"&=", TokenType::BITAND_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"^=", TokenType::BITXOR_ASSIGN}}},
        Nonterminal{"assign-op"} >> SymList{Terminal{Token{"|=", TokenType::BITOR_ASSIGN}}},

        /**
         * unary-op -> '&' | '*' | '+' | '-' | '~' | '!'
         */
        Nonterminal{"unary-op"} >> SymList{Terminal{Token{"&", TokenType::BITAND}}},
        Nonterminal{"unary-op"} >> SymList{Terminal{Token{"*", TokenType::MUL}}},
        Nonterminal{"unary-op"} >> SymList{Terminal{Token{"+", TokenType::ADD}}},
        Nonterminal{"unary-op"} >> SymList{Terminal{Token{"-", TokenType::SUB}}},
        Nonterminal{"unary-op"} >> SymList{Terminal{Token{"~", TokenType::BITNOT}}},
        Nonterminal{"unary-op"} >> SymList{Terminal{Token{"!", TokenType::NOT}}},

        /**
         * type-name -> spec-qualifiers+ abstract-declarator
         * type-name -> spec-qualifiers+
         */
        Nonterminal{"type-name"} >>
            SymList{Nonterminal{"spec-qualifiers+"}, Nonterminal{"abstract-declarator"}},
        Nonterminal{"type-name"} >> SymList{Nonterminal{"spec-qualifiers+"}},

        /**
         * spec-qualifiers+ -> spec-qualifiers+ spec-qualifier
         * spec-qualifiers+ -> spec-qualifier
         */
        Nonterminal{"spec-qualifiers+"} >>
            SymList{Nonterminal{"spec-qualifiers+"}, Nonterminal{"spec-qualifier"}},
        Nonterminal{"spec-qualifiers+"} >> SymList{Nonterminal{"spec-qualifier"}},

        /**
         * param-type-list -> param-list
         * param-type-list -> param-list ',' '...'
         * NOTE: the second rule(VA_ARGS) hasn't been supported yet, ignore it.
         */
        Nonterminal{"param-type-list"} >> SymList{Nonterminal{"param-list"}},

        /**
         * param-list -> param-decl
         * param-list -> param-list ',' param-decl
         */
        Nonterminal{"param-list"} >> SymList{Nonterminal{"param-decl"}},
        Nonterminal{"param-list"} >> SymList{Nonterminal{"param-list"},
                                             Terminal{Token{",", TokenType::COMMA}},
                                             Nonterminal{"param-decl"}},

        /**
         * param-decl -> decl-specs+ declarator
         * param-decl -> decl-specs+ abstract-declarator
         * param-decl -> decl-specs+
         */
        Nonterminal{"param-decl"} >> SymList{Nonterminal{"decl-specs+"}, Nonterminal{"declarator"}},
        Nonterminal{"param-decl"} >>
            SymList{Nonterminal{"decl-specs+"}, Nonterminal{"abstract-declarator"}},
        Nonterminal{"param-decl"} >> SymList{Nonterminal{"decl-specs+"}},

        /**
         * decl-specs+ -> decl-specs+ decl-spec
         * decl-specs+ -> decl-spec
         */
        Nonterminal{"decl-specs+"} >> SymList{Nonterminal{"decl-specs+"}, Nonterminal{"decl-spec"}},
        Nonterminal{"decl-specs+"} >> SymList{Nonterminal{"decl-spec"}},

        /**
         * abstract-declarator -> pointer
         * abstract-declarator -> pointer direct-abstract-declarator
         * abstract-declarator -> direct-abstract-declarator
         */
        Nonterminal{"abstract-declarator"} >> SymList{Nonterminal{"pointer"}},
        Nonterminal{"abstract-declarator"} >> SymList{Nonterminal{"direct-abstract-declarator"}},
        Nonterminal{"abstract-declarator"} >>
            SymList{Nonterminal{"pointer"}, Nonterminal{"direct-abstract-declarator"}},

        /**
         * direct-abstract-declarator -> '(' abstract-declarator ')'
         * direct-abstract-declarator -> direct-abstract-declarator? '[' const-expr? ']'
         * direct-abstract-declarator -> direct-abstract-declarator? '(' param-type-list?
         * ')'
         */
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Terminal{Token{"(", TokenType::LPAREN}}, Nonterminal{"abstract-declarator"},
                    Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Nonterminal{"direct-abstract-declarator"},
                    Terminal{Token{"[", TokenType::LSBRACKET}}, Nonterminal{"const-expr"},
                    Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Terminal{Token{"[", TokenType::LSBRACKET}}, Nonterminal{"const-expr"},
                    Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Nonterminal{"direct-abstract-declarator"},
                    Terminal{Token{"[", TokenType::LSBRACKET}},
                    Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Terminal{Token{"[", TokenType::LSBRACKET}},
                    Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Nonterminal{"direct-abstract-declarator"},
                    Terminal{Token{"(", TokenType::LPAREN}}, Nonterminal{"param-type-list"},
                    Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Terminal{Token{"(", TokenType::LPAREN}}, Nonterminal{"param-type-list"},
                    Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Nonterminal{"direct-abstract-declarator"},
                    Terminal{Token{"(", TokenType::LPAREN}},
                    Terminal{Token{")", TokenType::RPAREN}}},
        Nonterminal{"direct-abstract-declarator"} >>
            SymList{Terminal{Token{"(", TokenType::LPAREN}},
                    Terminal{Token{")", TokenType::RPAREN}}},

        /**
         * enum-spec -> ENUM IDENTIFIER '{' enumerator-list '}'
         * enum-spec -> ENUM '{' enumerator-list '}'
         * enum-spec -> ENUM IDENTIFIER
         */
        Nonterminal{"enum-spec"} >>
            SymList{
                Terminal{Token{"enum", TokenType::ENUM}},
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{"{", TokenType::LBRACE}},
                Nonterminal{"enumerator-list"},
                Terminal{Token{"}", TokenType::RBRACE}},
            },
        Nonterminal{"enum-spec"} >>
            SymList{
                Terminal{Token{"enum", TokenType::ENUM}},
                Terminal{Token{"{", TokenType::LBRACE}},
                Nonterminal{"enumerator-list"},
                Terminal{Token{"}", TokenType::RBRACE}},
            },
        Nonterminal{"enum-spec"} >>
            SymList{
                Terminal{Token{"enum", TokenType::ENUM}},
                Terminal{Token{"id", TokenType::IDENTIFIER}},
            },

        /**
         * enumerator-list -> enumerator
         * enumerator-list -> enumerator-list ',' enumerator
         */
        Nonterminal{"enumerator-list"} >> SymList{Nonterminal{"enumerator"}},
        Nonterminal{"enumerator-list"} >>
            SymList{
                Nonterminal{"enumerator-list"},
                Terminal{Token{",", TokenType::COMMA}},
                Nonterminal{"enumerator"},
            },

        /**
         * enumerator -> IDENTIFIER
         * enumerator -> IDENTIFIER '=' const-expr
         */
        Nonterminal{"enumerator"} >>
            SymList{
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{"=", TokenType::ASSIGN}},
                Nonterminal{"const-expr"},
            },

        /**
         * typedef-name -> IDENTIFIER
         */
        Nonterminal{"typedef-name"} >>
            SymList{
                Terminal{Token{"id", TokenType::IDENTIFIER}},
            },

        /**
         * decl -> decl-specs+ init-declarators
         */
        Nonterminal{"decl"} >>
            SymList{
                Nonterminal{"decl-specs+"},
                Nonterminal{"init-declarators"},
            },

        /**
         * init-declarators -> init-declarators init-declarator
         * init-declarators -> NIL
         */
        Nonterminal{"init-declarators"} >>
            SymList{
                Nonterminal{"init-declarators"},
                Nonterminal{"init-declarator"},
            },
        Nonterminal{"init-declarators"} >>
            SymList{
                Terminal{Token{"", TokenType::NIL}},
            },

        /**
         * init-declarator -> declarator
         * init-declarator -> declarator '=' initializer
         */
        Nonterminal{"init-declarator"} >>
            SymList{
                Nonterminal{"declarator"},
            },
        Nonterminal{"init-declarator"} >>
            SymList{
                Nonterminal{"declarator"},
                Terminal{Token{"=", TokenType::ASSIGN}},
                Nonterminal{"initializer"},
            },

        /**
         * initializer -> assign-expr
         * initializer -> '{' initializer-list '}'
         * initializer -> '{' initializer-list ',' '}'
         */
        Nonterminal{"initializer"} >>
            SymList{
                Nonterminal{"assign-expr"},
            },
        Nonterminal{"initializer"} >>
            SymList{
                Terminal{Token{"{", TokenType::LBRACE}},
                Nonterminal{"initializer-list"},
                Terminal{Token{"}", TokenType::RBRACE}},
            },
        Nonterminal{"initializer"} >>
            SymList{
                Terminal{Token{"{", TokenType::LBRACE}},
                Nonterminal{"initializer-list"},
                Terminal{Token{",", TokenType::COMMA}},
                Terminal{Token{"}", TokenType::RBRACE}},
            },

        /**
         * initializer-list -> initializer
         * initializer-list -> initializer-list ',' initializer
         */
        Nonterminal{"initializer-list"} >>
            SymList{
                Nonterminal{"initializer"},
            },
        Nonterminal{"initializer-list"} >>
            SymList{
                Nonterminal{"initializer-list"},
                Terminal{Token{",", TokenType::COMMA}},
                Nonterminal{"initializer"},
            },

        /**
         * compound-stmt -> '{' decls stmts '}'
         */
        Nonterminal{"compound-stmt"} >>
            SymList{
                Terminal{Token{"{", TokenType::LBRACE}},
                Nonterminal{"decls"},
                Nonterminal{"stmts"},
                Terminal{Token{"}", TokenType::RBRACE}},
            },

        /**
         * stmts -> stmts stmt
         * stmts -> NIL
         */
        Nonterminal{"stmts"} >>
            SymList{
                Nonterminal{"stmts"},
                Nonterminal{"stmt"},
            },
        Nonterminal{"stmts"} >>
            SymList{
                Terminal{Token{"", TokenType::NIL}},
            },

        /**
         * stmt -> label-stmt
         * stmt -> expr-stmt
         * stmt -> compound-stmt
         * stmt -> selection-stmt
         * stmt -> iter-stmt
         * stmt -> jump-stmt
         */
        Nonterminal{"stmt"} >> SymList{Nonterminal{"label-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"expr-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"compound-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"selection-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"iter-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"jump-stmt"}},

        /**
         * label-stmt -> IDENTIFIER ':' stmt
         * label-stmt -> CASE const-expr ':' stmt
         * label-stmt -> DEFAULT ':' stmt
         */
        Nonterminal{"label-stmt"} >>
            SymList{
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{":", TokenType::COLON}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"label-stmt"} >>
            SymList{
                Terminal{Token{"case", TokenType::CASE}},
                Nonterminal{"const-expr"},
                Terminal{Token{":", TokenType::COLON}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"label-stmt"} >>
            SymList{
                Terminal{Token{"default", TokenType::DEFAULT}},
                Terminal{Token{":", TokenType::COLON}},
                Nonterminal{"stmt"},
            },

        /**
         * expr-stmt -> expr? ';'
         */
        Nonterminal{"expr-stmt"} >>
            SymList{
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"expr-stmt"} >>
            SymList{
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

        /**
         * selection-stmt -> IF '(' expr ')' stmt
         * selection-stmt -> IF '(' expr ')' stmt ELSE stmt
         * selection-stmt -> SWITCH '(' expr ')' stmt
         */
        Nonterminal{"selection-stmt"} >>
            SymList{
                Terminal{Token{"if", TokenType::IF}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"selection-stmt"} >>
            SymList{
                Terminal{Token{"if", TokenType::IF}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
                Terminal{Token{"else", TokenType::ELSE}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"selection-stmt"} >>
            SymList{
                Terminal{Token{"switch", TokenType::SWITCH}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },

        /**
         * iter-stmt -> WHILE '(' expr ')' stmt
         * iter-stmt -> DO stmt WHILE '(' expr ')' ';'
         * iter-stmt -> FOR '(' expr? ';' expr? ';' expr? ')' stmt
         */
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"while", TokenType::WHILE}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"do", TokenType::DO}},
                Nonterminal{"stmt"},
                Terminal{Token{"while", TokenType::WHILE}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },

        /**
         * jump-stmt -> GOTO IDENTIFIER ';'
         * jump-stmt -> CONTINUE ';'
         * jump-stmt -> BREAK ';'
         * jump-stmt -> RETURN expr? ';'
         */
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"goto", TokenType::GOTO}},
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"continue", TokenType::CONTINUE}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"break", TokenType::BREAK}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"return", TokenType::RETURN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"return", TokenType::RETURN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

    });
#endif

    grammarPtr = std::make_unique<Grammar>(std::vector<Rule>{

        Nonterminal{"program-aug"} >> SymList{Nonterminal{"program"}},

        Nonterminal{"program"} >> SymList{Nonterminal{"ext-defs"}},

        Nonterminal{"ext-defs"} >> SymList{Nonterminal{"ext-defs"}, Nonterminal{"ext-def"}},
        Nonterminal{"ext-defs"} >> SymList{Nonterminal{"ext-def"}},

        Nonterminal{"ext-def"} >> SymList{Nonterminal{"var-def"}},
        Nonterminal{"ext-def"} >> SymList{Nonterminal{"func-def"}},

        Nonterminal{"var-def"} >>
            SymList{
                Nonterminal{"type-spec"},
                Nonterminal{"vars"},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"int", TokenType::INT}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"char", TokenType::CHAR}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"short", TokenType::SHORT}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"float", TokenType::FLOAT}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"double", TokenType::DOUBLE}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"unsigned", TokenType::UNSIGNED}}},
        Nonterminal{"type-spec"} >> SymList{Terminal{Token{"void", TokenType::VOID}}},

        Nonterminal{"vars"} >>
            SymList{
                Nonterminal{"vars"},
                Terminal{Token{",", TokenType::COMMA}},
                Nonterminal{"var"},
            },
        Nonterminal{"vars"} >> SymList{Nonterminal{"var"}},

        Nonterminal{"var"} >> SymList{Terminal{Token{"id", TokenType::IDENTIFIER}}},

        Nonterminal{"func-def"} >>
            SymList{
                Nonterminal{"type-spec"},
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"params"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"compound-stmt"},
            },

        Nonterminal{"params"} >>
            SymList{
                Nonterminal{"params"},
                Terminal{Token{",", TokenType::COMMA}},
                Nonterminal{"param"},
            },
        Nonterminal{"params"} >>
            SymList{
                Nonterminal{"param"},
            },

        Nonterminal{"param"} >>
            SymList{
                Nonterminal{"type-spec"},
                Terminal{Token{"id", TokenType::IDENTIFIER}},
            },
        Nonterminal{"param"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

        Nonterminal{"compound-stmt"} >>
            SymList{
                Terminal{Token{"{", TokenType::LBRACE}},
                Nonterminal{"var-defs"},
                Nonterminal{"stmts"},
                Terminal{Token{"}", TokenType::RBRACE}},
            },

        Nonterminal{"var-defs"} >>
            SymList{
                Nonterminal{"var-defs"},
                Nonterminal{"var-def"},
            },
        Nonterminal{"var-defs"} >>
            SymList{
                Terminal{Token{"nil", TokenType::NIL}},
            },

        Nonterminal{"stmts"} >>
            SymList{
                Nonterminal{"stmts"},
                Nonterminal{"stmt"},
            },
        Nonterminal{"stmts"} >>
            SymList{
                Terminal{Token{"nil", TokenType::NIL}},
            },

        /**
         * stmt -> label-stmt
         * stmt -> expr-stmt
         * stmt -> compound-stmt
         * stmt -> selection-stmt
         * stmt -> iter-stmt
         * stmt -> jump-stmt
         */
        Nonterminal{"stmt"} >> SymList{Nonterminal{"label-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"expr-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"compound-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"selection-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"iter-stmt"}},
        Nonterminal{"stmt"} >> SymList{Nonterminal{"jump-stmt"}},

        /**
         * label-stmt -> IDENTIFIER ':' stmt
         * label-stmt -> CASE const-expr ':' stmt
         * label-stmt -> DEFAULT ':' stmt
         */
        Nonterminal{"label-stmt"} >>
            SymList{
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{":", TokenType::COLON}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"label-stmt"} >>
            SymList{
                Terminal{Token{"case", TokenType::CASE}},
                Nonterminal{"expr"},
                Terminal{Token{":", TokenType::COLON}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"label-stmt"} >>
            SymList{
                Terminal{Token{"default", TokenType::DEFAULT}},
                Terminal{Token{":", TokenType::COLON}},
                Nonterminal{"stmt"},
            },

        /**
         * expr-stmt -> expr? ';'
         */
        Nonterminal{"expr-stmt"} >>
            SymList{
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"expr-stmt"} >>
            SymList{
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

        /**
         * selection-stmt -> IF '(' expr ')' stmt
         * selection-stmt -> IF '(' expr ')' stmt ELSE stmt
         * selection-stmt -> SWITCH '(' expr ')' stmt
         */
        Nonterminal{"selection-stmt"} >>
            SymList{
                Terminal{Token{"if", TokenType::IF}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"selection-stmt"} >>
            SymList{
                Terminal{Token{"if", TokenType::IF}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
                Terminal{Token{"else", TokenType::ELSE}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"selection-stmt"} >>
            SymList{
                Terminal{Token{"switch", TokenType::SWITCH}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },

        /**
         * iter-stmt -> WHILE '(' expr ')' stmt
         * iter-stmt -> DO stmt WHILE '(' expr ')' ';'
         * iter-stmt -> FOR '(' expr? ';' expr? ';' expr? ')' stmt
         */
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"while", TokenType::WHILE}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"do", TokenType::DO}},
                Nonterminal{"stmt"},
                Terminal{Token{"while", TokenType::WHILE}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },
        Nonterminal{"iter-stmt"} >>
            SymList{
                Terminal{Token{"for", TokenType::FOR}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{";", TokenType::SEMICOLON}},
                Terminal{Token{")", TokenType::RPAREN}},
                Nonterminal{"stmt"},
            },

        /**
         * jump-stmt -> GOTO IDENTIFIER ';'
         * jump-stmt -> CONTINUE ';'
         * jump-stmt -> BREAK ';'
         * jump-stmt -> RETURN expr? ';'
         */
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"goto", TokenType::GOTO}},
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"continue", TokenType::CONTINUE}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"break", TokenType::BREAK}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },
        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"return", TokenType::RETURN}},
                Nonterminal{"expr"},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

        Nonterminal{"jump-stmt"} >>
            SymList{
                Terminal{Token{"return", TokenType::RETURN}},
                Terminal{Token{";", TokenType::SEMICOLON}},
            },

        Nonterminal{"expr"} >>
            SymList{
                Nonterminal{"unary-expr"},
                Nonterminal{"binary-op"},
                Nonterminal{"expr"},
            },
        Nonterminal{"expr"} >>
            SymList{
                Nonterminal{"unary-expr"},
                Nonterminal{"assign-op"},
                Nonterminal{"expr"},
            },
        Nonterminal{"expr"} >>
            SymList{
                Nonterminal{"unary-expr"},
            },
        Nonterminal{"expr"} >> SymList{Nonterminal{"expr"},
                                       Terminal{Token{"?", TokenType::QUESTION_MARK}},
                                       Nonterminal{"expr"}, Terminal{Token{":", TokenType::COLON}},
                                       Nonterminal{"expr"}},

        /**
         * unary-expr -> postfix-expr
         * unary-expr -> '++' unary-expr
         * unary-expr -> '--' unary-expr
         * unary-expr -> unary-op cast-expr
         * unary-expr -> SIZEOF unary-expr
         * unary-expr -> SIZEOF type-name
         */
        Nonterminal{"unary-expr"} >> SymList{Nonterminal{"postfix-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"++", TokenType::INCR}}, Nonterminal{"unary-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"--", TokenType::DECR}}, Nonterminal{"unary-expr"}},
        Nonterminal{"unary-expr"} >> SymList{Nonterminal{"unary-op"}, Nonterminal{"cast-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"sizeof", TokenType::SIZEOF}}, Nonterminal{"unary-expr"}},
        Nonterminal{"unary-expr"} >>
            SymList{Terminal{Token{"sizeof", TokenType::SIZEOF}}, Nonterminal{"type-name"}},

        /**
         * postfix-expr -> primary-expr
         * postfix-expr -> postfix-expr '[' expr ']'
         * postfix-expr -> postfix-expr '(' assign-exprs ')'
         * postfix-expr -> postfix-expr '.' IDENTIFIER
         * postfix-expr -> postfix-expr '->' IDENTIFIER
         * postfix-expr -> postfix-expr '++'
         * postfix-expr -> postfix-expr '--'
         */
        Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"primary-expr"}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"[", TokenType::LSBRACKET}},
                    Nonterminal{"expr"}, Terminal{Token{"]", TokenType::RSBRACKET}}},
        Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"postfix-expr"},
                                               Terminal{Token{".", TokenType::PERIOD}},
                                               Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"postfix-expr"},
                                               Terminal{Token{"->", TokenType::ARROW}},
                                               Terminal{Token{"id", TokenType::IDENTIFIER}}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"++", TokenType::INCR}}},
        Nonterminal{"postfix-expr"} >>
            SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"--", TokenType::DECR}}},

        Nonterminal{"primary-expr"} >>
            SymList{
                Terminal{Token{"id", TokenType::IDENTIFIER}},
            },
        Nonterminal{"primary-expr"} >>
            SymList{
                Terminal{Token{"INT_CONST", TokenType::INT_CONST}},
            },
        Nonterminal{"primary-expr"} >>
            SymList{
                Terminal{Token{"FLOAT_CONST", TokenType::FLOAT_CONST}},
            },
        Nonterminal{"primary-expr"} >>
            SymList{
                Terminal{Token{"CHAR_CONST", TokenType::CHAR_CONST}},
            },
        Nonterminal{"primary-expr"} >>
            SymList{
                Terminal{Token{"STRING_CONST", TokenType::STRING_CONST}},
            },
        Nonterminal{"primary-expr"} >>
            SymList{
                Nonterminal{"func-call"},
            },
        Nonterminal{"primary-expr"} >>
            SymList{
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"expr"},
                Terminal{Token{")", TokenType::RPAREN}},
            },

        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"+", TokenType::ADD}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"-", TokenType::SUB}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"*", TokenType::MUL}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"/", TokenType::DIV}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"%", TokenType::MOD}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"==", TokenType::EQ}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"!=", TokenType::NEQ}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{">", TokenType::GT}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"<", TokenType::LT}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{">=", TokenType::GE}},
            },
        Nonterminal{"binary-op"} >>
            SymList{
                Terminal{Token{"<=", TokenType::LE}},
            },

        Nonterminal{"assign-op"} >>
            SymList{
                Terminal{Token{"=", TokenType::ASSIGN}},
            },

        Nonterminal{"func-call"} >>
            SymList{
                Terminal{Token{"id", TokenType::IDENTIFIER}},
                Terminal{Token{"(", TokenType::LPAREN}},
                Nonterminal{"actual-params"},
                Terminal{Token{")", TokenType::RPAREN}},
            },

        Nonterminal{"actual-params"} >>
            SymList{
                Nonterminal{"actual-params"},
                Terminal{Token{",", TokenType::COMMA}},
                Nonterminal{"expr"},
            },
        Nonterminal{"actual-params"} >>
            SymList{
                Nonterminal{"expr"},
            },
        Nonterminal{"actual-params"} >>
            SymList{
                Terminal{Token{"nil", TokenType::NIL}},
            },

    });

    // grammarPtr->checkLR1();
}