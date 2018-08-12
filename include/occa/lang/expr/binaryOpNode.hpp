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
#ifndef OCCA_LANG_EXPR_BINARYOPNODE_HEADER
#define OCCA_LANG_EXPR_BINARYOPNODE_HEADER

#include <occa/lang/expr/node.hpp>
#include <occa/lang/expr/opNode.hpp>

namespace occa {
  namespace lang {
    class binaryOperator_t;

    namespace expr {
      class binaryOpNode_t : public opNode_t {
      public:
        node_t *leftValue, *rightValue;

        binaryOpNode_t(const binaryOperator_t &op_,
                       const node_t &leftValue_,
                       const node_t &rightValue_);

        binaryOpNode_t(token_t *token,
                       const binaryOperator_t &op_,
                       const node_t &leftValue_,
                       const node_t &rightValue_);

        binaryOpNode_t(const binaryOpNode_t &other);

        virtual ~binaryOpNode_t();

        virtual udim_t type() const;

        virtual node_t* clone() const;

        virtual bool canEvaluate() const;
        virtual primitive evaluate() const;

        virtual node_t* startNode();
        virtual node_t* endNode();

        virtual void setChildren(nodeRefVector &children);

        virtual variable_t* getVariable();

        virtual void print(printer &pout) const;

        virtual void debugPrint(const std::string &prefix) const;
      };
    }
  }
}

#endif
