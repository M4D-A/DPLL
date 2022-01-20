//
// Created by adam on 20.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "features.h"

//---FEATURES---//
void print_formula(int** formula, int transposed){
    int i,j;
    printf("##################\n");
    printf("v: %d, c: %d\n",formula[0][0],formula[0][1]);
    printf("******************\n");
    int length = (transposed) ? formula[0][0] : formula[0][1];
    for(i = 1; i <= length; i++){
        if(formula[i]){
            printf("%3d(%3d): ", i, formula[i][0]);
            for(j = 1; j<= formula[i][0]; j++){
                printf("%3d, ", formula[i][j]);
            }
            printf("\n");
        }
        else{
            printf("%3d(SAT):\n", i);
        }

    }
}

void print_array(int* array){
    printf("******************\n");
    if(array){
        printf("%d: ", array[0]);
        int i;
        for(i = 1; i <= array[0]; i++){
            printf("%d, ", array[i]);
        }
        printf("\n");
    }
    else{
        printf("NULL ARRAY\n");
    }
}

void save_cnf_to_file(int** cnf, char* file_name){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    FILE *fp;
    fp = fopen(file_name, "w");

    fprintf(fp, "c SAT instance with %d variables and %d clauses\n", vars_num, clss_num);
    fprintf(fp, "p cnf %d %d\n", vars_num, clss_num);

    int i,j;
    for(i = 1; i <= clss_num; i++){
        int literals = cnf[i][0];
        for(j = 1; j <= literals; j++){
            fprintf(fp, "%d ", cnf[i][j]);
        }
        fprintf(fp, "0\n");
    }
    fclose(fp);
}

int** random_cnf(int k, int variables, int clauses){
    if(k > variables) return NULL;

    int** cnf = malloc((clauses + 1)*sizeof(int*));
    cnf[0] = malloc(2*sizeof(int));
    cnf[0][0] = variables;
    cnf[0][1] = clauses;
    int i,j;
    for(i = 1; i <= clauses; i++){
        int* variables_permutation = (int*)malloc(variables * sizeof(int));
        for(j = 0; j < variables; j++) variables_permutation[j] = j+1;

        for (j = variables-1; j >= 0; --j){
            int h = rand() % (j+1);
            int temp = variables_permutation[j];
            variables_permutation[j] = variables_permutation[h];
            variables_permutation[h] = temp;
        }

        cnf[i] = malloc((k+1) * sizeof(int));
        cnf[i][0] = k;
        for(j = 1; j <= k; j++){
            int r = variables_permutation[j-1];
            r = (rand() % 2) ? r : -r;
            cnf[i][j] = r;
        }
    }
    return cnf;
}
