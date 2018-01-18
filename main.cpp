#include "sqlite3api.hpp"

int main(int argc, char* argv[])
{
      SQLITEDB db("test.db");
      if(db.getState())
      {
            if(db.exec("CREATE TABLE IF NOT EXISTS 'sample'('NAME' TEXT PRIMARY KEY NOT NULL, 'MAIL' TEXT NOT NULL);") == 0)
            {
                  db.exec("INSERT INTO 'sample' VALUES('Allen_Huang', 'allen_huang@innodisk.com')");
                  db.exec("SELECT * FROM 'sample';");
                  for(const auto& x : db.queryRet)
                        std::cout << x.first << " = " << x.second << std::endl;
            }
      }
      return 0;
}
