#include "t3c.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n  %s G <dictionnaire> <table.t3c> [algo]\n  %s L <table.t3c> [hash]\n", argv[0], argv[0]);
        return 1;
    }

    // initialisation OpenSSL
    OPENSSL_init_crypto(0, NULL);

    // Choix du mode
    if (strcmp(argv[1], "G") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Mode G: arguments manquants\n");
            return 1;
        }
        const char *dict = argv[2];
        const char *out = argv[3];
        const char *algo = (argc >= 5) ? argv[4] : "sha256";
        return mode_generate(dict, out, algo);
    } else if (strcmp(argv[1], "L") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Mode L: arguments manquants\n");
            return 1;
        }
        const char *table = argv[2];
        const char *hash_arg = (argc >= 4) ? argv[3] : NULL;
        return mode_lookup(table, hash_arg);
    } else {
        fprintf(stderr, "Mode inconnu : %s (utiliser G ou L)\n", argv[1]);
        return 1;
    }
}
