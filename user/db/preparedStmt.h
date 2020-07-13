#ifndef INCLUDE_PREPARED_STATEMENT_H
#define INCLUDE_PREPARED_STATEMENT_H
#include <string>
#include <vector>
#include <map>

class PreparedStmt {
  public:
    explicit PreparedStmt(const std::string &sql);

    enum ElemType {
      INVALID,
      INT,
      STRING,
      DOUBLE
    };
    
    ElemType getType(int i) const;

    const std::string&  getString(int i) const;

    int getInt(int i) const;

    double getDouble(int i) const;
    
    void setType(int i, ElemType type);

    void setInt(int i, int value);
    void setString(int i, 
        const std::string & value);
    void setDouble(int i, double value);
    
    const std::string& sql() const;

    int size() const;

  private:
    const std::string NULLSTR_{""};
    std::string sql_;
    std::vector<ElemType> types_;
    std::map<int, int> intValues_;
    std::map<int, std::string> stringValues_;
    std::map<int, double> doubleValues_;
};
#endif
