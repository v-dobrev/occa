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
#include <occa/tools/env.hpp>
#include <occa/tools/testing.hpp>

#include <occa/lang/expr.hpp>
#include <occa/lang/tokenizer.hpp>
#include <occa/lang/builtins/types.hpp>
#include <occa/lang/variable.hpp>

using namespace occa;
using namespace occa::lang;

void testPrint();
void testTernary();
void testPairMatching();
void testSpecialOperators();
void testCanEvaluate();
void testEval();

expr::node_t* makeExpression(const std::string &s) {
  tokenVector tokens = tokenizer_t::tokenize(s);
  return expr::getExpression(tokens);
}

bool canEvaluate(const std::string &s) {
  expr::node_t *expr = makeExpression(s);
  bool ret = expr->canEvaluate();
  delete expr;
  return ret;
}

primitive eval(const std::string &s) {
  expr::node_t *expr = makeExpression(s);
  primitive value = expr->evaluate();
  delete expr;
  return value;
}

int main(const int argc, const char **argv) {
  testPrint();
  testTernary();
  testPairMatching();
  testSpecialOperators();
  testCanEvaluate();
  testEval();

  return 0;
}

void testStreamPrint();
void testPoutPrint();
void testDebugPrint();

void testPrint() {
  testStreamPrint();
  testPoutPrint();
  testDebugPrint();
}

