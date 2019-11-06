#include <iostream>
#include <string> 
using namespace std;

class Parent
{
public:
    virtual void Foo() {
        cout << "Class People：我正在吃饭，请不要跟我说话..." << endl;
    }
    virtual void FooNotOverridden() {}
};

class Derived : public Parent
{
public:
    void Foo() override {}
};

int main()
{
    Parent p1, p2;
    Derived d1, d2;
    p2.Foo();
    std::cout << "done" << std::endl;
}
