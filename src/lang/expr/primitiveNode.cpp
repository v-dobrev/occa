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
#include <occa/lang/expr/primitiveNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      primitiveNode_t::primitiveNode_t(token_t *token_,
                                       primitive value_) :
        node_t(token_),
        value(value_) {}

      primitiveNode_t::primitiveNode_t(const primitiveNode_t &other) :
        node_t(other.token),
        value(other.value) {}

      primitiveNode_t::~primitiveNode_t() {}

      udim_t primitiveNode_t::type() const {
        return nodeType::primitive;
      }

      node_t* primitiveNode_t::clone() const {
        return new primitiveNode_t(token, value);
      }

      bool primitiveNode_t::canEvaluate() const {
        return true;
      }

      primitive primitiveNode_t::evaluate() const {
        return value;
      }

      void primitiveNode_t::setChildren(nodeRefVector &children) {}

      void primitiveNode_t::print(printer &pout) const {
        pout << value.toString();
      }

      void primitiveNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << (*this);
        std::cerr << "] (primitive)\n";
      }
    }
  }
}
