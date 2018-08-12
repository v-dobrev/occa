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
#include <occa/lang/builtins/transforms/tile.hpp>
#include <occa/lang/builtins/transforms/replacer.hpp>
#include <occa/lang/expr.hpp>
#include <occa/lang/mode/oklForStatement.hpp>
#include <occa/lang/variable.hpp>

namespace occa {
  namespace lang {
    namespace transforms {
      tile::tile() {
        validStatementTypes = statementType::for_;
      }

      statement_t* tile::transformStatement(statement_t &smnt) {
        forStatement &forSmnt = (forStatement&) smnt;
        attributeTokenMap::iterator it = forSmnt.attributes.find("tile");
        if (it == forSmnt.attributes.end()) {
          return &smnt;
        }
        attributeToken_t &attr = it->second;
        expr::node_t &tileSize = *(attr.args[0].value);

        okl::oklForStatement oklForSmnt(forSmnt,
                                        "@tile");
        if (!oklForSmnt.isValid()) {
          return NULL;
        }

        // Create the block and inner-block for-loops
        forStatement &blockForSmnt = *(new forStatement(forSmnt.up,
                                                        forSmnt.source));
        forStatement &innerForSmnt = *(new forStatement(&blockForSmnt,
                                                        forSmnt.source));
        blockForSmnt.add(innerForSmnt);

        // Rename the block interator
        variable_t &iter = *(oklForSmnt.iterator);
        variable_t &blockIter = iter.clone();
        blockIter.name() = "_occa_tiled_" + iter.name();
        blockForSmnt.scope.add(blockIter);

        setupNewForStatements(attr,
                              oklForSmnt,
                              blockIter,
                              blockForSmnt, innerForSmnt);

        setupBlockForStatement(oklForSmnt,
                               tileSize,
                               blockIter,
                               blockForSmnt, innerForSmnt);

        setupInnerForStatement(oklForSmnt,
                               tileSize,
                               blockIter,
                               blockForSmnt, innerForSmnt);

        setupCheckStatement(attr,
                            oklForSmnt,
                            blockIter,
                            blockForSmnt, innerForSmnt);

        return &blockForSmnt;
      }

      void tile::setupNewForStatements(attributeToken_t &attr,
                                       okl::oklForStatement &oklForSmnt,
                                       variable_t &blockIter,
                                       forStatement &blockForSmnt,
                                       forStatement &innerForSmnt) {
        // Add @tile attributes
        const int attrArgCount = (int) attr.args.size();
        if (attrArgCount > 1) {
          attributeTokenMap &blockAttrs = attr.args[1].attributes;
          blockForSmnt.attributes.insert(blockAttrs.begin(), blockAttrs.end());
          if (attrArgCount > 2) {
            attributeTokenMap &innerAttrs = attr.args[2].attributes;
            innerForSmnt.attributes.insert(innerAttrs.begin(), innerAttrs.end());
          }
        }
        // Remove @tile to prevent recursive updates
        innerForSmnt.attributes.erase("tile");

        forStatement &forSmnt = oklForSmnt.forSmnt;
        innerForSmnt.swap(forSmnt);

        // Setup initial statements
        blockForSmnt.setLoopStatements(forSmnt.init, forSmnt.check, NULL);
        innerForSmnt.setLoopStatements(NULL, NULL, forSmnt.update);
        forSmnt.setLoopStatements(NULL, NULL, NULL);

        // Replace instances of x with _occa_tiled_x
        replaceVariables(*blockForSmnt.init,
                         *oklForSmnt.iterator,
                         blockIter);

        replaceVariables(*blockForSmnt.check,
                         *oklForSmnt.iterator,
                         blockIter);
      }

      void tile::setupBlockForStatement(okl::oklForStatement &oklForSmnt,
                                        expr::node_t &tileSize,
                                        variable_t &blockIter,
                                        forStatement &blockForSmnt,
                                        forStatement &innerForSmnt) {
        /*
          for (x = START; x < END; x += INC)
          ->
          for (xTile = START; xTile < END; NULL )
          ->
          for (xTile = START; xTile < END; xTile += (TILE * (INC)))
        */
        expr::node_t &updateExpr = *(((expressionStatement*) innerForSmnt.update)->expr);
        opType_t opType = ((expr::opNode_t&) updateExpr).opType();

        token_t *updateToken =updateExpr.startNode()->token;

        expr::node_t *updateSizeExpr = &tileSize;
        const binaryOperator_t *updateOp = &op::addEq;
        if (opType & (operatorType::leftDecrement |
                      operatorType::rightDecrement)) {
          updateOp = &op::subEq;
        }
        else if (opType & (operatorType::addEq |
                           operatorType::subEq)) {
          // INC
          expr::node_t *updateSize = ((expr::binaryOpNode_t&) updateExpr).rightValue;
          // (INC)
          expr::parenthesesNode_t updateInParen(updateToken,
                                                *updateSize);
          // TILE * (INC)
          expr::binaryOpNode_t mult(updateToken,
                                    op::mult,
                                    tileSize,
                                    updateInParen);
          // (TILE * (INC))
          updateSizeExpr = new expr::parenthesesNode_t(updateToken,
                                                       mult);
          if (opType & operatorType::subEq) {
            updateOp = &op::subEq;
          }
        }
        // VAR += (TILE * (INC))
        expr::variableNode_t varNode(updateToken, blockIter);
        expr::node_t *newUpdateExpr = new expr::binaryOpNode_t(updateToken,
                                                               *updateOp,
                                                               varNode,
                                                               *updateSizeExpr);
        if (updateSizeExpr != &tileSize) {
          // Delete (TILE * (INC)) if it was created
          delete updateSizeExpr;
        }

        blockForSmnt.update = new expressionStatement(&blockForSmnt,
                                                      *newUpdateExpr,
                                                      false);
      }

