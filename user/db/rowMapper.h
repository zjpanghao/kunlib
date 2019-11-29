#ifndef INCLUDE_ROW_MAPPER_H
#define INCLUDE_ROW_MAPPER_H
#include "zdb/zdb.h"
template<class T>
class RowMapper {
 public:
   virtual ~RowMapper() = default;
   int getDouble(
       ResultSet_T r,
       const std::string &name,
       double &value) {
     int rc = 0;
      TRY {
        value = ResultSet_getDoubleByName(r, name.c_str());
      } CATCH(SQLException) {
        LOG(ERROR) <<name << "getDoubler error" << Exception_frame.message;
        rc = -1;
      }
      END_TRY;
      return rc;
   }

   int getInt(
       ResultSet_T r,
       const std::string &name,
       int &value) {
     int rc = 0;
      TRY {
        value = ResultSet_getIntByName(r, name.c_str());
      } CATCH(SQLException) {
        LOG(ERROR) <<name << "getint error" << Exception_frame.message;
        rc = -1;
      }
      END_TRY;
      return rc;
   }

   int getString(
       ResultSet_T r,
       const std::string &name,
       std::string &value) {
     int rc = 0;
      TRY {
        const char *s = ResultSet_getStringByName(r, name.c_str());
        if (s) {
          value = s;
        }
      } CATCH(SQLException) {
        LOG(ERROR) <<name << "getStr error" << Exception_frame.message;
        rc = -1;
      }
      END_TRY;
      return rc;
   }

   virtual int  mapRow(ResultSet_T rs,
                  int i,
                  T &t) = 0;


};
#endif
