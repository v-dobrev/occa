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
#include <occa/lang/expr/node.hpp>
#include <occa/lang/token.hpp>
#include <occa/tools/string.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      namespace nodeType {
        const udim_t empty             = (1L << 0);
        const udim_t primitive         = (1L << 1);
        const udim_t char_             = (1L << 2);
        const udim_t string            = (1L << 3);
        const udim_t identifier        = (1L << 4);
        const udim_t type              = (1L << 5);
        const udim_t vartype           = (1L << 6);
        const udim_t variable          = (1L << 7);
        const udim_t function          = (1L << 8);

        const udim_t value             = (primitive |
                                          type      |
                                          vartype   |
                                          variable  |
                                          function);

        const udim_t rawOp             = (1L << 9);
        const udim_t leftUnary         = (1L << 10);
        const udim_t rightUnary        = (1L << 11);
        const udim_t binary            = (1L << 12);
        const udim_t ternary           = (1L << 13);
        const udim_t op                = (leftUnary  |
                                          rightUnary |
                                          binary     |
                                          ternary);

        const udim_t pair              = (1L << 14);

        const udim_t subscript         = (1L << 15);
        const udim_t call              = (1L << 16);

        const udim_t sizeof_           = (1L << 17);
        const udim_t sizeof_pack_      = (1L << 18);
        const udim_t new_              = (1L << 19);
        const udim_t delete_           = (1L << 20);
        const udim_t throw_            = (1L << 21);

        const udim_t typeid_           = (1L << 22);
        const udim_t noexcept_         = (1L << 23);
        const udim_t alignof_          = (1L << 24);

        const udim_t const_cast_       = (1L << 25);
        const udim_t dynamic_cast_     = (1L << 26);
        const udim_t static_cast_      = (1L << 27);
        const udim_t reinterpret_cast_ = (1L << 28);

        const udim_t funcCast          = (1L << 29);
        const udim_t parenCast         = (1L << 30);
        const udim_t constCast         = (1L << 31);
        const udim_t staticCast        = (1L << 32);
        const udim_t reinterpretCast   = (1L << 33);
        const udim_t dynamicCast       = (1L << 34);

        const udim_t parentheses       = (1L << 35);
        const udim_t tuple             = (1L << 36);
        const udim_t cudaCall          = (1L << 37);
      }

      node_t::node_t(token_t *token_) :
        token(token_t::clone(token_)) {}

      node_t::~node_t() {
        delete token;
      }

      node_t* node_t::clone(node_t *expr) {
        if (!expr) {
          return NULL;
        }
        return expr->clone();
      }

      bool node_t::canEvaluate() const {
        return false;
      }

      primitive node_t::evaluate() const {
        return primitive();
      }

      node_t* node_t::startNode() {
        return this;
      }

      node_t* node_t::endNode() {
        return this;
      }

      bool node_t::hasAttribute(const std::string &attr) const {
        return false;
      }

      variable_t* node_t::getVariable() {
        return NULL;
      }

      node_t* node_t::wrapInParentheses() {
        return clone();
      }

      std::string node_t::toString() const {
        std::stringstream ss;
        printer pout(ss);
        pout << (*this);
        return ss.str();
      }

      void node_t::printWarning(const std::string &message) const {
        token->printWarning(message);
      }

      void node_t::printError(const std::string &message) const {
        token->printError(message);
      }

      void node_t::debugPrint() const {
        debugPrint("");
        std::cerr << '\n';
      }

      void node_t::childDebugPrint(const std::string &prefix) const {
        debugPrint(prefix + "|   ");
      }

      std::ostream& operator << (std::ostream &out,
                                 const node_t &expr) {
        printer pout(out);
        expr.print(pout);
        return out;
      }

      printer& operator << (printer &pout,
                            const node_t &expr) {
        expr.print(pout);
        return pout;
      }

      void cloneNodeVector(nodeVector &dest,
                           const nodeVector &src) {
        const int nodes = (int) src.size();
        dest.clear();
        dest.reserve(nodes);
        for (int i = 0; i < nodes; ++i) {
          dest.push_back(src[i]->clone());
        }
      }

      void freeNodeVector(nodeVector &vec) {
        const int nodes = (int) vec.size();
        for (int i = 0; i < nodes; ++i) {
          delete vec[i];
        }
        vec.clear();
      }
    }
  }
}
