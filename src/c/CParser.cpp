#include "c/CParser.h"

namespace pluma {

CParser::CParser() : Parser() { this->genGrammar(); }

CParser::CParser(CParser &&other) : Parser(std::move(other)) {}

CParser &CParser::operator=(CParser &&rhs) {
    if (this == &rhs) {
        return *this;
    }

    Parser::operator=(std::move(rhs));
    return *this;
}

void CParser::genGrammar() {
    grammarPtr = std::make_unique<Grammar>(
        "../../src/c/CParser.cpp", "../../data/hash.txt",
        std::vector<Rule>{

            Nonterminal{"program-aug"} >> SymList{Nonterminal{"program"}},

            /**
             * program -> preprocessors ext-defs
             */
            Nonterminal{"program"} >>
                SymList{Nonterminal{"preprocessors"}, Nonterminal{"ext-defs"}},

            /**
             * preprocessors -> preprocessors preprocessor
             * preprocessors -> NIL
             */
            Nonterminal{"preprocessors"} >>
                SymList{Nonterminal{"preprocessor"}, Nonterminal{"preprocessors"}},
            Nonterminal{"preprocessors"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

            /**
             * preprocessor -> include-preprocessor
             * preprocessor -> define-preprocessor
             */
            Nonterminal{"preprocessor"} >> SymList{Nonterminal{"include-preprocessor"}},
            Nonterminal{"preprocessor"} >> SymList{Nonterminal{"define-preprocessor"}},

            /**
             * include-preprocessor -> '#' INCLUDE '<' path '>'
             * include-preprocessor -> '#' INCLUDE STRING_CONST
             */
            Nonterminal{"include-preprocessor"} >>
                SymList{
                    Terminal{Token{"#", TokenType::SHARP}},
                    Terminal{Token{"include", TokenType::INCLUDE}},
                    Terminal{Token{"<", TokenType::LT}},
                    Nonterminal{"path"},
                    Terminal{Token{">", TokenType::GT}},
                },
            Nonterminal{"include-preprocessor"} >>
                SymList{
                    Terminal{Token{"#", TokenType::SHARP}},
                    Terminal{Token{"include", TokenType::INCLUDE}},
                    Terminal{Token{"STRING_CONST", TokenType::STRING_CONST}},
                },

            /**
             * define-preprocessor -> '#' DEFINE id
             * define-preprocessor -> '#' DEFINE id expr
             */
            Nonterminal{"define-preprocessor"} >>
                SymList{
                    Terminal{Token{"#", TokenType::SHARP}},
                    Terminal{Token{"define", TokenType::DEFINE}},
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                },
            Nonterminal{"define-preprocessor"} >>
                SymList{
                    Terminal{Token{"#", TokenType::SHARP}},
                    Terminal{Token{"define", TokenType::DEFINE}},
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                    Nonterminal{"expr"},
                },

            /**
             * path -> path '/' file-or-dir
             * path -> file-or-dir
             */
            Nonterminal{"path"} >>
                SymList{
                    Nonterminal{"path"},
                    Terminal{Token{"/", TokenType::DIV}},
                    Nonterminal{"file-or-dir"},
                },
            Nonterminal{"path"} >>
                SymList{
                    Nonterminal{"file-or-dir"},
                },

            /**
             * file-or-dir -> '.' '.' | '.' | id | id '.' id
             * NOTE: this is an incomplete filename format.
             */
            Nonterminal{"file-or-dir"} >>
                SymList{
                    Terminal{Token{".", TokenType::PERIOD}},
                    Terminal{Token{".", TokenType::PERIOD}},
                },
            Nonterminal{"file-or-dir"} >>
                SymList{
                    Terminal{Token{".", TokenType::PERIOD}},
                },
            Nonterminal{"file-or-dir"} >>
                SymList{
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                },
            Nonterminal{"file-or-dir"} >>
                SymList{
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                    Terminal{Token{".", TokenType::PERIOD}},
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                },

            Nonterminal{"ext-defs"} >> SymList{Nonterminal{"ext-defs"}, Nonterminal{"ext-def"}},
            Nonterminal{"ext-defs"} >> SymList{Nonterminal{"ext-def"}},

            Nonterminal{"ext-def"} >> SymList{Nonterminal{"decl"}},
            Nonterminal{"ext-def"} >> SymList{Nonterminal{"func-def"}},

            /**
             * func-def -> decl-spec func-direct-declarator compound-stmt
             * func-def -> decl-spec func-direct-declarator ;
             */
            Nonterminal{"func-def"} >> SymList{Nonterminal{"decl-spec"},
                                               Nonterminal{"func-direct-declarator"},
                                               Nonterminal{"compound-stmt"}},
            Nonterminal{"func-def"} >> SymList{Nonterminal{"decl-spec"},
                                               Nonterminal{"func-direct-declarator"},
                                               Terminal{Token{";", TokenType::SEMICOLON}}},

            /**
             * decl-spec -> storage-class-spec? type-qualifier? type-spec
             */
            Nonterminal{"decl-spec"} >>
                SymList{
                    Nonterminal{"storage-class-spec?"},
                    Nonterminal{"type-qualifier?"},
                    Nonterminal{"type-spec"},
                },
            Nonterminal{"storage-class-spec?"} >> SymList{Nonterminal{"storage-class-spec"}},
            Nonterminal{"storage-class-spec?"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},
            Nonterminal{"type-qualifier?"} >> SymList{Nonterminal{"type-qualifier"}},
            Nonterminal{"type-qualifier?"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

            /**
             * storage-class-spec -> STATIC | EXTERN | TYPEDEF
             */
            Nonterminal{"storage-class-spec"} >>
                SymList{Terminal{Token{"static", TokenType::STATIC}}},
            Nonterminal{"storage-class-spec"} >>
                SymList{Terminal{Token{"extern", TokenType::EXTERN}}},
            Nonterminal{"storage-class-spec"} >>
                SymList{Terminal{Token{"typedef", TokenType::TYPEDEF}}},

            /**
             * type-spec -> VOID | CHAR | SHORT | INT | LONG | FLOAT
             *              | DOUBLE | SIGNED | UNSIGNED
             * type-spec -> struct-or-union-spec
             * type-spec -> enum-spec
             * type-spec -> IDENTIFIER
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
            Nonterminal{"type-spec"} >> SymList{Terminal{Token{"id", TokenType::IDENTIFIER}}},

            /**
             * struct-or-union-spec -> struct-or-union IDENTIFIER '{' struct-decls '}'
             * struct-or-union-spec -> struct-or-union '{' struct-decls '}'
             * struct-or-union-spec -> struct-or-union IDENTIFIER
             */
            Nonterminal{"struct-or-union-spec"} >>
                SymList{Nonterminal{"struct-or-union"},
                        Terminal{Token{"id", TokenType::IDENTIFIER}},
                        Terminal{Token{"{", TokenType::LBRACE}}, Nonterminal{"struct-decls*"},
                        Terminal{Token{"}", TokenType::RBRACE}}},
            Nonterminal{"struct-or-union-spec"} >>
                SymList{Nonterminal{"struct-or-union"}, Terminal{Token{"{", TokenType::LBRACE}},
                        Nonterminal{"struct-decls*"}, Terminal{Token{"}", TokenType::RBRACE}}},
            Nonterminal{"struct-or-union-spec"} >>
                SymList{Nonterminal{"struct-or-union"},
                        Terminal{Token{"id", TokenType::IDENTIFIER}}},

            /**
             * struct-or-union -> STRUCT | UNION
             */
            Nonterminal{"struct-or-union"} >> SymList{Terminal{Token{"struct", TokenType::STRUCT}}},
            Nonterminal{"struct-or-union"} >> SymList{Terminal{Token{"union", TokenType::UNION}}},

            /**
             * struct-decls -> struct-decls struct-decl
             * struct-decls -> nil
             */
            // TODO: Check it.
            Nonterminal{"struct-decls*"} >>
                SymList{Nonterminal{"struct-decls*"}, Nonterminal{"struct-decl"}},
            Nonterminal{"struct-decls*"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

            /**
             * struct-decl -> decl-spec struct-declarator-list ';'
             */
            Nonterminal{"struct-decl"} >>
                SymList{
                    Nonterminal{"decl-spec"},
                    Nonterminal{"struct-declarator-list"},
                    Terminal{Token{";", TokenType::SEMICOLON}},
                },

            /**
             * struct-declarator-list -> struct-declarator
             * struct-declarator-list -> struct-declarator-list ',' struct-declarator
             */
            Nonterminal{"struct-declarator-list"} >> SymList{Nonterminal{"struct-declarator"}},
            Nonterminal{"struct-declarator-list"} >> SymList{Nonterminal{"struct-declarator-list"},
                                                             Terminal{Token{",", TokenType::COMMA}},
                                                             Nonterminal{"struct-declarator"}},

            /**
             * struct-declarator -> declarator
             * struct-declarator -> declarator ':' expr
             * struct-declarator -> ':' expr
             */
            Nonterminal{"struct-declarator"} >> SymList{Nonterminal{"declarator"}},
            Nonterminal{"struct-declarator"} >> SymList{Nonterminal{"declarator"},
                                                        Terminal{Token{":", TokenType::COLON}},
                                                        Nonterminal{"expr"}},
            Nonterminal{"struct-declarator"} >>
                SymList{Terminal{Token{":", TokenType::COLON}}, Nonterminal{"expr"}},

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
             * enumerator -> IDENTIFIER '=' expr
             */
            Nonterminal{"enumerator"} >>
                SymList{
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                },
            Nonterminal{"enumerator"} >>
                SymList{
                    Terminal{Token{"id", TokenType::IDENTIFIER}},
                    Terminal{Token{"=", TokenType::ASSIGN}},
                    Nonterminal{"expr"},
                },

            /**
             * declarator -> ptr? direct-declarator
             * declarator -> direct-declarator
             */
            Nonterminal{"declarator"} >>
                SymList{Nonterminal{"ptr?"}, Nonterminal{"direct-declarator"}},
            Nonterminal{"declarator"} >> SymList{Nonterminal{"direct-declarator"}},
            Nonterminal{"ptr?"} >> SymList{Nonterminal{"ptr"}},
            Nonterminal{"ptr?"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

            /**
             * ptr -> * type-qualifiers* ptr
             * ptr -> * type-qualifiers*
             */
            // FIXME: Seems to be right-recursive, should be checked.
            Nonterminal{"ptr"} >> SymList{Terminal{Token{"*", TokenType::MUL}},
                                          Nonterminal{"type-qualifiers*"}, Nonterminal{"ptr"}},
            Nonterminal{"ptr"} >>
                SymList{Terminal{Token{"*", TokenType::MUL}}, Nonterminal{"type-qualifiers*"}},

            /**
             * type-qualifiers* -> type-qualifiers* type-qualifier
             * type-qualifiers* -> NIL
             */
            Nonterminal{"type-qualifiers*"} >>
                SymList{Nonterminal{"type-qualifiers*"}, Nonterminal{"type-qualifier"}},
            Nonterminal{"type-qualifiers*"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

            /**
             * type-qualifier -> CONST | VOLATILE
             */
            Nonterminal{"type-qualifier"} >> SymList{Terminal{Token{"const", TokenType::CONST}}},
            Nonterminal{"type-qualifier"} >>
                SymList{Terminal{Token{"volatile", TokenType::VOLATILE}}},

            /**
             * direct-declarator -> IDENTIFIER
             * direct-declarator -> '(' declarator ')'
             * direct-declarator -> array-direct-declarator
             * direct-declarator -> func-direct-declarator
             */
            Nonterminal{"direct-declarator"} >>
                SymList{Terminal{Token{"id", TokenType::IDENTIFIER}}},
            Nonterminal{"direct-declarator"} >>
                SymList{
                    Terminal{Token{"(", TokenType::LPAREN}},
                    Nonterminal{"declarator"},
                    Terminal{Token{")", TokenType::RPAREN}},
                },
            Nonterminal{"direct-declarator"} >> SymList{Nonterminal{"array-direct-declarator"}},
            Nonterminal{"direct-declarator"} >> SymList{Nonterminal{"func-direct-declarator"}},

            /**
             * func-direct-declarator -> direct-declarator '(' param-type-list? ')'
             */
            Nonterminal{"func-direct-declarator"} >>
                SymList{
                    Nonterminal{"direct-declarator"},
                    Terminal{Token{"(", TokenType::LPAREN}},
                    Nonterminal{"param-type-list?"},
                    Terminal{Token{")", TokenType::RPAREN}},
                },
            Nonterminal{"param-type-list?"} >> SymList{Nonterminal{"param-type-list"}},
            Nonterminal{"param-type-list?"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

            /**
             * param-type-list -> param-decl
             * param-type-list -> param-type-list ',' param-decl
             */
            Nonterminal{"param-type-list"} >> SymList{Nonterminal{"param-decl"}},
            Nonterminal{"param-type-list"} >> SymList{Nonterminal{"param-type-list"},
                                                      Terminal{Token{",", TokenType::COMMA}},
                                                      Nonterminal{"param-decl"}},

            /**
             * param-decl -> decl-spec declarator
             */
            Nonterminal{"param-decl"} >>
                SymList{Nonterminal{"decl-spec"}, Nonterminal{"declarator"}},

            /**
             * array-direct-declarator -> direct-declarator '[' expr? ']'
             */
            Nonterminal{"array-direct-declarator"} >>
                SymList{
                    Nonterminal{"direct-declarator"},
                    Terminal{Token{"[", TokenType::LSBRACKET}},
                    Nonterminal{"expr?"},
                    Terminal{Token{"]", TokenType::RSBRACKET}},
                },

            /**
             * decl -> decl-spec init-declarator-list? ';'
             */
            Nonterminal{"decl"} >>
                SymList{
                    Nonterminal{"decl-spec"},
                    Nonterminal{"init-declarator-list?"},
                    Terminal{Token{";", TokenType::SEMICOLON}},
                },

            /**
             * init-declarator-list? -> init-declarator-list
             * init-declarator-list? -> NIL
             */
            Nonterminal{"init-declarator-list?"} >>
                SymList{
                    Nonterminal{"init-declarator-list"},
                },
            Nonterminal{"init-declarator-list?"} >>
                SymList{
                    Terminal{Token{"nil", TokenType::NIL}},
                },

            /**
             * init-declarator-list -> init-declarator-list ',' init-declarator
             * init-declarator-list -> init-declarator
             */
            Nonterminal{"init-declarator-list"} >> SymList{Nonterminal{"init-declarator"}},
            Nonterminal{"init-declarator-list"} >> SymList{Nonterminal{"init-declarator-list"},
                                                           Terminal{Token{",", TokenType::COMMA}},
                                                           Nonterminal{"init-declarator"}},

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
             * initializer -> expr
             * initializer -> '{' initializer-list '}'
             * initializer -> '{' initializer-list ',' '}'
             */
            Nonterminal{"initializer"} >>
                SymList{
                    Nonterminal{"expr"},
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

            Nonterminal{"compound-stmt"} >>
                SymList{
                    Terminal{Token{"{", TokenType::LBRACE}},
                    Nonterminal{"decl-or-stmts*"},
                    Terminal{Token{"}", TokenType::RBRACE}},
                },

            Nonterminal{"decl-or-stmts*"} >>
                SymList{
                    Nonterminal{"decl-or-stmts*"},
                    Nonterminal{"decl-or-stmt"},
                },
            Nonterminal{"decl-or-stmts*"} >>
                SymList{
                    Terminal{Token{"nil", TokenType::NIL}},
                },

            Nonterminal{"decl-or-stmt"} >>
                SymList{
                    Nonterminal{"decl"},
                },
            Nonterminal{"decl-or-stmt"} >>
                SymList{
                    Nonterminal{"stmt"},
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
             * iter-stmt -> FOR '(' decl  expr? ';' expr? ')' stmt
             * NOTE: the last rule is ugly.
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
                    Nonterminal{"expr?"},
                    Terminal{Token{";", TokenType::SEMICOLON}},
                    Nonterminal{"expr?"},
                    Terminal{Token{";", TokenType::SEMICOLON}},
                    Nonterminal{"expr?"},
                    Terminal{Token{")", TokenType::RPAREN}},
                    Nonterminal{"stmt"},
                },
            Nonterminal{"iter-stmt"} >>
                SymList{
                    Terminal{Token{"for", TokenType::FOR}},
                    Terminal{Token{"(", TokenType::LPAREN}},
                    Nonterminal{"decl"},
                    // Terminal{Token{";", TokenType::SEMICOLON}},
                    Nonterminal{"expr?"},
                    Terminal{Token{";", TokenType::SEMICOLON}},
                    Nonterminal{"expr?"},
                    Terminal{Token{")", TokenType::RPAREN}},
                    Nonterminal{"stmt"},
                },
            Nonterminal{"expr?"} >> SymList{Nonterminal{"expr"}},
            Nonterminal{"expr?"} >> SymList{Terminal{Token{"nil", TokenType::NIL}}},

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
                    Nonterminal{"expr?"},
                    Terminal{Token{";", TokenType::SEMICOLON}},
                },

            /**
             * expr -> unary-expr binary-op expr
             * expr -> unary-expr assign-op expr
             * expr -> unary-expr
             * expr -> expr ? expr : expr
             */
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
            Nonterminal{"expr"} >>
                SymList{Nonterminal{"expr"}, Terminal{Token{"?", TokenType::QUESTION_MARK}},
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
             * cast-expr -> unary-expr
             * cast-expr -> '(' type-spec ptr? ')' cast-expr
             */
            Nonterminal{"cast-expr"} >> SymList{Nonterminal{"unary-expr"}},
            Nonterminal{"cast-expr"} >> SymList{Terminal{Token{"(", TokenType::LPAREN}},
                                                Nonterminal{"type-spec"}, Nonterminal{"ptr?"},
                                                Terminal{Token{")", TokenType::RPAREN}},
                                                Nonterminal{"cast-expr"}},

            /**
             * postfix-expr -> primary-expr
             * postfix-expr -> postfix-expr '[' expr ']'
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
                                                   Nonterminal{"primary-expr"}},
            Nonterminal{"postfix-expr"} >> SymList{Nonterminal{"postfix-expr"},
                                                   Terminal{Token{"->", TokenType::ARROW}},
                                                   Nonterminal{"primary-expr"}},
            Nonterminal{"postfix-expr"} >>
                SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"++", TokenType::INCR}}},
            Nonterminal{"postfix-expr"} >>
                SymList{Nonterminal{"postfix-expr"}, Terminal{Token{"--", TokenType::DECR}}},

            /**
             * primary-expr -> IDENTIFIER
             * primary-expr -> INT_CONST
             * primary-expr -> FLOAT_CONST
             * primary-expr -> CHAR_CONST
             * primary-expr -> STRING_CONST
             * primary-expr -> func-call
             * primary-expr -> '(' expr ')'
             */
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
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{"&&", TokenType::AND}},
                },
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{"||", TokenType::OR}},
                },
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{"<<", TokenType::LSHIFT}},
                },
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{">>", TokenType::RSHIFT}},
                },
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{"&", TokenType::BITAND}},
                },
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{"^", TokenType::BITXOR}},
                },
            Nonterminal{"binary-op"} >>
                SymList{
                    Terminal{Token{"|", TokenType::BITOR}},
                },

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
}

}  // namespace pluma