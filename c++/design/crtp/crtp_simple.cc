#include <iostream>

template <typename Child>
struct Base
{
    void interface()
    {
        static_cast<const Child*>(this)->implementation();
    }
};

struct Derived : Base<Derived>
{
    void implementation() const
    {
        std::cout << "Derived implementation\n";
    }
};

int main()
{
    Derived d;
    d.interface();  // Prints "Derived implementation"
}
