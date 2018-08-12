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
#include <occa/lang/builtins/types.hpp>
#include <occa/lang/builtins/transforms/dim.hpp>
#include <occa/lang/expr.hpp>
#include <occa/lang/statement.hpp>
#include <occa/lang/variable.hpp>

namespace occa {
  namespace lang {
    namespace transforms {
      dim::dim() :
        scopeSmnt(NULL) {
        validStatementTypes = (statementType::expression |
                               statementType::declaration);
        validExprNodeTypes = expr::nodeType::call;
      }

      statement_t* dim::transformStatement(statement_t &smnt) {
        bool success = true;
        if (smnt.type() & statementType::expression) {
          success = applyToExpr(smnt, ((expressionStatement&) smnt).expr);
        } else {
          success = applyToDeclStatement((declarationStatement&) smnt);
        }
        return success ? &smnt : NULL;
      }

      expr::node_t* dim::transformExprNode(expr::node_t &node) {
        expr::callNode_t &call = (expr::callNode_t&) node;
        if (!(call.value->type() & expr::nodeType::variable)) {
          return &node;
        }

        // Check @dim
        variable_t &var = ((expr::variableNode_t*) call.value)->value;
        attributeTokenMap::iterator it = var.attributes.find("dim");
        if (it == var.attributes.end()) {
          return &node;
        }
        attributeToken_t &dimAttr = it->second;
        if (!isValidDim(call, dimAttr)) {
          return NULL;
        }
        const int dimCount = (int) call.args.size();

        // Check @dimOrder
        intVector order(dimCount);
        it = var.attributes.find("dimOrder");
        if (it == var.attributes.end()) {
          for (int i = 0; i < dimCount; ++i) {
            order[i] = i;
          }
        } else if (!getDimOrder(dimAttr, it->second, order)) {
          return NULL;
        }
        // 3
        // 2 + (2 * 3)
        expr::node_t *index = call.args[order[dimCount - 1]];
        for (int i = (dimCount - 2); i >= 0; --i) {
          const int i2 = order[i];
          token_t *source = call.args[i2]->token;
          expr::node_t *indexInParen = index->wrapInParentheses();
          // Don't delete the initial call.args[...]
          if (i < (dimCount - 2)) {
            delete index;
          }
          expr::node_t *dimInParen = dimAttr.args[i2].value->wrapInParentheses();
          expr::binaryOpNode_t mult(source,
                                    op::mult,
                                    *dimInParen,
                                    *indexInParen);
          delete dimInParen;
          delete indexInParen;
          expr::parenthesesNode_t multInParen(source,
                                              mult);
          expr::node_t *argInParen = call.args[i2]->wrapInParentheses();

          index = new expr::binaryOpNode_t(source,
                                           op::add,
                                           *argInParen,
                                           multInParen);
          delete argInParen;
        }
        expr::node_t *newValue = new expr::subscriptNode_t(call.token,
                                                           *(call.value),
                                                           *index);
        // Don't delete the initial call.args[...]
        if (dimCount > 1) {
          delete index;
        }
        return scopeSmnt->replaceIdentifiers(newValue);
      }

      bool dim::isValidDim(expr::callNode_t &call,
                           attributeToken_t &dimAttr) {
        const int dimCount = (int) dimAttr.args.size();
        const int argCount = (int) call.args.size();
        if (dimCount == argCount) {
          return true;
        }

        if (dimCount < argCount) {
          call.args[dimCount]->token->printError("Too many dimensions, expected "
                                                 + occa::toString(dimCount)
                                                 + " argument(s)");
        } else {
          call.value->token->printError("Missing dimensions, expected "
                                        + occa::toString(dimCount)
                                        + " argument(s)");
        }
        return false;
      }

      bool dim::getDimOrder(attributeToken_t &dimAttr,
                            attributeToken_t &dimOrderAttr,
                            intVector &order) {
        const int dimCount   = (int) dimAttr.args.size();
        const int orderCount = (int) dimOrderAttr.args.size();
        if (dimCount < orderCount) {
          dimAttr.printError("Too many dimensions, expected "
                             + occa::toString(dimCount)
                             + " argument(s)");
          return false;
        }
        if (dimCount > orderCount) {
          dimAttr.printError("Missing dimensions, expected "
                             + occa::toString(dimCount)
                             + " argument(s)");
          return false;
        }
        for (int i = 0; i < orderCount; ++i) {
          order[i] = (int) dimOrderAttr.args[i].value->evaluate();
        }
        return true;
      }

      bool dim::applyToDeclStatement(declarationStatement &smnt) {
        const int declCount = (int) smnt.declarations.size();
        for (int i = 0; i < declCount; ++i) {
          if (!applyToExpr(smnt, smnt.declarations[i].value)) {
            return false;
          }
        }
        return true;
      }

      bool dim::applyToExpr(statement_t &smnt,
                            expr::node_t *&expr) {
        if (expr == NULL) {
          return true;
        }
        if (smnt.is<blockStatement>()) {
          scopeSmnt = (blockStatement*) &smnt;
        } else {
          scopeSmnt = smnt.up;
        }
        expr = exprTransform::apply(*expr);
        return expr;
      }

      bool applyDimTransforms(statement_t &smnt) {
        dim dimTransform;
        return dimTransform.statementTransform::apply(smnt);
      }
    }
  }
}
