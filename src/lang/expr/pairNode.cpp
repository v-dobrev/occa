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
#include <occa/lang/expr/pairNode.hpp>
#include <occa/lang/token.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      pairNode_t::pairNode_t(operatorToken &opToken,
                             const node_t &value_) :
        node_t(&opToken),
        op(*(opToken.op)),
        value(value_.clone()) {}

      pairNode_t::pairNode_t(const pairNode_t &other) :
        node_t(other.token),
        op(other.op),
        value(other.value->clone()) {}

      pairNode_t::~pairNode_t() {
        delete value;
      }

      udim_t pairNode_t::type() const {
        return nodeType::pair;
      }

      opType_t pairNode_t::opType() const {
        return op.opType;
      }

      node_t* pairNode_t::startNode() {
        return value->startNode();
      }

      node_t* pairNode_t::endNode() {
        return value->endNode();
      }

      node_t* pairNode_t::clone() const {
        return new pairNode_t(token->to<operatorToken>(),
                              *value);
      }

      bool pairNode_t::canEvaluate() const {
        token->printError("[Waldo] (pairNode_t) Unsure how you got here...");
        return false;
      }

      primitive pairNode_t::evaluate() const {
        token->printError("[Waldo] (pairNode_t) Unsure how you got here...");
        return primitive();
      }

      void pairNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      void pairNode_t::print(printer &pout) const {
        token->printError("[Waldo] (pairNode_t) Unsure how you got here...");
      }

      void pairNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << op;
        std::cerr << "] (pairNode_t)\n";
        value->childDebugPrint(prefix);
      }
    }
  }
}
