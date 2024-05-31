# include <iostream>

int func_add(int a, int b)
{
    return a + b;
}

void increase_value(int value)
{
    value++;
}

void increase_pointer(int* value)
{
    (*value)++;
}

void increase_reference(int& value)
{
    value++;
}

void increase_ref_pointer(int*& value)
{
    (*value)++;
}

int main()
{
    int a = 1;
    int b = 2;
    int c = func_add(a, b);

    std::cout << "a+b=" << c << std::endl;

    increase_value(a);
    std::cout << "increase_value(a) = " << a << std::endl;

    increase_pointer(&a);
    std::cout << "increase_pointer(&a) = " << a << std::endl;

    increase_reference(a);
    std::cout << "increase_reference(a) = " << a << std::endl;

    int* ptr = &a;
    increase_ref_pointer(ptr);
    std::cout << "increase_ref_pointer(ptr) = " << a << std::endl;

    return 0;
}
