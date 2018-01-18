#ifndef __SQLITE3API_HPP__
#define __SQLITE3API_HPP__
#include <sqlite3.h> 
#include <iostream>
#include <utility>
#include <vector>
#include <string>

using namespace std;

namespace HUPROJ
{
      const string LOG_TABLE{"LOG_TABLE"};
      const string PN_TABLE{"PN_TABLE"};
      const string WO_TABLE{"WO_TABLE"};
      const string FIO300N{"FIO300N"};
      const string FIO300L{"FIO300L"};
      const string FIO300H{"FIO300H"};
      const string DIO50L{"DIO50L"};
      const string DIOU4{"DIOU4"};
      const string DIO50H{"DIO50H"};
      const string DIOD4{"DIOD4"};
      const string DIOINIT{"DIOINIT"};
      const string FIO3360{"FIO3360"};
      const string TESTING{"testing"};
      const string FAILED{"failed"};
      const string PASSED{"passed"};
}

class SQLITEDB
{
public:
      SQLITEDB(const string& file) : db(nullptr), zErrMsg(nullptr), rc(0), sql(nullptr), state(false)
      {
            rc = sqlite3_open(file.c_str(), &db);
            if(rc == 0)
                  state = true;
      }
      ~SQLITEDB()
      {
            sqlite3_close(db);
      }

      int exec(const string& s)
      {
            rc = sqlite3_exec(db, s.c_str(), callback, 0, &zErrMsg);
            if(rc != 0)
                  cout << "sqlite error code: " << rc << " msg: " << zErrMsg << endl;
            return rc;
      }

      const char* getErroMsg(){ return zErrMsg; }
      bool getState(){ return state; }
      int busyTimeOut(const int& ms)
      {
            cout << "set busy time = " << ms << endl;
            return sqlite3_busy_timeout(db, ms);
      }

      static vector< pair<string, string> > queryRet;
private:
      sqlite3 *db;
      char *zErrMsg;
      int  rc;
      char *sql;
      bool state;

      static int callback(void *NotUsed, int argc, char **argv, char **azColName)
      {
            int i;
            pair<string, string> ret;
            for(i = 0; i < argc; i++)
            {
                  // cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << endl;
                  if(argv[i])
                        ret = make_pair(azColName[i], argv[i]);
                  else
                        ret = make_pair(azColName[i], "NULL");
                  queryRet.push_back(ret);
            }
            return 0;
      }
};

class HUDB
{
public:
      HUDB(SQLITEDB* d) : hdb(d), ret(0){}  
      ~HUDB(){}  

      int createDbTables();
      bool inserPN(const string& pn);
      bool inserWO(const string& wo);
      bool inserWO(const string& wo, const string& ver);
      bool insertSN(const string& sn, const string& state);
      bool insertSN(const string& sn, const string& state, const string& hn);
      bool queryPassedSn(const string& sn);
      bool delSN(const string& sn, const string& state);
      bool delSN(const string& sn, const string& state, const string& hn);
      bool updateFailSn(const string&sn, const string& state, const string& dutTime, const string& hn, const string& portNum);

      bool updateLog(const string& sn, const string& log, const string& mode);
      bool updatePassLog(const string& sn, const string& dutTime, const string& hn, const string& portNum);

      static string pnId;
      static string woId;
private:
      SQLITEDB* hdb;
      const string pnT{HUPROJ::PN_TABLE};
      const string woT{HUPROJ::WO_TABLE};
      const string logT{HUPROJ::LOG_TABLE};
      string cmd;
      int ret;

      int loopExec(const string& c);
};

#endif

