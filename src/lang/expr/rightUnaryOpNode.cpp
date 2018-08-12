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
#include <occa/lang/expr/rightUnaryOpNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      rightUnaryOpNode_t::rightUnaryOpNode_t(token_t *token_,
                                             const unaryOperator_t &op_,
                                             const node_t &value_) :
        opNode_t(token_, op_),
        value(value_.clone()) {}

      rightUnaryOpNode_t::rightUnaryOpNode_t(const rightUnaryOpNode_t &other) :
        opNode_t(other.token, other.op),
        value(other.value->clone()) {}

      rightUnaryOpNode_t::~rightUnaryOpNode_t() {
        delete value;
      }

      udim_t rightUnaryOpNode_t::type() const {
        return nodeType::rightUnary;
      }

      node_t* rightUnaryOpNode_t::clone() const {
        return new rightUnaryOpNode_t(token,
                                      (const unaryOperator_t&) op,
                                      *value);
      }

      bool rightUnaryOpNode_t::canEvaluate() const {
        return value->canEvaluate();
      }

      primitive rightUnaryOpNode_t::evaluate() const {
        primitive pValue = value->evaluate();
        return ((unaryOperator_t&) op)(pValue);
      }

      node_t* rightUnaryOpNode_t::startNode() {
        return value->startNode();
      }

      void rightUnaryOpNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      variable_t* rightUnaryOpNode_t::getVariable() {
        return value->getVariable();
      }

      void rightUnaryOpNode_t::print(printer &pout) const {
        pout << *value << op;
      }

      void rightUnaryOpNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << op;
        std::cerr << "] (rightUnary)\n";
        value->childDebugPrint(prefix);
      }
    }
  }
}
