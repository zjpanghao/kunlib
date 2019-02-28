#ifndef DB_POOL_H
#define DB_POOL_H
#include <zdb.h>
#include "datasource/dataSource.h"
class DBPool {
 public:
  DBPool();
  int PoolInit(const char *ip, 
               int port,  
               const char *dbname, 
               const char *user, 
               const char *passwd, 
               int poolsize, 
               int initsize, 
               int reapsec);
  int PoolInit(DataSource *source);
  void PoolFree();
  Connection_T GetConnection();
  void returnConnection(Connection_T conn);
  int PoolSizeGet(int &size);
  int PoolActiveSizeGet(int &size);
  static DBPool *GetInstance();
 
 private:
  static  DBPool *pinstance;
  ConnectionPool_T  pool;
  URL_T  url;

};

#endif

