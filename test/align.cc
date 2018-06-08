#undef NDEBUG
#include <gtest/gtest.h>
#include <VecCore/VecCore>

#include <array>
#include <memory>
#include <vector>

using namespace testing;
using namespace vecCore;

template<class Backend>
using VcTypes = Types
  <
    typename Backend::Float_v,
    typename Backend::Double_v,
    typename Backend::Int16_v,
    typename Backend::UInt16_v,
    typename Backend::Int32_v,
    typename Backend::UInt32_v
  >;

template<class Backend>
using UMESimdTypes = Types
  <
    typename Backend::Float_v,
    typename Backend::Double_v,
    typename Backend::Int16_v,
    typename Backend::UInt16_v,
    typename Backend::Int32_v,
    typename Backend::UInt32_v,
    typename Backend::Int64_v,
    typename Backend::UInt64_v
  >;

template<class Backend>
using AgnerAVXTypes = Types
  <
    typename Backend::Float_v,
    typename Backend::Double_v,
    typename Backend::Int16_v,
    typename Backend::UInt16_v,
    typename Backend::Int32_v,
    typename Backend::UInt32_v,
    typename Backend::Int64_v,
    typename Backend::UInt64_v
  >;

template<class Backend>
using AgnerAVX512Types = Types
  <
    typename Backend::Float_v,
    typename Backend::Double_v,
    typename Backend::Int32_v,
    typename Backend::UInt32_v,
    typename Backend::Int64_v,
    typename Backend::UInt64_v
  >;

template<class T>
class AlignmentTest : public Test {
public:
  using Scalar_t = typename vecCore::Scalar<T>;
  using Vector_t = T;
};

TYPED_TEST_CASE_P(AlignmentTest);

inline bool
is_aligned(const void *__restrict__ ptr, size_t align)
{
  return (uintptr_t)ptr % align == 0;
}

TYPED_TEST_P(AlignmentTest, Stack)
{
  using Vector_t = typename TestFixture::Vector_t;

  Vector_t v;

  EXPECT_TRUE(is_aligned(&v, VECCORE_SIMD_ALIGN));
}

TYPED_TEST_P(AlignmentTest, Heap)
{
  using Vector_t = typename TestFixture::Vector_t;

  Vector_t *v = new Vector_t();

  EXPECT_TRUE(is_aligned(v, VECCORE_SIMD_ALIGN));
}

TYPED_TEST_P(AlignmentTest, StdArray)
{
  using Vector_t = typename TestFixture::Vector_t;

  std::array<Vector_t, 8> v;

  EXPECT_TRUE(is_aligned(std::addressof(v), VECCORE_SIMD_ALIGN));
}

#if 0
TYPED_TEST_P(AlignmentTest, StdVector)
{
  using Vector_t = typename TestFixture::Vector_t;

  std::vector<Vector_t> v(8);

  EXPECT_TRUE(is_aligned(std::addressof(v), VECCORE_SIMD_ALIGN));
}

TYPED_TEST_P(AlignmentTest, Collection)
{
  using Vector_t = typename TestFixture::Vector_t;

  std::vector<std::array<Vector_t, 8>> v(8);

  EXPECT_TRUE(is_aligned(std::addressof(v), VECCORE_SIMD_ALIGN));
}

REGISTER_TYPED_TEST_CASE_P(AlignmentTest, Stack, Heap, StdArray, StdVector, Collection);
#endif
REGISTER_TYPED_TEST_CASE_P(AlignmentTest, Stack, Heap, StdArray);

#define TEST_BACKEND_P(name, types, x) \
  INSTANTIATE_TYPED_TEST_CASE_P(name, AlignmentTest, types<vecCore::backend::x>)

/*
 * Notes:
 *  - Scalar backends do not require strict alignment
 *  - UME::SIMD does not promise its types are aligned
 */

#if defined(VECCORE_ENABLE_VC)
TEST_BACKEND_P(VcVector, VcTypes, VcVector);
TEST_BACKEND_P(VcSimdArray, VcTypes, VcSimdArray<16>);
#endif

/* To be turned on when UME::SIMD has aligned types */

#if 0 && defined(VECCORE_ENABLE_UMESIMD)
TEST_BACKEND_P(UMESimd, UMESimdTypes, UMESimd);
TEST_BACKEND_P(UMESimdArray, UMESimdTypes, UMESimdArray<16>);
#endif

#if defined(VECCORE_ENABLE_AGNER)
TEST_BACKEND_P(AgnerAVX, AgnerAVXTypes, AgnerAVX);
TEST_BACKEND_P(AgnerAVX512, AgnerAVX512Types, AgnerAVX512);
#endif

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
