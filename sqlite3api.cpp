/*
  ==============================================================================

    sqlite3api.cpp
    Created: 18 Dec 2017 4:49:50pm
    Author:  allenh

  ==============================================================================
*/

#ifndef SQLITE3API_CPP_INCLUDED
#define SQLITE3API_CPP_INCLUDED

#include "sqlite3api.hpp"

extern vector< pair<string, string> > SQLITEDB::queryRet; 
string HUDB::pnId;
string HUDB::woId;


int HUDB::loopExec(const string& c)
{
    static int ms = 10000;
    while(1)
    {
        ret = hdb->exec("BEGIN IMMEDIATE;");
        // ret = hdb->exec(c);
        // if(ret == 5 || ret == 10) // 5:busy 10:io error
        if(ret == 5) // 5:busy 
        {
            if(hdb->busyTimeOut(--ms) != 0)
            {
                cout << "busyTimeOut != 0" << endl;
                return ret;
            }
        }
        else
        {
            hdb->exec(c);
            hdb->exec("COMMIT;");
            ms = 10000;
            return ret;
        }
    }
}

int HUDB::createDbTables()
{
    /* 'PN_TABLE', 'WO_TABLE', 'LOG_TABLE' */
    cmd = "CREATE TABLE IF NOT EXISTS '" + pnT + "'(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, 'PN' TEXT UNIQUE NOT NULL);"; 
    ret = hdb->exec(cmd);
    if(ret != 0)
    {
        return ret;
    }

    cmd = "CREATE TABLE IF NOT EXISTS '" + woT + "'("\
            "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"\
            "'PN_ID' INT NOT NULL,"\
            "'WO' TEXT UNIQUE NOT NULL,"\
            "'VERSION' TEXT,"\
            "FOREIGN KEY('PN_ID') REFERENCES PN(ID));"; 
    ret = hdb->exec(cmd);
    if(ret != 0)
    {
        return ret;
    }

    cmd = "CREATE TABLE IF NOT EXISTS '" + logT + "' ("\
                "'WO_ID' INT NOT NULL,"\
                "SN TEXT NOT NULL,"\
                "'RESULT_PASS' TEXT,"\
                "'STATE' TEXT NOT NULL,"\
                "'TIME_DATE' TEXT,"\
                "'HOST_NAME' TEXT,"\
                "'PORT_NUM' TEXT,"\
                "'4KR_25' TEXT,"\
                "'4KW_25' TEXT,"\
                "'512KR_25' TEXT,"\
                "'512KW_25' TEXT,"\
                "'4KR_N5' TEXT,"\
                "'4KW_N5' TEXT,"\
                "'512KR_N5' TEXT,"\
                "'512KW_N5' TEXT,"\
                "'4KR_50' TEXT,"\
                "'4KW_50' TEXT,"\
                "'512KR_50' TEXT,"\
                "'512KW_50' TEXT,"\
                "'DIO_N5' TEXT,"\
                "'DIO_UP' TEXT,"\
                "'DIO_50' TEXT,"\
                "'DIO_DOWN' TEXT,"\
                "'SMART_LTB' TEXT,"\
                "'SMART_CRC' TEXT,"\
                "PRIMARY KEY(SN, 'RESULT_PASS'),"\
                "FOREIGN KEY('WO_ID') REFERENCES WO('ID'));"; 
    ret = hdb->exec(cmd);
    if(ret != 0)
    {
        return ret;
    }

    return ret;
}

bool HUDB::inserWO(const string& wo)
{
    int ret = 0;

    cmd = "insert into '" + woT + "'('pn_id', 'wo') values('" + pnId + "', '" + wo + "');";
    loopExec(cmd);

    cmd = "SELECT ID FROM '" + woT + "' WHERE WO='" + wo + "';" ;
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() == 0)
    {
        cout << "insert WO failed." << endl;
        return false;
    }

    for(const auto& x : hdb->queryRet)
    {
        cout << "WO: "<< x.first << " = " << x.second << endl;
    }
    HUDB::woId = hdb->queryRet[0].second;
    return true;
}

