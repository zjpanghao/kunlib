#include "preparedStmt.h"
PreparedStmt::PreparedStmt(const std::string &sql) :sql_(sql) {
}

PreparedStmt::ElemType PreparedStmt::getType(int i) const{
  int j = i - 1;
  if (j < 0 || j >= types_.size()) {
    return ElemType::INVALID; 
  }
  return types_[j];
}

const std::string & PreparedStmt::getString(int i) const{
  if (getType(i) != ElemType::STRING) {
    return NULLSTR_;
  }
  auto it = stringValues_.find(i);
  if (it == stringValues_.end()) {
    return NULLSTR_;
  }
  return it->second;
}

int PreparedStmt::getInt(int i) const{
  if (getType(i) != ElemType::INT) {
    return 0;
  }
  auto it = intValues_.find(i);
  if (it == intValues_.end()) {
    return 0;
  }
  return it->second;
}

double PreparedStmt::getDouble(int i) const{
  if (getType(i) != ElemType::DOUBLE) {
    return 0;
  }
  auto it = doubleValues_.find(i);
  if (it == doubleValues_.end()) {
    return 0;
  }
  return it->second;
}

void PreparedStmt::setType(int i, ElemType type) {
  if (i <= 0) {
    return;
  }
  int n = i  - types_.size();
  while (n > 0) {
    types_.push_back(ElemType::INVALID); 
    n--;
  }
  types_[i-1] = type;
}

void PreparedStmt::setInt(int i, int value) {
  intValues_[i] = value;
  setType(i, ElemType::INT);
}
void PreparedStmt::setString(int i, const std::string & value) {
  setType(i, ElemType::STRING);
  stringValues_[i] = value;
}
void PreparedStmt::setDouble(int i, double value) {
  doubleValues_[i] = value;
  setType(i, ElemType::DOUBLE);
}

const std::string& PreparedStmt::sql() const{
  return sql_;
}

int PreparedStmt::size() const {
  return types_.size();
}

