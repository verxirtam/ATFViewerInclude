/*
 * =====================================================================================
 *
 *       Filename:  DBAccessor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月13日 01時59分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once

#include <iostream>

#include <string>
#include <sqlite3.h>

class DBAccessor
{
private:
	std::string dbFileName;
	sqlite3 *db;
	sqlite3_stmt* stmt;
	//ポインタメンバを持つがコピーは不要なので下記は禁止する
	//(宣言のみして定義しない)
	DBAccessor(const DBAccessor&) = delete;//コピーコンストラクタ
	DBAccessor& operator=(const DBAccessor&) = delete;//コピー代入演算子
	DBAccessor(DBAccessor&&) = delete;//ムーブコンストラクタ
	DBAccessor& operator=(DBAccessor&&) = delete;//ムーブ代入演算子
public:
	DBAccessor(const std::string& dbfilename):dbFileName(dbfilename),db(NULL),stmt(NULL)
	{
		int r = sqlite3_open(dbFileName.c_str(),&db);
		if(SQLITE_OK != r)
		{
			//open失敗
			std::cout<<"DBへの接続に失敗しました。"<<std::endl;
			sqlite3_close(db);
			db = NULL;
		}
	}
	~DBAccessor()
	{
		sqlite3_finalize(stmt);
		
		sqlite3_close(db);
		db = NULL;
	}
	void setQuery(const std::string& sql)
	{
		//予めstmtを開放しておく
		sqlite3_finalize(stmt);
		//sql文字列からstmtを生成
		sqlite3_prepare(db, sql.c_str(), -1, &stmt,NULL);
		//stmtの内部バッファを一旦クリア
		sqlite3_reset(stmt);
	}
	void resetStmt()
	{
		sqlite3_reset(stmt);
	}
	void bindInt(int i, int d)
	{
		sqlite3_bind_int(stmt, i, d);
	}
	void bindLongLong(int i, long long d)
	{
		sqlite3_bind_int64(stmt, i, d);
	}
	void bindDouble(int i, double d)
	{
		sqlite3_bind_double(stmt, i, d);
	}
	void bindString(int i, const std::string& s)
	{
		sqlite3_bind_text(stmt, i, s.c_str(), -1, SQLITE_TRANSIENT);
	}
	
	void begin()
	{
		sqlite3_exec(db, "BEGIN", NULL, NULL, NULL);
	}
	void step()
	{
		while(sqlite3_step(stmt) == SQLITE_BUSY);
	}
	void commit()
	{
		sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
	}
	int step_select()
	{
		return sqlite3_step(stmt);
	}
	int getColumnInt(int i)
	{
		return sqlite3_column_int(stmt,i);
	}
	long long getColumnLongLong(int i)
	{
		return (long long)sqlite3_column_int64(stmt,i);
	}
	double getColumnDouble(int i)
	{
		return sqlite3_column_double(stmt,i);
	}
	std::string getColumnString(int i)
	{
		return std::string((const char*)sqlite3_column_text(stmt,i));
	}

	std::string execQuery(const std::string& query)
	{
		char* err = NULL;
		sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
		if(err == NULL)
		{
			return std::string("");
		}
		return std::string(err);
	}
};


