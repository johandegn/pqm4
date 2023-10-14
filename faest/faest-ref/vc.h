#ifndef FAEST_VC_H
#define FAEST_VC_H

#include <stdint.h>

#include "instances.h"
#include "utils.h"

FAEST_BEGIN_C_DECL

typedef struct vec_com_t {
  uint8_t* h;
  uint8_t* k;
  uint8_t* com;
  uint8_t* sd;
} vec_com_t;

typedef struct stream_vec_com_t {
  uint8_t rootKey[MAX_LAMBDA_BYTES];
  uint32_t depth;
} stream_vec_com_t;

typedef struct vec_com_rec_t {
  uint8_t* h;
  uint8_t* k;
  uint8_t* com;
  uint8_t* s;
} vec_com_rec_t;

typedef struct stream_vec_com_rec_t {
  uint8_t* nodes;
  uint8_t* b;
  uint8_t* com_j;
  uint32_t depth;
} stream_vec_com_rec_t;

typedef struct stream_seeds_t {
  enum stream_seeds_type {SVECCOM, SVECCOMREC} type;
  union {
    stream_vec_com_t* sVecCom;
    stream_vec_com_rec_t* sVecComRec;
  };
  
} stream_seeds_t;


ATTR_CONST uint64_t getBinaryTreeNodeCount(unsigned int depth);
ATTR_CONST uint64_t getNodeIndex(uint64_t depth, uint64_t levelIndex);

int BitDec(unsigned int leafIndex, unsigned int depth, uint8_t* out);
unsigned int NumRec(unsigned int depth, const uint8_t* bi);

void vector_commitment(const uint8_t* rootKey, const uint8_t* iv, const faest_paramset_t* params,
                       uint32_t lambda, vec_com_t* vecCom, uint32_t depth);

void stream_sd_com(const stream_seeds_t stream_seeds, const uint8_t* iv, uint32_t lambda, unsigned int index, uint8_t* sd, uint8_t* com);
void stream_vector_commitment(const uint8_t* rootKey, uint32_t lambda, stream_vec_com_t* sVecCom, uint32_t depth);

void vector_open(const uint8_t* k, const uint8_t* com, const uint8_t* b, uint8_t* pdec,
                 uint8_t* com_j, uint32_t depth, uint32_t lambdaBytes);

void stream_vector_open(stream_vec_com_t* sVecCom, const uint8_t* b, uint8_t* cop,
                 uint8_t* com_j, uint32_t depth,  const uint8_t* iv, uint32_t lambda);

void vector_reconstruction(const uint8_t* iv, const uint8_t* pdec, const uint8_t* com_j,
                           const uint8_t* b, uint32_t lambda, uint32_t depth,
                           vec_com_rec_t* vecComRec);

void stream_vector_reconstruction(const uint8_t* cop, const uint8_t* com_j, const uint8_t* b, uint32_t lambda, uint32_t depth, stream_vec_com_rec_t* sVecComRec);

#if defined(FAEST_TESTS)
int vector_verify(const uint8_t* iv, const uint8_t* pdec, const uint8_t* com_j, const uint8_t* b,
                  uint32_t lambda, uint32_t depth, vec_com_rec_t* rec, const uint8_t* vecComH);
#endif

void vec_com_clear(vec_com_t* com);
void stream_vec_com_clear(stream_vec_com_t* scom);
void vec_com_rec_clear(vec_com_rec_t* rec);
void stream_vec_com_rec_clear(stream_vec_com_rec_t* srec);

FAEST_END_C_DECL

#endif
