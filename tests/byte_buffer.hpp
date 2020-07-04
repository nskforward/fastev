#include "../src/fastev/core/byte_buffer.hpp"
#include "testing_driver.hpp"

using namespace std;
using namespace fastev;

void test_byte_buffer()
{

    auto bb8 = ByteBuffer<8>();

    unittest_case_true("initial length", bb8.len() == 0);
    unittest_case_true("capacity", bb8.cap() == 8);

    bb8.append("aaa", 3);
    unittest_case_true("append 1", bb8.len() == 3);

    bb8.append("bbb", 3);
    unittest_case_true("append 2", bb8.len() == 6);

    bool catched = false;
    try
    {
        bb8.append("ccc", 3);
    }
    catch (KernelException &e)
    {
        catched = true;
    }
    unittest_case_true("overflow", catched);
    unittest_case_true("c_str", strcmp(bb8.c_str(), "aaabbb") == 0);

    bb8.reset();
    unittest_case_true("reset", bb8.len() == 0);

    bb8.append("aabbcc", 6);
    auto pos = bb8.str_pos((char *)"bb");
    unittest_case_true("strpos true", pos == 2);

    pos = bb8.str_pos((char *)"ac");
    unittest_case_true("strpos fales", pos == -1);
}