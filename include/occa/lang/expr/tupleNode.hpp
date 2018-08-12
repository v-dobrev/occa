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
#ifndef OCCA_LANG_EXPR_TUPLENODE_HEADER
#define OCCA_LANG_EXPR_TUPLENODE_HEADER

#include <occa/lang/expr/node.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      class tupleNode_t : public node_t {
      public:
        nodeVector args;

        tupleNode_t(token_t *token_,
                    const nodeVector &args_);

        tupleNode_t(const tupleNode_t &other);

        virtual ~tupleNode_t();

        inline int argCount() const {
          return (int) args.size();
        }

        virtual udim_t type() const;

        virtual node_t* clone() const;

        virtual node_t* startNode();
        virtual node_t* endNode();

        virtual void setChildren(nodeRefVector &children);

        virtual void print(printer &pout) const;

        virtual void debugPrint(const std::string &prefix) const;
      };
    }
  }
}

#endif
