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
#include <occa/lang/expr/callNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      callNode_t::callNode_t(token_t *token_,
                             const node_t &value_,
                             const nodeVector &args_) :
        node_t(token_),
        value(value_.clone()) {
        cloneNodeVector(args, args_);
      }

      callNode_t::callNode_t(const callNode_t &other) :
        node_t(other.token),
        value(other.value->clone()) {
        cloneNodeVector(args, other.args);
      }

      callNode_t::~callNode_t() {
        delete value;
        freeNodeVector(args);
      }

      udim_t callNode_t::type() const {
        return nodeType::call;
      }

      node_t* callNode_t::clone() const {
        return new callNode_t(token, *value, args);
      }

      node_t* callNode_t::startNode() {
        return value->startNode();
      }

      node_t* callNode_t::endNode() {
        const int argCount = (int) args.size();
        if (!argCount) {
          return value->endNode();
        }
        return args[argCount - 1]->endNode();
      }

      void callNode_t::setChildren(nodeRefVector &children) {
        const int argCount = (int) args.size();
        children.reserve(1 + argCount);

        children.push_back(&value);
        for (int i = 0; i < argCount; ++i) {
          children.push_back(&(args[i]));
        }
      }

      void callNode_t::print(printer &pout) const {
        pout << *value
             << '(';
        const int argCount = (int) args.size();
        for (int i = 0; i < argCount; ++i) {
          if (i) {
            pout << ", ";
          }
          pout << *(args[i]);
        }
        pout << ')';
      }

      void callNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[";
        pout << *value;
        std::cerr << "] (call)\n";
        for (int i = 0; i < ((int) args.size()); ++i) {
          args[i]->childDebugPrint(prefix);
        }
      }
    }
  }
}
