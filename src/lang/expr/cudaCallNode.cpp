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
#include <occa/lang/expr/cudaCallNode.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      cudaCallNode_t::cudaCallNode_t(token_t *token_,
                                     const node_t &value_,
                                     const node_t &blocks_,
                                     const node_t &threads_) :
        node_t(token_),
        value(value_.clone()),
        blocks(blocks_.clone()),
        threads(threads_.clone()) {}

      cudaCallNode_t::cudaCallNode_t(const cudaCallNode_t &other) :
        node_t(other.token),
        value(other.value->clone()),
        blocks(other.blocks->clone()),
        threads(other.threads->clone()) {}

      cudaCallNode_t::~cudaCallNode_t() {
        delete value;
        delete blocks;
        delete threads;
      }

      udim_t cudaCallNode_t::type() const {
        return nodeType::cudaCall;
      }

      node_t* cudaCallNode_t::startNode() {
        return value->startNode();
      }

      node_t* cudaCallNode_t::endNode() {
        return threads->endNode();
      }

      node_t* cudaCallNode_t::clone() const {
        return new cudaCallNode_t(token,
                                  *value,
                                  *blocks,
                                  *threads);
      }

      void cudaCallNode_t::setChildren(nodeRefVector &children) {
        children.push_back(&value);
        children.push_back(&blocks);
        children.push_back(&threads);
      }

      void cudaCallNode_t::print(printer &pout) const {
        pout << *value
             << "<<<"
             << *blocks << ", " << *threads
             << ">>>";
      }

      void cudaCallNode_t::debugPrint(const std::string &prefix) const {
        printer pout(std::cerr);
        std::cerr << prefix << "|\n"
                  << prefix << "|---[<<<...>>>";
        std::cerr << "] (cudaCall)\n";
        value->childDebugPrint(prefix);
        blocks->childDebugPrint(prefix);
        threads->childDebugPrint(prefix);
      }
    }
  }
}
