/*
 *  SPDX-License-Identifier: MIT
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vc.h"
#include "random_oracle.h"
#include "compat.h"
#include "aes.h"
#include "instances.h"

#include <assert.h>
#include <string.h>

unsigned int NumRec(unsigned int depth, const uint8_t* bi) {
  unsigned int out = 0;
  for (unsigned int i = 0; i < depth; i++) {
    out += ((unsigned int)bi[i]) << i;
  }
  return out;
}

static void get_children(const uint8_t* node, const uint8_t* iv, uint32_t lambda, uint8_t* l_child, uint8_t* r_child) {
  const unsigned int lambda_bytes = lambda / 8;
  uint8_t* children = malloc(lambda_bytes * 2);
  prg(node, iv, children, lambda, lambda_bytes * 2);
  memcpy(l_child, children, lambda_bytes);
  memcpy(r_child, children + lambda_bytes, lambda_bytes);
  free(children);
}

// index is the index i for (sd_i, com_i)
void get_sd_com(const stream_vec_com_t* sVecCom, const uint8_t* iv, uint32_t lambda, unsigned int index, uint8_t* sd, uint8_t* com) {
  const unsigned int lambdaBytes = lambda / 8;

  uint8_t* node = malloc(lambdaBytes);
  memcpy(node, sVecCom->rootKey, lambdaBytes);
  uint8_t* r_child = malloc(lambdaBytes);
  uint8_t* l_child = malloc(lambdaBytes);
  size_t lo = 0;
  unsigned int leaf_count = (1 << sVecCom->depth);
  size_t hi = leaf_count - 1;
  size_t center;

  // NOTE: Recompute from beginning. Kinda slow. Use the memory later..
  for (size_t i = 0; i < sVecCom->depth; i++) {
    get_children(node, iv, lambda, l_child, r_child);

    center = (hi - lo) / 2 + lo;
    if (index <= center) { // Left
      memcpy(node, l_child, lambdaBytes);
      hi = center;
    }
    else { // Right
      memcpy(node, r_child, lambdaBytes);
      lo = center + 1;
    }
  }

  H0_context_t h0_ctx;
  H0_init(&h0_ctx, lambda);
  H0_update(&h0_ctx, node, lambdaBytes);
  H0_update(&h0_ctx, iv, IV_SIZE);
  H0_final(&h0_ctx, sd, lambdaBytes, com, (lambdaBytes * 2));

  free(node);
  free(r_child);
  free(l_child);
}

void get_sd_com_rec(const stream_vec_com_rec_t* sVecComRec, const uint8_t* iv, uint32_t lambda, unsigned int index, uint8_t* sd, uint8_t* com) {
  const unsigned int lambdaBytes = lambda / 8;
  const unsigned int depth = sVecComRec->depth;
  uint8_t* node = malloc(lambdaBytes);
  //memcpy(node, sVecCom->rootKey, lambdaBytes);
  uint8_t* r_child = malloc(lambdaBytes);
  uint8_t* l_child = malloc(lambdaBytes);
  size_t lo = 0;
  unsigned int leaf_count = (1 << depth);
  size_t hi = leaf_count - 1;
  size_t center;
  uint8_t* b = sVecComRec->b;

  unsigned int idx = NumRec(depth, b);
  unsigned int real_index = index ^ idx;

  // Find first known node on path
  size_t i = 0;
  for (; i < depth; i++) {
    uint8_t left_node_known = b[depth - i - 1];

    center = (hi - lo) / 2 + lo;
    if (real_index <= center) { // Left
      hi = center;
      if (left_node_known) {
        break;
      }
    }
    else { // Right
      lo = center + 1;
      if (!left_node_known) {
        break;
      }
    }
  }
  // NOTE: we could have reaced the unknown leaf if malformed signature
  memcpy(node, sVecComRec->nodes + lambdaBytes * i, lambdaBytes);
  ++i;
  // Continue computing until leaf is reached
  for (; i < depth; i++) {
    get_children(node, iv, lambda, l_child, r_child);

    center = (hi - lo) / 2 + lo;
    if (real_index <= center) { // Left
      memcpy(node, l_child, lambdaBytes);
      hi = center;
    }
    else { // Right
      memcpy(node, r_child, lambdaBytes);
      lo = center + 1;
    }
  }
  
  H0_context_t h0_ctx;
  H0_init(&h0_ctx, lambda);
  H0_update(&h0_ctx, node, lambdaBytes);
  H0_update(&h0_ctx, iv, IV_SIZE);
  H0_final(&h0_ctx, sd, lambdaBytes, com, (lambdaBytes * 2));

  free(node);
  free(r_child);
  free(l_child);
}

void stream_vector_commitment(const uint8_t* rootKey, uint32_t lambda, stream_vec_com_t* sVecCom, uint32_t depth) {
  const unsigned int lambdaBytes = lambda / 8;
  //sVecCom->rootKey = malloc(lambdaBytes);
  memcpy(sVecCom->rootKey, rootKey, lambdaBytes);
  sVecCom->depth = depth;
}

void stream_vector_open(stream_vec_com_t* sVecCom, const uint8_t* b, uint8_t* cop,
                 uint8_t* com_j, uint32_t depth,  const uint8_t* iv, uint32_t lambda) {
  // Step: 1
  const unsigned int lambdaBytes = lambda / 8;
  uint8_t* node = malloc(lambdaBytes);
  uint8_t* l_child = malloc(lambdaBytes);
  uint8_t* r_child = malloc(lambdaBytes);
  memcpy(node, sVecCom->rootKey, lambdaBytes);

  // Step: 3..6
  uint8_t save_left;
  for (uint32_t i = 0; i < depth; i++) {
    // b = 0 => Right
    // b = 1 => Left
    get_children(node, iv, lambda, l_child, r_child);
    save_left = b[depth - 1 - i];
    if (save_left) {
      memcpy(cop + (lambdaBytes * i), l_child, lambdaBytes);
      memcpy(node, r_child, lambdaBytes);
    }
    else {
      memcpy(cop + (lambdaBytes * i), r_child, lambdaBytes);
      memcpy(node, l_child, lambdaBytes);
    }
  }
  free(l_child);
  free(r_child);

  // Step: 7
  uint64_t leafIndex = NumRec(depth, b);
  uint8_t* sd = malloc(lambdaBytes); // Byproduct
  get_sd_com(sVecCom, iv, lambda, leafIndex, sd, com_j);
  free(sd);
  free(node);
}

void stream_vector_reconstruction(const uint8_t* cop, const uint8_t* com_j, const uint8_t* b, uint32_t lambda, uint32_t depth, stream_vec_com_rec_t* sVecComRec) {
  const unsigned int lambdaBytes = lambda / 8;
  sVecComRec->depth = depth;
  memcpy(sVecComRec->b, b, depth);
  memcpy(sVecComRec->nodes, cop, lambdaBytes * depth);
  memcpy(sVecComRec->com_j, com_j, lambdaBytes * 2);
}

void stream_vec_com_rec_clear(stream_vec_com_rec_t* srec) {
  free(srec->b);
  free(srec->nodes);
  free(srec->com_j);
}
