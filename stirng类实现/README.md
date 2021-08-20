<!--
 * @Author: czw
 * @Date: 2021-08-20 22:48:02
 * @LastEditors: czw
 * @LastEditTime: 2021-08-20 22:59:09
-->
string 类 维护一个 char* str指针
普通构造     string ()
    默认构造
    str=new char[1];
    str[0]='\0';

拷贝构造    string(const string &s)
    获取被拷贝的string的长度  int len =
    堆区分配内存  str =new char[len]
    copy  strcpy(str,str2);

赋值函数    string& operator=(const string &s)
    自检 本对象指针是否等于目标对象  this==&str
    释放原堆区空间  delete [] str;   str指向一个字符串  所以是delete [] 而非delete
    申请新堆区空间大小
    copy 
    返回本对象 *this

析构函数    ~string()
    释放堆区空间    delete [] str;