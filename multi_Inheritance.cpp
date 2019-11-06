#include <iostream>
using namespace std;

class Grandparent
{
public:
    virtual void grandparent_foo() {}
    int grandparent_data = 4;
};

class Parent1 : virtual public Grandparent
{
public:
    virtual void parent1_foo() {}
    int parent1_data = 1;
};

class Parent2 : virtual public Grandparent
{
public:
    virtual void parent2_foo() {}
    int parent2_data = 2;
};

class Child : public Parent1, public Parent2
{
public:
    virtual void child_foo() {}
    int child_data = 3;
};

int main()
{
    Child child;
}