bool HUDB::inserWO(const string& wo, const string& ver)
{
    int ret = 0;

    cmd = "insert into '" + woT + "'('pn_id', 'wo') values('" + pnId + "', '" + wo + "');";
    loopExec(cmd);

    cmd = "SELECT ID FROM '" + woT + "' WHERE WO='" + wo + "';" ;
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() == 0)
    {
        cout << "insert WO failed." << endl;
        return false;
    }

    for(const auto& x : hdb->queryRet)
    {
        cout << "WO: "<< x.first << " = " << x.second << endl;
    }
    HUDB::woId = hdb->queryRet[0].second;
    cmd = "UPDATE '" + woT + "' SET VERSION='" + ver + "' WHERE WO='" + wo + "' AND PN_ID='" + pnId + "';";
    loopExec(cmd);
    return true;
}

bool HUDB::inserPN(const string& pn)
{
    int ret = 0;
    cmd = "INSERT INTO '" + pnT + "'('PN') VALUES('" + pn + "');";
    loopExec(cmd);

    cmd = "SELECT ID FROM '" + pnT + "' WHERE PN='" + pn + "';" ;
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() == 0)
    {
        cout << "insert PN failed." << endl;
        return false;
    }

    for(const auto& x : hdb->queryRet)
    {
        cout << "PN: "<< x.first << " = " << x.second << endl;
    }
    HUDB::pnId = hdb->queryRet[0].second;
    return true;
}


bool HUDB::insertSN(const string& sn, const string& state)
{
    cmd = "INSERT INTO '" + logT + "'('WO_ID', 'SN', 'STATE') VALUES('" + woId + "', '" + sn + "', '" + state + "');";
    loopExec(cmd);
    cmd = "SELECT SN FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND STATE='" + state + "' AND SN='" + sn + "';";
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() > 1)
    {
        cout << "sn check size > 2." << endl;
        for(const auto& x : hdb->queryRet)
        {
            cout << x.first << " = " << x.second << endl;
        }
        return false;
    }
    else
        return true;
}

bool HUDB::insertSN(const string& sn, const string& state, const string& hn)
{
    cmd = "SELECT SN FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND STATE='" + state + "' AND SN='" + sn + "';";
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() != 0)
    {
        cout << "sn check size != 0." << endl;
        for(const auto& x : hdb->queryRet)
        {
            cout << x.first << " = " << x.second << endl;
        }
        return false;
    }
    else
    {
        cmd = "INSERT INTO '" + logT + "'('WO_ID', 'SN', 'STATE', 'HOST_NAME') VALUES('" + woId + "', '" + sn + "', '" + state + "', '" + hn + "');";
        loopExec(cmd);
        return true;
    }
}


bool HUDB::delSN(const string& sn, const string& state)
{
    cmd = "DELETE FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + state + "';";
    loopExec(cmd);
    cmd = "SELECT SN FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND STATE='" + state + "' AND SN='" + sn + "';";
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() != 0)
    {
        cout << "delete sn failed." << endl;
        for(const auto& x : hdb->queryRet)
        {
            cout << x.first << " = " << x.second << endl;
        }
        return false;
    }
    else
        return true;
}

bool HUDB::delSN(const string& sn, const string& state, const string& hn)
{
    cmd = "DELETE FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + state + "' AND HOST_NAME='" + hn + "';";
    loopExec(cmd);
    cmd = "SELECT SN FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND STATE='" + state + "' AND SN='" + sn + "' AND HOST_NAME='" + hn + "';";
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() != 0)
    {
        cout << "delete sn failed." << endl;
        for(const auto& x : hdb->queryRet)
        {
            cout << x.first << " = " << x.second << endl;
        }
        return false;
    }
    else
        return true;
}

bool HUDB::updateFailSn(const string&sn, const string& state, const string& dutTime, const string& hn, const string& portNum)
{
    cmd = "UPDATE '" + logT + "' SET STATE='" + state + "', TIME_DATE='" + dutTime + "', HOST_NAME='" + hn + "', PORT_NUM='" + portNum + "'"\
          " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
    loopExec(cmd);
    cmd = "SELECT SN FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + state + "' AND TIME_DATE='" + dutTime + "' AND HOST_NAME='" + hn + "' AND PORT_NUM='" + portNum + "';";
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() == 0)
    {
        cout << "update fail sn failed." << endl;
        return false;
    }
    else
        return true;
}

