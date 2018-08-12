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
#include <occa/lang/expr/parenCastNode.hpp>
#include <occa/lang/expr/parenthesesNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      parenCastNode_t::parenCastNode_t(token_t *token_,
                                       const vartype_t &valueType_,
                                       const node_t &value_) :
        node_t(token_),
        valueType(valueType_),
        value(value_.clone()) {}

      parenCastNode_t::parenCastNode_t(const parenCastNode_t &other) :
        node_t(other.token),
        valueType(other.valueType),
        value(other.value->clone()) {}

      parenCastNode_t::~parenCastNode_t() {
        delete value;
      }

      udim_t parenCastNode_t::type() const {
        return nodeType::parenCast;
      }

      node_t* parenCastNode_t::startNode() {
        return value->startNode();
      }

      node_t* parenCastNode_t::endNode() {
        return value->endNode();
      }

      node_t* parenCastNode_t::clone() const {
        return new parenCastNode_t(token, valueType, *value);
      }

      void parenCastNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      node_t* parenCastNode_t::wrapInParentheses() {
        return new parenthesesNode_t(token, *this);
      }

      void parenCastNode_t::print(printer &pout) const {
        // TODO: Print type without qualifiers
        //       Also convert [] to *
        pout << '(' << valueType << ") " << *value;
      }

      void parenCastNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << valueType;
        std::cerr << "] (parenCast)\n";
        value->childDebugPrint(prefix);
      }
    }
  }
}
