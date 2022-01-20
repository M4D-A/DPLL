//
// Created by adam on 20.01.2022.
//

#include <stdlib.h>
#include <math.h>
#include "features.h"
#include "vitals.h"

//--$$--DONE--$$--// -- // MEMORY STABLE //
//---HEURISTICS---//
int JW(int** cnf){
    int vars_num = cnf[0][0];
    int clss_num = cnf[0][1];

    int variable = 0;
    double max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        double neg_i_score = 0;
        double pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            double length = cnf[ abs(t_cnf[i][j]) ][0];
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
    double max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        double neg_i_score = 0;
        double pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            double length = cnf[ abs(t_cnf[i][j]) ][0];
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
    double max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        double neg_i_score = 0;
        double pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            double length = cnf[ abs(t_cnf[i][j]) ][0];
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
    double max_score = 0.0;
    int** t_cnf = transpose_cnf(cnf);                               // NEW_0
    int i,j;
    for(i = 1; i <= vars_num; i++){
        double neg_i_score = 0;
        double pos_i_score = 0;
        int occurences = t_cnf[i][0];
        for(j = 1; j <= occurences; j++){
            double length = cnf[ abs(t_cnf[i][j]) ][0];
            if(t_cnf[i][j] < 0) neg_i_score += log( 1 + 1 / (pow(4,length) - pow(2,length+1)) );
            else                pos_i_score += log( 1 + 1 / (pow(4,length) - pow(2,length+1)) );
        }
        if (neg_i_score > pos_i_score){
            double score = neg_i_score + (2.5 * pos_i_score);
            if(score > max_score){
                max_score = score;
                variable = -i;
            }
        }
        else{
            double score = pos_i_score + (2.5 * neg_i_score);
            if(score > max_score){
                max_score = score;
                variable = i;
            }
        }
    }
    erase_formula(t_cnf, 1);                                        // DELETE_0
    return variable;
}
