#include "t3c.h"

// comparateur pour qsort/bsearch sur les champs 'hash'
static int cmp_entry_hash(const void *a, const void *b) {
    const Entry *ea = (const Entry *)a;
    const Entry *eb = (const Entry *)b;
    return strcmp(ea->hash, eb->hash);
}

// charge une table T3C depuis un fichier
static Table *load_table(const char *table_file) {
    FILE *f = fopen(table_file, "r");
    if (!f) { perror("Ouvrir table"); return NULL; }

    Table *tbl = malloc(sizeof(Table));
    if (!tbl) { fclose(f); return NULL; }
    tbl->entries = NULL;
    tbl->count = 0;
    tbl->capacity = 0;

    char line[2048]; // taille max d'une ligne
    size_t linenum = 0;
    while (fgets(line, sizeof(line), f)) {
        linenum++;
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '\0') continue;

        // trouver le séparateur : tab prioritaire, sinon premier espace
        char *sep = strchr(line, '\t');
        if (!sep) sep = strchr(line, ' ');
        if (!sep) continue; // format inattendu -> ignorer

        *sep = '\0';
        char *password = line;
        char *hash = sep + 1;
        if (hash[0] == '\0') continue;

        // augmenter capacité si nécessaire (doubling)
        if (tbl->count == tbl->capacity) {
            size_t newcap = (tbl->capacity == 0) ? 1024 : tbl->capacity * 2;
            Entry *tmp = realloc(tbl->entries, newcap * sizeof(Entry));
            if (!tmp) {
                perror("realloc");
                // libération partielle
                for (size_t i = 0; i < tbl->count; ++i) { free(tbl->entries[i].password); free(tbl->entries[i].hash); }
                free(tbl->entries); free(tbl);
                fclose(f);
                return NULL;
            }
            tbl->entries = tmp;
            tbl->capacity = newcap;
        }

        // dupliquer chaînes (allocation)
        tbl->entries[tbl->count].password = strdup(password);
        tbl->entries[tbl->count].hash = strdup(hash);
        if (!tbl->entries[tbl->count].password || !tbl->entries[tbl->count].hash) {
            perror("strdup");
            // libération partielle
            for (size_t i = 0; i < tbl->count; ++i) { free(tbl->entries[i].password); free(tbl->entries[i].hash); }
            free(tbl->entries); free(tbl);
            fclose(f);
            return NULL;
        }
        tbl->count++;

        // Affiche un message toutes les 5 millions d'entrées
        if (linenum % 5000000 == 0) {
            fprintf(stderr, "L: %zu lignes chargées...\n", tbl->count);
        }
    }

    fclose(f);


    // Tri par hash pour la recherche binaire rapide
    qsort(tbl->entries, tbl->count, sizeof(Entry), cmp_entry_hash);

    fprintf(stderr, "Table chargée (%zu entrées). Tri effectué.\n", tbl->count);
    return tbl;
}

// cherche un hash (string) dans la table triée par qsort : bsearch
static const char *lookup_hash_in_table(const Table *tbl, const char *hash) {
    Entry key;
    key.hash = (char *)hash;
    Entry *result = bsearch(&key, tbl->entries, tbl->count, sizeof(Entry), cmp_entry_hash);
    if (result) return result->password;
    return NULL;
}


int mode_lookup(const char *table_file, const char *hash_arg) {
    Table *tbl = load_table(table_file);
    if (!tbl) return 1;

    int ret = 0;

    // recherche un seul hash depuis l'argument
    if (hash_arg && hash_arg[0] != '\0') {
        const char *pwd = lookup_hash_in_table(tbl, hash_arg);
        if (pwd) printf("%s\n", pwd);
        else {
            fprintf(stderr, "Hash non trouvé.\n");
            ret = 2;
        }
    } else {
        // lire depuis stdin : plusieurs hashes séparés par '\n'
        char line[2048];
        while (fgets(line, sizeof(line), stdin)) {
            line[strcspn(line, "\r\n")] = 0;
            if (line[0] == '\0') continue;
            const char *pwd = lookup_hash_in_table(tbl, line);
            if (pwd) printf("%s\n", pwd);
            else printf("\n"); // si non trouvé, imprimer une ligne vide
        }
    }

    // Libération mémoire
    for (size_t i = 0; i < tbl->count; ++i) {
        free(tbl->entries[i].password);
        free(tbl->entries[i].hash);
    }
    free(tbl->entries);
    free(tbl);
    return ret;
}
