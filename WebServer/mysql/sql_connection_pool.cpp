/*
 * @Author: czw
 * @Date: 2021-07-21 12:31:28
 * @LastEditors: czw
 * @LastEditTime: 2021-07-21 13:31:21
 */

#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"

using namespace std;

//构造
connection_pool::connection_pool()
{
	m_CurConn = 0;
	m_FreeConn = 0;
}

//懒汉写法 静态 
connection_pool *connection_pool::GetInstance()
{
	static connection_pool connPool;
	return &connPool;
}


//初始化
void connection_pool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn, int close_log)
{
	m_url = url; //地址
	m_Port = Port; //sql 端口
	m_User = User;  //用户
	m_PassWord = PassWord; //密码
	m_DatabaseName = DBName; //db名
	m_close_log = close_log; //日志开关

    //循环创建数据库连接
	for (int i = 0; i < MaxConn; i++)
	{

		MYSQL *con = NULL;
		con = mysql_init(con);

		if (con == NULL)
		{
			LOG_ERROR("MySQL Error");
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

		if (con == NULL)
		{
			LOG_ERROR("MySQL Error");
			exit(1);
		}
        //加入数据库连接池
		connList.push_back(con);
        //++
		++m_FreeConn;
	}

    //设置信号量
	reserve = sem(m_FreeConn);
    //最大连接数
	m_MaxConn = m_FreeConn;
}


//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *connection_pool::GetConnection()
{
	MYSQL *con = NULL;

	if (0 == connList.size())
		return NULL;

    //信号wait
	reserve.wait();
	
	lock.lock();

	con = connList.front();
	connList.pop_front();

	--m_FreeConn;
	++m_CurConn;

	lock.unlock();
	return con;
}

//释放当前使用的连接
bool connection_pool::ReleaseConnection(MYSQL *con)
{
	if (NULL == con)
		return false;

	lock.lock();

	connList.push_back(con);
	++m_FreeConn;
	--m_CurConn;

	lock.unlock();

	reserve.post();
	return true;
}

//销毁数据库连接池
void connection_pool::DestroyPool()
{

	lock.lock();
	if (connList.size() > 0)
	{
		list<MYSQL *>::iterator it;
        //循环释放所有数据库连接
		for (it = connList.begin(); it != connList.end(); ++it)
		{
			MYSQL *con = *it;
			mysql_close(con);
		}
		m_CurConn = 0;
		m_FreeConn = 0;
		connList.clear();
	}

	lock.unlock();
}

//当前空闲的连接数
int connection_pool::GetFreeConn()
{
	return this->m_FreeConn;
}

//析构
connection_pool::~connection_pool()
{
	DestroyPool();
}

//RALL 机制封装sql 连接 
//RALL 间接获取sql 
//并且RALL 生存周期 在线程栈上 
//使用完毕自动调用析构函数回首资源
connectionRAII::connectionRAII(MYSQL **SQL, connection_pool *connPool){
	*SQL = connPool->GetConnection();
	
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}