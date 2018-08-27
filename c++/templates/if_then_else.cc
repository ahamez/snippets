#include <iostream>

/////////////////////////////////////////////////////////

template
<
	  bool Condition
	, typename Then
	, typename Else
>
struct IF
{
	typedef Then RET;
};


/////////////////////////////////////////////////////////

template
<
	  typename Then
	, typename Else
>
struct IF
<
	  false
	, Then
	, Else
>
{
	typedef Else RET;
};

/////////////////////////////////////////////////////////

class print_true
{
public:

	void
	operator()()
	{
		std::cout << "true" << std::endl;
	}
	
};

/////////////////////////////////////////////////////////

class print_false
{
public:

	void
	operator()()
	{
		std::cout << "false" << std::endl;
	}
	
};

/////////////////////////////////////////////////////////

int
main()
{
	IF< 1<2, print_true, print_false >::RET print;
	print();
	IF< 2/2 == 3, print_true, print_false >::RET print2;
	print2();
	
	return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////