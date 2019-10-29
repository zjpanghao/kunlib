#include "dbpool.h"
#include <stdio.h>
#include "glog/logging.h"
DBPool::DBPool():pool(NULL), url(NULL) {
}

Connection_T DBPool::GetConnection() {
  if(pool == NULL)
    return NULL;
  return ConnectionPool_getConnection((ConnectionPool_S*)pool);
}

int DBPool::PoolInit(MysqlDataSource *dataSource) {
  return PoolInit(dataSource->ip().c_str(), dataSource->port(), dataSource->db().c_str(), 
      dataSource->user().c_str(), dataSource->password().c_str(), 20, 1, 60);
}

int DBPool::PoolInit(const char *ip, int port, const char *dbname, const char *user, const char *passwd, int poolsize, int initsize, int reapsec) {
  char usr_str[128];
  int rc = 0;
  snprintf(usr_str, 127, "mysql://%s:%d/%s?user=%s&password=%s", ip, port,  dbname, user, passwd);
  url = URL_new(usr_str);
  if(!url)
    return -1;
  pool = ConnectionPool_new(url);
  if(!pool)
  {
    URL_free(&url);
    url = NULL;
    return -2;
  }
  TRY
  {
    if(initsize < 0 || initsize < 0 || initsize > poolsize)
      return -3;
    ConnectionPool_setInitialConnections(pool, initsize);
    ConnectionPool_setMaxConnections(pool, poolsize);
    if(reapsec > 0)
      ConnectionPool_setReaper(pool, reapsec);
    ConnectionPool_start(pool);

  }CATCH(SQLException)
  {
    LOG(ERROR) << "connection start error:" << Exception_frame.message;
    rc = -4;
  }
  FINALLY
  {

  }

  END_TRY;
  return rc;
}

void DBPool::PoolFree(){
  if(pool)
    ConnectionPool_free(&pool);
  if(url)
    URL_free(&url);
}

int DBPool::PoolSizeGet(int &size) {
  if(!pool)
    return -1;
  if(pool)
    size = ConnectionPool_size(pool);
  return 0;
}

int DBPool::PoolActiveSizeGet(int &size){
  if(!pool)
    return -1;
  if(pool)
    size = ConnectionPool_active(pool);
  return 0;
}

void DBPool::returnConnection(Connection_T conn) {
  ConnectionPool_returnConnection(pool, conn);
}

DBPoolGuard::DBPoolGuard(std::shared_ptr<DBPool> pool, Connection_T *conn) : pool_(pool), conn_(NULL) {
  if (pool != nullptr) {
    *conn = pool->GetConnection();
    if (*conn) {
      conn_ = *conn;
    }
  }
}

DBPoolGuard::~DBPoolGuard(){
  if (pool_ && conn_) {
    pool_->returnConnection(conn_);
  }
}
