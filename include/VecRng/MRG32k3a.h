#ifndef VECRNG_MRG32K3A_H
#define VECRNG_MRG32K3A_H 1

/**
 * MRG32k3a: A SIMD/SIMT implementation of MRG32k3a based on RngStream.h(cpp)
 *
 * RngStream is a class generating multiple streams of random numbers created
 * by Prof. Pierre L'Ecuyer, University of Montreal (lecuyer@iro.umontreal.ca)
 * Original source codes of RngStream.h(cpp) is available at
 * http://www.iro.umontreal.ca/~lecuyer/myftp/streams00/c++/
 *
 * Relevant articles in which MRG32k3a and the package with multiple streams
 * were proposed:
 *
 * P. L'Ecuyer, ``Good Parameter Sets for Combined Multiple Recursive Random
 * Number Generators'', Operations Research, 47, 1 (1999), 159--164.
 *
 * P. L'Ecuyer, R. Simard, E. J. Chen, and W. D. Kelton, ``An Objected-Oriented
 * Random-Number Package with Many Long Streams and Substreams'', Operations
 * Research, 50, 6 (2002), 1073--1075
 */

#include "MRG.h"
#include "VecRNG.h"

#include <iostream>
#include <cstring>

namespace vecRng {
inline namespace VECRNG_IMPL_NAMESPACE {

// struct MRG32k3a_t (random state of MRG32k3a)

template <typename BackendT>
struct MRG32k3a_t {
  // fCg[row=MRG::vsize][column=VectorSize<Double_v>()]
  typename BackendT::Double_v fCg[MRG::vsize];
};

//class MRG32k3a<BackendT>

template <typename BackendT>
class MRG32k3a : public VecRNG<MRG32k3a<BackendT>, BackendT, MRG32k3a_t<BackendT> > {

private:
  static Real_t fSeed[MRG::vsize];
  typename BackendT::Double_v fBg[MRG::vsize];

  // Information on a stream: The arrays {Cg, Bg, Ig} (from the RngStream)
  // contain the current state of the stream, the starting state of the current
  // SubStream, and the starting state of the stream (not used in this class).
  // The next seed will be the seed of the next declared RngStream.

public:

  VECCORE_ATT_HOST_DEVICE
  MRG32k3a() {}

  VECCORE_ATT_HOST_DEVICE
  MRG32k3a(MRG32k3a_t<BackendT> *states)
    : VecRNG<MRG32k3a<BackendT>, BackendT, MRG32k3a_t<BackendT> >(states) {}

  VECCORE_ATT_HOST_DEVICE
  ~MRG32k3a() {}

  VECCORE_ATT_HOST_DEVICE
  MRG32k3a(const MRG32k3a &rng);

  // Static methods
  VECCORE_ATT_HOST
  VECCORE_FORCE_INLINE
  void Initialize() { SetNextStream(); }

  VECCORE_ATT_HOST
  VECCORE_FORCE_INLINE
  void Initialize(MRG32k3a_t<BackendT> *states, int blocks, int threads);

  // Returns pRNG<BackendT> between 0 and 1
  template <typename ReturnTypeBackendT>
  VECCORE_ATT_HOST_DEVICE
  VECCORE_FORCE_INLINE
  typename ReturnTypeBackendT::Double_v Kernel(MRG32k3a_t<BackendT>& state);

  // Auxiliary methods
  VECCORE_ATT_HOST
  VECCORE_FORCE_INLINE
  void AdvanceState(long e, long c);

  VECCORE_ATT_HOST_DEVICE
  void SetSeed(Real_t seed[MRG::vsize]);

  VECCORE_ATT_HOST_DEVICE
  Real_t* GetSeed() const { return fSeed; }

  VECCORE_ATT_HOST
  void PrintState() const ;

private:

  // the mother is friend of this
  friend class VecRNG<MRG32k3a<BackendT>, BackendT, MRG32k3a_t<BackendT> >;

  // Set Stream to NextStream/NextSubStream.
  VECCORE_ATT_HOST
  VECCORE_FORCE_INLINE
  void SetNextStream();

  VECCORE_ATT_HOST
  VECCORE_FORCE_INLINE
  void SetNextSubstream();

  // MRG32k3a utility methods
  VECCORE_ATT_HOST
  double MultModM(double a, double s, double c, double m);

  VECCORE_ATT_HOST
  void MatVecModM(const double A[MRG::ndim][MRG::ndim],
                  const double s[MRG::ndim], double v[MRG::ndim], double m);

  VECCORE_ATT_HOST
  void MatTwoPowModM(const double A[MRG::ndim][MRG::ndim], double B[MRG::ndim][MRG::ndim], double m, long e);

  VECCORE_ATT_HOST
  void MatPowModM(const double A[MRG::ndim][MRG::ndim], double B[MRG::ndim][MRG::ndim], double m, long n);

  VECCORE_ATT_HOST
  void MatMatModM (const double A[MRG::ndim][MRG::ndim], const double B[MRG::ndim][MRG::ndim], double C[MRG::ndim][MRG::ndim], double m);

  VECCORE_ATT_HOST
  void TransitionVector(double C1[MRG::ndim][MRG::ndim], double C2[MRG::ndim][MRG::ndim], double e, double c);

};

// The default seed of MRG32k3a
template <class BackendT>
Real_t MRG32k3a<BackendT>::fSeed[MRG::vsize] = {12345., 12345., 12345., 12345., 12345., 12345.};

//
// Class Implementation
//

// Copy constructor
template <typename BackendT>
VECCORE_ATT_HOST_DEVICE
MRG32k3a<BackendT>::MRG32k3a(const MRG32k3a<BackendT> &rng)
  : VecRNG<MRG32k3a<BackendT>, BackendT, MRG32k3a_t<BackendT> >()
{
  for(int i = 0 ; i < MRG::vsize ; ++i) {
    this->fState->fCg[i] = rng.fState->fCg[i];
    fSeed[i] = rng.fSeed[i];
    fBg[i] = rng.fBg[i];
  }
}

// Reset stream to the next Stream.
template <typename BackendT>
VECCORE_ATT_HOST
VECCORE_FORCE_INLINE
void MRG32k3a<BackendT>::SetNextStream()
{
  for(size_t i = 0 ; i < VectorSize<Double_v>() ; ++i) {
    for (int j = 0; j < MRG::vsize ; ++j) {
      this->fState->fCg[j][i] = fBg[j][i] = fSeed[j];
    }
    MatVecModM(MRG::A1p127, fSeed, fSeed, MRG::m1);
    MatVecModM(MRG::A2p127, &fSeed[3], &fSeed[3], MRG::m2);
  }
}

// Scalar specialization of SetNextStream
template <>
VECCORE_ATT_HOST
VECCORE_FORCE_INLINE
void MRG32k3a<ScalarBackend>::SetNextStream()
{
  for (int j = 0; j < MRG::vsize ; ++j) {
    this->fState->fCg[j] = fBg[j] = fSeed[j];
  }
  MatVecModM(MRG::A1p127, fSeed, fSeed, MRG::m1);
  MatVecModM(MRG::A2p127, &fSeed[3], &fSeed[3], MRG::m2);
}

// Reset stream to the next substream.
template <typename BackendT>
VECCORE_ATT_HOST
VECCORE_FORCE_INLINE
void MRG32k3a<BackendT>::SetNextSubstream()
{
  for(size_t i = 0 ; i < VectorSize<Double_v>() ; ++i) {
    for (int j = 0; j < MRG::vsize ; ++j) {
      this->fState->fCg[j][i] = fBg[j][i];
    }
    MatVecModM(MRG::A1p76, fBg, fBg, MRG::m1);
    MatVecModM(MRG::A2p76, &fBg[3], &fBg[3], MRG::m2);
  }
}

// Scalar specialization of SetNextSubstream
template <>
VECCORE_ATT_HOST
VECCORE_FORCE_INLINE
void MRG32k3a<ScalarBackend>::SetNextSubstream()
{
  for (int j = 0; j < MRG::vsize ; ++j) {
     this->fState->fCg[j] = fBg[j];
  }
  MatVecModM(MRG::A1p76, fBg, fBg, MRG::m1);
  MatVecModM(MRG::A2p76, &fBg[3], &fBg[3], MRG::m2);
}

// Specialization for the scalar backend to initialize an arrary of states of which size is [blocks*threads].
// "states" should be allocated beforehand, but can used for both host and device pointers
template <>
VECCORE_ATT_HOST
VECCORE_FORCE_INLINE
void MRG32k3a<ScalarBackend>::Initialize(MRG32k3a_t<ScalarBackend> *states, int blocks, int threads)
{
  MRG32k3a_t<ScalarBackend>* hstates
    = (MRG32k3a_t<ScalarBackend> *) malloc (blocks*threads*sizeof(MRG32k3a_t<ScalarBackend>));

  unsigned int nthreads = blocks * threads;
  for (unsigned int tid = 0 ; tid < nthreads ; ++tid) {
    SetNextStream();
    for(size_t j = 0 ; j < MRG::vsize ; ++j) {
      hstates[tid].fCg[j] = this->fState->fCg[j];
    }
  }
#ifdef VECCORE_CUDA
  cudaMemcpy(states, hstates, nthreads*sizeof(MRG32k3a_t<ScalarBackend>), cudaMemcpyHostToDevice);
#else
  memcpy(states, hstates, nthreads*sizeof(MRG32k3a_t<ScalarBackend>));
#endif
  free(hstates);
}

// Print information of the current state
template <typename BackendT>
VECCORE_ATT_HOST
void MRG32k3a<BackendT>::PrintState() const
{
  for(size_t j = 0 ; j < MRG::vsize ; ++j) {
    std::cout << this->fState->fCg[j] << std::endl;
  }
}

// Set the next seed
template <typename BackendT>
VECCORE_ATT_HOST_DEVICE
void MRG32k3a<BackendT>::SetSeed(Real_t seed[MRG::vsize])
{
  for(int i = 0 ; i < MRG::vsize ; ++i) fSeed[i] = seed[i];
}

// Kernel to generate the next random number(s) with ReturnTypeBackendT (based on RngStream::U01d)
template <class BackendT>
template <class ReturnTypeBackendT>
VECCORE_ATT_HOST_DEVICE
VECCORE_FORCE_INLINE
typename ReturnTypeBackendT::Double_v
MRG32k3a<BackendT>::Kernel(MRG32k3a_t<BackendT>& state)
{
  using Double_v = typename ReturnTypeBackendT::Double_v;
  Double_v k, p1, p2;

  // Component 1
  p1 = MRG::a12 * state.fCg[1] - MRG::a13n * state.fCg[0];
#if __CUDA_ARCH__ > 0
  k = trunc (fma (p1, MRG::rh1, p1 * MRG::rl1));
#else
  k = math::Floor (p1 / MRG::m1);
#endif
  p1 -= k * MRG::m1;

  Mask_v<Double_v> negative = (p1 < 0.);
  MaskedAssign(p1, negative, p1 + MRG::m1);

  state.fCg[0] = state.fCg[1];
  state.fCg[1] = state.fCg[2];
  state.fCg[2] = p1;

  p2 = MRG::a21 * state.fCg[5] - MRG::a23n * state.fCg[3];
#if __CUDA_ARCH__ > 0
  k = trunc (fma (p2, MRG::rh2, p2 * MRG::rl2));
#else
  k = math::Floor (p2 / MRG::m2);
#endif
  p2 -= k * MRG::m2;

  negative = (p2 < 0.);
  MaskedAssign(p2, negative, p2 + MRG::m2);

  state.fCg[3] = state.fCg[4];
  state.fCg[4] = state.fCg[5];
  state.fCg[5] = p2;

  // Combination
  return Blend((p1 > p2),(p1 - p2) * MRG::norm, (p1 - p2 + MRG::m1) * MRG::norm);

  // Extended (53 bits) precision
  // Double_v random =  Blend((p1 > p2),(p1 - p2) * MRG::norm, (p1 - p2 + MRG::m1) * MRG::norm);
  // random *= (1.0 + MRG::fact);
  // return Blend((random < 1.0), random, random - 1.0);
}

// Sepecialization for the scalar method of VectorBackend pRNG class
#ifndef VECCORE_CUDA
template <>
template <>
VECCORE_FORCE_INLINE
VECCORE_ATT_HOST_DEVICE
ScalarBackend::Double_v
MRG32k3a<VectorBackend>::Kernel<ScalarBackend>(MRG32k3a_t<VectorBackend>& state)
{
  // Return the sequence of the first sub-stream of vector backend. 
  // If the efficiency of this specialization matters, implement the 
  // corresponding scalar method using the first state of vector states, 
  // state.fCg[x], i.e., state.fCg[0][x]

  return Kernel<VectorBackend>(state)[0];
}  
#endif

// Utility functions from RngSteam

// if e > 0, let n = 2^e + c;
// if e < 0, let n = -2^(-e) + c;
// if e = 0, let n = c.
// Jump n steps forward if n > 0, backwards if n < 0.
template <class BackendT>
VECCORE_FORCE_INLINE
VECCORE_ATT_HOST void MRG32k3a<BackendT>::AdvanceState (long e, long c)
{
  double C1[MRG::ndim][MRG::ndim], C2[MRG::ndim][MRG::ndim];

  TransitionVector(C1, C2, e, c);

  double state[MRG::vsize];

  for(size_t i = 0 ; i < VectorSize<Double_v>() ; ++i) {
    // reshape transition input 
    for (int j = 0; j < MRG::vsize ; ++j) {
      state[j] = this->fState->fCg[j][i];
    }

    // advance the given state by (2^e + c) 
    MatVecModM (C1, state, state, MRG::m1);
    MatVecModM (C2, &state[3], &state[3], MRG::m2);

    // put the state back to this->fState
    for (int j = 0; j < MRG::vsize ; ++j) {
       this->fState->fCg[j][i] = state[j];
    }
  }
}

template <>
VECCORE_FORCE_INLINE
VECCORE_ATT_HOST void MRG32k3a<ScalarBackend>::AdvanceState (long e, long c)
{
  double C1[MRG::ndim][MRG::ndim], C2[MRG::ndim][MRG::ndim];

  TransitionVector(C1, C2, e, c);

  MatVecModM (C1, this->fState->fCg, this->fState->fCg, MRG::m1);
  MatVecModM (C2, &(this->fState->fCg[3]), &(this->fState->fCg[3]), MRG::m2);
}

template <class BackendT>
VECCORE_ATT_HOST
void MRG32k3a<BackendT>::TransitionVector(double C1[MRG::ndim][MRG::ndim], 
                                          double C2[MRG::ndim][MRG::ndim], double e, double c)
{
  double B1[MRG::ndim][MRG::ndim], B2[MRG::ndim][MRG::ndim];

  if (e > 0) {
    MatTwoPowModM (MRG::A1p0, B1, MRG::m1, e);
    MatTwoPowModM (MRG::A2p0, B2, MRG::m2, e);
  } else if (e < 0) {
    MatTwoPowModM (MRG::InvA1, B1, MRG::m1, -e);
    MatTwoPowModM (MRG::InvA2, B2, MRG::m2, -e);
  }

  if (c >= 0) {
    MatPowModM (MRG::A1p0, C1, MRG::m1, c);
    MatPowModM (MRG::A2p0, C2, MRG::m2, c);
  } else {
      MatPowModM (MRG::InvA1, C1, MRG::m1, -c);
      MatPowModM (MRG::InvA2, C2, MRG::m2, -c);
  }

  if (e) {
    MatMatModM (B1, C1, C1, MRG::m1);
    MatMatModM (B2, C2, C2, MRG::m2);
  }
}

// Return (a*s + c) MOD m; a, s, c and m must be < 2^35
template <class BackendT>
VECCORE_ATT_HOST double MRG32k3a<BackendT>::MultModM (double a, double s, double c, double m)
{
  double v;
  long a1;

  v = a * s + c;

  if (v >= MRG::two53 || v <= -MRG::two53) {
    a1 = static_cast<long> (a / MRG::two17);
    a -= a1 * MRG::two17;
    v  = a1 * s;
    a1 = static_cast<long> (v / m);
    v -= a1 * m;
    v = v * MRG::two17 + a * s + c;
  }

  a1 = static_cast<long> (v / m);
  // in case v < 0)
  if ((v -= a1 * m) < 0.0) return v += m;
  else return v;
}

// Compute the vector v = A*s MOD m. Assume that -m < s[i] < m. Works also when v = s.
template <class BackendT>
VECCORE_ATT_HOST
void MRG32k3a<BackendT>::MatVecModM (const double A[MRG::ndim][MRG::ndim], 
                                     const double s[MRG::ndim], double v[MRG::ndim], double m)
{
  int i;
  // Necessary if v = s
  double x[MRG::ndim]; 

  for (i = 0; i < MRG::ndim ; ++i) {
      x[i] = MultModM (A[i][0], s[0], 0.0, m);
      x[i] = MultModM (A[i][1], s[1], x[i], m);
      x[i] = MultModM (A[i][2], s[2], x[i], m);
  }
  for (i = 0; i < MRG::ndim ; ++i) v[i] = x[i];

}

// Compute the matrix C = A*B MOD m. Assume that -m < s[i] < m.
// Note: works also if A = C or B = C or A = B = C.
template <class BackendT>
VECCORE_ATT_HOST
void MRG32k3a<BackendT>::MatMatModM (const double A[MRG::ndim][MRG::ndim], 
                                     const double B[MRG::ndim][MRG::ndim],
                                     double C[MRG::ndim][MRG::ndim], double m)
{
  int i, j;
  double V[MRG::ndim], W[MRG::ndim][MRG::ndim];

  for (i = 0; i < MRG::ndim ; ++i) {
    for (j = 0; j < MRG::ndim ; ++j) V[j] = B[j][i];
    MatVecModM (A, V, V, m);
    for (j = 0; j < MRG::ndim ; ++j) W[j][i] = V[j];
  }
  for (i = 0; i < MRG::ndim ; ++i) {
    for (j = 0; j < MRG::ndim ; ++j) C[i][j] = W[i][j];
  }
}

// Compute the matrix B = (A^(2^e) Mod m);  works also if A = B. 
template <class BackendT>
VECCORE_ATT_HOST
void MRG32k3a<BackendT>::MatTwoPowModM (const double A[MRG::ndim][MRG::ndim], 
                                        double B[MRG::ndim][MRG::ndim], double m, long e)
{
   int i, j;

   // Initialize: B = A
   if (A != B) {
     for (i = 0; i < MRG::ndim ; ++i) {
       for (j = 0; j < MRG::ndim ; ++j) B[i][j] = A[i][j];
     }
   }
   // Compute B = A^(2^e) mod m
   for (i = 0; i < e; i++) MatMatModM (B, B, B, m);
}

// Compute the matrix B = (A^n Mod m);  works even if A = B.
template <class BackendT>
VECCORE_ATT_HOST
void MRG32k3a<BackendT>::MatPowModM (const double A[MRG::ndim][MRG::ndim], 
                                     double B[MRG::ndim][MRG::ndim], double m, long n)
{
  int i, j;
  double W[MRG::ndim][MRG::ndim];

  // initialize: W = A; B = I 
  for (i = 0; i < MRG::ndim ; ++i) {
    for (j = 0; j < MRG::ndim ; ++j) {
      W[i][j] = A[i][j];
      B[i][j] = 0.0;
    }
  }
  for (j = 0; j < MRG::ndim ; ++j) B[j][j] = 1.0;

  // Compute B = A^n mod m using the binary decomposition of n 
  while (n > 0) {
    if (n % 2) MatMatModM (W, B, B, m);
    MatMatModM (W, W, W, m);
    n /= 2;
  }
}

} // end namespace impl
} // end namespace vecRng

#endif
