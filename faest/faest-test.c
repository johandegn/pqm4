#include <stdio.h>
#include <string.h>
#include "api.h"

int main(void) {
    printf("Variant: %s\n", CRYPTO_ALGNAME);
    printf("Generating Keypair..");
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    if (crypto_sign_keypair(pk, sk)) {
        fprintf(stderr, "Error creating keypair\n");
        return 1;
    }
    printf(" Success!\n");

    printf("Signing..");
    const unsigned char *m = "This is the message to sign.";
    unsigned long long mlen = strlen(m);
    unsigned char sm[CRYPTO_BYTES + mlen];
    unsigned long long smlen;
    if (crypto_sign(sm, &smlen, m, mlen, sk)) {
        fprintf(stderr, "Error signing\n");
        return 1;
    }
    printf(" Success!\n");

    printf("Verifying..");

    unsigned char open_m[mlen];
    strncpy(open_m, m, mlen);
    unsigned long long open_mlen;

    if (crypto_sign_open(open_m, &open_mlen, sm, smlen, pk)) {
        fprintf(stderr, "Error verifying\n");
        return 1;
    }
    printf(" Success!\n");

    return 0;
}