void testStreamPrint() {
  expr::primitiveNode_t a(NULL, 1);
  expr::primitiveNode_t b(NULL, 2.0);
  expr::primitiveNode_t c(NULL, false);

  qualifiers_t q1;
  q1 += volatile_;

  vartype_t t1_0(float_);
  t1_0 += const_;
  vartype_t t1_1(t1_0);
  t1_1 += pointer_t();
  vartype_t t1(t1_1);
  t1 += pointer_t();

  identifierToken varName(fileOrigin(),
                          "var");
  variable_t var_(t1, &varName);
  expr::variableNode_t var(NULL, var_);

  expr::primitiveNode_t one(NULL, 1);
  expr::primitiveNode_t two(NULL, 2);
  expr::primitiveNode_t three(NULL, 3);

  expr::nodeVector args;
  args.push_back(&one);
  args.push_back(&two);
  args.push_back(&three);

  std::cerr
    << "\n---[ Testing << Printing ]------------------------\n"
    //---[ Left Unary ]---------------
    << "not_                : " << expr::leftUnaryOpNode_t(NULL, op::not_, a).toString() << '\n'
    << "positive            : " << expr::leftUnaryOpNode_t(NULL, op::positive, a).toString() << '\n'
    << "negative            : " << expr::leftUnaryOpNode_t(NULL, op::negative, a).toString() << '\n'
    << "tilde               : " << expr::leftUnaryOpNode_t(NULL, op::tilde, a).toString() << '\n'
    << "leftIncrement       : " << expr::leftUnaryOpNode_t(NULL, op::leftIncrement, a).toString() << '\n'
    << "leftDecrement       : " << expr::leftUnaryOpNode_t(NULL, op::leftDecrement, a).toString() << '\n'
    //================================

    //---[ Right Unary ]--------------
    << "rightIncrement      : " << expr::rightUnaryOpNode_t(NULL, op::rightIncrement, a).toString() << '\n'
    << "rightDecrement      : " << expr::rightUnaryOpNode_t(NULL, op::rightDecrement, a).toString() << '\n'
    //================================

    //---[ Binary ]-------------------
    << "add                 : " << expr::binaryOpNode_t(NULL, op::add, a, b).toString() << '\n'
    << "sub                 : " << expr::binaryOpNode_t(NULL, op::sub, a, b).toString() << '\n'
    << "mult                : " << expr::binaryOpNode_t(NULL, op::mult, a, b).toString() << '\n'
    << "div                 : " << expr::binaryOpNode_t(NULL, op::div, a, b).toString() << '\n'
    << "mod                 : " << expr::binaryOpNode_t(NULL, op::mod, a, b).toString() << '\n'
    << "lessThan            : " << expr::binaryOpNode_t(NULL, op::lessThan, a, b).toString() << '\n'
    << "lessThanEq          : " << expr::binaryOpNode_t(NULL, op::lessThanEq, a, b).toString() << '\n'
    << "equal               : " << expr::binaryOpNode_t(NULL, op::equal, a, b).toString() << '\n'
    << "compare             : " << expr::binaryOpNode_t(NULL, op::compare, a, b).toString() << '\n'
    << "notEqual            : " << expr::binaryOpNode_t(NULL, op::notEqual, a, b).toString() << '\n'
    << "greaterThan         : " << expr::binaryOpNode_t(NULL, op::greaterThan, a, b).toString() << '\n'
    << "greaterThanEq       : " << expr::binaryOpNode_t(NULL, op::greaterThanEq, a, b).toString() << '\n'
    << "and_                : " << expr::binaryOpNode_t(NULL, op::and_, a, b).toString() << '\n'
    << "or_                 : " << expr::binaryOpNode_t(NULL, op::or_, a, b).toString() << '\n'
    << "bitAnd              : " << expr::binaryOpNode_t(NULL, op::bitAnd, a, b).toString() << '\n'
    << "bitOr               : " << expr::binaryOpNode_t(NULL, op::bitOr, a, b).toString() << '\n'
    << "xor_                : " << expr::binaryOpNode_t(NULL, op::xor_, a, b).toString() << '\n'
    << "leftShift           : " << expr::binaryOpNode_t(NULL, op::leftShift, a, b).toString() << '\n'
    << "rightShift          : " << expr::binaryOpNode_t(NULL, op::rightShift, a, b).toString() << '\n'
    << "addEq               : " << expr::binaryOpNode_t(NULL, op::addEq, a, b).toString() << '\n'
    << "subEq               : " << expr::binaryOpNode_t(NULL, op::subEq, a, b).toString() << '\n'
    << "multEq              : " << expr::binaryOpNode_t(NULL, op::multEq, a, b).toString() << '\n'
    << "divEq               : " << expr::binaryOpNode_t(NULL, op::divEq, a, b).toString() << '\n'
    << "modEq               : " << expr::binaryOpNode_t(NULL, op::modEq, a, b).toString() << '\n'
    << "andEq               : " << expr::binaryOpNode_t(NULL, op::andEq, a, b).toString() << '\n'
    << "orEq                : " << expr::binaryOpNode_t(NULL, op::orEq, a, b).toString() << '\n'
    << "xorEq               : " << expr::binaryOpNode_t(NULL, op::xorEq, a, b).toString() << '\n'
    << "leftShiftEq         : " << expr::binaryOpNode_t(NULL, op::leftShiftEq, a, b).toString() << '\n'
    << "rightShiftEq        : " << expr::binaryOpNode_t(NULL, op::rightShiftEq, a, b).toString() << '\n'
    //================================

    //---[ Ternary ]------------------
    << "ternary             : " << expr::ternaryOpNode_t(a, b, c).toString() << '\n'
    //================================

    //---[ Other Nodes ]--------------
    << "one                 : " << one.toString() << '\n'
    << "var                 : " << var.toString() << '\n'
    << "subscript           : " << expr::subscriptNode_t(NULL, var, one).toString() << '\n'
    << "callNode            : " << expr::callNode_t(NULL, var, args).toString() << '\n'
    << "newNode             : " << expr::newNode_t(NULL, t1, var, three).toString() << '\n'
    << "newNode             : " << expr::newNode_t(NULL, t1, var).toString() << '\n'
    << "deleteNode          : " << expr::deleteNode_t(NULL, var, false).toString() << '\n'
    << "deleteNode          : " << expr::deleteNode_t(NULL, var, true).toString() << '\n'
    << "throwNode           : " << expr::throwNode_t(NULL, one).toString() << '\n'
    << "sizeofNode          : " << expr::sizeofNode_t(NULL, var).toString() << '\n'
    << "funcCastNode        : " << expr::funcCastNode_t(NULL, t1, var).toString() << '\n'
    << "parenCastNode       : " << expr::parenCastNode_t(NULL, t1, var).toString() << '\n'
    << "constCastNode       : " << expr::constCastNode_t(NULL, t1, var).toString() << '\n'
    << "staticCastNode      : " << expr::staticCastNode_t(NULL, t1, var).toString() << '\n'
    << "reinterpretCastNode : " << expr::reinterpretCastNode_t(NULL, t1, var).toString() << '\n'
    << "dynamicCastNode     : " << expr::dynamicCastNode_t(NULL, t1, var).toString() << '\n'
    << "parenthesesNode     : " << expr::parenthesesNode_t(NULL, var).toString() << '\n'
    << "cudaCallNode        : " << expr::cudaCallNode_t(NULL, var, one, two).toString() << '\n'
    //================================
    ;
}

