#include <poincare/subtraction.h>
#include "layout/horizontal_layout.h"

Subtraction::Subtraction(Expression * first_operand, Expression * second_operand) {
  m_left = first_operand;
  m_right = second_operand;
}

Subtraction::~Subtraction() {
  delete m_left;
  delete m_right;
}

Expression * Subtraction::clone() {
  return new Subtraction(m_left->clone(), m_right->clone());
}

float Subtraction::approximate(Context& context) {
  return m_left->approximate(context) - m_right->approximate(context);
}

Expression::Type Subtraction::type() {
  return Expression::Type::Subtraction;
}

ExpressionLayout * Subtraction::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_left, '-', m_right);
}
