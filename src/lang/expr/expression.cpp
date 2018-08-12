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
#include <occa/tools/string.hpp>
#include <occa/lang/expr.hpp>

namespace occa {
  namespace lang {
    namespace expr {
      static const int outputTokenType = (tokenType::identifier |
                                          tokenType::type       |
                                          tokenType::vartype    |
                                          tokenType::variable   |
                                          tokenType::function   |
                                          tokenType::primitive  |
                                          tokenType::char_      |
                                          tokenType::string);

      //---[ Expression Scoped State ]----
      scopeState_t::scopeState_t(token_t *beforePairToken_) :
        beforePairToken(beforePairToken_) {}

      void scopeState_t::free() {
        nodeList::iterator outputIt = output.begin();
        while (outputIt != output.end()) {
          delete *outputIt;
          ++outputIt;
        }
        output.clear();

        operatorList::iterator operatorIt = operators.begin();
        while (operatorIt != operators.end()) {
          delete *operatorIt;
          ++operatorIt;
        }
        operators.clear();
      }

      void scopeState_t::debugPrint() {
        std::cout << "Outputs:\n";
        nodeList::iterator it = output.begin();
        while (it != output.end()) {
          (*it)->debugPrint();
          ++it;
        }

        std::cout << "Operators:\n";
        operatorList::iterator itOp = operators.begin();
        while (itOp != operators.end()) {
          std::cout << '[' << *((*itOp)->token) << "]\n";
          ++itOp;
        }
      }
      //==================================

      //---[ Expression State ]-----------
      state_t::state_t(tokenVector &tokens_) :
        tokens(tokens_),
        prevToken(NULL),
        nextToken(NULL),
        beforePairToken(NULL),
        hasError(false) {
        scopedStates.push_back(scopeState_t());
        scopedState = &(scopedStates.back());
      }

      state_t::~state_t() {
        freeTokenVector(tokens);
        while (scopedStates.size()) {
          scopedStates.back().free();
          scopedStates.pop_back();
        }
        while (usedOutput.size()) {
          delete usedOutput.back();
          usedOutput.pop_back();
        }
        while (usedOperators.size()) {
          delete usedOperators.back();
          usedOperators.pop_back();
        }
      }

      int state_t::outputCount() {
        return (int) scopedState->output.size();
      }

      int state_t::operatorCount() {
        return (int) scopedState->operators.size();
      }

      node_t& state_t::lastOutput() {
        return *(scopedState->output.back());
      }

      opNode_t& state_t::lastOperator() {
        return *(scopedState->operators.back());
      }

      void state_t::pushOutput(node_t *expr) {
        scopedState->output.push_back(expr);
      }

      void state_t::pushOperator(operatorToken *token) {
        scopedState->operators.push_back(
          new opNode_t(*token)
        );
      }

      void state_t::pushOperator(opNode_t *expr) {
        scopedState->operators.push_back(expr);
      }

      node_t& state_t::unsafePopOutput() {
        node_t &ret = *(scopedState->output.back());
        scopedState->output.pop_back();
        return ret;
      }

      node_t& state_t::popOutput() {
        node_t &ret = *(scopedState->output.back());
        usedOutput.push_back(&ret);
        scopedState->output.pop_back();
        return ret;
      }

      opNode_t& state_t::popOperator() {
        opNode_t &ret = *(scopedState->operators.back());
        usedOperators.push_back(&ret);
        scopedState->operators.pop_back();
        return ret;
      }

      void state_t::pushPair(token_t *beforePairToken_) {
        scopedStates.push_back(scopeState_t(beforePairToken_));
        scopedState = &(scopedStates.back());

        beforePairToken = beforePairToken_;
      }

      void state_t::popPair() {
        beforePairToken = scopedState->beforePairToken;

        scopeState_t prevScopedState = scopedStates.back();
        scopedStates.pop_back();
        scopedState = &(scopedStates.back());

        // Copy left-overs
        scopedState->output.insert(scopedState->output.end(),
                                   prevScopedState.output.begin(),
                                   prevScopedState.output.end());

        scopedState->operators.insert(scopedState->operators.end(),
                                      prevScopedState.operators.begin(),
                                      prevScopedState.operators.end());
      }

      void state_t::debugPrint() {
        std::cout << "\n---[ Scopes ]---------------------------\n";
        scopeStateList::iterator it = scopedStates.begin();
        while (it != scopedStates.end()) {
          it->debugPrint();
          ++it;
          if (it != scopedStates.end()) {
            std::cout << " - - - - - - - - - - - - - - - - - - - -\n";
          }
        }
        std::cout << "========================================\n";
      }
      //==================================

