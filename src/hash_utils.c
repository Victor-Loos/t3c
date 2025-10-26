#include "t3c.h"

// Calcule le hash en hex (malloc -> free par l'appelant)
char *compute_hash_hex(const char *input, const char *algo) {
    const EVP_MD *md = EVP_get_digestbyname(algo); // obtenir l'algorithme
    if (!md) md = EVP_sha256(); // défaut SHA256

    EVP_MD_CTX *ctx = EVP_MD_CTX_new(); // créer le contexte
    if (!ctx) return NULL;

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;

    // Processus en trois phases utilisé par l'API EVP d'OpenSSL 
    // 1) Initialisation, 2) mise à jour et 3) finalisation du hash. 
    // En cas d'erreur, libérer le contexte et retourner NULL.
    if (1 != EVP_DigestInit_ex(ctx, md, NULL)) { EVP_MD_CTX_free(ctx); return NULL; }
    if (1 != EVP_DigestUpdate(ctx, input, strlen(input))) { EVP_MD_CTX_free(ctx); return NULL; }
    if (1 != EVP_DigestFinal_ex(ctx, digest, &digest_len)) { EVP_MD_CTX_free(ctx); return NULL; }

    EVP_MD_CTX_free(ctx);

    // conversion du hash binaire -> hex
    char *hex = malloc(digest_len * 2 + 1);
    if (!hex) return NULL;
    for (unsigned int i = 0; i < digest_len; ++i) {
        sprintf(hex + i * 2, "%02x", digest[i]);
    }
    hex[digest_len * 2] = '\0';
    return hex;
}
