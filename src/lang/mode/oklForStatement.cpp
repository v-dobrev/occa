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
#include <occa/lang/builtins/types.hpp>
#include <occa/lang/builtins/transforms/finders.hpp>
#include <occa/lang/expr.hpp>
#include <occa/lang/mode/oklForStatement.hpp>
#include <occa/lang/statement.hpp>
#include <occa/lang/variable.hpp>

namespace occa {
  namespace lang {
    namespace okl {
      oklForStatement::oklForStatement(forStatement &forSmnt_,
                                       const std::string &source_,
                                       const bool printErrors_) :
        forSmnt(forSmnt_),
        source(source_),
        printErrors(printErrors_),
        iterator(NULL),
        initValue(NULL),
        checkOp(NULL),
        checkValue(NULL),
        updateOp(NULL),
        updateValue(NULL),
        valid(false) {

        valid = (
          hasValidInit()
          && hasValidCheck()
          && hasValidUpdate()
        );
      }

      bool oklForStatement::isValid() {
        return valid;
      }

      bool oklForStatement::isValid(forStatement &forSmnt_,
                                    const std::string &source_,
                                    const bool printErrors_) {
        return oklForStatement(
          forSmnt_,
          source_,
          printErrors_
        ).isValid();
      }

      bool oklForStatement::hasValidInit() {
        statement_t &initSmnt = *(forSmnt.init);
        // Check for declaration
        if (initSmnt.type() != statementType::declaration) {
          if (printErrors) {
            initSmnt.printError(sourceStr() + "Expected a declaration statement");
          }
          return false;
        }
        // Can only have one declaration
        declarationStatement &declSmnt = (declarationStatement&) initSmnt;
        if (declSmnt.declarations.size() > 1) {
          if (printErrors) {
            declSmnt.declarations[1].printError(
              sourceStr() + "Can only have 1 iterator variable"
            );
          }
          return false;
        }
        // Get iterator and value
        variableDeclaration &decl = declSmnt.declarations[0];
        iterator  = decl.variable;
        initValue = decl.value;
        // Valid types: {char, short, int, long}
        const type_t *type = iterator->vartype.flatten().type;
        if (!type ||
            ((*type != char_)  &&
             (*type != short_) &&
             (*type != int_))) {
          if (printErrors) {
            iterator->printError(sourceStr() + "Iterator variable needs to be of type"
                                 " [char, short, int, long]");
          }
          return false;
        }
        return true;
      }

      bool oklForStatement::hasValidCheck() {
        statement_t &checkSmnt = *(forSmnt.check);
        // Check an expression statement exists
        if (checkSmnt.type() != statementType::expression) {
          if (printErrors) {
            checkSmnt.printError(sourceStr() + "Expected comparing ["
                                 + iterator->name()
                                 + "] with some bound");
          }
          return false;
        }
        // Check valid operator (<, <=, >=, >)
        expr::node_t &expr = *(((expressionStatement&) checkSmnt).expr);
        if (expr.type() != expr::nodeType::binary) {
          if (printErrors) {
            checkSmnt.printError(sourceStr() + "{0} Expected to compare ["
                                 + iterator->name()
                                 + "] with one of these operators [<, <=, >=, >]");
          }
          return false;
        }
        // Set check operator
        checkOp = (expr::binaryOpNode_t*) &expr;
        opType_t checkOpType = checkOp->opType();
        if (!(checkOpType & (operatorType::lessThan      |
                             operatorType::lessThanEq    |
                             operatorType::greaterThanEq |
                             operatorType::greaterThan))) {
          if (printErrors) {
            checkSmnt.printError(sourceStr() + "{1} Expected to compare ["
                                 + iterator->name()
                                 + "] with one of these operators [<, <=, >=, >]");
          }
          return false;
        }
        checkIsInclusive = (
          checkOpType & (operatorType::lessThanEq    |
                         operatorType::greaterThanEq)
        );
        // Set check value
        int checkOrder = usesIterator(*checkOp,
                                      checkValue);
        if (!checkOrder) {
          if (printErrors) {
            checkSmnt.printError(sourceStr() + "{2} Expected to compare ["
                                 + iterator->name()
                                 + "] with one of these operators [<, <=, >=, >]");
          }
          return false;
        }
        checkValueOnRight = (checkOrder < 0);
        return true;
      }