      node_t* getExpression(tokenVector &tokens) {
        if (!tokens.size()) {
          return noNode.clone();
        }

        state_t state(tokens);
        expr::getInitialExpression(tokens, state);
        if (state.hasError) {
          return NULL;
        }

        // Finish applying operators
        while (state.operatorCount()) {
          expr::applyOperator(state.popOperator(),
                              state);

          if (state.hasError) {
            return NULL;
          }
        }

        // Make sure we only have 1 root node
        const int outputCount = state.outputCount();
        if (!outputCount) {
          return noNode.clone();
        }

        expr::applyTernary(state);

        if (outputCount > 1) {
          state.debugPrint();
          state.popOutput();
          state.lastOutput().token->printError("Unable to form an expression");
          return NULL;
        }

        return &(state.unsafePopOutput());
      }

      namespace expr {
        void getInitialExpression(tokenVector &tokens,
                                  state_t &state) {
          const int count = (int) tokens.size();
          for (int i = 0; i < count; ++i) {
            token_t *token = tokens[i];
            if (!token) {
              continue;
            }

            state.nextToken = NULL;
            for (int j = (i + 1); j < count; ++j) {
              if (tokens[j]) {
                state.nextToken = tokens[j];
                break;
              }
            }

            const int tokenType = token->type();
            if (tokenType & outputTokenType) {
              pushOutputNode(token, state);
            }
            else if (tokenType & tokenType::op) {
              operatorToken &opToken = token->to<operatorToken>();

              if (opToken.opType() & operatorType::pairStart) {
                state.pushPair(state.prevToken);
                state.pushOperator(&opToken);
              }
              else if (opToken.opType() & operatorType::pairEnd) {
                state.pushOperator(&opToken);
                state.popPair();
                closePair(state);
                if (!state.hasError) {
                  attachPair(opToken, state);
                }
              }
              else {
                applyFasterOperators(opToken, state);
              }
            }

            if (state.hasError) {
              return;
            }
            // Store prevToken at the end since opToken
            //   might have changed from an ambiguous type
            state.prevToken = token;
          }
        }

        void pushOutputNode(token_t *token,
                            state_t &state) {
          const int tokenType = token->type();
          if (tokenType & tokenType::identifier) {
            identifierToken &t = token->to<identifierToken>();
            state.pushOutput(new identifierNode_t(token, t.value));
          }
          else if (tokenType & tokenType::variable) {
            variableToken &t = token->to<variableToken>();
            state.pushOutput(new variableNode_t(token, t.value));
          }
          else if (tokenType & tokenType::function) {
            functionToken &t = token->to<functionToken>();
            state.pushOutput(new functionNode_t(token, t.value));
          }
          else if (tokenType & tokenType::type) {
            typeToken &t = token->to<typeToken>();
            state.pushOutput(new typeNode_t(token, t.value));
          }
          else if (tokenType & tokenType::vartype) {
            vartypeToken &t = token->to<vartypeToken>();
            state.pushOutput(new vartypeNode_t(token, t.value));
          }
          else if (tokenType & tokenType::primitive) {
            primitiveToken &t = token->to<primitiveToken>();
            state.pushOutput(new primitiveNode_t(token, t.value));
          }
          else if (tokenType & tokenType::char_) {
            // TODO: Handle char udfs here
            charToken &t = token->to<charToken>();
            state.pushOutput(new charNode_t(token, t.value));
          }
          else if (tokenType & tokenType::string) {
            // TODO: Handle string udfs here
            stringToken &t = token->to<stringToken>();
            state.pushOutput(new stringNode_t(token, t.value));
          }
        }

        void closePair(state_t &state) {
          opNode_t &opNode = state.popOperator();
          const operator_t &op = opNode.op;
          const opType_t opType = op.opType;
          operatorToken *errorToken = (operatorToken*) opNode.token;

          while (state.operatorCount()) {
            opNode_t &nextOpNode = state.popOperator();
            const opType_t nextOpType = nextOpNode.opType();

            if (nextOpType & operatorType::pairStart) {
              if (opType == (nextOpType << 1)) {
                applyTernary(state);
                applyOperator(opNode, state);
                return;
              }
              errorToken = (operatorToken*) nextOpNode.token;
              break;
            }

            applyOperator(nextOpNode, state);

            if (state.hasError) {
              return;
            }
          }

          // Found a pairStart that doesn't match
          state.hasError = true;

          std::stringstream ss;
          ss << "Could not find ";
          if (errorToken->opType() & operatorType::pairStart) {
            ss << "a closing";
          } else {
            ss << "an opening";
          }
          ss << " '"
             << ((pairOperator_t*) errorToken->op)->pairStr
             << '\'';
          errorToken->printError(ss.str());
        }

