#include "test_utils.h"

namespace TinySTL {
namespace Test {
int CountLife::ctor_cnt = 0;
int CountLife::dtor_cnt = 0;
int CountLife::copy_ctor_cnt = 0;
int CountLife::copy_assignment_cnt = 0;
}
}