bool HUDB::updateLog(const string& sn, const string& log, const string& mode)
{
    if(mode.find("FIO300") != string::npos)
    {
        vector<string> fioVal;
        string buf{log};
        size_t f = 0;
        for(int i = 0; i < 4; ++i)
        {
            f = buf.find(',');
            fioVal.push_back(buf.substr(0, f));
            buf = buf.substr(f + 1);
        }

        for(auto x : fioVal)
            cout << x << " | ";
        cout << "\n";

        if(mode == HUPROJ::FIO300N)
        {
            cmd = "UPDATE '" + logT + "'"\
                    " SET '4KR_25'='" + fioVal[0] + "', '4KW_25'='" + fioVal[1] + "', '512KR_25'='" + fioVal[2] + "', '512KW_25'='" + fioVal[3] + "'"\
                    " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
            loopExec(cmd);
        }
        else if(mode == HUPROJ::FIO300L)
        {
            cmd = "UPDATE '" + logT + "'"\
                    " SET '4KR_N5'='" + fioVal[0] + "', '4KW_N5'='" + fioVal[1] + "', '512KR_N5'='" + fioVal[2] + "', '512KW_N5'='" + fioVal[3] + "'"\
                    " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
            loopExec(cmd);
        }
        else if(mode == HUPROJ::FIO300H)
        {
            cmd = "UPDATE '" + logT + "'"\
                    " SET '4KR_50'='" + fioVal[0] + "', '4KW_50'='" + fioVal[1] + "', '512KR_50'='" + fioVal[2] + "', '512KW_50'='" + fioVal[3] + "'"\
                    " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
            loopExec(cmd);
        }
    }
    else if(mode == HUPROJ::DIO50H)
    {
        cmd = "UPDATE '" + logT + "'"\
                " SET 'DIO_50'='" + log + "'"\
                " WHERE WO_ID= '" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
        loopExec(cmd);
    }
    else if(mode == HUPROJ::DIO50L)
    {
        cmd = "UPDATE '" + logT + "'"\
                " SET 'DIO_N5'='" + log + "'"\
                " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
        loopExec(cmd);
    }
    else if(mode == HUPROJ::DIOD4)
    {
        cmd = "UPDATE '" + logT + "'"\
                " SET 'DIO_DOWN'='" + log + "'"\
                " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
        loopExec(cmd);
    }
    else if(mode == HUPROJ::DIOU4)
    {
        cmd = "UPDATE '" + logT + "'"\
                " SET 'DIO_UP'='" + log + "'"\
                " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
        loopExec(cmd);
    }
    else if(mode == HUPROJ::FIO3360)
    {
        cmd = "UPDATE '" + logT + "'"\
                " SET 'SMART_LTB'='" + log.at(0) + "', 'SMART_CRC'='" + log.at(1) + "'"\
                " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
        loopExec(cmd);
    }

    return true;
}


bool HUDB::updatePassLog(const string& sn, const string& dutTime, const string& hn, const string& portNum)
{
    cmd = "UPDATE '" + logT + "'"\
            " SET 'RESULT_PASS'='" + HUPROJ::PASSED + "', 'STATE'='" + HUPROJ::PASSED + "', 'TIME_DATE'='" + dutTime + "',"\
            " 'HOST_NAME'='" + hn + "', 'PORT_NUM'='" + portNum + "'"\
            " WHERE WO_ID='" + woId + "' AND SN='" + sn + "' AND STATE='" + HUPROJ::TESTING + "';";
    if(loopExec(cmd) != 0)
    {
        cout << "update final log failed." << endl;
        return false;
    }
    else
        return true;
}

bool HUDB::queryPassedSn(const string& sn)
{
    cmd = "SELECT SN FROM '" + logT + "' WHERE WO_ID='" + woId + "' AND STATE='" + HUPROJ::PASSED + "' AND SN='" + sn + "' AND RESULT_PASS='" + HUPROJ::PASSED + "';";
    hdb->queryRet.clear();
    hdb->exec(cmd);

    if(hdb->queryRet.size() != 0)
    {
        cout << "sn has passed." << endl;
        for(const auto& x : hdb->queryRet)
        {
            cout << x.first << " = " << x.second << endl;
        }
        return false;
    }
    else
        return true;
}









#endif  // SQLITE3API_CPP_INCLUDED