void testPoutPrint() {
  expr::primitiveNode_t a(NULL, 1);
  expr::primitiveNode_t b(NULL, 2.0);
  expr::primitiveNode_t c(NULL, false);

  qualifiers_t q1;
  q1 += volatile_;

  vartype_t t1_0(float_);
  t1_0 += const_;
  vartype_t t1_1(t1_0);
  t1_1 += pointer_t();
  vartype_t t1(t1_1);
  t1 += pointer_t();

  identifierToken varName(fileOrigin(),
                          "var");
  variable_t var_(t1, &varName);
  expr::variableNode_t var(NULL, var_);

  expr::primitiveNode_t one(NULL, 1);
  expr::primitiveNode_t two(NULL, 2);
  expr::primitiveNode_t three(NULL, 3);

  expr::nodeVector args;
  args.push_back(&one);
  args.push_back(&two);
  args.push_back(&three);

  printer pout(std::cerr);

  std::cerr << "\n---[ Testing pout Printing ]----------------------\n";

  //---[ Left Unary ]---------------
  expr::leftUnaryOpNode_t(NULL, op::not_, a).print(pout); pout << '\n';
  expr::leftUnaryOpNode_t(NULL, op::positive, a).print(pout); pout << '\n';
  expr::leftUnaryOpNode_t(NULL, op::negative, a).print(pout); pout << '\n';
  expr::leftUnaryOpNode_t(NULL, op::tilde, a).print(pout); pout << '\n';
  expr::leftUnaryOpNode_t(NULL, op::leftIncrement, a).print(pout); pout << '\n';
  expr::leftUnaryOpNode_t(NULL, op::leftDecrement, a).print(pout); pout << '\n';
  //================================

  //---[ Right Unary ]--------------
  expr::rightUnaryOpNode_t(NULL, op::rightIncrement, a).print(pout); pout << '\n';
  expr::rightUnaryOpNode_t(NULL, op::rightDecrement, a).print(pout); pout << '\n';
  //================================

  //---[ Binary ]-------------------
  expr::binaryOpNode_t(NULL, op::add, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::sub, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::mult, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::div, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::mod, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::lessThan, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::lessThanEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::equal, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::compare, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::notEqual, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::greaterThan, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::greaterThanEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::and_, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::or_, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::bitAnd, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::bitOr, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::xor_, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::leftShift, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::rightShift, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::addEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::subEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::multEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::divEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::modEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::andEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::orEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::xorEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::leftShiftEq, a, b).print(pout); pout << '\n';
  expr::binaryOpNode_t(NULL, op::rightShiftEq, a, b).print(pout); pout << '\n';
  //================================

  //---[ Ternary ]------------------
  expr::ternaryOpNode_t(a, b, c).print(pout); pout << '\n';
  //================================

  //---[ Other Nodes ]--------------
  one.print(pout); pout << '\n';
  var.print(pout); pout << '\n';
  expr::subscriptNode_t(NULL, var, one).print(pout); pout << '\n';
  expr::callNode_t(NULL, var, args).print(pout); pout << '\n';
  expr::newNode_t(NULL, t1, var, three).print(pout); pout << '\n';
  expr::newNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::deleteNode_t(NULL, var, false).print(pout); pout << '\n';
  expr::deleteNode_t(NULL, var, true).print(pout); pout << '\n';
  expr::throwNode_t(NULL, one).print(pout); pout << '\n';
  expr::sizeofNode_t(NULL, var).print(pout); pout << '\n';
  expr::funcCastNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::parenCastNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::constCastNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::staticCastNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::reinterpretCastNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::dynamicCastNode_t(NULL, t1, var).print(pout); pout << '\n';
  expr::parenthesesNode_t(NULL, var).print(pout); pout << '\n';
  expr::cudaCallNode_t(NULL, var, one, two).print(pout); pout << '\n';
  //================================
}

