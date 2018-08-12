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
#include <occa/lang/builtins/transforms/replacer.hpp>
#include <occa/lang/expr.hpp>
#include <occa/lang/statement.hpp>
#include <occa/lang/variable.hpp>

namespace occa {
  namespace lang {
    namespace transforms {
      typeReplacer_t::typeReplacer_t() :
        statementExprTransform(expr::nodeType::type),
        from(NULL),
        to(NULL) {}

      void typeReplacer_t::set(const type_t &from_,
                               type_t &to_) {
        from = &from_;
        to   = &to_;
      }

      expr::node_t* typeReplacer_t::transformExprNode(expr::node_t &node) {
        if (!from || !to) {
          return &node;
        }
        type_t &type = ((expr::typeNode_t&) node).value;
        if (&type != from) {
          return &node;
        }
        return new expr::typeNode_t(node.token, *to);
      }

      variableReplacer_t::variableReplacer_t() :
        statementExprTransform(expr::nodeType::variable),
        from(NULL),
        to(NULL) {}

      void variableReplacer_t::set(const variable_t &from_,
                                   variable_t &to_) {
        from = &from_;
        to   = &to_;
      }

      expr::node_t* variableReplacer_t::transformExprNode(expr::node_t &node) {
        if (!from || !to) {
          return &node;
        }
        variable_t &var = ((expr::variableNode_t&) node).value;
        if (&var != from) {
          return &node;
        }
        return new expr::variableNode_t(node.token, *to);
      }

      functionReplacer_t::functionReplacer_t() :
        statementExprTransform(expr::nodeType::function),
        from(NULL),
        to(NULL) {}

      void functionReplacer_t::set(const function_t &from_,
                                   function_t &to_) {
        from = &from_;
        to   = &to_;
      }

      expr::node_t* functionReplacer_t::transformExprNode(expr::node_t &node) {
        if (!from || !to) {
          return &node;
        }
        function_t &func = ((expr::functionNode_t&) node).value;
        if (&func != from) {
          return &node;
        }
        return new expr::functionNode_t(node.token, *to);
      }
    }

    void replaceTypes(statement_t &smnt,
                      const type_t &from,
                      type_t &to) {
      transforms::typeReplacer_t replacer;
      replacer.set(from, to);
      replacer.statementTransform::apply(smnt);
    }

    void replaceTypes(expr::node_t &expr,
                      const type_t &from,
                      type_t &to) {
      transforms::typeReplacer_t replacer;
      replacer.set(from, to);
      replacer.exprTransform::apply(expr);
    }

    void replaceVariables(statement_t &smnt,
                          const variable_t &from,
                          variable_t &to) {
      transforms::variableReplacer_t replacer;
      replacer.set(from, to);
      replacer.statementTransform::apply(smnt);
    }

    void replaceVariables(expr::node_t &expr,
                          const variable_t &from,
                          variable_t &to) {
      transforms::variableReplacer_t replacer;
      replacer.set(from, to);
      replacer.exprTransform::apply(expr);
    }

    void replaceFunctions(statement_t &smnt,
                          const function_t &from,
                          function_t &to) {
      transforms::functionReplacer_t replacer;
      replacer.set(from, to);
      replacer.statementTransform::apply(smnt);
    }

    void replaceFunctions(expr::node_t &expr,
                          const function_t &from,
                          function_t &to) {
      transforms::functionReplacer_t replacer;
      replacer.set(from, to);
      replacer.exprTransform::apply(expr);
    }

    void replaceKeywords(statement_t &smnt,
                         const keyword_t &from,
                         keyword_t &to) {
      const int kType = from.type();
      if (kType & keywordType::type) {
        const type_t &fromValue = ((const typeKeyword&) from).type_;
        type_t &toValue = ((typeKeyword&) to).type_;
        replaceTypes(smnt, fromValue, toValue);
      }
      else if (kType & keywordType::variable) {
        const variable_t &fromValue = ((const variableKeyword&) from).variable;
        variable_t &toValue = ((variableKeyword&) to).variable;
        replaceVariables(smnt, fromValue, toValue);
      }
      else if (kType & keywordType::function) {
        const function_t &fromValue = ((const functionKeyword&) from).function;
        function_t &toValue = ((functionKeyword&) to).function;
        replaceFunctions(smnt, fromValue, toValue);
      }
    }

    void replaceKeywords(expr::node_t &expr,
                         const keyword_t &from,
                         keyword_t &to) {
      const int kType = from.type();
      if (kType & keywordType::type) {
        const type_t &fromValue = ((const typeKeyword&) from).type_;
        type_t &toValue = ((typeKeyword&) to).type_;
        replaceTypes(expr, fromValue, toValue);
      }
      else if (kType & keywordType::variable) {
        const variable_t &fromValue = ((const variableKeyword&) from).variable;
        variable_t &toValue = ((variableKeyword&) to).variable;
        replaceVariables(expr, fromValue, toValue);
      }
      else if (kType & keywordType::function) {
        const function_t &fromValue = ((const functionKeyword&) from).function;
        function_t &toValue = ((functionKeyword&) to).function;
        replaceFunctions(expr, fromValue, toValue);
      }
    }
  }
}
