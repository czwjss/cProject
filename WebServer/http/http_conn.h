/*
 * @Author: czw
 * @Date: 2021-07-21 12:02:54
 * @LastEditors: czw
 * @LastEditTime: 2021-07-21 13:02:35
 */


#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>

#include "../lock/locker.h"
#include "../mysql/sql_connection_pool.h"
#include "../time/lst_timer.h"
#include "../log/log.h"

class http_conn
{
public:
    static const int FILENAME_LEN = 200;
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;
    //请求类型
    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATH
    };
    //
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };
    //
    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    //
    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    //http连接构造
    http_conn() {}
    //析构
    ~http_conn() {}

public:
    //初始化  socket 
    void init(int sockfd, const sockaddr_in &addr, char *, int, int, string user, string passwd, string sqlname);
    //关闭http连接
    void close_conn(bool real_close = true);
    //
    void process();
    //读取浏览器发送的全部信息
    bool read_once();
    //响应报文写入
    bool write();

    //获取地址
    sockaddr_in *get_address()
    {
        return &m_address;
    }
    //初始化 数据库user读取表
    void initmysql_result(connection_pool *connPool);
    //定时器标志
    int timer_flag;

    int improv;


private:

    void init();
    //读取缓冲流 并处理请求报文
    HTTP_CODE process_read();
    //向写缓冲流中写入响应报文
    bool process_write(HTTP_CODE ret);
    //主状态机 解析请求报文行信息
    HTTP_CODE parse_request_line(char *text);
    //主状态机 解析请求报文头信息
    HTTP_CODE parse_headers(char *text);
    //主状态机 解析请求报文请求内容
    HTTP_CODE parse_content(char *text);
    //生成响应报文（处理请求—）
    HTTP_CODE do_request();

    char *get_line() { return m_read_buf + m_start_line; };
    //从状态机 获取一行分析内容属于那部分
    LINE_STATUS parse_line();

    void unmap();
    //响应报文生成函数 8部分
    bool add_response(const char *format, ...);
    bool add_content(const char *content);
    bool add_status_line(int status, const char *title);
    bool add_headers(int content_length);
    bool add_content_type();
    bool add_content_length(int content_length);
    bool add_linger();
    bool add_blank_line();

public:

    static int m_epollfd; 
    static int m_user_count; 
    MYSQL *mysql; //数据库连接
    int m_state;  //状态 读为0, 写为1

private:
    int m_sockfd;
    sockaddr_in m_address;
    //读最大 缓冲区
    char m_read_buf[READ_BUFFER_SIZE];
    //读长度
    int m_read_idx;
    //读的起始位置
    int m_checked_idx;
    //以解析的字符数
    int m_start_line;
    //写最大 缓冲区
    char m_write_buf[WRITE_BUFFER_SIZE];
    //写长度
    int m_write_idx;
    //主状态机状态
    CHECK_STATE m_check_state;
    //请求方法
    METHOD m_method;

    //解析请求的6变量
    char m_real_file[FILENAME_LEN];
    char *m_url;
    char *m_version;
    char *m_host;
    int m_content_length;
    bool m_linger;


    char *m_file_address; //服务器上的文件地址
    struct stat m_file_stat;
    struct iovec m_iv[2];
    int m_iv_count;
    int cgi;        //是否启用的POST
    char *m_string; //存储请求头数据
    int bytes_to_send; //剩余发送字节
    int bytes_have_send; //以发送字节
    char *doc_root;

    map<string, string> m_users; //用户表
    int m_TRIGMode;
    //关闭log 
    int m_close_log;

    //sql 用户名
    char sql_user[100];
    //sql 密码
    char sql_passwd[100];
    //sql 库名
    char sql_name[100];
};

#endif
