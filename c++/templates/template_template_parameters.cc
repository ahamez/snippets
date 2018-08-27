#include <iostream>

////////////////////////////////////////////

template < typename T >
class a
{

private:

	T x_;

public:
	
	a()
		: 
		x_()
	{}
	
	void print()
	{
		std::cout << ++x_ << std::endl;
	}
};

////////////////////////////////////////////
template 
	<
		typename T,
		template 
			<
				typename DUMMY
			>
		class CLASS
	>
class b
{

private:

	CLASS<T> a_;

public:
	
	b()
		:
		a_()
	{}
		
	void process()
	{
		a_.print();
	}
};

////////////////////////////////////////////

int main()
{
	typedef b< double , a > my_type;
	
	my_type my_b;
	my_b.process();
	
	return 0;
}