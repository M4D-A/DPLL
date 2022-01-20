//
// Created by M4DA - Adam Jagielski, jagielski.adamm@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //isdigit()
#include <string.h>
#include <time.h>
#include "features.h"
#include "vitals.h"

//--$$--DONE--$$--// -- // MEMORY STABLE //
// READS DIMACS FILE AND STORE FORMULA'S DATA INTO STRUCTURE //
int** read_cnf_from_file(char* filename){
    // ++++ READ FILE VARIABLES ++++ //
    FILE* cnf_file;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int clauses_number = 0;
    int variables_number = 0;
    int** cnf;

    // ++++ READING FILE AND CREATING DATA STRUCTURE ++++ //
    cnf_file = fopen(filename,"r");
    if (cnf_file == NULL)
        return NULL;

    while ((read = getline(&line, &len, cnf_file)) != -1) { // ommit commentary
        if(line[0] == 'c')
            continue;
        else
            break;
    }

    char* temp = line;
    while(temp){ // get number of variables from cnf header
        if(isdigit(*temp)){
            variables_number = strtol(temp, &temp, 10);
            break;
        }
        else
            temp++;
    }

    while(temp){ // get number of clauses from cnf header
        if(isdigit(*temp)){
            clauses_number = strtol(temp, &temp, 10);
            break;
        }
        else temp++;
    }

    cnf = (int**)malloc(sizeof(int*) * (clauses_number + 1) ); // allocate clause arrays + header array

    cnf[0] = (int*)malloc(sizeof(int) * 2); // header allocation
    cnf[0][1] = clauses_number; // defining header
    cnf[0][0] = variables_number;

    int i = 1;
    int j = 1;

    while ((read = getline(&line, &len, cnf_file)) != -1) { // load clauses to clauses data structure
        int number_of_literals = 0; // number of literals in given clause
        temp = line;
        while(*temp != '\0'){ // count literals in given clause due to " " termination
            if(*temp == ' ') number_of_literals++;
            temp++;
        }
        cnf[i] = (int*)malloc(sizeof(int) * (number_of_literals + 1) ); // allocate memory for i clause
        cnf[i][0] = number_of_literals; // save number of variables in each clause
        temp = line;
        j = 1;
        while (*temp){ // find literals in clause, save them to structure
            if (  isdigit(*temp) || ( *temp=='-' && isdigit(*(temp+1)) )  ){
                int literal = strtol(temp, &temp, 10);
                if(literal) cnf[i][j++] = literal;
            }

            else
                temp++;
        }
        i++;
    }

    fclose(cnf_file); // Finishing file usage

    if (line)
        free(line);

    return cnf;
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
// PRODUCES COPY OF CNF FORMULA //
int** copy_cnf(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int** copy = (int**)malloc( sizeof(int*) * (clss_num + 1) ); // / allocate formula with num of rows equal to num of caluses + header
    copy[0] = (int*)malloc( sizeof(int) * 2 ); // allocate header
    memcpy(copy[0], cnf[0], sizeof(int) * 2 ); // copy header

    int i;
    for(i = 1; i <= clss_num; i++){ // iterate through clauses
        if(cnf[i]){ // check if clause is satisfied null-clause
            copy[i] = (int*)malloc( sizeof(int) * (cnf[i][0] + 1) ); // allocate clause in new formula
            memcpy(copy[i], cnf[i], sizeof(int) * (cnf[i][0] + 1) ); // copy clause into new formula
        }
        else{ // if clause is satisfied set copy to null
            copy[i] = NULL;
        }
    }
    return copy;
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
// TRANSPOSES CLAUSES->VARIABLES CNF FORMULA INTO VARIABLES->CLAUSES T_CNF FORMULA //
int** transpose_cnf(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int** tcnf = (int**)malloc(sizeof(int*) *  (vars_num + 1) ); // allocate formula with num of rows equal to num of variables + header
    tcnf[0] = (int*)malloc( sizeof(int) * 2 ); // allocate header
    memcpy(tcnf[0], cnf[0], sizeof(int) * 2 ); // copy header

    int* temp_var_count = (int*)calloc(sizeof(int), vars_num + 1); // contains nums of variables' occurences

    int i,j;
    for(i = 1; i <= clss_num; i++){ // count variables' occurences
        if(cnf[i]){ // skip satisfied null-clauses
            int lits_in_cls = cnf[i][0];
            for(j = 1; j <= lits_in_cls; j++){
                temp_var_count[ abs(cnf[i][j]) ]++; // add variable occurence
            }
        }
    }

    for(i = 1; i <= vars_num; i++){
        tcnf[i] = (int*)malloc(sizeof(int) * (temp_var_count[i] + 1)); // allocate memory for occurences
        tcnf[i][0] = temp_var_count[i]; // stash number of occurences into formula
    }

    for(i = 1; i <= clss_num; i++){
        if(cnf[i]){ // skip satisfied null-clauses
            int lits_in_cls = cnf[i][0];
            for(j = 1; j <= lits_in_cls; j++){
                int literal = cnf[i][j];
                int variable = abs(literal);
                int index = temp_var_count[variable]--; //get index where to stash occurence, decrement it for next occurence
                tcnf[variable][index] = literal > 0 ? i : -i; // stash occurence with respecting sign
            }
        }
    }

    free(temp_var_count);

    return tcnf;
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
// SEARCHES FOR UNIT LITERALS IN CNF FORMULA OR NULL IF CONFLICTING UNITS FOUND //
int* find_units(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];
    int* units = (int*)malloc( (vars_num + 1) * sizeof(int));
    int i;
    units[0] = vars_num;
    for(i = 1; i <= vars_num; i++) units[i] = 0;
    for(i = 1; i <= clss_num; i++){
        if(cnf[i]){
            if(cnf[i][0] == 1){
                int unit = cnf[i][1];
                int var = abs(unit);
                if(units[var] == unit) continue;
                else if(units[var] == 0) units[var] = unit;
                else{
                    free(units);
                    return NULL;
                }
            }
        }
    }

    int count = 1;
    for(i = 1; i <= vars_num; i++){
        if(units[i] != 0) units[count++] = units[i];
    }
    int* temp = (int*)malloc(count*sizeof(int));
    memcpy(temp, units, count*sizeof(int));
    temp[0] = count-1;
    free(units);
    return temp;
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
// SEARCHES FOR PURE LITERALS IN T_CNF FORMULA //
int* find_pures(int** cnf){

    int** t_cnf = transpose_cnf(cnf);
    int vars_num = t_cnf[0][0];
    int* pures = NULL;
    int number_of_pures = 0;
    int i,j;

    for(i = 1; i <= vars_num; i++){ // Iterate through variables to find pure ones and count them
        if(t_cnf[i][0] > 0){
            int sign = get_sign(t_cnf[i][1]);
            number_of_pures++; // assume that variable is pure
            for(j = 1; j <= t_cnf[i][0]; j++){
                if(get_sign(t_cnf[i][j]) != sign){
                    number_of_pures--; // withdraw assumption if conflicting signs found
                    break; // skip to next variable
                }
                else{
                    continue; // check next occurence
                }
            }
        }
    }

    pures = (int*)malloc(sizeof(int) * (number_of_pures + 1)); // allocate memory for pure variables
    pures[0] = number_of_pures; // stash number of pure variables

    int k = 1; // Indicates where to stash next pure variable
    for(i = 1; i <= vars_num; i++){ // Iterate through variables to find pure ones and stash them
        if(k > number_of_pures) break;
        if(t_cnf[i][0] > 0){
            int sign = get_sign(t_cnf[i][1]);
            pures[k++] = i*sign; // assume that variable is pure, save it with respecting sign and increment index
            for(j = 1; j <= t_cnf[i][0]; j++){
                if(get_sign(t_cnf[i][j]) != sign){
                    k--; // withdraw assumption if conflicting signs found
                    break; // skip to next variable
                }
                else{
                    continue; // check next occurence
                }
            }
        }
    }
    erase_formula(t_cnf, 1);
    return pures;
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
// JOINS TWO ARRAYS, RETURNS OUTCOME REFERENCE, FREES SUBSTRATS
int* concat(int* a, int* b){
    int* c;
    int a_size = a[0];
    int b_size = b[0];
    int c_size = a_size + b_size;
    c = (int*)malloc( (c_size + 1) * sizeof(int) );
    c[0] = c_size;
    memcpy(c+1, a+1, a_size*sizeof(int));
    memcpy(c+a_size+1, b+1, b_size*sizeof(int));
    free(a);
    free(b);
    return c;
}

//--$$--DONE--$$--// -- //  MEMORY STABLE //
// RETURNS -1,0 or 1 FOR NEGATIVE, ZERO AND POSITIVE INTEGERS RESPECTIVELY //
int get_sign(int num){
    return (num > 0) - (num < 0);
}

//--##--DONE--##--// -- // MEMORY WRITE ERROR ???, NO LEAKS, STABLE //
// REDUCES FORMULAE THROUGHOUT CROSS-OUT RULE BASED ON PASSED ASSUMPTIONS //
void reduce(int** cnf, int* reducibles){
    if(reducibles){
        int vars_num = cnf[0][0];
        int clss_num = cnf[0][1];
        int rdcs_num = reducibles[0];
        int i,j,k;
        for(k = 1; k <= rdcs_num; k++){
            for(i = 1; i <= clss_num; i++){
                if(cnf[i]){
                    int cls_size = cnf[i][0];
                    for(j = 1; j<= cls_size; j++){
                        int literal = cnf[i][j];
                        if(literal == -reducibles[k]){
                            cnf[i][j] = cnf[i][cls_size];
                            cnf[i][0]--;
                        }
                        else if(literal == reducibles[k]){
                            free(cnf[i]);
                            cnf[i] = NULL;
                            break;
                        }
                    }
                }
            }
        }
    }
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
// DEALLOCATES MEMORY UNDER CNF OR T_CNF //
void erase_formula(int** formula, int transposed){
    int length = (transposed) ? formula[0][0] : formula[0][1];
    int i;
    for(i = 0; i <= length; i++){
        free(formula[i]);
    }
    free(formula);
}

//--$$--DONE--$$--//
// DEALLOCATES MEMORY UNDER CNF OR T_CNF //
void return_solution(int* solution, int variables){
    if(solution){
        int* full_solution = (int*)malloc( (variables+1) * sizeof(int) );
        int i;

        for(i = 1; i <= variables; i++){
            full_solution[i] = i;
        }
        for(i = 1; i <= solution[0]; i++){
            full_solution[ abs(solution[i]) ] = solution[i];
        }
        printf("c --- RESULT ---\n");
        printf("c\n");
        printf("s SATISFIABLE\n");
        for(i = 1; i <= variables; i++){
            if(i % 16 == 1) printf("v");
            printf(" %d", full_solution[i]);
            if(i % 16 == 0) printf("\n");
        }
        if(variables % 16) printf("\n");
        printf("v 0\n");
        free(full_solution);
    }
    else{
        printf("c --- RESULT ---\n");
        printf("c\n");
        printf("s UNSATISFIABLE\n");
        printf("c\n");
        printf("c---------------\n");

    }
}
