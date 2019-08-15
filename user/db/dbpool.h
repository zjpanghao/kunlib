#ifndef DB_POOL_H
#define DB_POOL_H
#include <zdb.h>
#include <memory>
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
 
 private:
  ConnectionPool_T  pool;
  URL_T  url;

};

class DBPoolGuard {
 public:
   DBPoolGuard(std::shared_ptr<DBPool> pool, Connection_T *conn);
   ~DBPoolGuard();

 private:
   std::shared_ptr<DBPool> pool_;
   Connection_T conn_;
};

#endif

