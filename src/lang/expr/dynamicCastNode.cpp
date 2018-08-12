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
#include <occa/lang/expr/dynamicCastNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      dynamicCastNode_t::dynamicCastNode_t(token_t *token_,
                                           const vartype_t &valueType_,
                                           const node_t &value_) :
        node_t(token_),
        valueType(valueType_),
        value(value_.clone()) {}

      dynamicCastNode_t::dynamicCastNode_t(const dynamicCastNode_t &other) :
        node_t(other.token),
        valueType(other.valueType),
        value(other.value->clone()) {}

      dynamicCastNode_t::~dynamicCastNode_t() {
        delete value;
      }

      udim_t dynamicCastNode_t::type() const {
        return nodeType::dynamicCast;
      }

      node_t* dynamicCastNode_t::endNode() {
        return value->endNode();
      }

      node_t* dynamicCastNode_t::clone() const {
        return new dynamicCastNode_t(token, valueType, *value);
      }

      void dynamicCastNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
      }

      void dynamicCastNode_t::print(printer &pout) const {
        // TODO: Print type without qualifiers
        //       Also convert [] to *
        pout << "dynamic_cast<" << valueType << ">("
             << *value << ')';
      }

      void dynamicCastNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << valueType;
        std::cerr << "] (dynamicCast)\n";
        value->childDebugPrint(prefix);
      }
    }
  }
}
