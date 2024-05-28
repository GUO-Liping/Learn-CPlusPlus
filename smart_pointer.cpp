# include <iostream>
# include <vector>
# include <memory>

using namespace std;

class AA
{
    public:
    string aa_name;
    AA() {cout << aa_name << "调用构造函数AA()。 \n";}
    AA(const string & name): aa_name(name) {cout << "调用构造函数AA(" << aa_name << ")。 \n";}
    ~AA() {cout << "调用了析构函数~AA(" << aa_name << ")。 \n";}
};

int main()
{
    AA* p = new AA("小明");
    unique_ptr<AA> up1 (new AA("小明"));
    // unique_ptr<AA> up1(p);
    cout << "aa_name=" << (*up1).aa_name << endl;
    cout << "aa_name=" << up1 -> aa_name << endl;
    cout << "aa_name=" << (*p).aa_name << endl;
    cout << "aa_name=" << p -> aa_name << endl;

    cout << "裸指针的值" << p  << endl;
    cout << "裸指针的值" << &p  << endl;
    //cout << "裸指针的值" << *p  << endl;
    //cout << "unique指针的值" << *up1  << endl;
    cout << "unique指针.get()的值" << up1.get() << endl;
    cout << "unique指针的地址" << &up1 << endl;
    
    // delete p;
}
