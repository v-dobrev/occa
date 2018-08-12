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
#include <occa/lang/expr/throwNode.hpp>
#include <occa/lang/expr/parenthesesNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      throwNode_t::throwNode_t(token_t *token_,
                               const node_t &value_) :
        node_t(token_),
        value(value_.clone()) {}

      throwNode_t::throwNode_t(const throwNode_t &other) :
        node_t(other.token),
        value(other.value->clone()) {}

      throwNode_t::~throwNode_t() {
        delete value;
      }

      udim_t throwNode_t::type() const {
        return nodeType::throw_;
      }

      node_t* throwNode_t::clone() const {
        return new throwNode_t(token, *value);
      }

      node_t* throwNode_t::endNode() {
        return value->endNode();
      }

      void throwNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      node_t* throwNode_t::wrapInParentheses() {
        return new parenthesesNode_t(token, *this);
      }

      void throwNode_t::print(printer &pout) const {
        pout << "throw";
        if (value->type() != nodeType::empty) {
          pout << ' ' << *value;
        }
      }

      void throwNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|\n"
                  << prefix << "|---[";
        pout << *value;
        std::cerr << "] (throw)\n";
      }
    }
  }
}
