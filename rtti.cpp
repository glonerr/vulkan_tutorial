#include <iostream>
using namespace std;
//基类
class Top
{
public:
    void func() { cout << "Top" << endl; }

protected:
    int top = 1;
};
//派生类
class Left : virtual public Top
{
public:
    void func() { cout << "Left" << endl; }

private:
    int left = 3;
};
//派生类
class Right : virtual public Top
{
public:
    void func() { cout << "Right" << endl; }

private:
    int right = 3;
};

class Bottom : public Left, public Right
{
public:
    void func() { cout << "Bottom" << endl; }

private:
    int bottom = 4;
};

int main()
{
    Top t;
    Left l;
    Right r;
    Bottom b;
    return 0;
}