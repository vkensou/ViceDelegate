ViceDelegate
==========
##Introduction

This is a C++11 Delegate library. It is head file only. And it's different with std::function, it could compare with each other.

##Example

```
#include "ViceDelegate.hpp"
#include <iostream>
#include <vector>

using namespace std;

class A
{
public:
	A()
	{
		cout << "A::A()" << endl;
	}

	A(const A& a)
	{
		cout << "A::A(const A&)" << endl;
	}

	void foo(int a)
	{
		std::cout << "method got: " << a << std::endl;
	}
};

void foo(int a)
{
	std::cout << "function foo: " << a << std::endl;
}

struct KK
{
	void operator()(int a)
	{
		cout << "KK::operator(" << a << ")" << endl;
	}
};

int main()
{
	typedef Vice::Delegate<void(int)> D;
	D d1(&foo);
	D d2([](int x){cout << "lambda: " << x << endl; });
	D d3 = KK();
	D d4(&a, &A::foo);
	D d5(d4);
	D d6(&foo);
	D d7;
	d7 = D(&foo);

	d1(1);
	d2(2);
	d3(1);
	d4(123);
	d5(123);
	cout << (d1 == d6) << endl;
	cout << (d1 != d6) << endl;

	std::vector<D> vd;
	vd.emplace_back(d1);
	vd.emplace_back(d2);
	vd.emplace_back(d3);
	vd.emplace_back(d4);
	vd.emplace_back(d5);
	vd.emplace_back(d6);
	vd.emplace_back(d7);
	vd[0](1);
	vd[1](1);
	vd[2](1);
	vd[3](1);
	vd[4](1);
	vd[5](1);
	vd[6](1);

	swap(d1, d2);
	return 0;
}
```
