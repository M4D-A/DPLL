//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//

#ifndef DPLL_FEATURES_H
#define DPLL_FEATURES_H

#endif //DPLL_FEATURES_H

//---FEATURES---//
void print_formula(int** formula, int transposed);
void print_array(int* array);
void save_cnf_to_file(int** cnf, char* file_name);
int** random_cnf(int k, int variables, int clauses);