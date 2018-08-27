#pragma once

template <typename T>
class AccumulationTraits;

template<>
class AccumulationTraits<char>
{
public:
	typedef int AccT;

public:
	static
	AccT
	zero()
	{
		return 0;
	}
};

template<>
class AccumulationTraits<int>
{
public:
	typedef long AccT;

public:
	static
	AccT
	zero()
	{
		return 0;
	}
};
