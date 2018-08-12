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
#include <occa/lang/token.hpp>
#include <occa/lang/expr/opNode.hpp>
#include <occa/lang/expr/parenthesesNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      opNode_t::opNode_t(operatorToken &token_) :
        node_t(&token_),
        op(*(token_.op)) {}

      opNode_t::opNode_t(token_t *token_,
                         const operator_t &op_) :
        node_t(token_),
        op(op_) {}

      opType_t opNode_t::opType() const {
        return op.opType;
      }

      udim_t opNode_t::type() const {
        return nodeType::rawOp;
      }

      node_t* opNode_t::clone() const {
        return new opNode_t(token, op);
      }

      void opNode_t::setChildren(nodeRefVector &children) {}

      node_t* opNode_t::wrapInParentheses() {
        return new parenthesesNode_t(token, *this);
      }

      void opNode_t::print(printer &pout) const {
        token->printError("[Waldo] (opNode_t) Unsure how you got here...");
      }

      void opNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << op;
        std::cerr << "] (opNode_t)\n";
      }
    }
  }
}