void testDebugPrint() {
  expr::primitiveNode_t a(NULL, 1);
  expr::primitiveNode_t b(NULL, 2.0);
  expr::primitiveNode_t c(NULL, false);

  qualifiers_t q1;
  q1 += volatile_;

  vartype_t t1_0(float_);
  t1_0 += const_;
  vartype_t t1_1(t1_0);
  t1_1 += pointer_t();
  vartype_t t1(t1_1);
  t1 += pointer_t();

  identifierToken varName(fileOrigin(),
                          "var");
  variable_t var_(t1, &varName);
  expr::variableNode_t var(NULL, var_);

  expr::primitiveNode_t one(NULL, 1);
  expr::primitiveNode_t two(NULL, 2);
  expr::primitiveNode_t three(NULL, 3);

  expr::nodeVector args;
  args.push_back(&one);
  args.push_back(&two);
  args.push_back(&three);

  std::cerr << "\n---[ Testing Debug Printing ]---------------------\n";

  //---[ Left Unary ]---------------
  expr::leftUnaryOpNode_t(NULL, op::not_, a).debugPrint("");
  expr::leftUnaryOpNode_t(NULL, op::positive, a).debugPrint("");
  expr::leftUnaryOpNode_t(NULL, op::negative, a).debugPrint("");
  expr::leftUnaryOpNode_t(NULL, op::tilde, a).debugPrint("");
  expr::leftUnaryOpNode_t(NULL, op::leftIncrement, a).debugPrint("");
  expr::leftUnaryOpNode_t(NULL, op::leftDecrement, a).debugPrint("");
  //================================

  //---[ Right Unary ]--------------
  expr::rightUnaryOpNode_t(NULL, op::rightIncrement, a).debugPrint("");
  expr::rightUnaryOpNode_t(NULL, op::rightDecrement, a).debugPrint("");
  //================================

  //---[ Binary ]-------------------
  expr::binaryOpNode_t(NULL, op::add, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::sub, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::mult, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::div, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::mod, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::lessThan, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::lessThanEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::equal, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::compare, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::notEqual, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::greaterThan, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::greaterThanEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::and_, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::or_, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::bitAnd, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::bitOr, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::xor_, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::leftShift, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::rightShift, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::addEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::subEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::multEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::divEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::modEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::andEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::orEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::xorEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::leftShiftEq, a, b).debugPrint("");
  expr::binaryOpNode_t(NULL, op::rightShiftEq, a, b).debugPrint("");
  //================================

  //---[ Ternary ]------------------
  expr::ternaryOpNode_t(a, b, c).debugPrint("");
  //================================

  //---[ Other Nodes ]--------------
  one.debugPrint("");
  var.debugPrint("");
  expr::subscriptNode_t(NULL, var, one).debugPrint("");
  expr::callNode_t(NULL, var, args).debugPrint("");
  expr::newNode_t(NULL, t1, var, three).debugPrint("");
  expr::newNode_t(NULL, t1, var).debugPrint("");
  expr::deleteNode_t(NULL, var, false).debugPrint("");
  expr::deleteNode_t(NULL, var, true).debugPrint("");
  expr::throwNode_t(NULL, one).debugPrint("");
  expr::sizeofNode_t(NULL, var).debugPrint("");
  expr::funcCastNode_t(NULL, t1, var).debugPrint("");
  expr::parenCastNode_t(NULL, t1, var).debugPrint("");
  expr::constCastNode_t(NULL, t1, var).debugPrint("");
  expr::staticCastNode_t(NULL, t1, var).debugPrint("");
  expr::reinterpretCastNode_t(NULL, t1, var).debugPrint("");
  expr::dynamicCastNode_t(NULL, t1, var).debugPrint("");
  expr::parenthesesNode_t(NULL, var).debugPrint("");
  expr::cudaCallNode_t(NULL, var, one, two).debugPrint("");
  //================================
}

void testTernary() {
  expr::node_t *expr;

  expr = makeExpression("a = true ? 1 : 2");
  expr->debugPrint();
  delete expr;

  expr = makeExpression("a = true ? (false ? 1 : (false ? 2 : 3)) : 4");
  expr->debugPrint();
  delete expr;
}

