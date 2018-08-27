#include <iostream>

////////////////////////////////////////

template 
	< 
		typename T1 , 
		typename T2 
	>
class a
{
public:
	
	void print()
	{
		std::cout << "General." << std::endl;
	}
	
	void print2()
	{
		std::cout << "General2" << std::endl;
	}
};

////////////////////////////////////////

template 
	< 
		typename T1,
		typename T2
	>
class a < T1* , T2& >
{
public:
	
	void print()
	{
		std::cout << "Partial." << std::endl;
	}	

};

////////////////////////////////////////

template 
	< 
		typename T2
	>
class a < int , T2& >
{
public:
	
	void print()
	{
		std::cout << "Partial ref." << std::endl;
	}	

};

////////////////////////////////////////

int main()
{
	typedef a<int,int > general;
	typedef a<int*,int& > partial;
	typedef a<int,int& > partial_ref;
	
	general a1;
	partial a2;
	partial_ref a3;
	
	a1.print();
	a1.print2();
	a2.print();
	a3.print();
	
	return EXIT_SUCCESS;
}