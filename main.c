using namespace std;
#include "chess.h"

int tam = 5, complete = 0;
pthread_mutex_t lock;

int main(int argc, char *argv[]){
    vector<state> tasks;
    state firstPlay;
    int i=0, j=0, threadNum=2, n;
    pthread_t *threads;


    if(argc < 5){
        printf("Argumentos esperados: <Tamanho do tabuleiro> <Linha Inicial> <Coluna Inicial> <Número de threads>\n");
        return -1;
    }

    tam = atoi(argv[1]);
    i = atoi(argv[2]);
    j = atoi(argv[3]);
    threadNum = atoi(argv[4]);

    if((threads = (pthread_t *) malloc(threadNum * sizeof(pthread_t))) == NULL){
        printf("Erro ao alocar threads!\n");
        return -1;
    }

    if(create_state(&firstPlay, i, j) == ERROR){
        printf("Erro ao alocar primeira jogada!\n");
        return -1;
    }

    tasks.push_back(firstPlay);

    for(n=0;n < threadNum;n++){
        pthread_create(&threads[n], NULL, thread_main, &tasks);
        printf("Criando pthread: %d \n", n);
    }

    for(n=0;n < threadNum;n++){
        pthread_join(threads[n], NULL);
    }

    return 0;
}

void *thread_main(void *args){
    vector<state> *tasks = (vector<state>*) args;
    vector<tuple<int, int, int>> thread_moves;
    tuple<int, int, int> tmpMove, nextMove;
    state tmpState, currentState = NULL, nextState = NULL;
    int i, j;
    while(complete == 0){
        tmpState = NULL;

        if(nextState != NULL){
            //Tem tarefa definida
            currentState = nextState;
        }else{
            pthread_mutex_lock(&lock);
            if(tasks->size() > 0){
                //Busca tarefa no vector para todas as threads
                currentState = tasks->back();;
                tasks->pop_back();
            }
            pthread_mutex_unlock(&lock);
        }

        nextState = NULL;

        if(currentState != NULL && find_moves(currentState, &thread_moves) == 1){
            nextMove = thread_moves.back();
            thread_moves.pop_back();

            for(auto tmpMove: thread_moves){
                i = get<1>(tmpMove);
                j = get<2>(tmpMove);
                tmpState = branch_state(&currentState, i, j);
                pthread_mutex_lock(&lock);
                tasks->push_back(tmpState);
                pthread_mutex_unlock(&lock);
            }
            /* Ordenação do vector compartilhado -> Diminuiu bastante o desempenho
            pthread_mutex_lock(&lock);
            if(tasks->size() > 0){
                sort(begin(*tasks), end(*tasks), [](auto const &t1, auto const &t2){
                    return t1->turno < t2->turno;
                });
            }
            pthread_mutex_unlock(&lock);
            */
            i = get<1>(nextMove);
            j = get<2>(nextMove);

            if(move_knight(currentState, i, j) == SUCCESS){
                nextState = currentState;

                if(nextState->turno >= pow(tam, 2)){
                    //Completou o tabuleiro
                    printf("Completou o tabuleiro!\n");
                    print_game(currentState->board);
                    complete = 1;
                    pthread_exit(NULL);
                }
            }else{
                //Garantir que jogada trancada não realizará movimento
                delete_state(&currentState);
            }
        }else if(currentState != NULL){
            //Jogada que se trancou
            delete_state(&currentState);
        }
    }
    pthread_exit(NULL);
}

int valid_pos(int i, int j){
    if(i < 0 || j < 0 || i >= tam || j >= tam){
        return ERROR;
    }

    return SUCCESS;
}

int count_moves(int **board, int i, int j){
    int score = 1;
    if(i < 0 || j < 0 || i >= tam || j >= tam || board[i][j] != 0){
        return 0;
    }

    score += valid_move(board, i-2, j-1);
    score += valid_move(board, i-2, j+1);
    score += valid_move(board, i-1, j-2);
    score += valid_move(board, i-1, j+2);
    score += valid_move(board, i+1, j-2);
    score += valid_move(board, i+1, j+2);
    score += valid_move(board, i+2, j-1);
    score += valid_move(board, i+2, j+1);

    return score;
}

int valid_move(int **board, int i, int j){
    if(i < 0 || j < 0 || i >= tam || j >= tam){
        return 0;
    }
    if(board[i][j] == 0){
        return 1;
    }

    return 0;
}

//Circulo com jogadas possíveis ao redor da atual
int find_moves(state move, vector<tuple<int, int, int>> *nextPlays){
    nextPlays->clear();
    int score, isBranch = 0;
    int i = (move->i), j = (move->j);

    if((score = count_moves(move->board, i-2, j-1)) > 0){
        nextPlays->push_back(make_tuple(score, i-2, j-1));
    }
    if((score = count_moves(move->board, i-2, j+1)) > 0){
        nextPlays->push_back(make_tuple(score, i-2, j+1));
    }
    if((score = count_moves(move->board, i-1, j-2)) > 0){
        nextPlays->push_back(make_tuple(score, i-1, j-2));
    }
    if((score = count_moves(move->board, i-1, j+2)) > 0){
        nextPlays->push_back(make_tuple(score, i-1, j+2));
    }
    if((score = count_moves(move->board, i+1, j-2)) > 0){
        nextPlays->push_back(make_tuple(score, i+1, j-2));
    }
    if((score = count_moves(move->board, i+1, j+2)) > 0){
        nextPlays->push_back(make_tuple(score, i+1, j+2));
    }
    if((score = count_moves(move->board, i+2, j-1)) > 0){
        nextPlays->push_back(make_tuple(score, i+2, j-1));
    }
    if((score = count_moves(move->board, i+2, j+1)) > 0){
        nextPlays->push_back(make_tuple(score, i+2, j+1));
    }

    if(nextPlays->size() > 0){
        sort(begin(*nextPlays), end(*nextPlays), [](auto const &t1, auto const &t2){
            return get<0>(t1) > get<0>(t2);
        });
        isBranch = 1;
    }

    return isBranch;
}