        void extractArgs(nodeVector &args,
                         node_t &expr,
                         state_t &state) {
          // We need to push all args and reverse it at the end
          //   since commaNode looks like (...tail, head)
          node_t *commaNode = &expr;
          while (true) {
            if (commaNode->type() & nodeType::binary) {
              binaryOpNode_t &opNode = commaNode->to<binaryOpNode_t>();
              if (opNode.opType() & operatorType::comma) {
                args.push_back(opNode.rightValue);
                commaNode = opNode.leftValue;
                continue;
              }
            }
            args.push_back(commaNode);
            break;
          }

          // Reverse arguments back to original order
          const int argCount = (int) args.size();
          for (int i = 0 ; i < (argCount / 2); ++i) {
            node_t *arg_i = args[i];
            args[i] = args[argCount - i - 1];
            args[argCount - i - 1] = arg_i;
          }
        }

        void transformLastPair(operatorToken &opToken,
                               state_t &state) {
          // Guaranteed to have the pairNode
          pairNode_t &pair = state.popOutput().to<pairNode_t>();

          if (!(pair.opType() & (operatorType::parentheses |
                                 operatorType::braces))) {
            state.hasError = true;
            state.debugPrint();
            opToken.printError("Expected identifier or proper expression before");
            return;
          }

          if (pair.opType() & operatorType::parentheses) {
            if (pair.value->type() & (nodeType::type |
                                      nodeType::vartype)) {
              state.pushOperator(
                new leftUnaryOpNode_t(&opToken,
                                      op::parenCast,
                                      *(pair.value))
              );
            } else {
              state.pushOutput(
                new parenthesesNode_t(pair.token,
                                      *pair.value)
              );
            }
          } else {
            nodeVector args;
            extractArgs(args, *pair.value, state);
            state.pushOutput(
              new tupleNode_t(pair.token,
                              args)
            );
          }
        }

        void attachPair(operatorToken &opToken,
                        state_t &state) {
          if ((state.outputCount() < 2)) {
            transformLastPair(opToken, state);
            return;
          }

          // Only consider () as a function call if preceeded by:
          //   - identifier
          //   - pairEnd
          const int prevTokenType = state.beforePairToken->type();
          if (!(prevTokenType & (outputTokenType |
                                 tokenType::op))) {
            transformLastPair(opToken, state);
            return;
          }
          if (prevTokenType & tokenType::op) {
            operatorToken &prevOpToken = state.beforePairToken->to<operatorToken>();
            if (!(prevOpToken.opType() & operatorType::pairEnd)) {
              transformLastPair(opToken, state);
              return;
            }
          }

          pairNode_t &pair  = state.popOutput().to<pairNode_t>();
          node_t &value = state.popOutput();

          // func(...)
          if (pair.opType() & operatorType::parentheses) {
            nodeVector args;
            extractArgs(args, *pair.value, state);
            state.pushOutput(
              new callNode_t(value.token,
                             value,
                             args)
            );
            return;
          }
          // array[...]
          if (pair.opType() & operatorType::brackets) {
            state.pushOutput(
              new subscriptNode_t(value.token,
                                  value,
                                  *pair.value)
            );
            return;
          }
          // func<<<...>>>
          if (pair.opType() & operatorType::cudaCall) {
            nodeVector args;
            extractArgs(args, *pair.value, state);

            const int argCount = (int) args.size();
            if (argCount == 1) {
              args[0]->token->printError("Must also have threads per block"
                                         " as the second argument");
              state.hasError = true;
            } else if (argCount > 2) {
              args[0]->token->printError("Kernel call only takes 2 arguments");
              state.hasError = true;
            }

            if (!state.hasError) {
              state.pushOutput(
                new cudaCallNode_t(value.token,
                                   value,
                                   *args[0],
                                   *args[1])
              );
            }
            return;
          }

          state.hasError = true;
          opToken.printError("[Waldo] (attachPair) Unsure how you got here...");
        }