      void tile::setupInnerForStatement(okl::oklForStatement &oklForSmnt,
                                        expr::node_t &tileSize,
                                        variable_t &blockIter,
                                        forStatement &blockForSmnt,
                                        forStatement &innerForSmnt) {
        /*
          for (x = START; x < END; x += INC)
          ->
          for (NULL; NULL; x += INC)
          ->
          for (x = xTile; x < (xTile + TILE); x += INC)
        */
        // Init variables
        variableDeclaration &decl = (((declarationStatement*) blockForSmnt.init)
                                     ->declarations[0]);
        token_t *initToken = decl.variable->source;
        expr::variableNode_t iterNode(initToken,
                                      *oklForSmnt.iterator);
        expr::variableNode_t blockIterNode(initToken, blockIter);

        // Check variables
        expr::binaryOpNode_t &checkExpr = ((expr::binaryOpNode_t&)
                                           *(((expressionStatement*) blockForSmnt.check)->expr));
        token_t *checkToken = checkExpr.startNode()->token;

        // Update variables
        const operator_t &updateOp = (
          ((expr::binaryOpNode_t&)
           *(((expressionStatement*) blockForSmnt.update)->expr)
          ).op);
        const bool addUpdate = (updateOp.opType & operatorType::addEq);

        // Create init
        innerForSmnt.init = new declarationStatement(&innerForSmnt, initToken);
        variableDeclarationVector &decls = (
          ((declarationStatement*) innerForSmnt.init)
          ->declarations
        );
        decls.push_back(
          variableDeclaration(*oklForSmnt.iterator,
                              *(blockIterNode.clone()))
        );

        // Create check
        expr::binaryOpNode_t checkValueNode(checkToken,
                                            addUpdate ? op::add : op::sub,
                                            blockIterNode,
                                            tileSize);
        expr::parenthesesNode_t checkInParen(checkToken,
                                             checkValueNode);

        const bool varInLeft = oklForSmnt.checkValueOnRight;
        expr::binaryOpNode_t &newCheckNode = *(
          new expr::binaryOpNode_t(
            checkToken,
            (const binaryOperator_t&) checkExpr.op,
            varInLeft ? (expr::node_t&) iterNode : (expr::node_t&) checkInParen,
            varInLeft ? (expr::node_t&) checkInParen : (expr::node_t&) iterNode
          ));
        innerForSmnt.check = new expressionStatement(&innerForSmnt,
                                                     newCheckNode);
      }

      void tile::setupCheckStatement(attributeToken_t &attr,
                                     okl::oklForStatement &oklForSmnt,
                                     variable_t &blockIter,
                                     forStatement &blockForSmnt,
                                     forStatement &innerForSmnt) {
        attributeArgMap::iterator it = attr.kwargs.find("check");
        bool check = true;
        if (it != attr.kwargs.end()) {
          check = (bool) it->second.value->evaluate();
        }
        if (!check) {
          return;
        }
        // Check variables
        expr::binaryOpNode_t &checkExpr = ((expr::binaryOpNode_t&)
                                           *(((expressionStatement*) blockForSmnt.check)->expr));
        token_t *checkToken = checkExpr.startNode()->token;
        const bool varInLeft = oklForSmnt.checkValueOnRight;
        // Make ifStatement
        ifStatement &ifSmnt = *(new ifStatement(&innerForSmnt,
                                                checkToken));
        innerForSmnt.swap(ifSmnt);
        innerForSmnt.scope.swap(ifSmnt.scope);
        innerForSmnt.add(ifSmnt);
        // Get global check
        token_t *iterToken = (varInLeft
                              ? checkExpr.leftValue->token
                              : checkExpr.rightValue->token);
        expr::variableNode_t iterNode(iterToken,
                                      *oklForSmnt.iterator);
        expr::binaryOpNode_t &newCheckNode = *(
          new expr::binaryOpNode_t(
            checkExpr.token,
            (const binaryOperator_t&) checkExpr.op,
            varInLeft ? (expr::node_t&) iterNode : *(checkExpr.leftValue),
            varInLeft ? (expr::node_t&) *(checkExpr.rightValue) : (expr::node_t&) iterNode
          ));

        ifSmnt.setCondition(new expressionStatement(&ifSmnt,
                                                    newCheckNode,
                                                    false));
      }

      bool applyTileTransforms(statement_t &smnt) {
        tile tileTransform;
        return tileTransform.apply(smnt);
      }
    }
  }
}
