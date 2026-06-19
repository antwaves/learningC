#ifndef hash_included
#define hash_included

#include "../dict/src/hash_table.c"

#endif

ht_hash_table** csv_to_dict_list(char* filename);
void del_csv_list(ht_hash_table** rows);