      bool oklForStatement::hasValidUpdate() {
        statement_t &updateSmnt = *(forSmnt.update);
        // Check an expression statement exists
        if (updateSmnt.type() != statementType::expression) {
          if (printErrors) {
            updateSmnt.printError(sourceStr() + "Expected to update ["
                                  + iterator->name()
                                  + "]");
          }
          return false;
        }
        // Check valid operator (++, --, +=, -=)
        expr::node_t *updateExpr = ((expressionStatement&) updateSmnt).expr;
        udim_t eType = updateExpr->type();
        if (!(eType & (expr::nodeType::leftUnary  |
                       expr::nodeType::rightUnary |
                       expr::nodeType::binary))) {
          if (printErrors) {
            updateSmnt.printError(sourceStr() + "Expected update ["
                                  + iterator->name()
                                  + "] with one of these operators [++, --, +=, -=]");
          }
          return false;
        }
        // Make sure we're using the same iterator variable
        bool validOp  = false;
        bool validVar = false;
        updateOp = (expr::opNode_t*) updateExpr;
        if (eType == expr::nodeType::leftUnary) {
          expr::leftUnaryOpNode_t &opNode = (expr::leftUnaryOpNode_t&) *updateOp;
          opType_t opType = opNode.opType();
          validOp = (opType & (operatorType::leftIncrement |
                               operatorType::leftDecrement));
          validVar = usesIterator(opNode);
          positiveUpdate = (opType & operatorType::leftIncrement);
        }
        else if (eType == expr::nodeType::rightUnary) {
          expr::rightUnaryOpNode_t &opNode = (expr::rightUnaryOpNode_t&) *updateOp;
          opType_t opType = opNode.opType();
          validOp = (opType & (operatorType::rightIncrement |
                               operatorType::rightDecrement));
          validVar = usesIterator(opNode);
          positiveUpdate = (opType & operatorType::rightIncrement);
        }
        else { // eType == expr::nodeType::binary
          expr::binaryOpNode_t &opNode = (expr::binaryOpNode_t&) *updateOp;
          opType_t opType = opNode.opType();
          validOp = (opType & (operatorType::addEq |
                               operatorType::subEq));
          validVar = usesIterator(opNode, updateValue);
          positiveUpdate = (opType & operatorType::addEq);
        }
        if (!validOp) {
          if (printErrors) {
            updateOp->printError(sourceStr() + "Expected update ["
                                 + iterator->name()
                                 + "] with one of these operators [++, --, +=, -=]");
          }
          return false;
        }
        if (!validVar) {
          if (printErrors) {
            updateOp->startNode()->printError(sourceStr() + "Expected update ["
                                              + iterator->name()
                                              + "] with one of these operators [++, --, +=, -=]");
          }
          return false;
        }
        return true;
      }

      bool oklForStatement::usesIterator(expr::leftUnaryOpNode_t &opNode) {
        if (opNode.value->type() != expr::nodeType::variable) {
          return false;
        }
        variable_t &var = ((expr::variableNode_t*) opNode.value)->value;
        return (&var == iterator);
      }

      bool oklForStatement::usesIterator(expr::rightUnaryOpNode_t &opNode) {
        if (opNode.value->type() != expr::nodeType::variable) {
          return false;
        }
        variable_t &var = ((expr::variableNode_t*) opNode.value)->value;
        return (&var == iterator);
      }

      int oklForStatement::usesIterator(expr::binaryOpNode_t &opNode,
                                        expr::node_t *&value) {

        if (opNode.leftValue->type() == expr::nodeType::variable) {
          variable_t &var = ((expr::variableNode_t*) opNode.leftValue)->value;
          if (&var == iterator) {
            value = opNode.rightValue;
            return -1;
          }
        }
        if (opNode.rightValue->type() == expr::nodeType::variable) {
          variable_t &var = ((expr::variableNode_t*) opNode.rightValue)->value;
          if (&var == iterator) {
            value = opNode.leftValue;
            return 1;
          }
        }
        return 0;
      }

