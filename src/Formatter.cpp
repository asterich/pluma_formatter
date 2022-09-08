#include "Formatter.h"

namespace pluma {

Formatter::Formatter(std::string filename) {
    this->out.open(filename);
    if (!this->out.is_open()) {
        std::cout << "Failed to open the target file.\n";
    }
}

Formatter::~Formatter() {
    if (this->out.is_open()) {
        this->out.close();
    }
}

inline void Formatter::printIndents(const size_t indents) {
    std::string indentStr(indentSize * indents, ' ');
    this->out << indentStr;
}

inline bool isStmtCompoundStmt(const AstNode *stmt) {
    // stmt's son node must be nonterminal.
    return std::get<Nonterminal>(stmt->son->sym).token == "compound-stmt";
}

void Formatter::formatNode(const AstNode *nodePtr, const size_t indents) {
    if (nodePtr == nullptr) {
        return;
    }

    if (std::holds_alternative<Terminal>(nodePtr->sym)) {
        // Terminal
        this->out << nodePtr->sym;
        auto &currToken = std::get<Terminal>(nodePtr->sym).token;
        if (currToken.comments.size() != 0) {
            size_t neededIndents = currToken.tokenType == TokenType::LBRACE ? indents + 1 : indents;
            for (auto comment : std::get<Terminal>(nodePtr->sym).token.comments) {
                this->out << '\n';
                printIndents(neededIndents);
                this->out << comment;
            }
        }
        return;
    } else {
        // Nonterminal
        auto &nt = std::get<Nonterminal>(nodePtr->sym);
        if (nt.token == "program") {
            AstNode *preprocessors = nodePtr->son;
            AstNode *ext_defs = preprocessors->brother;

            formatNode(preprocessors, indents);
            if (preprocessors->son != nullptr) {
                this->out << "\n";
            }
            formatNode(ext_defs, indents);

        } else if (nt.token == "preprocessors") {
            if (nodePtr->son) {
                AstNode *preprocessor = nodePtr->son;
                AstNode *preprocessors = preprocessor->brother;

                formatNode(preprocessor, indents);
                this->out << '\n';
                formatNode(preprocessors, indents);
            }
        } else if (nt.token == "include-preprocessor") {
            switch (nodePtr->sonCnt) {
                case 3: {
                    AstNode *sharp = nodePtr->son;
                    AstNode *include_ = sharp->brother;
                    AstNode *string_const = include_->brother;

                    formatNode(sharp, indents);
                    formatNode(include_, indents);
                    this->out << ' ';
                    formatNode(string_const, indents);
                    break;
                }
                case 5: {
                    AstNode *sharp = nodePtr->son;
                    AstNode *include_ = sharp->brother;
                    AstNode *lt = include_->brother;
                    AstNode *path = lt->brother;
                    AstNode *gt = path->brother;

                    formatNode(sharp, indents);
                    formatNode(include_, indents);
                    this->out << ' ';
                    formatNode(lt, indents);
                    formatNode(path, indents);
                    formatNode(gt, indents);
                    break;
                }
            }
        } else if (nt.token == "define-preprocessor") {
            switch (nodePtr->sonCnt) {
                case 3: {
                    AstNode *sharp = nodePtr->son;
                    AstNode *define_ = sharp->brother;
                    AstNode *id = define_->brother;

                    formatNode(sharp, indents);
                    formatNode(define_, indents);
                    this->out << ' ';
                    formatNode(id, indents);
                    break;
                }
                case 4: {
                    AstNode *sharp = nodePtr->son;
                    AstNode *define_ = sharp->brother;
                    AstNode *id = define_->brother;
                    AstNode *expr = id->brother;

                    formatNode(sharp, indents);
                    formatNode(define_, indents);
                    this->out << ' ';
                    formatNode(id, indents);
                    this->out << ' ';
                    formatNode(expr, indents);
                    break;
                }
            }
        } else if (nt.token == "ext-defs") {
            // ext-defs ->  ext-defs  ext-def
            // ext-defs ->  ext-def
            switch (nodePtr->sonCnt) {
                case 1: {
                    // ext-defs ->  ext-def
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 2: {
                    // ext-defs ->  ext-defs  ext-def
                    formatNode(nodePtr->son, indents);
                    this->out << '\n' << '\n';

                    formatNode(nodePtr->son->brother, indents);
                    break;
                }
            }
        } else if (nt.token == "func-def") {
            // func-def ->  decl-spec  func-direct-declarator  compound-stmt
            // func-def ->  decl-spec  func-direct-declarator  ;
            // Always at the top level of ast-tree, so there's no need to print indents.
            AstNode *decl_spec = nodePtr->son;
            AstNode *func_direct_declarator = decl_spec->brother;
            AstNode *compound_stmt_or_semicolon = func_direct_declarator->brother;

            formatNode(decl_spec, indents);
            this->out << ' ';

            formatNode(func_direct_declarator, indents);
            this->out << ' ';

            if (std::holds_alternative<Terminal>(compound_stmt_or_semicolon->sym)) {
                // SEMICOLON
                formatNode(compound_stmt_or_semicolon, indents);
                this->out << '\n';
            } else {
                // compound-stmt ->  {  decl-or-stmts*  }
                AstNode *lbrace = compound_stmt_or_semicolon->son;
                AstNode *decl_or_stmts_zero_or_more = lbrace->brother;
                AstNode *rbrace = decl_or_stmts_zero_or_more->brother;

                formatNode(lbrace, indents);
                // this->out << '\n';

                // '\n'
                formatNode(decl_or_stmts_zero_or_more, indents + 1);
                this->out << '\n';

                // '}' must be the first character of the line, so indents should be printed here.
                printIndents(indents);
                formatNode(rbrace, indents);
            }
        } else if (nt.token == "decl-spec") {
            // decl-spec ->  storage-class-spec?  type-qualifier?  type-spec
            AstNode *storage_class_spec_opt = nodePtr->son;
            AstNode *type_qualifier_opt = storage_class_spec_opt->brother;
            AstNode *type_spec = type_qualifier_opt->brother;

            if (storage_class_spec_opt->son != nullptr) {
                formatNode(storage_class_spec_opt->son, indents);
                this->out << ' ';
            }

            if (type_qualifier_opt->son != nullptr) {
                formatNode(type_qualifier_opt->son, indents);
                this->out << ' ';
            }

            formatNode(type_spec, indents);

        } else if (nt.token == "struct-or-union-spec") {
            // struct-or-union-spec ->  struct-or-union  id  {  struct-decls*  }
            // struct-or-union-spec ->  struct-or-union  {  struct-decls*  }
            // struct-or-union-spec ->  struct-or-union  id
            switch (nodePtr->sonCnt) {
                case 2: {
                    // struct-or-union-spec ->  struct-or-union  id
                    AstNode *struct_or_union = nodePtr->son;
                    AstNode *id = struct_or_union->brother;

                    formatNode(struct_or_union, indents);
                    this->out << ' ';

                    formatNode(id, indents);
                    break;
                }
                case 4: {
                    // struct-or-union-spec ->  struct-or-union  {  struct-decls*  }
                    AstNode *struct_or_union = nodePtr->son;
                    AstNode *lbrace = struct_or_union->brother;
                    AstNode *struct_decls_zero_or_more = lbrace->brother;
                    AstNode *rbrace = struct_decls_zero_or_more->brother;

                    formatNode(struct_or_union, indents);
                    this->out << ' ';

                    formatNode(lbrace, indents);
                    this->out << '\n';

                    formatNode(struct_decls_zero_or_more, indents + 1);
                    this->out << '\n';

                    printIndents(indents);
                    formatNode(rbrace, indents);
                    break;
                }
                case 5: {
                    // struct-or-union-spec ->  struct-or-union  id  {  struct-decls*  }
                    AstNode *struct_or_union = nodePtr->son;
                    AstNode *id = struct_or_union->brother;
                    AstNode *lbrace = id->brother;
                    AstNode *struct_decls_zero_or_more = lbrace->brother;
                    AstNode *rbrace = struct_decls_zero_or_more->brother;

                    formatNode(struct_or_union, indents);
                    this->out << ' ';

                    formatNode(id, indents);
                    this->out << ' ';

                    formatNode(lbrace, indents);
                    this->out << '\n';

                    formatNode(struct_decls_zero_or_more, indents + 1);

                    printIndents(indents);
                    formatNode(rbrace, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "struct-decls*") {
            // struct-decls* ->  struct-decls*  struct-decl
            // struct-decls* ->  nil
            switch (nodePtr->sonCnt) {
                case 0: {
                    break;
                }
                case 2: {
                    // struct-decls* ->  struct-decls*  struct-decl
                    AstNode *struct_decls_zero_or_more = nodePtr->son;
                    AstNode *struct_decl = struct_decls_zero_or_more->brother;

                    formatNode(struct_decls_zero_or_more, indents);

                    printIndents(indents);
                    formatNode(struct_decl, indents);
                    break;
                }
            }

        } else if (nt.token == "struct-decl") {
            // struct-decl ->  decl-spec  struct-declarator-list  ;
            AstNode *decl_spec = nodePtr->son;
            AstNode *struct_declarator_list = decl_spec->brother;
            AstNode *semicolon = struct_declarator_list->brother;

            formatNode(decl_spec, indents);
            this->out << ' ';

            // No spaces here.
            formatNode(struct_declarator_list, indents);

            formatNode(semicolon, indents);
            this->out << '\n';

        } else if (nt.token == "struct-declarator-list" || nt.token == "param-type-list" ||
                   nt.token == "init-declarator-list") {
            // This is the branch for rules which are similar to:
            // SOMETHING_LIST -> SOMETHING_LIST , SOMETHING
            // and don't need '\n'.

            switch (nodePtr->sonCnt) {
                case 1: {
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    AstNode *something_list = nodePtr->son;
                    AstNode *comma = something_list->brother;
                    AstNode *something = comma->brother;

                    formatNode(something_list, indents);

                    formatNode(comma, indents);
                    this->out << ' ';

                    formatNode(something, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }

        } else if (nt.token == "struct-declarator") {
            // struct-declarator ->  declarator
            // struct-declarator ->  declarator  :  expr
            // struct-declarator ->  :  expr
            switch (nodePtr->sonCnt) {
                case 1: {
                    // struct-declarator ->  declarator
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    // struct-declarator ->  declarator  :  expr
                    AstNode *declarator = nodePtr->son;
                    AstNode *colon = declarator->brother;
                    AstNode *expr = colon->brother;

                    formatNode(declarator, indents);
                    this->out << ' ';

                    formatNode(colon, indents);
                    this->out << ' ';

                    formatNode(expr, indents);

                    break;
                }
                case 2: {
                    // struct-declarator ->   :  expr
                    AstNode *colon = nodePtr->son;
                    AstNode *expr = colon->brother;

                    // Space may not be needed here.

                    formatNode(colon, indents);
                    this->out << ' ';

                    formatNode(expr, indents);

                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "enum-spec") {
            // enum-spec ->  enum  id  {  enumerator-list  }
            // enum-spec ->  enum  {  enumerator-list  }
            // enum-spec ->  enum  id
            switch (nodePtr->sonCnt) {
                case 2: {
                    // enum-spec ->  enum  id
                    AstNode *enum_ = nodePtr->son;
                    AstNode *id = enum_->brother;

                    formatNode(enum_, indents);
                    this->out << ' ';

                    formatNode(id, indents);
                    break;
                }
                case 4: {
                    // enum-spec ->  enum  {  enumerator-list  }
                    AstNode *enum_ = nodePtr->son;
                    AstNode *lbrace = enum_->brother;
                    AstNode *enumerator_list = lbrace->brother;
                    AstNode *rbrace = enumerator_list->brother;

                    formatNode(enum_, indents);
                    this->out << ' ';

                    formatNode(lbrace, indents);
                    this->out << '\n';

                    formatNode(enumerator_list, indents + 1);

                    printIndents(indents);
                    formatNode(rbrace, indents);
                    break;
                }
                case 5: {
                    // enum-spec ->  enum  id  {  enumerator-list  }
                    AstNode *enum_ = nodePtr->son;
                    AstNode *id = enum_->brother;
                    AstNode *lbrace = id->brother;
                    AstNode *enumerator_list = lbrace->brother;
                    AstNode *rbrace = enumerator_list->brother;

                    formatNode(enum_, indents);
                    this->out << ' ';

                    formatNode(id, indents);
                    this->out << ' ';

                    formatNode(lbrace, indents);
                    this->out << '\n';

                    formatNode(enumerator_list, indents + 1);

                    printIndents(indents);
                    formatNode(rbrace, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "enumerator-list" || nt.token == "initializer-list") {
            // This is the branch for rules which are similar to:
            // SOMETHING_LIST -> SOMETHING_LIST , SOMETHING
            // and need '\n'.

            switch (nodePtr->sonCnt) {
                case 1: {
                    // Indents.
                    printIndents(indents);

                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    AstNode *something_list = nodePtr->son;
                    AstNode *comma = something_list->brother;
                    AstNode *something = comma->brother;

                    formatNode(something_list, indents);

                    formatNode(comma, indents);
                    this->out << '\n';

                    // Indents.
                    printIndents(indents);
                    formatNode(something, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "enumerator") {
            // enumerator ->  id
            // enumerator ->  id  =  expr
            switch (nodePtr->sonCnt) {
                case 1: {
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    AstNode *id = nodePtr->son;
                    AstNode *eq = id->brother;
                    AstNode *expr = eq->brother;

                    formatNode(id, indents);
                    this->out << ' ';

                    formatNode(eq, indents);
                    this->out << ' ';

                    formatNode(expr, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "declarator") {
            // declarator ->  ptr?  direct-declarator
            // declarator ->  direct-declarator
            switch (nodePtr->sonCnt) {
                case 1: {
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 2: {
                    AstNode *ptr_opt = nodePtr->son;
                    AstNode *direct_declarator = ptr_opt->brother;

                    formatNode(ptr_opt, indents);

                    formatNode(direct_declarator, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "ptr") {
            // ptr ->  *  type-qualifiers*  ptr
            // ptr ->  *  type-qualifiers*
            switch (nodePtr->sonCnt) {
                case 3: {
                    AstNode *mul = nodePtr->son;
                    AstNode *type_qualifiers_zero_or_more = mul->brother;
                    AstNode *ptr = type_qualifiers_zero_or_more->brother;

                    formatNode(mul, indents);

                    formatNode(type_qualifiers_zero_or_more, indents);

                    formatNode(ptr, indents);
                    break;
                }
                case 2: {
                    AstNode *mul = nodePtr->son;
                    AstNode *type_qualifiers_zero_or_more = mul->brother;

                    formatNode(mul, indents);

                    formatNode(type_qualifiers_zero_or_more, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "decl") {
            // decl ->  decl-spec  init-declarator-list?  ;
            // Indents should be handled in the branches like "decls-or-stmts*".

            AstNode *decl_spec = nodePtr->son;
            AstNode *init_declarator_list_opt = decl_spec->brother;
            AstNode *semicolon = init_declarator_list_opt->brother;

            formatNode(decl_spec, indents);
            this->out << ' ';

            formatNode(init_declarator_list_opt, indents);

            formatNode(semicolon, indents);

        } else if (nt.token == "param-decl") {
            // param-decl -> decl-spec declarator
            AstNode *decl_spec = nodePtr->son;
            AstNode *declarator = decl_spec->brother;

            formatNode(decl_spec, indents);
            this->out << ' ';

            formatNode(declarator, indents);

        } else if (nt.token == "init-declarator") {
            // init-declarator ->  declarator
            // init-declarator ->  declarator  =  initializer
            switch (nodePtr->sonCnt) {
                case 1: {
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    AstNode *declarator = nodePtr->son;
                    AstNode *eq = declarator->brother;
                    AstNode *initializer = eq->brother;

                    formatNode(declarator, indents);
                    this->out << ' ';

                    formatNode(eq, indents);
                    this->out << ' ';

                    formatNode(initializer, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "initializer") {
            // initializer ->  expr
            // initializer ->  {  initializer-list  }
            // initializer ->  {  initializer-list  ,  }
            switch (nodePtr->sonCnt) {
                case 1: {
                    // initializer ->  expr
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    // initializer ->  {  initializer-list  }
                    AstNode *lbrace = nodePtr->son;
                    AstNode *initializer_list = lbrace->brother;
                    AstNode *rbrace = initializer_list->brother;

                    formatNode(lbrace, indents);
                    this->out << '\n';

                    formatNode(initializer_list, indents + 1);

                    formatNode(rbrace, indents);

                    break;
                }
                case 4: {
                    // initializer ->  {  initializer-list  ,  }
                    AstNode *lbrace = nodePtr->son;
                    AstNode *initializer_list = lbrace->brother;
                    AstNode *comma = initializer_list->brother;
                    AstNode *rbrace = comma->brother;

                    formatNode(lbrace, indents);
                    this->out << '\n';

                    formatNode(initializer_list, indents + 1);

                    formatNode(comma, indents);
                    this->out << '\n';

                    formatNode(rbrace, indents);

                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "compound-stmt") {
            // compound-stmt ->  {  decl-or-stmts*  }
            AstNode *lbrace = nodePtr->son;
            AstNode *decl_or_stmts_zero_or_more = lbrace->brother;
            AstNode *rbrace = decl_or_stmts_zero_or_more->brother;

            // Indents should be handled elsewhere.
            formatNode(lbrace, indents);

            formatNode(decl_or_stmts_zero_or_more, indents + 1);
            this->out << '\n';

            printIndents(indents);
            formatNode(rbrace, indents);

        } else if (nt.token == "decl-or-stmts*") {
            switch (nodePtr->sonCnt) {
                case 1: {
                    // ext-defs ->  ext-def
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 2: {
                    // ext-defs ->  ext-defs  ext-def
                    formatNode(nodePtr->son, indents);
                    this->out << '\n';

                    printIndents(indents);
                    formatNode(nodePtr->son->brother, indents);
                    break;
                }
            }
        } else if (nt.token == "label-stmt") {
            // label-stmt ->  id  :  stmt
            // label-stmt ->  case  expr  :  stmt
            // label-stmt ->  default  :  stmt
            switch (nodePtr->sonCnt) {
                case 3: {
                    AstNode *id_or_default = nodePtr->son;
                    AstNode *colon = id_or_default->brother;
                    AstNode *stmt = colon->brother;

                    formatNode(id_or_default, indents);

                    formatNode(colon, indents);
                    this->out << '\n';

                    printIndents(indents);
                    formatNode(stmt, indents);
                    break;
                }
                case 4: {
                    AstNode *case_ = nodePtr->son;
                    AstNode *expr = case_->brother;
                    AstNode *colon = expr->brother;
                    AstNode *stmt = colon->brother;

                    formatNode(case_, indents);
                    this->out << ' ';

                    formatNode(expr, indents);

                    formatNode(colon, indents);
                    this->out << '\n';

                    printIndents(indents);
                    formatNode(stmt, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "selection-stmt") {
            // selection-stmt ->  if  (  expr  )  stmt
            // selection-stmt ->  if  (  expr  )  stmt  else  stmt
            // selection-stmt ->  switch  (  expr  )  stmt

            switch (nodePtr->sonCnt) {
                case 5: {
                    AstNode *if_or_switch = nodePtr->son;
                    AstNode *lparen = if_or_switch->brother;
                    AstNode *expr = lparen->brother;
                    AstNode *rparen = expr->brother;
                    AstNode *stmt = rparen->brother;

                    formatNode(if_or_switch, indents);
                    this->out << ' ';

                    formatNode(lparen, indents);

                    formatNode(expr, indents);

                    formatNode(rparen, indents);

                    if (!isStmtCompoundStmt(stmt)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt, indents + 1);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt, indents);
                    }

                    break;
                }
                case 7: {
                    AstNode *if_ = nodePtr->son;
                    AstNode *lparen = if_->brother;
                    AstNode *expr = lparen->brother;
                    AstNode *rparen = expr->brother;
                    AstNode *stmt = rparen->brother;
                    AstNode *else_ = stmt->brother;
                    AstNode *stmt_2 = else_->brother;

                    formatNode(if_, indents);
                    this->out << ' ';

                    formatNode(lparen, indents);

                    formatNode(expr, indents);

                    formatNode(rparen, indents);

                    if (!isStmtCompoundStmt(stmt)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt, indents + 1);
                        this->out << '\n';
                        printIndents(indents);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt, indents);
                        this->out << ' ';
                    }

                    formatNode(else_, indents);

                    if (!isStmtCompoundStmt(stmt_2)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt_2, indents + 1);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt_2, indents);
                    }
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else if (nt.token == "iter-stmt") {
            // iter-stmt ->  while  (  expr  )  stmt
            // iter-stmt ->  do  stmt  while  (  expr  )  ;
            // iter-stmt ->  for  (  expr?  ;  expr?  ;  expr?  )  stmt
            // iter-stmt ->  for  (  decl  expr?  ;  expr?  )  stmt

            // FIXME: Should add special judgment on "compound-stmt".
            switch (nodePtr->sonCnt) {
                case 5: {
                    // iter-stmt ->  while  (  expr  )  stmt
                    AstNode *while_ = nodePtr->son;
                    AstNode *lparen = while_->brother;
                    AstNode *expr = lparen->brother;
                    AstNode *rparen = expr->brother;
                    AstNode *stmt = rparen->brother;

                    formatNode(while_, indents);
                    this->out << ' ';

                    formatNode(lparen, indents);

                    formatNode(expr, indents);

                    formatNode(rparen, indents);

                    if (!isStmtCompoundStmt(stmt)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt, indents + 1);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt, indents);
                    }
                    break;
                }
                case 7: {
                    // iter-stmt ->  do  stmt  while  (  expr  )  ;
                    AstNode *do_ = nodePtr->son;
                    AstNode *stmt = do_->brother;
                    AstNode *while_ = stmt->brother;
                    AstNode *lparen = while_->brother;
                    AstNode *expr = lparen->brother;
                    AstNode *rparen = expr->brother;
                    AstNode *semicolon = rparen->brother;

                    formatNode(do_, indents);

                    if (!isStmtCompoundStmt(stmt)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt, indents + 1);
                        printIndents(indents);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt, indents);
                        this->out << ' ';
                    }

                    formatNode(while_, indents);
                    this->out << ' ';

                    formatNode(lparen, indents);
                    formatNode(expr, indents);
                    formatNode(rparen, indents);
                    formatNode(semicolon, indents);

                    break;
                }
                case 8: {
                    // iter-stmt ->  for  (  decl  expr?  ;  expr?  )  stmt
                    AstNode *for_ = nodePtr->son;
                    AstNode *lparen = for_->brother;
                    AstNode *decl = lparen->brother;
                    AstNode *expr_opt_2 = decl->brother;
                    AstNode *semicolon_2 = expr_opt_2->brother;
                    AstNode *expr_opt_3 = semicolon_2->brother;
                    AstNode *rparen = expr_opt_3->brother;
                    AstNode *stmt = rparen->brother;

                    formatNode(for_, indents);
                    this->out << ' ';

                    formatNode(lparen, indents);

                    formatNode(decl, indents);
                    this->out << ' ';

                    formatNode(expr_opt_2, indents);

                    formatNode(semicolon_2, indents);
                    this->out << ' ';

                    formatNode(expr_opt_3, indents);

                    formatNode(rparen, indents);

                    if (!isStmtCompoundStmt(stmt)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt, indents + 1);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt, indents);
                    }

                    break;
                }
                case 9: {
                    // iter-stmt ->  for  (  expr?  ;  expr?  ;  expr?  )  stmt
                    AstNode *for_ = nodePtr->son;
                    AstNode *lparen = for_->brother;
                    AstNode *expr_opt_1 = lparen->brother;
                    AstNode *semicolon_1 = expr_opt_1->brother;
                    AstNode *expr_opt_2 = semicolon_1->brother;
                    AstNode *semicolon_2 = expr_opt_2->brother;
                    AstNode *expr_opt_3 = semicolon_2->brother;
                    AstNode *rparen = expr_opt_3->brother;
                    AstNode *stmt = rparen->brother;

                    formatNode(for_, indents);
                    this->out << ' ';

                    formatNode(lparen, indents);

                    formatNode(expr_opt_1, indents);

                    formatNode(semicolon_1, indents);
                    this->out << ' ';

                    formatNode(expr_opt_2, indents);

                    formatNode(semicolon_2, indents);
                    this->out << ' ';

                    formatNode(expr_opt_3, indents);

                    formatNode(rparen, indents);

                    if (!isStmtCompoundStmt(stmt)) {
                        // not compound stmt
                        this->out << '\n';
                        printIndents(indents + 1);
                        formatNode(stmt, indents + 1);
                    } else {
                        // is compound stmt
                        this->out << ' ';
                        formatNode(stmt, indents);
                    }
                    break;
                }
                default: {
                    std::cout << "Ast error!\n";
                    return;
                }
            }
        } else if (nt.token == "jump-stmt") {
            // jump-stmt ->  goto  id  ;
            // jump-stmt ->  continue  ;
            // jump-stmt ->  break  ;
            // jump-stmt ->  return  expr?  ;
            switch (nodePtr->sonCnt) {
                case 2: {
                    // jump-stmt ->  continue  ;
                    // jump-stmt ->  break  ;
                    AstNode *son = nodePtr->son;
                    while (son != nullptr) {
                        formatNode(son, indents);
                        son = son->brother;
                    }
                    break;
                }
                case 3: {
                    // jump-stmt ->  goto  id  ;
                    // jump-stmt ->  return  expr?  ;
                    AstNode *goto_or_return = nodePtr->son;
                    AstNode *id_or_expr = goto_or_return->brother;
                    AstNode *semicolon = id_or_expr->brother;

                    formatNode(goto_or_return, indents);
                    this->out << ' ';

                    formatNode(id_or_expr, indents);

                    formatNode(semicolon, indents);

                    break;
                }
            }
        } else if (nt.token == "expr") {
            // expr ->  unary-expr  binary-op  expr
            // expr ->  unary-expr  assign-op  expr
            // expr ->  unary-expr
            switch (nodePtr->sonCnt) {
                case 1: {
                    formatNode(nodePtr->son, indents);
                    return;
                }
                case 3: {
                    AstNode *unary_expr = nodePtr->son;
                    AstNode *op = unary_expr->brother;
                    AstNode *expr = op->brother;

                    formatNode(unary_expr, indents);
                    this->out << ' ';

                    formatNode(op, indents);
                    this->out << ' ';

                    formatNode(expr, indents);

                    break;
                }
                case 5: {
                    AstNode *expr_1 = nodePtr->son;
                    AstNode *question_mark = expr_1->brother;
                    AstNode *expr_2 = question_mark->brother;
                    AstNode *colon = expr_2->brother;
                    AstNode *expr_3 = colon->brother;

                    formatNode(expr_1, indents);
                    this->out << ' ';

                    formatNode(question_mark, indents);
                    this->out << ' ';

                    formatNode(expr_2, indents);
                    this->out << ' ';

                    formatNode(colon, indents);
                    this->out << ' ';

                    formatNode(expr_3, indents);

                    break;
                }
                default: {
                    std::cout << "Ast error";
                    return;
                }
            }
        } else if (nt.token == "unary-expr") {
            AstNode *son = nodePtr->son;
            if (std::holds_alternative<Terminal>(son->sym)) {
                Terminal firstSym = std::get<Terminal>(son->sym);
                if (firstSym.token.tokenType == TokenType::SIZEOF) {
                    formatNode(son, indents);
                    this->out << ' ';

                    formatNode(son->brother, indents);

                    return;
                }
            }

            while (son != nullptr) {
                formatNode(son, indents);
                son = son->brother;
            }
        } else if (nt.token == "cast-expr") {
            switch (nodePtr->sonCnt) {
                case 1: {
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 5: {
                    AstNode *lparen = nodePtr->son;
                    AstNode *type_spec = lparen->brother;
                    AstNode *ptr_opt = type_spec->brother;
                    AstNode *rparen = ptr_opt->brother;
                    AstNode *unay_expr = rparen->brother;

                    formatNode(lparen, indents);

                    formatNode(type_spec, indents);
                    this->out << ' ';

                    formatNode(ptr_opt, indents);

                    formatNode(rparen, indents);

                    formatNode(unay_expr, indents);

                    break;
                }
                default: {
                    std::cout << "Ast error!\n";
                    return;
                }
            }

        } else if (nt.token == "actual-params") {
            switch (nodePtr->sonCnt) {
                case 0:
                    break;
                case 1: {
                    formatNode(nodePtr->son, indents);
                    break;
                }
                case 3: {
                    AstNode *something_list = nodePtr->son;
                    AstNode *comma = something_list->brother;
                    AstNode *something = comma->brother;

                    formatNode(something_list, indents);

                    formatNode(comma, indents);
                    this->out << ' ';

                    formatNode(something, indents);
                    break;
                }
                default: {
                    std::cout << "Ast error!";
                    return;
                }
            }
        } else {
            // This branch is for the rules which don't need special spaces, newlines or indents
            // inserted, or those which don't need "switch"es.
            // Just traverse their sons.
            AstNode *son = nodePtr->son;
            while (son != nullptr) {
                formatNode(son, indents);
                son = son->brother;
            }
        }
    }
    return;
}

void Formatter::format(const Ast &ast) {
    std::cout << std::endl;
    formatNode(ast.head, 0);
    std::cout << std::endl;
    return;
}

}  // namespace pluma