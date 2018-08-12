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
#ifndef OCCA_LANG_EXPR_EXPRESSION_HEADER
#define OCCA_LANG_EXPR_EXPRESSION_HEADER

#include <list>
#include <vector>

#include <occa/lang/token.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      class node_t;
      class opNode_t;
      class scopeState_t;

      typedef std::list<node_t*>      nodeList;
      typedef std::list<token_t*>     tokenList;
      typedef std::list<opNode_t*>    operatorList;
      typedef std::list<scopeState_t> scopeStateList;

      //---[ Expression State ]-----------
      class scopeState_t {
      public:
        token_t *beforePairToken;

        nodeList output;
        operatorList operators;

        scopeState_t(token_t *beforePairToken_ = NULL);

        void free();

        void debugPrint();
      };

      class state_t {
      public:
        tokenVector &tokens;

        // Keep track of the prev/next tokens
        //   to break ++ and -- left/right
        //   unary ambiguity
        token_t *prevToken;
        token_t *nextToken;

        // Token before the pair started
        token_t *beforePairToken;

        scopeStateList scopedStates;
        scopeState_t *scopedState;

        nodeList usedOutput;
        operatorList usedOperators;

        bool hasError;

        state_t(tokenVector &tokens_);
        ~state_t();

        int outputCount();
        int operatorCount();

        node_t& lastOutput();
        opNode_t& lastOperator();

        void pushOutput(node_t *expr);
        void pushOperator(operatorToken *token);
        void pushOperator(opNode_t *expr);

        node_t& unsafePopOutput();

        node_t& popOutput();
        opNode_t& popOperator();

        void pushPair(token_t *beforePairToken_);
        void popPair();

        void debugPrint();
      };
      //==================================

      // Using Shunting-Yard algorithm
      node_t* getExpression(tokenVector &tokens);

      namespace expr {
        void getInitialExpression(tokenVector &tokens,
                                  state_t &state);

        void pushOutputNode(token_t *token,
                            state_t &state);

        void closePair(state_t &state);

        void extractArgs(nodeVector &args,
                         node_t &expr,
                         state_t &state);

        void transformLastPair(operatorToken &opToken,
                               state_t &state);

        void attachPair(operatorToken &opToken,
                        state_t &state);

        bool operatorIsLeftUnary(operatorToken &opToken,
                                 state_t &state);

        void updateOperatorToken(operatorToken &opToken,
                                 state_t &state);

        void applyFasterOperators(operatorToken &opToken,
                                  state_t &state);

        void applyOperator(opNode_t &opNode,
                           state_t &state);

        void applyLeftUnaryOperator(opNode_t &opNode,
                                    node_t &value,
                                    state_t &state);

        bool applyTernary(state_t &state);
      }
    }
  }
}

#endif
