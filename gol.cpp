#include <omp.h>
#include <iostream>
#include <stdio.h>
#define HEIGHT 10
#define WIDTH 10

struct Shape {
public:
    char xCoord;
    char yCoord;
    char height;
    char width;
    char **figure;
};

struct Blinker : public Shape {
    static const char BLINKER_HEIGHT = 3;
    static const char BLINKER_WIDTH = 1;
    Blinker( char x , char y );
    ~Blinker();
};

class GameOfLife {
public:
    GameOfLife( Shape sh );
    void print();
    void update();
    char getState( char state , char xCoord , char yCoord , bool toggle);
    void iterate(unsigned int iterations);
private:
    char board[HEIGHT][WIDTH];
    char otherBoard[HEIGHT][WIDTH];
    bool toggle;
    Shape shape;
};

GameOfLife::GameOfLife( Shape sh ) :
    shape(sh) ,
    toggle(true)
{
    for ( char i = 0; i < HEIGHT; i++ ) {
        for ( char j = 0; j < WIDTH; j++ ) {
            board[i][j] = '.';
        }
    }
    for ( char i = shape.yCoord; i - shape.yCoord < shape.height; i++ ) {
        for ( char j = shape.xCoord; j - shape.xCoord < shape.width; j++ ) {
            if ( i < HEIGHT && j < WIDTH ) {
                board[i][j] =
                    shape.figure[ i - shape.yCoord ][j - shape.xCoord ];
            }
        }
    }
}

void GameOfLife::print() {

    if ( toggle ) {
       // Change attempted here
       for ( char i = 0; i < HEIGHT; i++ ) {
       for ( char j = 0; j < WIDTH; j++ ) {
                std::cout << board[i][j];
            }
            std::cout << std::endl;
        }
    } else {
       // Change attempted here
       for ( char i = 0; i < HEIGHT; i++ ) {
            for ( char j = 0; j < WIDTH; j++ ) {
                std::cout << otherBoard[i][j];
            }
            std::cout << std::endl;
        }
    }
    for ( char i = 0; i < WIDTH; i++ ) {
       std::cout << '=';
    }
    std::cout << std::endl;
}

void GameOfLife::update() {
    if (toggle) {
        #pragma omp parallel
        for ( char i = 0; i < HEIGHT; i++ ) {
            #pragma omp for
            for ( char j = 0; j < WIDTH; j++ ) {
                otherBoard[i][j] =
                    GameOfLife::getState(board[i][j] , i , j , toggle);
            }
        }
        toggle = !toggle;
    } else {
        #pragma omp parallel
        for ( char i = 0; i < HEIGHT; i++ ) {
            #pragma omp for
            for ( char j = 0; j < WIDTH; j++ ) {
                board[i][j] =
                    GameOfLife::getState(otherBoard[i][j] , i , j , toggle);
            }
        }
        toggle = !toggle;
    }
}

char GameOfLife::getState( char state, char yCoord, char xCoord, bool toggle ) {
    char neighbors = 0;
    #pragma omp parallel
    {
    if ( toggle ) {
       #pragma omp for reduction(+:neighbors)
       for ( char i = yCoord - 1; i <= yCoord + 1; i++ ) {
            for ( char j = xCoord - 1; j <= xCoord + 1; j++ ) {
                if ( i == yCoord && j == xCoord ) {
                    continue;
                }
                if ( i > -1 && i < HEIGHT && j > -1 && j < WIDTH ) {
                    if ( board[i][j] == 'X' ) {
                        neighbors++;
                    }
                }
            }
        }
     } else {
        #pragma omp for reduction(+:neighbors)
        for ( char i = yCoord - 1; i <= yCoord + 1; i++ ) {
            for ( char j = xCoord - 1; j <= xCoord + 1; j++ ) {
                if ( i == yCoord && j == xCoord ) {
                    continue;
                }
                if ( i > -1 && i < HEIGHT && j > -1 && j < WIDTH ) {
                    if ( otherBoard[i][j] == 'X' ) {
                        neighbors++;
                    }
                }
            }
        }
     }
     }
     if (state == 'X') {
        return ( neighbors > 1 && neighbors < 4 ) ? 'X' : '.';
     }
     else {
        return ( neighbors == 3 ) ? 'X' : '.';
     }
}

void GameOfLife::iterate( unsigned int n ) {
    //cant paralleze this loop without impacting state
    for ( int i = 0; i < n; i++ ) {
        print();
        update();
    }
}

Blinker::Blinker( char x , char y ) {
    xCoord = x;
    yCoord = y;
    height = BLINKER_HEIGHT;
    width = BLINKER_WIDTH;
    figure = new char*[BLINKER_HEIGHT];
    for ( char i = 0; i < BLINKER_HEIGHT; i++ ) {
        figure[i] = new char[BLINKER_WIDTH];
    }
    for ( char i = 0; i < BLINKER_HEIGHT; i++ ) {
        for ( char j = 0; j < BLINKER_WIDTH; j++ ) {
            figure[i][j] = 'X';
        }
    }
}

Blinker::~Blinker() {
    for ( char i = 0; i < BLINKER_HEIGHT; i++ ) {
        delete[] figure[i];
    }
    delete[] figure;
}

int main() {
    Blinker blinker(1,0);
    GameOfLife gameoflife(blinker);

    double start = omp_get_wtime();
    gameoflife.iterate(100000);
    printf("Time: \t %f \n", omp_get_wtime()-start);
}
