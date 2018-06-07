#ifndef VECCORE_BACKEND_AGNER_VECTORCLASS_H
#define VECCORE_BACKEND_AGNER_VECTORCLASS_H

#define VECCORE_ENABLE_AGNER 1

#define VCL_NAMESPACE vcl
#define MAX_VECTOR_SIZE 512
#include "vectorclass/vectorclass.h"
#include "vectorclass/vectormath_exp.h"
#include "vectorclass/vectormath_trig.h"

#include <cstdint>

namespace vecCore {

#define AGNER_IMPL_TRAIT_BOOL(TYPE)                                            \
  template <> struct TypeTraits<TYPE> {                                        \
    using IndexType = size_t;                                                  \
    using ScalarType = Bool_s;                                                 \
  };

// AVX or AVX2
AGNER_IMPL_TRAIT_BOOL(vcl::Vec4db);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec8fb);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec4qb);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec8ib);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec16sb);

template <> struct TypeTraits<vcl::Vec4d> {
  using ScalarType = double;
  using MaskType = vcl::Vec4db;
  using IndexType = vcl::Vec4q;
};

template <> struct TypeTraits<vcl::Vec8f> {
  using ScalarType = float;
  using MaskType = vcl::Vec8fb;
  using IndexType = vcl::Vec8i;
};

template <> struct TypeTraits<vcl::Vec8i> {
  using ScalarType = int32_t;
  using MaskType = vcl::Vec8ib;
  using IndexType = vcl::Vec8i;
};

template <> struct TypeTraits<vcl::Vec16s> {
  using ScalarType = int16_t;
  using MaskType = vcl::Vec16sb;
  using IndexType = vcl::Vec16s;
};

template <> struct TypeTraits<vcl::Vec4q> {
  using ScalarType = int64_t;
  using MaskType = vcl::Vec4qb;
  using IndexType = vcl::Vec4q;
};

template <> struct TypeTraits<vcl::Vec8ui> {
  using ScalarType = uint32_t;
  using MaskType = vcl::Vec8ib;
  using IndexType = vcl::Vec8i;
};

template <> struct TypeTraits<vcl::Vec16us> {
  using ScalarType = uint16_t;
  using MaskType = vcl::Vec16sb;
  using IndexType = vcl::Vec16s;
};

template <> struct TypeTraits<vcl::Vec4uq> {
  using ScalarType = uint64_t;
  using MaskType = vcl::Vec4qb;
  using IndexType = vcl::Vec4q;
};

// AVX512
AGNER_IMPL_TRAIT_BOOL(vcl::Vec8db);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec16fb);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec8qb);
AGNER_IMPL_TRAIT_BOOL(vcl::Vec16ib);

template <> struct TypeTraits<vcl::Vec8d> {
  using ScalarType = double;
  using MaskType = vcl::Vec8db;
  using IndexType = vcl::Vec8q;
};

template <> struct TypeTraits<vcl::Vec16f> {
  using ScalarType = float;
  using MaskType = vcl::Vec16fb;
  using IndexType = vcl::Vec16i;
};

template <> struct TypeTraits<vcl::Vec16i> {
  using ScalarType = int32_t;
  using MaskType = vcl::Vec16ib;
  using IndexType = vcl::Vec16i;
};

template <> struct TypeTraits<vcl::Vec16ui> {
  using ScalarType = uint32_t;
  using MaskType = vcl::Vec16ib;
  using IndexType = vcl::Vec16i;
};

template <> struct TypeTraits<vcl::Vec8q> {
  using ScalarType = int64_t;
  using MaskType = vcl::Vec8qb;
  using IndexType = vcl::Vec8q;
};

template <> struct TypeTraits<vcl::Vec8uq> {
  using ScalarType = uint64_t;
  using MaskType = vcl::Vec8qb;
  using IndexType = vcl::Vec8q;
};

namespace backend {

class AgnerAVX {
public:
  using Real_v = vcl::Vec4d;
  using Float_v = vcl::Vec8f;
  using Double_v = vcl::Vec4d;

  using Int_v = vcl::Vec8i;
  using Int16_v = vcl::Vec16s;
  using Int32_v = vcl::Vec8i;
  using Int64_v = vcl::Vec4q;

