#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "features.h"
#include "vitals.h"
#include "heuristics.h"

//---MAIN ALGORITHM---//
int* DPLL(int** cnf, int* current, int* initial);

int START_TIME;
int HEURISTIC_ID;
char* CNF_FILE_NAME;
int main(int argc, char** argv){
    if(argc < 3){
    	printf("Nie podano wszystkich argumentow - [file_name] [heuristic_id]\n");
    	return 1;
	}
    srand(time(NULL));
    START_TIME = time(NULL);
    CNF_FILE_NAME = argv[1];
    HEURISTIC_ID = atoi( argv[2] );

    int** cnf = read_cnf_from_file(CNF_FILE_NAME);
    //print_formula(cnf, 0);
    int vars = cnf[0][0];
    int* cu = malloc(4); cu[0]=0;
    int* in = malloc(4); in[0]=0;
    //print_array(in);
    int* solution = DPLL(cnf,cu,in);
    return_solution(solution, vars);
    printf("c time elapsed: %ld\n", time(NULL) - START_TIME);
    free(solution);
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
int* DPLL(int** cnf, int* current, int* initial){
    reduce(cnf, initial);
    current = concat(current, initial);
    int* units = NULL;
    int* pures = NULL;
    int units_size;
    int pures_size;
    do{
        units = find_units(cnf);
        if(!units) {
            free(current);
            erase_formula(cnf,0);
            return NULL;
        }
        //{printf("un\n");print_array(units);}
        reduce(cnf, units);
        units_size = units[0];
        current = concat(current, units);
        units = NULL;
        //{printf("pu\n");print_array(pures);}
        pures = find_pures(cnf);
        reduce(cnf, pures);
        pures_size = pures[0];
        current = concat(current, pures);
        pures = NULL;
    }while(units_size > 0 || pures_size > 0);
    int i;
    int SAT = 1;
    int UNSAT = 0;
    for(i = 1; i <= cnf[0][1]; i++){
        if(cnf[i]) {
            SAT = 0;
            if(cnf[i][0] == 0) UNSAT = 1;
        }
    }
    if(SAT){
        erase_formula(cnf,0); 
        return current;
    }
    if(UNSAT){
        free(current);
        erase_formula(cnf,0); 
        return NULL;
    }

    int (*heuristic)(int**);
    switch( HEURISTIC_ID ){
        case 0: heuristic = JW; break;
        case 1: heuristic = HV; break;
        case 2: heuristic = VGT; break;
        case 3: heuristic = CSAT; break;
        default: heuristic = JW; break;
    }

    int variable = heuristic(cnf);
    
    int** cnf_c = copy_cnf(cnf);
    int*  current_c  = malloc( (current[0]+1)*sizeof(int) );
    memcpy(current_c, current, (current[0]+1)*sizeof(int) );


    int* assumption = (int*)malloc(2 * sizeof(int));
    assumption[0] = 1;
    assumption[1] = variable;

    int* solution = DPLL(cnf, current, assumption);
    if(solution){
        free(current_c);
        erase_formula(cnf_c,0);
        return solution;
    } 
    else{
        int* assumption = (int*)malloc(2 * sizeof(int));
        assumption[0] = 1;
        assumption[1] = -variable;
        solution = DPLL(cnf_c, current_c, assumption);
        return solution;
    }
}



