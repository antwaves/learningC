#include "../../csv/csv_handle.c"
#include <time.h>
#include <stdint.h>

int main() {
    double final = 0.0;

    for (int i = 0; i < 1; i++){
        clock_t begin, end;
        begin = clock();
        ht_hash_table** result = csv_to_dict_list("customers-100000.csv");

        end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        final += time_spent;
        del_csv_list(result);
    }

    printf("%f", final / 1);
}