  using UInt_v = vcl::Vec8ui;
  using UInt16_v = vcl::Vec16us;
  using UInt32_v = vcl::Vec8ui;
  using UInt64_v = vcl::Vec4uq;
};

class AgnerAVX512 {
public:
  using Real_v = vcl::Vec8d;
  using Float_v = vcl::Vec16f;
  using Double_v = vcl::Vec8d;

  using Int_v = vcl::Vec16i;
  using Int32_v = vcl::Vec16i;
  using Int64_v = vcl::Vec8q;

  using UInt_v = vcl::Vec16ui;
  using UInt32_v = vcl::Vec16ui;
  using UInt64_v = vcl::Vec8uq;
};

} // namespace backend

#define INDEX_IMPL_AGNER_BOOL(TYPE)                                            \
  template <> VECCORE_FORCE_INLINE Bool_s MaskEmpty(const TYPE &mask) {        \
    return vcl::horizontal_and(!mask);                                         \
  }                                                                            \
  template <> VECCORE_FORCE_INLINE Bool_s MaskFull(const TYPE &mask) {         \
    return vcl::horizontal_and(mask);                                          \
  }                                                                            \
                                                                               \
  template <> struct IndexingImplementation<TYPE> {                            \
    using M = TYPE;                                                            \
    static inline Bool_s Get(const M &mask, size_t i) {                        \
      return mask.extract(i);                                                  \
    }                                                                          \
    static inline void Set(M &mask, size_t i, const Bool_s val) {              \
      mask.insert(i, val);                                                     \
    }                                                                          \
  };

INDEX_IMPL_AGNER_BOOL(vcl::Vec4db)
INDEX_IMPL_AGNER_BOOL(vcl::Vec8fb)
INDEX_IMPL_AGNER_BOOL(vcl::Vec4qb)
INDEX_IMPL_AGNER_BOOL(vcl::Vec8ib)
INDEX_IMPL_AGNER_BOOL(vcl::Vec16sb)

INDEX_IMPL_AGNER_BOOL(vcl::Vec8db)
INDEX_IMPL_AGNER_BOOL(vcl::Vec16fb)
INDEX_IMPL_AGNER_BOOL(vcl::Vec8qb)
INDEX_IMPL_AGNER_BOOL(vcl::Vec16ib)

#define LOADSTORE_IMPL_AGNER(TYPE)                                             \
  template <> struct LoadStoreImplementation<TYPE> {                           \
    using V = TYPE;                                                            \
    template <typename S = Scalar<V>>                                          \
    static inline void Load(V &v, S const *ptr) {                              \
      v.load(ptr);                                                             \
    }                                                                          \
                                                                               \
    template <typename S = Scalar<V>>                                          \
    static inline void Store(V const &v, S *ptr) {                             \
      v.store(ptr);                                                            \
    }                                                                          \
  };

LOADSTORE_IMPL_AGNER(vcl::Vec4d);
LOADSTORE_IMPL_AGNER(vcl::Vec8f);
LOADSTORE_IMPL_AGNER(vcl::Vec4q);
LOADSTORE_IMPL_AGNER(vcl::Vec4uq);
LOADSTORE_IMPL_AGNER(vcl::Vec8i);
LOADSTORE_IMPL_AGNER(vcl::Vec8ui);
LOADSTORE_IMPL_AGNER(vcl::Vec16s);
LOADSTORE_IMPL_AGNER(vcl::Vec16us);

LOADSTORE_IMPL_AGNER(vcl::Vec8d);
LOADSTORE_IMPL_AGNER(vcl::Vec16f);
LOADSTORE_IMPL_AGNER(vcl::Vec8q);
LOADSTORE_IMPL_AGNER(vcl::Vec8uq);
LOADSTORE_IMPL_AGNER(vcl::Vec16i);
LOADSTORE_IMPL_AGNER(vcl::Vec16ui);

