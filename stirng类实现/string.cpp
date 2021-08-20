/*
 * @Author: czw
 * @Date: 2021-08-20 21:46:45
 * @LastEditors: czw
 * @LastEditTime: 2021-08-20 22:32:19
 */
#include <iostream>
#include <string.h>
using namespace std;

//string 类实现
class String
{
private:
    char *s;

public:
    //默认构造函数
    String()
    {
        s = new char[1];
        s[0] = '\0';
    }
    //拷贝构造函数
    String(const char *str)
    {
        //判断空字符串
        if (str == nullptr)
        {
            s = new char[1];
            s[0] = '\0';
        }
        else
        {
            int len = strlen(str) + 1;
            s = new char[len];
            strcpy(s, str);
        }
    }
    //拷贝对象构造
    String(const String &str)
    {
        int len=strlen(str.s)+1;
        s=new char[len];
        strcpy(s,str.s);
    }
    //深copy
    String &operator=(const String &str)
    {
        //先自检
        if(this==&str)
            return *this;
        //释放堆区内存
        int len =strlen(str.s)+1;
        delete [] s;
        s=new char[len];
        strcpy(s,str.s);
        //返回本对象
        return *this;
    }
    //析构函数
    ~String()
    {
        if(s!=nullptr)
        {
            cout<<"析构字符串："<<s<<endl;
            delete [] s;
            s=nullptr;
        }
    }
    //获取函数
    char * gets()
    {
        return s;
    }
};

int main()
{
    //测试
    String s1("hhh");
    String s2("czwjss");
    String s3(s2);
    s2=s1;
    s1.~String();
    cout<<"s2:"<<s2.gets()<<endl;
    cout<<"s3:"<<s3.gets()<<endl;

    return 0;
}