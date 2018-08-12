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
#include <occa/lang/expr/leftUnaryOpNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      leftUnaryOpNode_t::leftUnaryOpNode_t(token_t *token_,
                                           const unaryOperator_t &op_,
                                           const node_t &value_) :
        opNode_t(token_, op_),
        value(value_.clone()) {}

      leftUnaryOpNode_t::leftUnaryOpNode_t(const leftUnaryOpNode_t &other) :
        opNode_t(other.token, other.op),
        value(other.value->clone()) {}

      leftUnaryOpNode_t::~leftUnaryOpNode_t() {
        delete value;
      }

      udim_t leftUnaryOpNode_t::type() const {
        return nodeType::leftUnary;
      }

      node_t* leftUnaryOpNode_t::clone() const {
        return new leftUnaryOpNode_t(token,
                                     (const unaryOperator_t&) op,
                                     *value);
      }

      bool leftUnaryOpNode_t::canEvaluate() const {
        if (op.opType & (operatorType::dereference |
                         operatorType::address)) {
          return false;
        }
        return value->canEvaluate();
      }

      primitive leftUnaryOpNode_t::evaluate() const {
        primitive pValue = value->evaluate();
        return ((unaryOperator_t&) op)(pValue);
      }

      node_t* leftUnaryOpNode_t::endNode() {
        return value->endNode();
      }

      void leftUnaryOpNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      variable_t* leftUnaryOpNode_t::getVariable() {
        return value->getVariable();
      }

      void leftUnaryOpNode_t::print(printer &pout) const {
        pout << op << *value;
      }

      void leftUnaryOpNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << op;
        std::cerr << "] (leftUnary)\n";
        value->childDebugPrint(prefix);
      }
    }
  }
}
