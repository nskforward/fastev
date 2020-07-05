#ifndef TESTING_FRAMEWORK
#define TESTING_FRAMEWORK

#include <iostream>
#include "../core/exception.hpp"

using namespace std;
using namespace fastev;

void unittest_welcome()
{
    cout << "====================================================" << endl
         << "- - - - - - - - \e[93mSTART UNIT TESTING\e[0m - - - - - - - - -" << endl
         << "====================================================" << endl;
}

void unittest_fail()
{
    cout << "====================================================" << endl
         << "                       \033[31mFAIL\e[0m" << endl
         << "====================================================" << endl;

    exit(1);
}

void unittest_success()
{
    cout << "====================================================" << endl
         << "                       \e[92mSUCCESS\e[0m" << endl
         << "====================================================" << endl;
    exit(0);
}

void unittest_component(std::string name, void (*f)())
{
    cout << endl
         << "# " << name << endl
         << "----------------------------------------------------" << endl;
    try
    {
        f();
    }
    catch (exception &e)
    {
        cout << "ERROR: " << e.what() << endl;
        unittest_fail();
    }
}

void unittest_case_true(std::string name, bool condition)
{
    if (name.size() > 32)
    {
        throw KernelException("case size cannot be greater than 16 for case: %s", name.c_str());
    }
    cout << "- " << name;
    int padding = 32 - name.size();
    for (int i = 0; i < padding; i++)
    {
        cout << " ";
    }
    cout << "\t\t";
    if (!condition)
    {
        cout << "\033[31mFAIL\e[0m" << endl;
        unittest_fail();
    }
    cout << "\e[92mOK\e[0m" << endl;
}

#endif