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

#ifndef OCCA_LANG_BUILTINS_TRANSFORMS_DIM_HEADER
#define OCCA_LANG_BUILTINS_TRANSFORMS_DIM_HEADER

#include <occa/lang/expr/node.hpp>
#include <occa/lang/expr/callNode.hpp>
#include <occa/lang/exprTransform.hpp>
#include <occa/lang/statement.hpp>
#include <occa/lang/statementTransform.hpp>

namespace occa {
  namespace lang {
    namespace transforms {
      class dim : public statementTransform,
                  public exprTransform {
      public:
        blockStatement *scopeSmnt;

        dim();

        virtual statement_t* transformStatement(statement_t &smnt);
        virtual expr::node_t* transformExprNode(expr::node_t &node);

        bool isValidDim(expr::callNode_t &call,
                        attributeToken_t &dimAttr);

        bool getDimOrder(attributeToken_t &dimAttr,
                         attributeToken_t &dimOrderAttr,
                         intVector &order);

        bool applyToDeclStatement(declarationStatement &smnt);
        bool applyToExpr(statement_t &smnt,
                         expr::node_t *&expr);
      };

      bool applyDimTransforms(statement_t &smnt);
    }
  }
}

#endif
