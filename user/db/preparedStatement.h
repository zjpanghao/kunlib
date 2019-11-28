#ifndef INCLUDE_PREPARED_STATEMENT_H
#define INCLUDE_PREPARED_STATEMENT_H
#include <string>
#include <vector>
#include <map>

class PreparedStatement {
  public:
    PreparedStatement(const std::string &sql) :sql_(sql) {
    }

    enum ElemType {
      INVALID,
      INT,
      STRING,
      DOUBLE
    };
    
    ElemType getType(int i) const{
      int j = i - 1;
      if (j < 0 || j >= types_.size()) {
        return ElemType::INVALID; 
      }
      return types_[j];
    }

    std::string getString(int i) const{
      if (getType(i) != ElemType::STRING) {
        return "";
      }
      auto it = stringValues_.find(i);
      if (it == stringValues_.end()) {
        return "";
      }
      return it->second;
    }

    int getInt(int i) const{
      if (getType(i) != ElemType::INT) {
        return 0;
      }
      auto it = intValues_.find(i);
      if (it == intValues_.end()) {
        return 0;
      }
      return it->second;
    }

    double getDouble(int i) const{
      if (getType(i) != ElemType::DOUBLE) {
        return 0;
      }
      auto it = doubleValues_.find(i);
      if (it == doubleValues_.end()) {
        return 0;
      }
      return it->second;
    }
    
    void setType(int i, ElemType type) {
      if (i < 0) {
        return;
      }
      int n = i + 1 - types_.size();
      while (n > 0) {
        types_.push_back(ElemType::INVALID); 
        n--;
      }
      types_[i] = type;
    }

    void setInt(int i, int value) {
      intValues_[i] = value;
      setType(i - 1, ElemType::INT);
    }
    void setString(int i, const std::string & value) {
      setType(i - 1, ElemType::STRING);
      stringValues_[i] = value;
    }
    void setDouble(int i, double value) {
      doubleValues_[i] = value;
      setType(i - 1, ElemType::DOUBLE);
    }
    
    std::string sql() const{
      return sql_;
    }

    int size() const {
      return types_.size();
    }

  private:
    std::string sql_;
    std::vector<ElemType> types_;
    std::map<int, int> intValues_;
    std::map<int, std::string> stringValues_;
    std::map<int, double> doubleValues_;
};
#endif
