#include "t3c.h"

int mode_generate(const char *dict_file, const char *output_file, const char *algo) {
    FILE *fin = fopen(dict_file, "r");
    if (!fin) {
        perror("Ouvrir dictionnaire");
        return 1;
    }
    FILE *fout = fopen(output_file, "w");
    if (!fout) {
        perror("Ouvrir sortie");
        fclose(fin);
        return 1;
    }

    char line[1024]; // accepte des mots assez longs
    size_t count = 0;

    // Lecture ligne par ligne ; '\n' séparateur
    while (fgets(line, sizeof(line), fin)) {
        // supprime '\n' et '\r' en fin de ligne (\n : Unix, \r\n : Windows)
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '\0') continue; /* ignorer lignes vides */

        char *hash = compute_hash_hex(line, algo);
        if (!hash) {
            fprintf(stderr, "Erreur de hachage sur la ligne %zu\n", count + 1);
            fclose(fin); fclose(fout);
            return 1;
        }

        // écriture immédiate (format tabulé)
        fprintf(fout, "%s\t%s\n", line, hash);
        free(hash);

        ++count;
        // Affiche un message toutes les 5 millions d'entrées
        if (count % 5000000 == 0) {
            fprintf(stderr, "G: %zu entrées traitées...\n", count);
        }
    }

    fprintf(stderr, "Génération terminée : %zu entrées écrites dans %s\n", count, output_file);

    fclose(fin);
    fclose(fout);
    return 0;
}
