//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//

#ifndef DPLL_VITALS_H
#define DPLL_VITALS_H

#endif //DPLL_VITALS_H

//---VITAL FUNCTIONS---//
int** read_cnf_from_file(char* filename);           // READS DIMACS FILE AND STORE FORMULA'S DATA INTO STRUCTURE //
int** copy_cnf(int** cnf);                          // PRODUCES COPY OF CNF FORMULA //
int** transpose_cnf(int** cnf);                     // TRANSPOSES CLAUSES->VARIABLES CNF FORMULA INTO VARIABLES->CLAUSES T_CNF FORMULA //
int* find_units(int** cnf);                         // SEARCHES FOR UNIT LITERALS IN CNF FORMULA //
int* find_pures(int** t_cnf);                       // SEARCHES FOR PURE LITERALS IN T_CNF FORMULA //
int* concat(int* a, int* b);                        // JOINS TWO ARRAYS, RETURNS OUTCOME REFERENCE, FREES COMPONENTS //
int get_sign(int num);                              // RETURNS -1,0 or 1 FOR NEGATIVE, ZERO AND POSITIVE INTEGERS RESPECTIVELY //
void reduce(int** cnf, int* reducibles);            // REDUCES FORMULAE THROUGHOUT CROSS-OUT RULE BASED ON PASSED ASSUMPTIONS //
void erase_formula(int** formula, int transposed);  // DEALLOCATES MEMORY UNDER CNF OR T_CNF //
void return_solution(int* solution, int variables);