void testPairMatching() {
  expr::node_t *expr = makeExpression("func(0,1,2,3,4)");
  ASSERT_EQ_BINARY(expr::nodeType::call,
                   expr->type());
  expr::callNode_t &func = expr->to<expr::callNode_t>();

  ASSERT_EQ("func", func.value->to<expr::identifierNode_t>().value);
  ASSERT_EQ(5, (int) func.args.size());
  for (int i = 0; i < 5; ++i) {
    expr::primitiveNode_t &arg = func.args[i]->to<expr::primitiveNode_t>();
    ASSERT_EQ(i, (int) arg.value);
  }

  delete expr;
  expr = makeExpression("(0,1,2,3,4)");
  ASSERT_EQ_BINARY(expr::nodeType::parentheses,
                   expr->type());

  delete expr;
  expr = makeExpression("{0,1,2,3,4}");
  ASSERT_EQ_BINARY(expr::nodeType::tuple,
                   expr->type());

  delete expr;
  expr = makeExpression("array[0 + 1]");
  ASSERT_EQ_BINARY(expr::nodeType::subscript,
                   expr->type());

  delete expr;
  expr = makeExpression("func<<<0,1>>>");
  ASSERT_EQ_BINARY(expr::nodeType::cudaCall,
                   expr->type());

  delete expr;

  std::cerr << "\nTesting pair errors:\n";
  makeExpression("(0,1,2]");
  makeExpression("[0,1,2}");
  makeExpression("{0,1,2)");
  makeExpression("<<<0,1,2)");
}

void testSpecialOperators() {
  expr::node_t *expr = makeExpression("sizeof(int)");
  ASSERT_EQ_BINARY(expr::nodeType::sizeof_,
                   expr->type());

  delete expr;
  expr = makeExpression("throw 2 + 2");
  ASSERT_EQ_BINARY(expr::nodeType::throw_,
                   expr->type());

  delete expr;

  std::cerr << "\nTesting unsupported new and delete:\n";
  expr = makeExpression("new int[2]");
  expr = makeExpression("delete foo");
  expr = makeExpression("delete [] foo");
}

void testCanEvaluate() {
  ASSERT_TRUE(canEvaluate("1 + 2 / (3)"));
  ASSERT_FALSE(canEvaluate("1 + 2 / (3) + '1'"));
  ASSERT_FALSE(canEvaluate("&1"));
  ASSERT_FALSE(canEvaluate("*1"));
  ASSERT_FALSE(canEvaluate("1::2"));
  ASSERT_FALSE(canEvaluate("(1).(2)"));
  ASSERT_FALSE(canEvaluate("(1).*(2)"));
  ASSERT_FALSE(canEvaluate("1->2"));
  ASSERT_FALSE(canEvaluate("1->*2"));
}

void testEval() {
  ASSERT_EQ((int) (1 + (2 * 3)),
            (int) eval("1 + (2 * 3)"));

  ASSERT_EQ((int) (1 + 2 / (3)),
            (int) eval("1 + 2 / (3)"));

  ASSERT_EQ((double) ((1 + 2 / 3.1 * 4.4) / 1.2),
            (double) eval("(1 + 2 / 3.1 * 4.4) / 1.2"));

  ASSERT_EQ((int) 3,
            (int) eval("++++1"));

  ASSERT_EQ((int) 1,
            (int) eval("1++++"));

  ASSERT_EQ((int) 4,
            (int) eval("1 ++ + ++ 2"));

  ASSERT_EQ((int) 5,
            (int) eval("1 ++ + ++ + ++ 2"));

  ASSERT_EQ((int) -1,
            (int) eval("----1"));

  ASSERT_EQ((int) 1,
            (int) eval("1----"));

  ASSERT_EQ((int) 2,
            (int) eval("1 -- + -- 2"));

  ASSERT_EQ((int) 1,
            (int) eval("1 -- + -- + -- 2"));

  ASSERT_EQ((int) 1,
            (int) eval("+ + + + + + 1"));

  ASSERT_EQ((int) -1,
            (int) eval("- - - - - 1"));

  ASSERT_EQ((int) 1,
            (int) eval("- - - - - - 1"));
}
