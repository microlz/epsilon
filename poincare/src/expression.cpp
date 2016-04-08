#include <poincare/expression.h>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"
extern "C" {
#include <assert.h>
}

#include "simplify/simplification_rules.h"

int poincare_expression_yyparse(yyscan_t scanner, Expression ** expressionOutput);

Expression::~Expression() {
}

Expression * Expression::parse(char const * string) {
  void * scanner;
  poincare_expression_yylex_init(&scanner);
  YY_BUFFER_STATE buf = poincare_expression_yy_scan_string(string, scanner);
  Expression * expression = 0;
  poincare_expression_yyparse(scanner, &expression);
  poincare_expression_yy_delete_buffer(buf, scanner);
  poincare_expression_yylex_destroy(scanner);

  return expression;
}

Expression * Expression::simplify() {
  /* We make sure that the simplification is deletable.
   * Indeed, we don't want an expression with some parts deletable and some not
   */

  // If we have a leaf node nothing can be simplified.
  if (this->numberOfOperands()==0) {
    return this->clone();
  }

  /* We recursively simplify the children expressions.
   * Note that we are sure to get the samne number of children as we had before
   */
  Expression ** simplifiedOperands = (Expression**) malloc(this->numberOfOperands() * sizeof(Expression*));
  for (int i = 0; i < this->numberOfOperands(); i++) {
    simplifiedOperands[i] = this->operand(i)->simplify();
  }

  /* Note that we don't need to clone the simplified children because they are
   * already cloned before. */
  Expression * result = this->cloneWithDifferentOperands(simplifiedOperands, this->numberOfOperands(), false);

  // The table is no longer needed.
  free(simplifiedOperands);

  bool simplification_pass_was_useful = true;
  while (simplification_pass_was_useful) {
    simplification_pass_was_useful = false;
    for (int i=0; i<knumberOfSimplifications; i++) {
      const Simplification * simplification = (simplifications + i); // Pointer arithmetics
      Expression * simplified = simplification->simplify(result);
      if (simplified != nullptr) {
        simplification_pass_was_useful = true;
        delete result;
        result = simplified;
      }
    }
  }

  return result;
}

bool Expression::sequentialOperandsIdentity(Expression * e) {
  /* Here we simply test all operands for identity in the order they are defined
   * in. */
  for (int i=0; i<this->numberOfOperands(); i++) {
    if (!e->operand(i)->isIdenticalTo(this->operand(i))) {
      return false;
    }
  }
  return true;
}

bool Expression::combinatoryCommutativeOperandsIdentity(Expression * e,
    bool * operandMatched, int leftToMatch) {
  if (leftToMatch == 0) {
    return true;
  }

  // We try to test for equality the i-th operand of our first expression.
  int i = this->numberOfOperands() - leftToMatch;
  for (int j = 0; j<e->numberOfOperands(); j++) {
    /* If the operand of the second expression has already been associated with
     * a previous operand we skip it */
    if (operandMatched[j]) {
      continue;
    }
    if (this->operand(i)->isIdenticalTo(e->operand(j))) {
      // We managed to match this operand.
      operandMatched[j] = true;
      /* We check that we can match the rest in this configuration, if so we
       * are good. */
      if (this->combinatoryCommutativeOperandsIdentity(e, operandMatched, leftToMatch - 1)) {
        return true;
      }
      // Otherwise we backtrack.
      operandMatched[j] = false;
    }
  }

  return false;
}

bool Expression::commutativeOperandsIdentity(Expression * e) {
  int leftToMatch = this->numberOfOperands();

  /* We create a table allowing us to know which operands of the second
   * expression have been associated with one of the operands of the first
   * expression */
  bool * operandMatched = (bool *) malloc (this->numberOfOperands() * sizeof(bool));
  for (int i(0); i<this->numberOfOperands(); i++) {
    operandMatched[i] = false;
  }

  // We call our recursive helper.
  bool commutativelyIdentical = this->combinatoryCommutativeOperandsIdentity(e, operandMatched, leftToMatch);

  free(operandMatched);
  return commutativelyIdentical;
}

bool Expression::isIdenticalTo(Expression * e) {
  if (e->type() != this->type() || e->numberOfOperands() != this->numberOfOperands()) {
    return false;
  }
  if (this->isCommutative()) {
    if (!this->commutativeOperandsIdentity(e)) {
      return false;
    }
  } else {
    if (!this->sequentialOperandsIdentity(e)) {
      return false;
    }
  }
  return e->valueEquals(this);
}

bool Expression::valueEquals(Expression * e) {
  assert(this->type() == e->type());
  /* This behavior makes sense for value-less nodes (addition, product, fraction
   * power, etc… For nodes with a value (Integer, Float), this must be over-
   * -riden. */
  return true;
}

bool Expression::isCommutative() {
  return false;
}