        bool operatorIsLeftUnary(operatorToken &opToken,
                                 state_t &state) {
          const opType_t opType = opToken.opType();

          // Test for chaining increments
          // 1 + ++ ++ x
          // (2) ++ ++
          opType_t chainable = (operatorType::increment |
                                operatorType::decrement |
                                operatorType::parentheses);

          // ++ and -- operators
          const bool onlyUnary = (opType & (operatorType::increment |
                                            operatorType::decrement));

          // If this is the first token, it's left unary
          // If this is the last token, it's binary or right unary
          if (!state.prevToken ||
              !state.nextToken) {
            return !state.prevToken;
          }

          opType_t prevOpType = state.prevToken->getOpType();
          if (prevOpType & operatorType::pairStart) {
            return true;
          }

          // Test for left unary first
          const bool prevTokenIsOp = prevOpType & (operatorType::unary |
                                                   operatorType::binary);
          if (prevTokenIsOp) {
            // + + + 1
            // a = + 1
            if ((prevOpType & operatorType::leftUnary) ||
                ((prevOpType & operatorType::binary) &&
                 !(prevOpType & operatorType::unary))) {
              return true;
            }
            if (!onlyUnary) {
              return false;
            }
          }

          const bool nextTokenIsOp = (
            state.nextToken->getOpType() & (operatorType::unary |
                                            operatorType::binary)
          );

          //   v check right
          // 1 + ++ x
          //     ^ check left
          if (prevTokenIsOp != nextTokenIsOp) {
            return (onlyUnary
                    ? prevTokenIsOp
                    : nextTokenIsOp);
          }
          // y ++ x (Unable to apply operator)
          // y + x
          if (!prevTokenIsOp) {
            if (onlyUnary) {
              state.hasError = true;
              opToken.printError("Ambiguous operator");
            }
            return false;
          }

          opType_t nextOpType = state.nextToken->to<operatorToken>().opType();

          // x ++ ++ ++ y
          if ((prevOpType & chainable) &&
              (nextOpType & chainable)) {
            state.hasError = true;
            opToken.printError("Ambiguous operator");
            return false;
          }

          return !(prevOpType & chainable);
        }

