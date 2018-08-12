/* The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 David Medina and Tim Warburton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 */
#ifndef OCCA_LANG_STATEMENT_HEADER
#define OCCA_LANG_STATEMENT_HEADER

#include <vector>

#include <occa/lang/baseStatement.hpp>
#include <occa/lang/scope.hpp>
#include <occa/lang/token.hpp>
#include <occa/lang/type.hpp>

namespace occa {
  namespace lang {
    class ifStatement;
    class elifStatement;
    class elseStatement;
    class variableDeclaration;

    typedef std::vector<elifStatement*>      elifStatementVector;
    typedef std::vector<variableDeclaration> variableDeclarationVector;

    //---[ Preprocessor ]---------------
    class directiveStatement : public statement_t {
    public:
      directiveToken &token;

      directiveStatement(blockStatement *up_,
                         const directiveToken &token_);
      directiveStatement(blockStatement *up_,
                         const directiveStatement &other);
      ~directiveStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      std::string& value();
      const std::string& value() const;

      virtual void print(printer &pout) const;
    };

    class pragmaStatement : public statement_t {
    public:
      pragmaToken &token;

      pragmaStatement(blockStatement *up_,
                      const pragmaToken &token_);
      pragmaStatement(blockStatement *up_,
                      const pragmaStatement &other);
      ~pragmaStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      std::string& value();
      const std::string& value() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Type ]-----------------------
    // TODO: Type declaration
    class functionStatement : public statement_t {
    public:
      function_t &function;

      functionStatement(blockStatement *up_,
                        function_t &function_);
      functionStatement(blockStatement *up_,
                        const functionStatement&other);
      ~functionStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class functionDeclStatement : public blockStatement {
    public:
      function_t &function;

      functionDeclStatement(blockStatement *up_,
                            function_t &function_);
      functionDeclStatement(blockStatement *up_,
                            const functionDeclStatement &other);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      bool updateScope(const bool force = false);
      void addArgumentsToScope(const bool force = false);

      virtual void print(printer &pout) const;
    };

    class classAccessStatement : public statement_t {
    public:
      int access;

      classAccessStatement(blockStatement *up_,
                           token_t *source_,
                           const int access_);
      classAccessStatement(blockStatement *up_,
                           const classAccessStatement &other);
      ~classAccessStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Expression ]-----------------
    class expressionStatement : public statement_t {
    public:
      expr::node_t *expr;
      bool hasSemicolon;

      expressionStatement(blockStatement *up_,
                          expr::node_t &expr_,
                          const bool hasSemicolon_ = true);
      expressionStatement(blockStatement *up_,
                          const expressionStatement &other);
      ~expressionStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class declarationStatement : public statement_t {
    public:
      variableDeclarationVector declarations;

      declarationStatement(blockStatement *up_,
                           token_t *source_);
      declarationStatement(blockStatement *up_,
                           const declarationStatement &other);
      ~declarationStatement();

      void clearDeclarations();
      void freeDeclarations();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      bool addDeclaration(const variableDeclaration &decl,
                          const bool force = false);

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Goto ]-----------------------
    class gotoStatement : public statement_t {
    public:
      identifierToken &labelToken;

      gotoStatement(blockStatement *up_,
                    identifierToken &labelToken_);
      gotoStatement(blockStatement *up_,
                    const gotoStatement &other);
      ~gotoStatement();

      std::string& label();
      const std::string& label() const;

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class gotoLabelStatement : public statement_t {
    public:
      identifierToken &labelToken;

      gotoLabelStatement(blockStatement *up_,
                         identifierToken &labelToken_);
      gotoLabelStatement(blockStatement *up_,
                         const gotoLabelStatement &other);
      ~gotoLabelStatement();

      std::string& label();
      const std::string& label() const;

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Namespace ]------------------
    class namespaceStatement : public blockStatement {
    public:
      identifierToken &nameToken;

      namespaceStatement(blockStatement *up_,
                         identifierToken &nameToken_);
      namespaceStatement(blockStatement *up_,
                         const namespaceStatement &other);
      ~namespaceStatement();

      std::string& name();
      const std::string& name() const;

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ If ]-------------------------
    class ifStatement : public blockStatement {
    public:
      statement_t *condition;

      elifStatementVector elifSmnts;
      elseStatement *elseSmnt;

      ifStatement(blockStatement *up_,
                  token_t *source_);
      ifStatement(blockStatement *up_,
                  const ifStatement &other);
      ~ifStatement();

      void setCondition(statement_t *condition_);

      void addElif(elifStatement &elifSmnt);
      void addElse(elseStatement &elseSmnt_);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class elifStatement : public blockStatement {
    public:
      statement_t *condition;

      elifStatement(blockStatement *up_,
                    token_t *source_);
      elifStatement(blockStatement *up_,
                    const elifStatement &other);
      ~elifStatement();

      void setCondition(statement_t *condition_);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class elseStatement : public blockStatement {
    public:
      elseStatement(blockStatement *up_,
                    token_t *source_);
      elseStatement(blockStatement *up_,
                    const elseStatement &other);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ For ]------------------------
    class forStatement : public blockStatement {
    public:
      statement_t *init, *check, *update;

      forStatement(blockStatement *up_,
                   token_t *source_);
      forStatement(blockStatement *up_,
                   const forStatement &other);
      ~forStatement();

      void setLoopStatements(statement_t *init_,
                             statement_t *check_,
                             statement_t *update_);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ While ]----------------------
    class whileStatement : public blockStatement {
    public:
      statement_t *condition;
      bool isDoWhile;

      whileStatement(blockStatement *up_,
                     token_t *source_,
                     const bool isDoWhile_ = false);
      whileStatement(blockStatement *up_,
                     const whileStatement &other);
      ~whileStatement();

      void setCondition(statement_t *condition_);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Switch ]---------------------
    class switchStatement : public blockStatement {
    public:
      statement_t *condition;

      switchStatement(blockStatement *up_,
                      token_t *source_);
      switchStatement(blockStatement *up_,
                      const switchStatement& other);
      ~switchStatement();

      void setCondition(statement_t *condition_);

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Case ]-----------------------
    class caseStatement : public statement_t {
    public:
      expr::node_t *value;

      caseStatement(blockStatement *up_,
                    token_t *source_,
                    expr::node_t &value_);
      caseStatement(blockStatement *up_,
                    const caseStatement &other);
      ~caseStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class defaultStatement : public statement_t {
    public:
      defaultStatement(blockStatement *up_,
                       token_t *source_);
      defaultStatement(blockStatement *up_,
                       const defaultStatement &other);
      ~defaultStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Exit ]-----------------------
    class continueStatement : public statement_t {
    public:
      continueStatement(blockStatement *up_,
                        token_t *source_);
      continueStatement(blockStatement *up_,
                        const continueStatement &other);
      ~continueStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class breakStatement : public statement_t {
    public:
      breakStatement(blockStatement *up_,
                     token_t *source_);
      breakStatement(blockStatement *up_,
                        const breakStatement &other);
      ~breakStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };

    class returnStatement : public statement_t {
    public:
      expr::node_t *value;

      returnStatement(blockStatement *up_,
                      token_t *source_,
                      expr::node_t *value_);
      returnStatement(blockStatement *up_,
                        const returnStatement &other);
      ~returnStatement();

      virtual statement_t& clone_(blockStatement *up_) const;

      virtual int type() const;
      virtual std::string statementName() const;

      virtual void print(printer &pout) const;
    };
    //==================================
  }
}

#endif
