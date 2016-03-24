#include <poincare/power.h>
#include <math.h>
#include "layout/exponent_layout.h"

Power::Power(Expression * base, Expression * exponent) :
  m_base(base),
  m_exponent(exponent) {
}

Power::~Power() {
  delete m_exponent;
  delete m_base;
}

Expression * Power::clone() {
  return new Power(m_base->clone(), m_exponent->clone());
}

float Power::approximate(Context& context) {
  return powf(m_base->approximate(context), m_exponent->approximate(context));
}

Expression::Type Power::type() {
  return Expression::Type::Power;
}

ExpressionLayout * Power::createLayout(ExpressionLayout * parent) {
  return new ExponentLayout(parent, m_base, m_exponent);
}
