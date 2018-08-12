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
#include <occa/lang/expr/parenthesesNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      parenthesesNode_t::parenthesesNode_t(token_t *token_,
                                           const node_t &value_) :
        node_t(token_),
        value(value_.clone()) {}

      parenthesesNode_t::parenthesesNode_t(const parenthesesNode_t &other) :
        node_t(other.token),
        value(other.value->clone()) {}

      parenthesesNode_t::~parenthesesNode_t() {
        delete value;
      }

      udim_t parenthesesNode_t::type() const {
        return nodeType::parentheses;
      }

      node_t* parenthesesNode_t::startNode() {
        return value->startNode();
      }

      node_t* parenthesesNode_t::endNode() {
        return value->endNode();
      }

      node_t* parenthesesNode_t::clone() const {
        return new parenthesesNode_t(token, *value);
      }

      bool parenthesesNode_t::canEvaluate() const {
        return value->canEvaluate();
      }

      primitive parenthesesNode_t::evaluate() const {
        return value->evaluate();
      }

      void parenthesesNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      variable_t* parenthesesNode_t::getVariable() {
        return value->getVariable();
      }

      void parenthesesNode_t::print(printer &pout) const {
        pout << '(' << *value << ')';
      }

      void parenthesesNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[()] (parentheses)\n";
        value->childDebugPrint(prefix);
      }
    }
  }
}
