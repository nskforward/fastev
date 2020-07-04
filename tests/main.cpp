#include <iostream>
#include "byte_buffer.hpp"
#include "testing_driver.hpp"

using namespace std;
using namespace fastev;

int main()
{
    unittest_welcome();
    unittest_component("core/byte_buffer", test_byte_buffer);
    unittest_success();
}