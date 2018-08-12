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
#include <occa/lang/expr/ternaryOpNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      ternaryOpNode_t::ternaryOpNode_t(const node_t &checkValue_,
                                       const node_t &trueValue_,
                                       const node_t &falseValue_) :
        opNode_t(checkValue_.token, op::ternary),
        checkValue(checkValue_.clone()),
        trueValue(trueValue_.clone()),
        falseValue(falseValue_.clone()) {}

      ternaryOpNode_t::ternaryOpNode_t(const ternaryOpNode_t &other) :
        opNode_t(other.token, op::ternary),
        checkValue(other.checkValue->clone()),
        trueValue(other.trueValue->clone()),
        falseValue(other.falseValue->clone()) {}

      ternaryOpNode_t::~ternaryOpNode_t() {
        delete checkValue;
        delete trueValue;
        delete falseValue;
      }

      udim_t ternaryOpNode_t::type() const {
        return nodeType::ternary;
      }

      opType_t ternaryOpNode_t::opType() const {
        return operatorType::ternary;
      }

      node_t* ternaryOpNode_t::clone() const {
        return new ternaryOpNode_t(*checkValue,
                                   *trueValue,
                                   *falseValue);
      }

      bool ternaryOpNode_t::canEvaluate() const {
        return (checkValue->canEvaluate() &&
                trueValue->canEvaluate()  &&
                falseValue->canEvaluate());
      }

      primitive ternaryOpNode_t::evaluate() const {
        if ((bool) checkValue->evaluate()) {
          return trueValue->evaluate();
        }
        return falseValue->evaluate();
      }

      node_t* ternaryOpNode_t::startNode() {
        return checkValue->startNode();
      }

      node_t* ternaryOpNode_t::endNode() {
        return falseValue->endNode();
      }

      void ternaryOpNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&checkValue);
        children.push_back(&trueValue);
        children.push_back(&falseValue);
      }

      void ternaryOpNode_t::print(printer &pout) const {
        pout << *checkValue
             << " ? " << *trueValue
             << " : " << *falseValue;
      }

      void ternaryOpNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[?:] (ternary)\n";
        checkValue->childDebugPrint(prefix);
        trueValue->childDebugPrint(prefix);
        falseValue->childDebugPrint(prefix);
      }
    }
  }
}