        void updateOperatorToken(operatorToken &opToken,
                                 state_t &state) {

          const opType_t opType = opToken.opType();
          if (!(opType & operatorType::ambiguous)) {
            return;
          }

          fileOrigin origin = opToken.origin;

          bool isLeftUnary = operatorIsLeftUnary(opToken, state);
          if (state.hasError) {
            return;
          }

          const operator_t *newOperator = NULL;
          if (opType & operatorType::plus) {           // +
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::positive
                           : (const operator_t*) &op::add);
          }
          else if (opType & operatorType::minus) {     // -
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::negative
                           : (const operator_t*) &op::sub);
          }
          else if (opType & operatorType::asterisk) {  // *
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::dereference
                           : (const operator_t*) &op::mult);
          }
          else if (opType & operatorType::ampersand) { // &
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::address
                           : (const operator_t*) &op::bitAnd);
          }
          else if (opType & operatorType::increment) { // ++
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::leftIncrement
                           : (const operator_t*) &op::rightIncrement);
          }
          else if (opType & operatorType::decrement) { // --
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::leftDecrement
                           : (const operator_t*) &op::rightDecrement);
          }
          else if (opType & operatorType::scope) {     // ::
            newOperator = (isLeftUnary
                           ? (const operator_t*) &op::globalScope
                           : (const operator_t*) &op::scope);
          }

          if (newOperator) {
            opToken.op = newOperator;
            return;
          }

          state.hasError = true;
          opToken.printError("Unable to parse ambiguous token");
        }

        void applyFasterOperators(operatorToken &opToken,
                                  state_t &state) {

          updateOperatorToken(opToken, state);
          if (state.hasError) {
            return;
          }

          const operator_t &op = *(opToken.op);
          while (state.operatorCount()) {
            const operator_t &prevOp = state.lastOperator().op;

            if (prevOp.opType & operatorType::pairStart) {
              break;
            }

            if ((op.precedence > prevOp.precedence) ||
                ((op.precedence == prevOp.precedence) &&
                 op::associativity[prevOp.precedence] == op::leftAssociative)) {

              applyOperator(state.popOperator(),
                            state);

              if (state.hasError) {
                return;
              }
              continue;
            }

            break;
          }

          // After applying faster operators,
          //   place opToken in the stack
          state.pushOperator(&opToken);
        }

        void applyOperator(opNode_t &opNode,
                           state_t &state) {

          operatorToken &opToken = *((operatorToken*) opNode.token);
          const operator_t &op = opNode.op;
          const opType_t opType = op.opType;
          const int outputCount = state.outputCount();

          if (opType & operatorType::binary) {
            if (outputCount >= 2) {
              node_t &right = state.popOutput();
              node_t &left = state.popOutput();
              state.pushOutput(
                new binaryOpNode_t(&opToken,
                                   (const binaryOperator_t&) op,
                                   left,
                                   right)
              );
              return;
            }
            state.hasError = true;
          }
          else if (opType & operatorType::leftUnary) {
            if (outputCount >= 1) {
              node_t &value = state.popOutput();
              applyLeftUnaryOperator(opNode,
                                     value,
                                     state);
              if (opType & operatorType::colon) {
                applyTernary(state);
              }
              return;
            }
            state.hasError = true;
          }
          else if (opType & operatorType::rightUnary) {
            if (outputCount >= 1) {
              node_t &value = state.popOutput();
              state.pushOutput(
                new rightUnaryOpNode_t(&opToken,
                                       (const unaryOperator_t&) op,
                                       value)
              );
              return;
            }
            state.hasError = true;
          }
          else if (opType & operatorType::pair) {
            // Make sure we have content in the parentheses
            if ((outputCount >= 1)
                && !(state.prevToken->getOpType() & operatorType::pairStart)) {
              node_t &value = state.popOutput();
              state.pushOutput(
                new pairNode_t(opToken, value)
              );
            } else {
              state.pushOutput(
                new pairNode_t(opToken, noNode)
              );
            }
          }
          if (state.hasError) {
            opToken.printError("Unable to apply operator");
          }
        }

        void applyLeftUnaryOperator(opNode_t &opNode,
                                    node_t &value,
                                    state_t &state) {

          operatorToken &opToken = *((operatorToken*) opNode.token);
          const unaryOperator_t &op = (unaryOperator_t&) opNode.op;
          const opType_t opType = op.opType;

          if (!(opType & operatorType::special)) {
            state.pushOutput(new leftUnaryOpNode_t(&opToken,
                                                   (const unaryOperator_t&) op,
                                                   value));
            return;
          }

          if (opType & operatorType::parenCast) {
            leftUnaryOpNode_t &parenOpNode = (leftUnaryOpNode_t&) opNode;
            node_t *valueNode = parenOpNode.value;
            if (valueNode->type() & nodeType::type) {
              state.pushOutput(
                new parenCastNode_t(parenOpNode.token,
                                    ((typeNode_t*) valueNode)->value,
                                    value)
              );
            } else {
              state.pushOutput(
                new parenCastNode_t(parenOpNode.token,
                                    ((vartypeNode_t*) valueNode)->value,
                                    value)
              );
            }
          }
          else if (opType & operatorType::sizeof_) {
            state.pushOutput(
              new sizeofNode_t(&opToken, value)
            );
          }
          else if (opType & operatorType::new_) {
            state.hasError = true;
            opToken.printError("'new' not supported yet");
          }
          else if (opType & operatorType::delete_) {
            state.hasError = true;
            opToken.printError("'delete' not supported yet");
          }
          else if (opType & operatorType::throw_) {
            state.pushOutput(
              new throwNode_t(&opToken, value)
            );
          }
          else {
            state.hasError = true;
            opToken.printError("[Waldo] (applyLeftUnaryOperator)"
                               " Unsure how you got here...");
          }
        }

        bool applyTernary(state_t &state) {
          if (state.outputCount() < 3) {
            return false;
          }
          // Don't use state's garbage collection yet
          node_t &falseValue = state.unsafePopOutput();
          node_t &trueValue  = state.unsafePopOutput();
          node_t &checkValue = state.unsafePopOutput();

          if ((trueValue.type() & nodeType::leftUnary)
              && (falseValue.type() & nodeType::leftUnary)) {

            leftUnaryOpNode_t &trueOpValue  = (leftUnaryOpNode_t&) trueValue;
            leftUnaryOpNode_t &falseOpValue = (leftUnaryOpNode_t&) falseValue;

            opType_t op1 = trueOpValue.opType();
            opType_t op2 = falseOpValue.opType();

            if ((op1 == operatorType::questionMark)
                && (op2 == operatorType::colon)) {

              state.pushOutput(
                new ternaryOpNode_t(checkValue,
                                    *(trueOpValue.value),
                                    *(falseOpValue.value))
              );
              // Manually delete since we're avoiding garbage collection
              delete &checkValue;
              delete &trueValue;
              delete &falseValue;

              return true;
            }
          }

          state.pushOutput(&checkValue);
          state.pushOutput(&trueValue);
          state.pushOutput(&falseValue);
          return false;
        }
      }
    }
  }
}
