extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix_data.h>
#include <poincare/integer.h>

MatrixData::MatrixData(ListData * listData) :
  m_numberOfRows(1),
  m_numberOfColumns(0)
{
  assert(listData != nullptr);
  m_numberOfColumns = listData->numberOfOperands();
  m_operands = (Expression **)malloc(m_numberOfColumns*sizeof(Expression *));
  for (int i = 0; i < m_numberOfColumns; i++) {
    m_operands[i] = (Expression *)listData->operand(i);
  }
}

MatrixData::MatrixData(Expression ** newOperands, int numberOfOperands, int numberOfColumns, int numberOfRows, bool cloneOperands) :
  m_numberOfRows(numberOfRows),
  m_numberOfColumns(numberOfColumns)
{
  assert(newOperands != nullptr);
  m_operands = (Expression **)malloc(m_numberOfRows*m_numberOfColumns*sizeof(Expression *));
  for (int i = 0; i < m_numberOfColumns*m_numberOfRows; i++) {
    if (cloneOperands) {
      m_operands[i] = i < numberOfOperands ? newOperands[i]->clone() : defaultExpression();
    } else {
      m_operands[i] = i < numberOfOperands ? newOperands[i] : defaultExpression();
    }
  }
}

Integer * MatrixData::defaultExpression() {
  static Integer * defaultExpression = new Integer(0);
  return defaultExpression;
}

MatrixData::~MatrixData() {
  for (int i=0; i<m_numberOfColumns*m_numberOfRows; i++) {
    // Warning: avoid deleting the defaultExpression
    if (m_operands[i] != defaultExpression()) {
      delete m_operands[i];
    }
  }
  free(m_operands);
}

void MatrixData::pushListData(ListData * listData) {
  Expression ** newOperands = (Expression **)malloc(((m_numberOfRows+1)*m_numberOfColumns)*sizeof(Expression *));
  for (int i = 0; i < m_numberOfRows*m_numberOfColumns; i++) {
    newOperands[i] = m_operands[i];
  }
  for (int i = 0; i < m_numberOfColumns; i++) {
    int max = listData->numberOfOperands();
    newOperands[m_numberOfRows*m_numberOfColumns+i] = i < max ? (Expression *)listData->operand(i) : defaultExpression();
  }
  free(m_operands);
  m_operands = newOperands;
  m_numberOfRows++;
}

int MatrixData::numberOfRows() {
  return m_numberOfRows;
}

int MatrixData::numberOfColumns() {
  return m_numberOfColumns;
}

Expression ** MatrixData::operands() const {
  return m_operands;
}

