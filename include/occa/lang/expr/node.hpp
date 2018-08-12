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
#ifndef OCCA_LANG_EXPR_NODE_HEADER
#define OCCA_LANG_EXPR_NODE_HEADER

#include <stack>
#include <vector>

#include <occa/types.hpp>
#include <occa/lang/primitive.hpp>
#include <occa/lang/printer.hpp>

namespace occa {
  namespace lang {
    class token_t;
    class type_t;
    class variable_t;
    class function_t;

    namespace expr {
      class node_t;

      typedef std::vector<node_t*>  nodeVector;
      typedef std::vector<node_t**> nodeRefVector;
      typedef std::stack<node_t*>   nodeStack;
      typedef std::vector<token_t*> tokenVector;

      namespace nodeType {
        extern const udim_t empty;
        extern const udim_t primitive;
        extern const udim_t char_;
        extern const udim_t string;
        extern const udim_t identifier;
        extern const udim_t type;
        extern const udim_t vartype;
        extern const udim_t variable;
        extern const udim_t function;
        extern const udim_t value;

        extern const udim_t rawOp;
        extern const udim_t leftUnary;
        extern const udim_t rightUnary;
        extern const udim_t binary;
        extern const udim_t ternary;
        extern const udim_t op;

        extern const udim_t pair;

        extern const udim_t subscript;
        extern const udim_t call;

        extern const udim_t sizeof_;
        extern const udim_t sizeof_pack_;
        extern const udim_t new_;
        extern const udim_t delete_;
        extern const udim_t throw_;

        extern const udim_t typeid_;
        extern const udim_t noexcept_;
        extern const udim_t alignof_;

        extern const udim_t const_cast_;
        extern const udim_t dynamic_cast_;
        extern const udim_t static_cast_;
        extern const udim_t reinterpret_cast_;

        extern const udim_t funcCast;
        extern const udim_t parenCast;
        extern const udim_t constCast;
        extern const udim_t staticCast;
        extern const udim_t reinterpretCast;
        extern const udim_t dynamicCast;

        extern const udim_t parentheses;
        extern const udim_t tuple;
        extern const udim_t cudaCall;
      }

      class node_t {
      public:
        token_t *token;

        node_t(token_t *token_);

        virtual ~node_t();

        template <class TM>
        inline bool is() const {
          return (dynamic_cast<const TM*>(this) != NULL);
        }

        template <class TM>
        inline TM& to() {
          TM *ptr = dynamic_cast<TM*>(this);
          OCCA_ERROR("Unable to cast node_t::to",
                     ptr != NULL);
          return *ptr;
        }

        template <class TM>
        inline const TM& to() const {
          const TM *ptr = dynamic_cast<const TM*>(this);
          OCCA_ERROR("Unable to cast node_t::to",
                     ptr != NULL);
          return *ptr;
        }

        virtual udim_t type() const = 0;

        virtual node_t* clone() const = 0;
        static node_t* clone(node_t *expr);

        virtual bool canEvaluate() const;
        virtual primitive evaluate() const;

        virtual node_t* startNode();
        virtual node_t* endNode();

        virtual void setChildren(nodeRefVector &children) = 0;

        virtual bool hasAttribute(const std::string &attr) const;

        virtual variable_t* getVariable();

        virtual node_t* wrapInParentheses();

        virtual void print(printer &pout) const = 0;

        std::string toString() const;

        void printWarning(const std::string &message) const;
        void printError(const std::string &message) const;

        void debugPrint() const;

        virtual void debugPrint(const std::string &prefix) const = 0;

        void childDebugPrint(const std::string &prefix) const;
      };

      std::ostream& operator << (std::ostream &out,
                                 const node_t &expr);

      printer& operator << (printer &pout,
                            const node_t &expr);

      void cloneNodeVector(nodeVector &dest,
                           const nodeVector &src);

      void freeNodeVector(nodeVector &vec);
    }
  }
}

#endif
