#ifndef INCLUDE_ROW_MAPPER_H
#define INCLUDE_ROW_MAPPER_H
#include "zdb/zdb.h"
template<class T>
class RowMapper {
 public:
   virtual ~RowMapper() = default;
   virtual int  mapRow(ResultSet_T rs,
                  int i,
                  T &t) = 0;


};
#endif
