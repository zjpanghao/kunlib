#ifndef INCLUDE_SQL_TEMPLATE_H
#define INCLUDE_SQL_TEMPLATE_H
#include <string>
#include <list>
#include "dbpool.h"
#include "rowMapper.h"
#include "glog/logging.h"
#include <memory>
#include "preparedStmt.h"
class DBPool;
class SqlTemplate {
  public:
    explicit SqlTemplate(std::shared_ptr<DBPool> pool) 
      : pool_(pool)
  {

    }
    
    int update(const std::string sql) {
      int rc = 0;
      Connection_T conn; 
      DBPoolGuard guard(pool_, &conn);
      if (conn == NULL) {
        return -1;
      }
      TRY {
        Connection_execute(conn, 
            sql.c_str());
      } CATCH(SQLException) {
        LOG(ERROR) << "conn execute error" << Exception_frame.message;
        rc = -1;
      }
      END_TRY;
      return rc;
    }

    void setPreparedStmt(const PreparedStmt &ps, PreparedStatement_T &p) {
      for (int i = 0; i < ps.size(); i++) {
        int inx = i + 1;
        switch(ps.getType(inx)) {
          case PreparedStmt::ElemType::STRING:
            PreparedStatement_setString(p,                     inx,ps.getString(inx).c_str());
            break;
          case PreparedStmt::ElemType::INT:
            PreparedStatement_setInt(p,                     inx,ps.getInt(inx));
            break;
        }
      }
    }

    int update(
        const PreparedStmt &ps) {
      int rc = 0;
      Connection_T conn; 
      DBPoolGuard guard(pool_, &conn);
      if (conn == NULL) {
        return -1;
      }
      TRY {
        //Connection_execute(conn, "set names utf8"); 
        PreparedStatement_T p 
          =  Connection_prepareStatement(conn, ps.sql().c_str());
        setPreparedStmt(ps, p);
        PreparedStatement_execute(p);
      } CATCH(SQLException) {
        LOG(ERROR) << "execute error" << Exception_frame.message;
        rc = -1;
      }
      END_TRY;
      return rc;
    }
    template <class T>
      int query(
          const PreparedStmt &ps,
          RowMapper<T>   &mapper,
          std::list<T> &l) {
        int rc = 0;
        Connection_T conn; 
        DBPoolGuard guard(pool_, &conn);
        if (conn == NULL) {
          return -1;
        }
        ResultSet_T r;
        TRY {
          PreparedStatement_T p
            = Connection_prepareStatement(conn, ps.sql().c_str());
            setPreparedStmt(ps, p);
            r = PreparedStatement_executeQuery(p);
          } CATCH(SQLException) {
            LOG(ERROR) << "load person faces error" << Exception_frame.message;
            rc = -1;
            l.clear();
          }
          END_TRY;
          if (rc != 0) {
            return rc;
          }
          TRY {
            while (ResultSet_next(r)) {
              T t;
              int count = ResultSet_getColumnCount(r);
              int rt = 0;
              rt = mapper.mapRow(r, 0, t);
              if(rt == 0) {
                l.push_back(t);
              }
            }
          } CATCH(SQLException) {
            LOG(ERROR) << "query error:" << Exception_frame.message;
            l.clear();
            rc = -1;
          }
          END_TRY;
        }
      template <class T>
        int query(const std::string sql,
            RowMapper<T>   &mapper,
            std::list<T> &l) {
          PreparedStmt ps(sql);
          return query<T>(ps, mapper, l);
          int rc = 0;
          Connection_T conn; 
          DBPoolGuard guard(pool_, &conn);
          if (conn == NULL) {
          return -1;
        }
        ResultSet_T r;
        TRY {
          PreparedStatement_T p
            = Connection_prepareStatement(conn, sql.c_str());
          r = PreparedStatement_executeQuery(p);
        } CATCH(SQLException) {
          LOG(ERROR) << "load person faces error" << Exception_frame.message;
          rc = -1;
          l.clear();
        }
        END_TRY;
        if (rc != 0) {
          return rc;
        }
        TRY {
          while (ResultSet_next(r)) {
            T t;
            int count = ResultSet_getColumnCount(r);
            int rt = 0;
            rt = mapper.mapRow(r, 0, t);
            if(rt == 0) {
              l.push_back(t);
            }
          }
        } CATCH(SQLException) {
          LOG(ERROR) << "query error:" << Exception_frame.message;
          l.clear();
          rc = -1;
        }
        END_TRY;
      }

  private:
    std::shared_ptr<DBPool> pool_;
};
#endif