// template <typename T>
// struct LoadStoreImplementation<Vc::Mask<T>> {
// using M = Vc::Mask<T>;
//
// template <typename S = Scalar<T>>
// static inline void Load(M &mask, Bool_s const *ptr)
//{
// mask.load(ptr);
//}
//
// template <typename S = Scalar<T>>
// static inline void Store(M const &mask, S *ptr)
//{
// mask.store(ptr);
//}
//};

#define MASKING_IMPL_AGNER(TYPE)                                               \
  template <> struct MaskingImplementation<TYPE> {                             \
    using M = vecCore::TypeTraits<TYPE>::MaskType;                             \
    using V = TYPE;                                                            \
                                                                               \
    static inline void Assign(V &dst, M const &mask, V const &src) {           \
      dst = vcl::select(mask, src, dst);                                       \
    }                                                                          \
                                                                               \
    static inline void Blend(V &dst, M const &mask, V const &src1,             \
                             V const src2) {                                   \
      dst = vcl::select(mask, src1, src2);                                     \
    }                                                                          \
  };

MASKING_IMPL_AGNER(vcl::Vec4d);
MASKING_IMPL_AGNER(vcl::Vec8f);
MASKING_IMPL_AGNER(vcl::Vec4q);
MASKING_IMPL_AGNER(vcl::Vec4uq);
MASKING_IMPL_AGNER(vcl::Vec8i);
MASKING_IMPL_AGNER(vcl::Vec8ui);
MASKING_IMPL_AGNER(vcl::Vec16s);
MASKING_IMPL_AGNER(vcl::Vec16us);

MASKING_IMPL_AGNER(vcl::Vec8d);
MASKING_IMPL_AGNER(vcl::Vec16f);
MASKING_IMPL_AGNER(vcl::Vec8q);
MASKING_IMPL_AGNER(vcl::Vec8uq);
MASKING_IMPL_AGNER(vcl::Vec16i);
MASKING_IMPL_AGNER(vcl::Vec16ui);

namespace math {

#define FLOATMATH_IMPL_AGNER(TYPE)                                             \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Sqrt(const TYPE &x) { return vcl::sqrt(x); }                            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Cbrt(const TYPE &x) { return vcl::cbrt(x); }                            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Exp(const TYPE &x) { return vcl::exp(x); }                              \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Log(const TYPE &x) { return vcl::log(x); }                              \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Tan(const TYPE &x) { return vcl::tan(x); }                              \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Sin(const TYPE &x) { return vcl::sin(x); }                              \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Cos(const TYPE &x) { return vcl::cos(x); }                              \
  VECCORE_FORCE_INLINE                                                         \
  void SinCos(const TYPE &x, TYPE *s, TYPE *c) { *s = vcl::sincos(c, x); }     \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Pow(const TYPE &x, const TYPE &y) { return vcl::pow(x, y); }            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE ATan(const TYPE &x) { return vcl::atan(x); }                            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE ATan2(const TYPE &x, const TYPE &y) { return vcl::atan2(x, y); }        \
  VECCORE_FORCE_INLINE                                                         \
  TYPE ACos(const TYPE &x) { return vcl::acos(x); }                            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE ASin(const TYPE &x) { return vcl::asin(x); }                            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Abs(const TYPE &x) { return vcl::abs(x); }                              \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Floor(const TYPE &x) { return vcl::floor(x); }                          \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Ceil(const TYPE &x) { return vcl::ceil(x); }                            \
                                                                               \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Trunc(const TYPE &x) { return vcl::floor(x); }                          \
  VECCORE_FORCE_INLINE                                                         \
  TYPE CopySign(const TYPE &x, const TYPE &y) {                                \
    return vcl::sign_combine(x, y);                                            \
  }                                                                            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Max(const TYPE &x, const TYPE &y) { return vcl::max(x, y); }            \
  VECCORE_FORCE_INLINE                                                         \
  TYPE Min(const TYPE &x, const TYPE &y) { return vcl::min(x, y); }

FLOATMATH_IMPL_AGNER(vcl::Vec4d);
FLOATMATH_IMPL_AGNER(vcl::Vec8f);

FLOATMATH_IMPL_AGNER(vcl::Vec8d);
FLOATMATH_IMPL_AGNER(vcl::Vec16f);

} // namespace math

} // namespace vecCore

#endif
