#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> //isdigit()
#include <math.h>
#include <string.h>
#include <time.h>


//---MAIN ALGORITHM---//
int* DPLL(int** cnf, int* current, int* initial);

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


//---HEURISTICS---//
int JW(int** cnf);
int HV(int** cnf);
int VGT(int** cnf);
int CSAT(int** cnf);

//---FEATURES---//
void print_formula(int** formula, int transposed);
void print_array(int* array);
void save_cnf_to_file(int** cnf, char* file_name);
int** random_cnf(int k, int variables, int clauses);

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
    int vars = cnf[0][0];
    int* cu = malloc(4); cu[0]=0;
    int* in = malloc(4); in[0]=0;
    //print_array(in);
    int* solution = DPLL(cnf,cu,in);
    return_solution(solution, vars);
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

        if(units) free(units);
        if(pures) free(pures);
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

//--##--DONE--##--// -- // MEMORY WRITE ERROR, NO LEAKS, STABLE //
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
    printf("c time elapsed: %ld\n", time(NULL) - START_TIME);
}

//--$$--DONE--$$--// -- // MEMORY STABLE //
//---HEURISTICS---//
int JW(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int variable = 0;
    float max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        float neg_i_score = 0;
        float pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            float length = cnf[ abs(t_cnf[i][j]) ][0];
            if(t_cnf[i][j] < 0) neg_i_score += pow(2.0, -length);
            else                pos_i_score += pow(2.0, -length);
        }
        if(neg_i_score > max_score){
            max_score = neg_i_score;
            variable = -i;
        }
        if(pos_i_score > max_score){
            max_score = pos_i_score;
            variable = i;
        }
    }
    erase_formula(t_cnf, 1);                                        // DELETE_0
    return variable;
}

int HV(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int variable = 0;
    float max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        float neg_i_score = 0;
        float pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            float length = cnf[ abs(t_cnf[i][j]) ][0];
            if(t_cnf[i][j] < 0) neg_i_score += pow(2.0, -length);
            else                pos_i_score += pow(2.0, -length);
        }
        if(neg_i_score + pos_i_score > max_score){
            max_score = neg_i_score + pos_i_score;
            if(pos_i_score > neg_i_score)   variable = i;
            else                            variable = -i;
        }
    }
    erase_formula(t_cnf, 1);                                        // DELETE_0
    return variable;
}

int VGT(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int variable = 0;
    float max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        float neg_i_score = 0;
        float pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            float length = cnf[ abs(t_cnf[i][j]) ][0];
            if(t_cnf[i][j] < 0) neg_i_score += pow(2.0, -length);
            else                pos_i_score += pow(2.0, -length);
        }
        if(neg_i_score * pos_i_score > max_score){
            max_score = neg_i_score * pos_i_score;
            if(pos_i_score > neg_i_score)   variable = i;
            else                            variable = -i;
        }
    }
    erase_formula(t_cnf, 1);                                        // DELETE_0
    return variable;
}

int CSAT(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int variable = 0;
    float max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        float neg_i_score = 0;
        float pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            float length = cnf[ abs(t_cnf[i][j]) ][0];
            if(t_cnf[i][j] < 0) neg_i_score += log( 1 + 1 / (pow(4,length) - pow(2,length+1)) );
            else                pos_i_score += log( 1 + 1 / (pow(4,length) - pow(2,length+1)) );
        }
        if (neg_i_score > pos_i_score){
            float score = neg_i_score + (2.5 * pos_i_score);
            if(score > max_score){
                max_score = score;
                variable = -i;
            }
        }
        else{
            float score = pos_i_score + (2.5 * neg_i_score);
            if(score > max_score){
                max_score = score;
                variable = i;
            }
        }
    }
    erase_formula(t_cnf, 1);                                        // DELETE_0
    return variable;
}

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

