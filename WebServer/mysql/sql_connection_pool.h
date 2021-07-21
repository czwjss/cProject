/*
 * @Author: czw
 * @Date: 2021-07-21 09:45:55
 * @LastEditors: czw
 * @LastEditTime: 2021-07-21 12:42:16
*/

#ifndef _CONNECTION_POOL_
#define _CONNECTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"

using namespace std;

//数据库池
class  connection_pool
{
public:

	MYSQL *GetConnection();				 //获取数据库连接池中的一个可用连接

	bool ReleaseConnection(MYSQL *conn); //释放连接归还数据库连接池

	int GetFreeConn();					 //获取连接可用的连接数量

	void DestroyPool();					 //销毁所有数据库连接池

	//单例模式
	static connection_pool *GetInstance();

    //数据库连接池初始化
    //主机地址 数据库端口号 数据库用户 数据库密码 数据库名称 日志开关
	void init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log); 

private:
    //单列模式 私有化构造析构函数
	connection_pool();
	~connection_pool();

	int m_MaxConn;  //最大连接数
	int m_CurConn;  //当前已使用的连接数
	int m_FreeConn; //当前空闲的连接数
    //互斥锁
	locker lock;
	list<MYSQL *> connList; //连接池
	sem reserve;   //信号量

public:
	string m_url;		 //主机地址
	string m_Port;		 //数据库端口号

	string m_User;		 //登陆数据库用户名
	string m_PassWord;	 //登陆数据库密码

	string m_DatabaseName; //使用数据库名
	int m_close_log;	//日志开关
};

//RALL 封装
class connectionRAII{

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII;
	connection_pool *poolRAII;
};

#endif
