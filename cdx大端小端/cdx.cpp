#include<iostream> 
using namespace std;
int main ()
{
	int a=0x123456;
	if((*(char*)(&a))==0x12)
		cout<<"大端";
	else
		cout<<"小端";
	cout<<endl;

	return 0;
}

