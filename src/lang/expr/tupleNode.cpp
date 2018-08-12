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
#include <occa/lang/expr/tupleNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      tupleNode_t::tupleNode_t(token_t *token_,
                               const nodeVector &args_) :
        node_t(token_) {
        cloneNodeVector(args, args_);
      }

      tupleNode_t::tupleNode_t(const tupleNode_t &other) :
        node_t(other.token) {
        cloneNodeVector(args, other.args);
      }

      tupleNode_t::~tupleNode_t() {
        freeNodeVector(args);
      }

      udim_t tupleNode_t::type() const {
        return nodeType::tuple;
      }

      node_t* tupleNode_t::startNode() {
        const int argCount = (int) args.size();
        return (argCount ? args[0]->startNode() : this);
      }

      node_t* tupleNode_t::endNode() {
        const int argCount = (int) args.size();
        return (argCount ? args[argCount - 1]->endNode() : this);
      }

      node_t* tupleNode_t::clone() const {
        return new tupleNode_t(token, args);
      }

      void tupleNode_t::setChildren(nodeRefVector &children) {
        const int argCount = (int) args.size();
        if (!argCount) {
          return;
        }

        children.reserve(argCount);
        for (int i = 0; i < argCount; ++i) {
          children.push_back(&(args[i]));
        }
      }

      void tupleNode_t::print(printer &pout) const {
        pout << '{';
        const int argCount = (int) args.size();
        for (int i = 0; i < argCount; ++i) {
          if (i) {
            pout << ", ";
          }
          pout << *(args[i]);
        }
        pout << '}';
      }

      void tupleNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---(tuple)\n";
        for (int i = 0; i < ((int) args.size()); ++i) {
          args[i]->childDebugPrint(prefix);
        }
      }
    }
  }
}
