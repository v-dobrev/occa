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
#include <occa/lang/expr/binaryOpNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      binaryOpNode_t::binaryOpNode_t(token_t *token_,
                                     const binaryOperator_t &op_,
                                     const node_t &leftValue_,
                                     const node_t &rightValue_) :
        opNode_t(token_, op_),
        leftValue(leftValue_.clone()),
        rightValue(rightValue_.clone()) {}

      binaryOpNode_t::binaryOpNode_t(const binaryOpNode_t &other) :
        opNode_t(other.token, other.op),
        leftValue(other.leftValue->clone()),
        rightValue(other.rightValue->clone()) {}

      binaryOpNode_t::~binaryOpNode_t() {
        delete leftValue;
        delete rightValue;
      }

      udim_t binaryOpNode_t::type() const {
        return nodeType::binary;
      }

      node_t* binaryOpNode_t::clone() const {
        return new binaryOpNode_t(token,
                                  (const binaryOperator_t&) op,
                                  *leftValue,
                                  *rightValue);
      }

      bool binaryOpNode_t::canEvaluate() const {
        if (op.opType & (operatorType::scope     |
                         operatorType::dot       |
                         operatorType::dotStar   |
                         operatorType::arrow     |
                         operatorType::arrowStar)) {
          return false;
        }
        return (leftValue->canEvaluate() &&
                rightValue->canEvaluate());
      }

      primitive binaryOpNode_t::evaluate() const {
        primitive pLeft  = leftValue->evaluate();
        primitive pRight = rightValue->evaluate();
        return ((binaryOperator_t&) op)(pLeft, pRight);
      }

      node_t* binaryOpNode_t::startNode() {
        return leftValue->startNode();
      }

      node_t* binaryOpNode_t::endNode() {
        return rightValue->endNode();
      }

      void binaryOpNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&leftValue);
        children.push_back(&rightValue);
      }

      variable_t* binaryOpNode_t::getVariable() {
        return leftValue->getVariable();
      }

      void binaryOpNode_t::print(printer &pout) const {
        if (op.opType & (operatorType::scope |
                         operatorType::dot |
                         operatorType::dotStar |
                         operatorType::arrow |
                         operatorType::arrowStar)) {
          pout << *leftValue << op << *rightValue;
        }
        else if (op.opType & operatorType::comma) {
          pout << *leftValue << ", " << *rightValue;
        } else {
          pout << *leftValue << ' ' << op << ' ' << *rightValue;
        }
      }

      void binaryOpNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << op;
        std::cerr << "] (binary)\n";
        leftValue->childDebugPrint(prefix);
        rightValue->childDebugPrint(prefix);
      }
    }
  }
}
