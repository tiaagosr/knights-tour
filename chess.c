#include "chess.h"

extern int tam;

int create_state(state *memAddress, int i, int j){
	int n, n1, *tmpMatrix, **tmpArray;
	if((*memAddress = (state) malloc(sizeof(st))) == NULL){
		return ERROR;
	}
	if((tmpArray = (int**) malloc(sizeof(int*)*tam)) == NULL){
		return ERROR;
	}
	if((tmpMatrix = (int*) malloc(sizeof(int)*pow(tam, 2))) == NULL){
		return ERROR;
	}

	(*memAddress)->turno = 1;
	(*memAddress)->board = tmpArray;
	for(n=0;n < tam;n++){
		(*memAddress)->board[n] = (int *) tmpMatrix+(tam*n);
		for(n1=0;n1 < tam;n1++){
			(*memAddress)->board[n][n1] = 0;
		}
	}

	(*memAddress)->board[i][j] = 1;
	(*memAddress)->i = i;
	(*memAddress)->j = j;

	return SUCCESS;
}

void delete_state(state *memAddress){
	free((*memAddress)->board[0]);
	free((*memAddress)->board);
	free(*memAddress);
	*memAddress = NULL;
}

state branch_state(state *memAddress, int i, int j){
	state newBranch;
	int **tmpArray, *tmpMatrix, n;
	if((newBranch = (state) malloc(sizeof(st))) == NULL){
		return NULL;
	}
	if((tmpArray = (int**) malloc(sizeof(int*)*tam)) == NULL){
		return NULL;
	}
	if((tmpMatrix = (int*) malloc(sizeof(int)*pow(tam, 2))) == NULL){
		return NULL;
	}

	newBranch->turno = (*memAddress)->turno;
	memcpy(tmpMatrix, (*memAddress)->board[0], sizeof(int)*pow(tam, 2));


	newBranch->board = tmpArray;
	for(n=0;n < tam;n++){
		newBranch->board[n] = (int *) tmpMatrix+(tam*n);
	}

	move_knight(newBranch, i, j);

	return newBranch;
}

int move_knight(state move, int i, int j){
    if(move->board[i][j] != 0){
        return ERROR;
    }
    move->board[i][j] = ++(move->turno);
    move->i = i;
    move->j = j;
    return SUCCESS;
}

void print_game(int **matrix){
	int i, j;
	for(i=0;i<tam;i++){
		for(j=0;j<tam;j++){
			printf("%2d ", matrix[i][j]);
		}
		printf("\n");
	}
}