      expr::node_t* oklForStatement::getIterationCount() {
        if (!valid) {
          return NULL;
        }

        expr::node_t *initInParen = initValue->wrapInParentheses();
        expr::node_t *count = (
          new expr::binaryOpNode_t(iterator->source,
                                   positiveUpdate ? op::sub : op::add,
                                   *checkValue,
                                   *initInParen)
        );
        delete initInParen;

        if (checkIsInclusive) {
          expr::primitiveNode_t inc(iterator->source, 1);

          expr::node_t *countWithInc = (
            new expr::binaryOpNode_t(iterator->source,
                                     positiveUpdate ? op::sub : op::add,
                                     *count,
                                     inc)
          );
          delete count;
          count = countWithInc;
        }

        if (updateValue) {
          expr::node_t *updateInParen = updateValue->wrapInParentheses();

          expr::primitiveNode_t one(iterator->source, 1);
          expr::binaryOpNode_t boundCheck(iterator->source,
                                          positiveUpdate ? op::add : op::sub,
                                          *count,
                                          *updateInParen);
          expr::binaryOpNode_t boundCheck2(iterator->source,
                                           positiveUpdate ? op::sub : op::add,
                                           boundCheck,
                                           one);
          expr::node_t *boundCheckInParen = boundCheck2.wrapInParentheses();

          expr::node_t *countWithUpdate = (
            new expr::binaryOpNode_t(iterator->source,
                                     op::div,
                                     *boundCheckInParen,
                                     *updateInParen)
          );
          delete count;
          delete updateInParen;
          count = countWithUpdate;
        }

        return count;
      }

      expr::node_t* oklForStatement::makeDeclarationValue(expr::node_t &magicIterator) {
        if (!valid) {
          return NULL;
        }

        expr::node_t *blockValue = magicIterator.wrapInParentheses();
        if (updateValue) {
          expr::node_t *updateInParen = updateValue->wrapInParentheses();
          expr::binaryOpNode_t mult(iterator->source,
                                    op::mult,
                                    *updateInParen,
                                    *blockValue);
          delete updateInParen;
          delete blockValue;
          blockValue = mult.wrapInParentheses();
        }

        expr::node_t *initInParen = initValue->wrapInParentheses();
        expr::binaryOpNode_t *value = (
          new expr::binaryOpNode_t(iterator->source,
                                   positiveUpdate ? op::add : op::sub,
                                   *initInParen,
                                   *blockValue)
        );

        delete blockValue;
        delete initInParen;

        return value;
      }

      bool oklForStatement::isInnerLoop() {
        return forSmnt.hasAttribute("inner");
      }

      bool oklForStatement::isOuterLoop() {
        return forSmnt.hasAttribute("outer");
      }

      int oklForStatement::oklLoopIndex() {
        return oklLoopIndex(forSmnt);
      }

      int oklForStatement::oklLoopIndex(forStatement &forSmnt) {
        std::string attr;
        if (forSmnt.hasAttribute("inner")) {
          attr = "inner";
        } else if (forSmnt.hasAttribute("outer")) {
          attr = "outer";
        } else {
          return -1;
        }

        attributeToken_t &oklAttr = forSmnt.attributes[attr];
        if (oklAttr.args.size()) {
          return (int) oklAttr.args[0].value->evaluate();
        }

        statementPtrVector smnts;
        findStatementsByAttr(statementType::for_,
                             attr,
                             forSmnt,
                             smnts);
        int smntCount = (int) smnts.size();
        int maxIndex = 0;
        for (int i = 0; i < smntCount; ++i) {
          forStatement &iSmnt = *((forStatement*) smnts[i]);
          if (&iSmnt == &forSmnt) {
            continue;
          }

          int index = 1;
          statement_t *up = iSmnt.up;
          while (up != &forSmnt) {
            index += up->hasAttribute(attr);
            up = up->up;
          }
          if (index > maxIndex) {
            maxIndex = index;
          }
        }
        return maxIndex;
      }

      void oklForStatement::getOKLLoopPath(statementPtrVector &path) {
        getOKLLoopPath(forSmnt, path);
      }

      void oklForStatement::getOKLLoopPath(forStatement &forSmnt,
                                           statementPtrVector &path) {
        // Fill in path
        statement_t *smnt = &forSmnt;
        while (smnt) {
          if ((smnt->type() & statementType::for_)
              && (smnt->hasAttribute("inner")
                  || smnt->hasAttribute("outer"))) {
            path.push_back(smnt);
          }
          smnt = smnt->up;
        }
        // Reverse
        const int pathCount = (int) path.size();
        for (int i = 0; i < (pathCount / 2); ++i) {
          statement_t *pi = path[i];
          path[i] = path[pathCount - i - 1];
          path[pathCount - i - 1] = pi;
        }
      }

      std::string oklForStatement::sourceStr() {
        if (source.size()) {
          return ("[" + source + "] ");
        }
        return "";
      }

      void oklForStatement::printWarning(const std::string &message) {
        forSmnt.printWarning(message);
      }

      void oklForStatement::printError(const std::string &message) {
        forSmnt.printError(message);
      }
    }
  }
}
