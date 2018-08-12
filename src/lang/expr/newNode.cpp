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
#include <occa/lang/expr/emptyNode.hpp>
#include <occa/lang/expr/newNode.hpp>
#include <occa/lang/expr/parenthesesNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      newNode_t::newNode_t(token_t *token_,
                           const vartype_t &valueType_,
                           const node_t &value_) :
        node_t(token_),
        valueType(valueType_),
        value(value_.clone()),
        size(noNode.clone()) {}

      newNode_t::newNode_t(token_t *token_,
                           const vartype_t &valueType_,
                           const node_t &value_,
                           const node_t &size_) :
        node_t(token_),
        valueType(valueType_),
        value(value_.clone()),
        size(size_.clone()) {}

      newNode_t::newNode_t(const newNode_t &other) :
        node_t(other.token),
        valueType(other.valueType),
        value(other.value->clone()),
        size(other.size->clone()) {}

      newNode_t::~newNode_t() {
        delete value;
        delete size;
      }

      udim_t newNode_t::type() const {
        return nodeType::new_;
      }

      node_t* newNode_t::clone() const {
        return new newNode_t(token, valueType, *value, *size);
      }

      node_t* newNode_t::endNode() {
        return (size ? size : value)->endNode();
      }

      void newNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
        children.push_back(&size);
      }

      node_t* newNode_t::wrapInParentheses() {
        return new parenthesesNode_t(token, *this);
      }

      void newNode_t::print(printer &pout) const {
        // TODO: Print type without qualifiers
        //       Also convert [] to *
        pout << "new " << valueType << *value;
        if (size->type() != nodeType::empty) {
          pout << '[' << *size << ']';
        }
      }

      void newNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << valueType;
        std::cerr << "] (new)\n";
        value->childDebugPrint(prefix);
        size->childDebugPrint(prefix);
      }
    }
  }
}
