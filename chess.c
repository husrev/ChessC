/*  BIL105E Term Project: Game of Chess in C
	M Hüsrev Cilasun, 040120183
	Instructor: Dr Turker Küyel
	@ Istanbul Technical University, 2014

	***** SHORT DESCRIPTION OF CHESS RULES *****
	player is under check && has no move to get rid of check =>> LOSE
	player is not under check && has no move =>> DRAW
	both players have no piece but their kings =>> DRAW
	pawn->queen conversion is ignored.		*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // needed to fix "boolean" variable issue of older DevC++ versions
#include <windows.h> // needed for MessageBox prompts
#include <time.h> // needed for time calculations
#define SCAN_ORDER 4 // amount of moves to predict. SCAN_ORDER (n), it predicts (n+1) moves.
#define TIME_LIMIT 60000 // in terms of milliseconds
int ai_i = 0; // i-coordinate of computer's target
int ai_j = 0; // j-coordinate of computer's target

typedef struct item // piece definition
{
    int icoordinate;
    int jcoordinate;
    char symbol;
    struct item* nextpiece;
} PIECE;

PIECE* pieceref;	// piece pointer for computer's move.

//  print statement for the board
void print(char board[8][16])
{
    printf("\n    Active Pieces      Dead Pieces \n\n");
    int i;
    for(i=0; i<=7; i++)
    {
        printf(" %d  ",7-i+1);
        int j;
        for(j=0; j<=15; j++)
        {
            if(j == 7)
            {
                if(board[i][j] == ' ' && (i+j)%2 == 0)
                    printf("     ");
                else if(board[i][j] == ' ' && (i+j)%2 == 1)
                    printf("X    ");
                else
                    printf("%c    ",board[i][j]);
            }
            else if(j < 7)
            {
                if(board[i][j] == ' ' && (i+j)%2 == 0)
                    printf("  ");
                else if(board[i][j] == ' ' && (i+j)%2 == 1)
                    printf("X ");
                else
                    printf("%c ",board[i][j]);
            }
            else if(j > 7)
            {
                printf("%c ",board[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n    A B C D E F G H \n");
}
// coordinate correcter from A-H to 0-7
int correcter(char i)
{
    if(i == 'A' || i == 'a')
        return 0;
    else if(i == 'B' || i == 'b')
        return 1;
    else if(i == 'C' || i == 'c')
        return 2;
    else if(i == 'D' || i == 'd')
        return 3;
    else if(i == 'E' || i == 'e')
        return 4;
    else if(i == 'F' || i == 'f')
        return 5;
    else if(i == 'G' || i == 'g')
        return 6;
    else if(i == 'H' || i == 'h')
        return 7;
    else
        return 999;
}

// coordinate correcter from 0-7 to A-H
char reverser(int i)
{
    if(i == 0)
        return 'a';
    else if(i == 1)
        return 'b';
    else if(i == 2)
        return 'c';
    else if(i == 3)
        return 'd';
    else if(i == 4)
        return 'e';
    else if(i == 5)
        return 'f';
    else if(i == 6)
        return 'g';
    else if(i == 7)
        return 'h';
    else
        return 'o';
}

// this function locates given piece onto the target coordinate. piece is determined by its coordinate
void locate(int final_i,int final_j,PIECE* pointer,char board[8][16])
{
    board[pointer->icoordinate][pointer->jcoordinate] = ' ';
    pointer->icoordinate = final_i;
    pointer->jcoordinate = final_j;
    board[final_i][final_j] = pointer->symbol;
}

// returns 1 if requested move is a legal move and 0 otherwise.
bool movetest(char c,int i,int js,int ti,int tjs,char board[8][16])
{
    if(ti > 7  || ti < 0 || tjs > 7 || tjs < 0)
    {
        return 0;
    }
    switch(c)
    {
    case 'q':
    case 'Q':
        if(ti == i)
        {
            if(js < tjs)
            {
                int z;
                for(z = js+1; z < tjs; z++)
                    if(board[i][z] != ' ')
                        return 0;
            }
            if(tjs < js)
            {
                int y;
                for(y = tjs+1; y < js; y++)
                    if(board[i][y] != ' ')
                        return 0;
            }
        }
        if(ti == i)
        {
            return 1;
        }
        if(tjs == js)
        {
            if(i < ti)
            {
                int t;
                for(t = i+1; t < ti; t++)
                    if(board[t][js] != ' ')
                        return 0;
            }
            if(ti < i)
            {
                int w;
                for(w = ti+1; w < i; w++)
                    if(board[w][js] != ' ')
                        return 0;
            }
        }
        if(tjs == js)
        {
            return 1;
        }
        if(abs(ti - i) == abs(tjs - js))
        {
            if(ti > i && tjs > js)
            {
                int y,z;
                for(y = i+1, z = js+1; y < ti; y++,z++)
                    if(board[y][z] != ' ')
                        return 0;
            }
            if(ti > i && tjs < js)
            {
                int y,z;
                for(y = i+1, z = js-1; y < ti; y++,z--)
                    if(board[y][z] != ' ')
                        return 0;
            }
            if(ti < i && tjs > js)
            {
                int y,z;
                for(y = i-1, z = js+1; y > ti ; y--,z++)
                    if(board[y][z] != ' ')
                        return 0;
            }
            if(ti < i && tjs < js)
            {
                int y,z;
                for(y = i-1, z = js-1; y > ti; y--,z--)
                    if(board[y][z] != ' ')
                        return 0;
            }
            return 1;
        }
        return 0;
        break;
    case 'P':
        if((board[ti][tjs] == ' ' && tjs == js && (i-ti == 1 || (board[ti+1][tjs] == ' ' && i-ti == 2 && i == 6))) || ( abs(tjs-js) == 1 && i-ti == 1 && (board[ti][tjs] == 'p' || board[ti][tjs] == 'r' || board[ti][tjs] == 'b' || board[ti][tjs] == 'h' || board[ti][tjs] == 'q')))
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 'p':
        if((board[ti][tjs] == ' ' && tjs == js && (ti-i == 1 || (board[ti-1][tjs] == ' ' && ti-i == 2 && i == 1 ))) || ( abs(js-tjs) == 1 && ti-i == 1 && (board[ti][tjs] == 'P' || board[ti][tjs] == 'R' || board[ti][tjs] == 'B' || board[ti][tjs] == 'H' || board[ti][tjs] == 'Q')))
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 'K':
    case 'k':
        if(abs(tjs-js) < 2 && abs(ti-i) < 2)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 'H':
    case 'h':
        if((abs(ti-i) == 1 && abs(tjs-js) ==2) || (abs(ti-i) == 2 && abs(tjs-js) == 1))
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 'R':
    case 'r':
        if(ti == i)
        {
            if(js < tjs)
            {
                int z;
                for(z = js+1; z < tjs; z++)
                    if(board[i][z] != ' ')
                        return 0;
            }
            if(tjs < js)
            {
                int y;
                for(y = tjs+1; y < js; y++)
                    if(board[i][y] != ' ')
                        return 0;
            }
            return 1;
        }
        if(tjs == js)
        {
            if(i < ti)
            {
                int t;
                for(t = i+1; t < ti; t++)
                    if(board[t][js] != ' ')
                        return 0;
            }
            if(ti < i)
            {

                int w;
                for(w = ti+1; w < i; w++)
                    if(board[w][js] != ' ')
                        return 0;
            }
            return 1;
        }
        return 0;
        break;
    case 'B':
    case 'b':
        if(abs(ti - i) == abs(tjs - js))
        {
            if(ti > i && tjs > js)
            {
                int y,z;
                for(y = i+1, z = js+1; y < ti; y++,z++)
                    if(board[y][z] != ' ')
                        return 0;
            }
            if(ti > i && tjs < js)
            {
                int y,z;
                for(y = i+1, z = js-1; y < ti; y++,z--)
                    if(board[y][z] != ' ')
                        return 0;
            }
            if(ti < i && tjs > js)
            {
                int y,z;
                for(y = i-1, z = js+1; y > ti ; y--,z++)
                    if(board[y][z] != ' ')
                        return 0;
            }
            if(ti < i && tjs < js)
            {
                int y,z;
                for(y = i-1, z = js-1; y > ti; y--,z--)
                    if(board[y][z] != ' ')
                        return 0;
            }
            return 1;
        }
        return 0;
    }
}

bool checksearch(PIECE* pointer,char board[8][16])
{
    while(pointer != NULL)
    {
        int m,n;
        switch(pointer->symbol)
        {
        case 'P':
            if(board[pointer->icoordinate-1][pointer->jcoordinate-1] == 'k' || board[pointer->icoordinate-1][pointer->jcoordinate+1] == 'k' )
                return 1;
            break;
        case 'p':
            if(board[pointer->icoordinate+1][pointer->jcoordinate-1] == 'K' || board[pointer->icoordinate+1][pointer->jcoordinate+1] == 'K' )
                return 1;
            break;
        case 'k':
            if(board[pointer->icoordinate-1][pointer->jcoordinate-1] == 'K' || board[pointer->icoordinate-1][pointer->jcoordinate] == 'K' || board[pointer->icoordinate-1][pointer->jcoordinate+1] == 'K' || board[pointer->icoordinate][pointer->jcoordinate-1] == 'K' || board[pointer->icoordinate][pointer->jcoordinate+1] == 'K' || board[pointer->icoordinate+1][pointer->jcoordinate-1] == 'K' || board[pointer->icoordinate+1][pointer->jcoordinate] == 'K' || board[pointer->icoordinate+1][pointer->jcoordinate+1] == 'K')
                return 1;
            break;
        case 'K':
            if(board[pointer->icoordinate-1][pointer->jcoordinate-1] == 'k' || board[pointer->icoordinate-1][pointer->jcoordinate] == 'k' || board[pointer->icoordinate-1][pointer->jcoordinate+1] == 'k' || board[pointer->icoordinate][pointer->jcoordinate-1] == 'k' || board[pointer->icoordinate][pointer->jcoordinate+1] == 'k' || board[pointer->icoordinate+1][pointer->jcoordinate-1] == 'k' || board[pointer->icoordinate+1][pointer->jcoordinate] == 'k' || board[pointer->icoordinate+1][pointer->jcoordinate+1] == 'k')
                return 1;
            break;
        case 'h':
            if(board[pointer->icoordinate-1][pointer->jcoordinate-2] == 'K' || board[pointer->icoordinate-1][pointer->jcoordinate+2] == 'K' || board[pointer->icoordinate-2][pointer->jcoordinate+1] == 'K' || board[pointer->icoordinate-2][pointer->jcoordinate-1] == 'K' || board[pointer->icoordinate+1][pointer->jcoordinate-2] == 'K' || board[pointer->icoordinate+1][pointer->jcoordinate+2] == 'K' || board[pointer->icoordinate+2][pointer->jcoordinate-1] == 'K' || board[pointer->icoordinate+2][pointer->jcoordinate+1] == 'K')
                return 1;
            break;
        case 'H':
            if(board[pointer->icoordinate-1][pointer->jcoordinate-2] == 'k' || board[pointer->icoordinate-1][pointer->jcoordinate+2] == 'k' || board[pointer->icoordinate-2][pointer->jcoordinate+1] == 'k' || board[pointer->icoordinate-2][pointer->jcoordinate-1] == 'k' || board[pointer->icoordinate+1][pointer->jcoordinate-2] == 'k' || board[pointer->icoordinate+1][pointer->jcoordinate+2] == 'k' || board[pointer->icoordinate+2][pointer->jcoordinate-1] == 'k' || board[pointer->icoordinate+2][pointer->jcoordinate+1] == 'k')
                return 1;
            break;
        case 'r':
            for(n = pointer->icoordinate+1; n <= 7 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'K'); n++)
                if(board[n][pointer->jcoordinate] == 'K')
                    return 1;
            for(n = pointer->jcoordinate+1; n <= 7 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'K'); n++)
                if(board[pointer->icoordinate][n] == 'K')
                    return 1;
            for(n = pointer->icoordinate-1; n >= 0 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'K'); n--)
                if(board[n][pointer->jcoordinate] == 'K')
                    return 1;
            for(n = pointer->jcoordinate-1; n >= 0 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'K'); n--)
                if(board[pointer->icoordinate][n] == 'K')
                    return 1;
            break;
        case 'R':
            for(n = pointer->icoordinate+1; n <= 7 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'k'); n++)
                if(board[n][pointer->jcoordinate] == 'k')
                    return 1;
            for(n = pointer->jcoordinate+1; n <= 7 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'k'); n++)
                if(board[pointer->icoordinate][n] == 'k')
                    return 1;
            for(n = pointer->icoordinate-1; n >= 0 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'k'); n--)
                if(board[n][pointer->jcoordinate] == 'k')
                    return 1;
            for(n = pointer->jcoordinate-1; n >= 0 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'k'); n--)
                if(board[pointer->icoordinate][n] == 'k')
                    return 1;
            break;
        case 'b':
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate+1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'K'); m++, n++)
                if(board[m][n] == 'K')
                    return 1;
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate-1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'K'); m++, n--)
                if(board[m][n] == 'K')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate+1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'K'); m--, n++)
                if(board[m][n] == 'K')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate-1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'K'); m--, n--)
                if(board[m][n] == 'K')
                    return 1;
            break;
        case 'B':
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate+1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'k'); m++, n++)
                if(board[m][n] == 'k')
                    return 1;
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate-1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'k'); m++, n--)
                if(board[m][n] == 'k')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate+1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'k'); m--, n++)
                if(board[m][n] == 'k')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate-1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'k'); m--, n--)
                if(board[m][n] == 'k')
                    return 1;
            break;
        case 'Q':
            // rook's code
            for(n = pointer->icoordinate+1; n <= 7 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'k'); n++)
                if(board[n][pointer->jcoordinate] == 'k')
                    return 1;
            for(n = pointer->jcoordinate+1; n <= 7 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'k'); n++)
                if(board[pointer->icoordinate][n] == 'k')
                    return 1;
            for(n = pointer->icoordinate-1; n >= 0 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'k'); n--)
                if(board[n][pointer->jcoordinate] == 'k')
                    return 1;
            for(n = pointer->jcoordinate-1; n >= 0 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'k'); n--)
                if(board[pointer->icoordinate][n] == 'k')
                    return 1;
            // bishop's code
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate+1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'k'); m++, n++)
                if(board[m][n] == 'k')
                    return 1;
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate-1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'k'); m++, n--)
                if(board[m][n] == 'k')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate+1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'k'); m--, n++)
                if(board[m][n] == 'k')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate-1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'k'); m--, n--)
                if(board[m][n] == 'k')
                    return 1;
            break;
        case 'q':
            // copy of rook's code
            for(n = pointer->icoordinate+1; n <= 7 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'K'); n++)
                if(board[n][pointer->jcoordinate] == 'K')
                    return 1;
            for(n = pointer->jcoordinate+1; n <= 7 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'K'); n++)
                if(board[pointer->icoordinate][n] == 'K')
                    return 1;
            for(n = pointer->icoordinate-1; n >= 0 && (board[n][pointer->jcoordinate] == ' ' || board[n][pointer->jcoordinate] == 'K'); n--)
                if(board[n][pointer->jcoordinate] == 'K')
                    return 1;
            for(n = pointer->jcoordinate-1; n >= 0 && (board[pointer->icoordinate][n] == ' ' || board[pointer->icoordinate][n] == 'K'); n--)
                if(board[pointer->icoordinate][n] == 'K')
                    return 1;
            // copy of bishop's code
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate+1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'K'); m++, n++)
                if(board[m][n] == 'K')
                    return 1;
            for(m = pointer->icoordinate+1, n = pointer->jcoordinate-1; m <= 7 && (board[m][n] == ' ' || board[m][n] == 'K'); m++, n--)
                if(board[m][n] == 'K')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate+1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'K'); m--, n++)
                if(board[m][n] == 'K')
                    return 1;
            for(m = pointer->icoordinate-1, n = pointer->jcoordinate-1; m >= 0 && (board[m][n] == ' ' || board[m][n] == 'K'); m--, n--)
                if(board[m][n] == 'K')
                    return 1;
            break;
        default:
            return 0;
            break;
        }
        pointer = pointer->nextpiece;
    }
    return 0;
}


// estimated costs for each piece
int cost(char c)
{
    if(c == 'P')
    {
        return 1;
    }
    else if(c == 'p')
    {
        return -1;
    }
    else if(c == 'H' || c == 'B')
    {
        return 3;
    }
    else if(c == 'h' || c == 'b')
    {
        return -3;
    }
    else if(c == 'R' )
    {
        return 5;
    }
    else if(c == 'r' )
    {
        return -5;
    }
    else if(c == 'Q')
    {
        return 9;
    }
    else if(c == 'q')
    {
        return -9;
    }
    else if(c == 'K' )
    {
        return 999;
    }
    else if(c == 'k' )
    {
        return -999;
    }
    else
    {
        return 0;
    }
}

//board array in two dimensions
char board[8][16] =
{
    {' ',' ',' ',' ',' ',' ',' ',' ','-','-','-','-','-','-','-','-'},
    {' ',' ',' ',' ',' ',' ',' ',' ','-','-','-','-','-','-','-','-'},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ','-','-','-','-','-','-','-','-'},
    {' ',' ',' ',' ',' ',' ',' ',' ','-','-','-','-','-','-','-','-'}
};

// determines its i-coordinate if a piece is assumed to die.
int dead_i(char x)
{
    switch(x)
    {
    case 'R':
        return 7;
        break;
    case 'K':
        return 7;
        break;
    case 'Q':
        return 7;
        break;
    case 'B':
        return 7;
        break;
    case 'H':
        return 7;
        break;
    case 'P':
        return 6;
        break;
    case 'r':
        return 0;
        break;
    case 'k':
        return 0;
        break;
    case 'q':
        return 0;
        break;
    case 'b':
        return 0;
        break;
    case 'h':
        return 0;
        break;
    case 'p':
        return 1;
        break;
    }
}

// determines its j-coordinate if a piece is assumed to die.
int dead_j(char x,char board[8][16])
{
    int n;
    switch(x)
    {
    case 'R':
        if(board[7][8] == 'R')
            return 15;
        return 8;
        break;
    case 'P':
        for(n=0; n<16; n++)
            if(board[6][n] == '-')
                return n;
        break;
    case 'H':
        if(board[7][9] == 'H')
            return 14;
        return 9;
        break;
    case 'B':
        if(board[7][10] == 'B')
            return 13;
        return 10;
        break;
    case 'Q':
        return 12;
        break;
    case 'r':
        if(board[0][8] == 'r')
            return 15;
        return 8;
        break;
    case 'h':
        if(board[0][9] == 'h')
            return 14;
        return 9;
        break;
    case 'b':
        if(board[0][10] == 'b')
            return 13;
        return 10;
        break;
    case 'p':
        for(n=0; n<16; n++)
            if(board[1][n] == '-')
                return n;
        break;
    case 'q':
        return 12;
        break;
    }
}

// places each piece onto the board
void update(PIECE* bk,PIECE* wk,char board[8][16])
{
    // white-linked list places active pieces on board.
    while(wk != NULL)
    {
        board[wk->icoordinate][wk->jcoordinate] = wk->symbol;
        wk = wk->nextpiece;
    }
    // black-linked list places active pieces on board.

    while(bk != NULL)
    {
        board[bk->icoordinate][bk->jcoordinate] = bk->symbol;
        bk = bk->nextpiece;
    }
}

// deletes the given piece from the linked list and returns its previous piece.
PIECE* delete_from_list(PIECE* pointer,PIECE* previous)
{
    while(previous->nextpiece != pointer)
        previous = previous->nextpiece;
    if(pointer->nextpiece == NULL)
    {
        previous->nextpiece = NULL;
    }
    else
    {
        previous->nextpiece = pointer->nextpiece;
    }
}

// kills the piece in given coordinate and returns its previous piece in linked list
PIECE* kill(int coord_i, int coord_j,PIECE* colorptr,PIECE* other,char board[8][16])
{
    PIECE* pointer = colorptr;
    PIECE* r;
    while(pointer != NULL)
    {
        if(pointer->icoordinate == coord_i && pointer->jcoordinate == coord_j)
        {
//           pointer->alive = 0;
            board[dead_i(pointer->symbol)][dead_j(pointer->symbol,board)] = pointer->symbol;
            r = delete_from_list(pointer,colorptr);
//          update(colorptr,other,board);
            break;
        }
        pointer = pointer->nextpiece;
    }
    return r;
}

// sometimes I need to revive some piece. this function re-orders the chars of "DEAD PIECES" section
int dead_j_reverse(char x,char board[8][16])
{
    int n;
    switch(x)
    {
    case 'R':
        if(board[7][15] == '-')
            return 8;
        return 15;
        break;
    case 'H':
        if(board[7][14] == '-')
            return 9;
        return 14;
        break;
    case 'B':
        if(board[7][13] == '-')
            return 10;
        return 13;
        break;
    case 'P':
        for(n=15; n>=0; n--)
            if(board[6][n] == 'P')
                return n;
        break;
    case 'Q':
        return 12;
        break;
    case 'r':
        if(board[0][15] == '-')
            return 8;
        return 15;
        break;
    case 'h':
        if(board[0][14] == '-')
            return 9;
        return 14;
        break;
    case 'b':
        if(board[0][13] == '-')
            return 10;
        return 13;
        break;
    case 'q':
        return 12;
        break;
    case 'p':
        for(n=15; n>=0; n--)
            if(board[1][n] == 'p')
                return n;
        break;
    }
}

// this function revives given piece after its assumptional death.
void revive(PIECE* dead,PIECE* colorptr,PIECE* other,char board[8][16],PIECE* rescuer)
{
//  dead->alive = 1;
    //  update(colorptr,other,board);
    rescuer->nextpiece = dead;
    board[dead_i(dead->symbol)][dead_j_reverse(dead->symbol,board)] = '-';
}

// unfortunately, these reference variables are declared globally.
// they will be called inside recursions and it would be so complicated to make them local variables.
int* costref;
int altxfiveref = 0;
int altxfourref = 0;
int altaltaltref=0;
int altaltref=0;
int altref=0;
int evaluate();
double difference;
PIECE* pref;
PIECE* search(int i, int j, char board[8][16],PIECE* pointer)
{
    while(pointer != NULL)
    {
        if(pointer->icoordinate == i && pointer->jcoordinate == j)
            return pointer;
        pointer = pointer->nextpiece;
    }
}
// nonrecursed process is needed for check of 'check' << check_evaluate function calls this.
int nonrecursed(PIECE* pointer, int c_i, int c_j, char board[8][16],PIECE* enemy,PIECE* current)
{
    if(movetest(pointer->symbol,pointer->icoordinate,pointer->jcoordinate,c_i,c_j,board) == 1 && (board[c_i][c_j] != 'R' && board[c_i][c_j] != 'K' && board[c_i][c_j] != 'B' && board[c_i][c_j] != 'H'  && board[c_i][c_j] != 'P' && board[c_i][c_j] != 'Q'))
    {
        // start
        int ri = pointer->icoordinate,rj = pointer->jcoordinate;
        PIECE* dead = enemy;
        PIECE* rescuer;
        // kill the target piece if exists. finds the 'dead' piece then.
        bool flag = 0;
        if(board[c_i][c_j] != ' ' && board[c_i][c_j] != 'k' && board[c_i][c_j] != 'R' && board[c_i][c_j] != 'K' &&board[c_i][c_j] != 'B' &&board[c_i][c_j] != 'H' &&board[c_i][c_j] != 'P' &&board[c_i][c_j] != 'Q')
        {
            while(dead != NULL)
            {
                if(dead->icoordinate == c_i && dead->jcoordinate == c_j)
                    break;
                dead = dead->nextpiece;
            }
            flag = 1;
            rescuer = kill(c_i,c_j,enemy,current,board);
        }
        // place the current piece onto target coordinate
        locate(c_i,c_j,pointer,board);
        update(current,enemy,board);
        // if this does not cause check, store move details @costref @ai_i @ai_j @pieceref
        if(checksearch(enemy,board) == 0)
            pref = pointer;
        // relocate the piece
        locate(ri,rj,pointer,board);
        if(flag == 1)
            revive(dead,enemy,current,board,rescuer);
        update(current,enemy,board);
    }
}
// the following code was repeated in my main recursion function "evaluate()" so i decided to seperate
// out into the "process() function to save almost 1000 lines of code :D
int process(PIECE* pointer, int *costref, int c_i, int c_j, char board[8][16],PIECE* enemy,PIECE* current,int scan_order,int times,int timed)
{
    int reference;
    if(movetest(pointer->symbol,pointer->icoordinate,pointer->jcoordinate,c_i,c_j,board) == 1 && ((current->symbol == 'k' && board[c_i][c_j] != 'r' && board[c_i][c_j] != 'k' && board[c_i][c_j] != 'b' && board[c_i][c_j] != 'h'  && board[c_i][c_j] != 'p' && board[c_i][c_j] != 'q') || (current->symbol == 'K' && board[c_i][c_j] != 'R' && board[c_i][c_j] != 'K' && board[c_i][c_j] != 'B' && board[c_i][c_j] != 'H'  && board[c_i][c_j] != 'P' && board[c_i][c_j] != 'Q')))
    {
        int ri = pointer->icoordinate, rj = pointer->jcoordinate;
        reference = cost(board[c_i][c_j]);
        PIECE* dead = enemy;
        PIECE* rescuer;
        // kill the target piece if exists
        bool flag = 0;
        if(board[c_i][c_j] != ' ' && ((current->symbol == 'k' && board[c_i][c_j] != 'K' && board[c_i][c_j] != 'r' && board[c_i][c_j] != 'k' &&board[c_i][c_j] != 'b' &&board[c_i][c_j] != 'h' &&board[c_i][c_j] != 'p' &&board[c_i][c_j] != 'q') || (current->symbol == 'K' && board[c_i][c_j] != 'R' && board[c_i][c_j] != 'k' &&board[c_i][c_j] != 'K' &&board[c_i][c_j] != 'B' &&board[c_i][c_j] != 'H' &&board[c_i][c_j] != 'P' &&board[c_i][c_j] != 'Q')))
        {
            while(dead != NULL)
            {
                if(dead->icoordinate == c_i && dead->jcoordinate == c_j)
                    break;
                dead = dead->nextpiece;
            }
            flag = 1;
            rescuer = delete_from_list(dead,enemy);
            board[dead->icoordinate][dead->jcoordinate] = ' ';
        }
        // place the current piece onto target coordinate
        locate(c_i,c_j,pointer,board);
        int gelen;
        if(scan_order > 0)
        {
            if(scan_order == SCAN_ORDER)
            {
                altref = 500;
            }
            else if(scan_order == SCAN_ORDER-1)
            {
                altaltref = -500;
            }
            else if(scan_order == SCAN_ORDER-2)
            {
                altaltaltref = 500;
            }
            else if(scan_order == SCAN_ORDER-3)
            {
                altxfourref = -500;
            }
            else if(scan_order == SCAN_ORDER-4)
            {
                altxfiveref = 500;
            }
            gelen=evaluate(enemy,current,--scan_order,*costref,times,timed);
            if(abs(gelen) > abs(reference))
                reference = gelen;
            scan_order++;
        }
        if(checksearch(enemy,board) == 0)
        {
            if(reference <= altxfiveref && scan_order == SCAN_ORDER-5)
            {
                altxfiveref = reference;
            }
            else if(reference >= altxfourref && scan_order == SCAN_ORDER-4)
            {
                altxfourref = reference;
            }
            else if(reference <= altaltaltref && scan_order == SCAN_ORDER-3)
            {
                altaltaltref = reference;
            }
            else if(reference >= altaltref && scan_order == SCAN_ORDER-2)
            {
                altaltref = reference;
            }
            else if(reference <= altref && scan_order == SCAN_ORDER-1)
            {
                altref = reference;
            }
            else if(reference >= *costref && scan_order == SCAN_ORDER)
            {
                *costref = reference;
                ai_i = c_i;
                ai_j = c_j;
                pieceref = pointer;
            }
        }
        //_________________________MY DEBUG KIT :D_______________________
        /*	char jjj = (pieceref == NULL) ? ('x') : (pieceref->symbol);
        	system("cls");
        	print(board);
        	printf(" reference:%d costref:%d altref:%d altaltref:%d altaltaltref:%d altxfourref: %d \n scan order:%d  ai_i: %d c_i: %d pieceref:%c",reference,*costref,altref,altaltref,altaltaltref,altxfourref,scan_order,ai_i,c_i,jjj);
        	MessageBox(0,"Hello","Caption",MB_OK);*/
        // relocate the piece
        locate(ri,rj,pointer,board);
        if(flag == 1)
        {
            rescuer->nextpiece = dead;
            board[dead->icoordinate][dead->jcoordinate] = dead->symbol;
        }
        update(current,enemy,board);
    }

    if(scan_order == SCAN_ORDER-5)
    {
        return altxfiveref;
    }
    else if(scan_order == SCAN_ORDER-4)
    {
        return altxfourref;
    }
    else if(scan_order == SCAN_ORDER-3)
    {
        return altaltaltref;
    }
    else if(scan_order == SCAN_ORDER-2)
    {
        return altaltref;
    }
    else if(scan_order == SCAN_ORDER-1)
    {
        return altref;
    }
}

// the main recursion function. 'evaluate's the legal moves of given color and
// chooses the move whose cost is greatest. when I try to recurse it, it got so
// complicated and variation of parameters has been scaled up. then i decided to
// create two discrete functions 'evaluate' and 'process' which are recursing
// by calling each other.

int evaluate(PIECE* current,PIECE* enemy,int scan_order,int cst,int times, int timed)
{
    int t=0,p=0;
    PIECE* pointer = current;
    int* costref = &cst;
    int reference;
    while(pointer != NULL && (difference < TIME_LIMIT || pieceref == NULL))
    {
        timed = clock();
        difference = timed-times;
        if(pointer->symbol == 'q' || pointer->symbol == 'Q')
        {
            // ROOK's CODE
            for(t = (pointer->icoordinate+1); t<=7; t++)
                reference=process(pointer,costref,t,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate-1); t>=0; t--)
                reference=process(pointer,costref,t,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
            for(t = pointer->jcoordinate+1; t<=7; t++)
                reference=process(pointer,costref,pointer->icoordinate,t,board,enemy,current,scan_order,times,timed);
            for(t = pointer->jcoordinate-1; t>=0; t--)
                reference=process(pointer,costref,pointer->icoordinate,t,board,enemy,current,scan_order,times,timed);
            // BISHOP's CODE
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate+1); t<=7,p<=7; t++,p++)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate-1); t<=7,p>=0; t++,p--)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate+1); t>=0,p<=7; t--,p++)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate-1); t>=0,p>=0; t--,p--)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
        }
        else if(pointer->symbol == 'r' || pointer->symbol == 'R')
        {
            for(t = (pointer->icoordinate+1); t<=7; t++)
                reference=process(pointer,costref,t,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate-1); t>=0; t--)
                reference=process(pointer,costref,t,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
            for(t = pointer->jcoordinate+1; t<=7; t++)
                reference=process(pointer,costref,pointer->icoordinate,t,board,enemy,current,scan_order,times,timed);
            for(t = pointer->jcoordinate-1; t>=0; t--)
                reference=process(pointer,costref,pointer->icoordinate,t,board,enemy,current,scan_order,times,timed);
        }
        else if(pointer->symbol == 'b' || pointer->symbol == 'B')
        {
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate+1); t<=7,p<=7; t++,p++)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate-1); t<=7,p>=0; t++,p--)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate+1); t>=0,p<=7; t--,p++)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate-1); t>=0,p>=0; t--,p--)
                reference=process(pointer,costref,t,p,board,enemy,current,scan_order,times,timed);
        }
        else if(pointer->symbol == 'k' || pointer->symbol == 'K')
        {
            for(p = -1; p <= 1; p++)
                for(t = -1; t <= 1; t ++)
                    if( pointer->icoordinate+t > -1 && pointer->icoordinate+t < 8 && pointer->jcoordinate+p > -1 && pointer->jcoordinate+p < 8 && !(p==0 && t==0))
                        reference=process(pointer,costref,pointer->icoordinate+t,pointer->jcoordinate+p,board,enemy,current,scan_order,times,timed);
        }
        else if(pointer->symbol == 'h' || pointer->symbol == 'H')
        {
            for(p = -2; p <= 2; p++)
                for(t = -2; t <= 2; t++)
                    if( pointer->icoordinate+t > -1 && pointer->icoordinate+t < 8 && pointer->jcoordinate+p > -1 && pointer->jcoordinate+p < 8 && ((abs(p)==1 && abs(t)==2) || (abs(p)==2 && abs(t)==1)))
                        reference=process(pointer,costref,pointer->icoordinate+t,pointer->jcoordinate+p,board,enemy,current,scan_order,times,timed);
        }
        else if(pointer->symbol == 'p')
        {
            reference=process(pointer,costref,pointer->icoordinate+1,pointer->jcoordinate+1,board,enemy,current,scan_order,times,timed);
            reference=process(pointer,costref,pointer->icoordinate+1,pointer->jcoordinate-1,board,enemy,current,scan_order,times,timed);
            reference=process(pointer,costref,pointer->icoordinate+1,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
            if(board[pointer->icoordinate+1][pointer->jcoordinate] == ' ' )
                reference=process(pointer,costref,pointer->icoordinate+2,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
        }
        else if(pointer->symbol == 'P')
        {
            reference=process(pointer,costref,pointer->icoordinate-1,pointer->jcoordinate+1,board,enemy,current,scan_order,times,timed);
            reference=process(pointer,costref,pointer->icoordinate-1,pointer->jcoordinate-1,board,enemy,current,scan_order,times,timed);
            reference=process(pointer,costref,pointer->icoordinate-1,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
            if(board[pointer->icoordinate-1][pointer->jcoordinate] == ' ' )
                reference=process(pointer,costref,pointer->icoordinate-2,pointer->jcoordinate,board,enemy,current,scan_order,times,timed);
        }
        pointer = pointer->nextpiece;
    }
    return reference;
}


void check_evaluate(PIECE* current, PIECE* enemy)
{
    int i=0,j=0,t=0,p=0;
    pieceref = NULL;
    PIECE* pointer = current;
    while(pointer != NULL)
    {
        if(pointer->symbol == 'Q')
        {
            // ROOK's CODE
            for(t = (pointer->icoordinate+1); t<=7; t++)
                nonrecursed(pointer,t,pointer->jcoordinate,board,enemy,current);
            for(t = (pointer->icoordinate-1); t>=0; t--)
                nonrecursed(pointer,t,pointer->jcoordinate,board,enemy,current);
            for(t = pointer->jcoordinate+1; t<=7; t++)
                nonrecursed(pointer,pointer->icoordinate,t,board,enemy,current);
            for(t = pointer->jcoordinate-1; t>=0; t--)
                nonrecursed(pointer,pointer->icoordinate,t,board,enemy,current);
            // BISHOP's CODE
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate+1); t<=7,p<=7; t++,p++)
                nonrecursed(pointer,t,p,board,enemy,current);
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate-1); t<=7,p>=0; t++,p--)
                nonrecursed(pointer,t,p,board,enemy,current);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate+1); t>=0,p<=7; t--,p++)
                nonrecursed(pointer,t,p,board,enemy,current);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate-1); t>=0,p>=0; t--,p--)
                nonrecursed(pointer,t,p,board,enemy,current);
        }

        else if(pointer->symbol == 'R')
        {
            for(t = (pointer->icoordinate+1); t<=7; t++)
                nonrecursed(pointer,t,pointer->jcoordinate,board,enemy,current);
            for(t = (pointer->icoordinate-1); t>=0; t--)
                nonrecursed(pointer,t,pointer->jcoordinate,board,enemy,current);
            for(t = pointer->jcoordinate+1; t<=7; t++)
                nonrecursed(pointer,pointer->icoordinate,t,board,enemy,current);
            for(t = pointer->jcoordinate-1; t>=0; t--)
                nonrecursed(pointer,pointer->icoordinate,t,board,enemy,current);
        }

        else if(pointer->symbol == 'B')
        {
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate+1); t<=7,p<=7; t++,p++)
                nonrecursed(pointer,t,p,board,enemy,current);
            for(t = (pointer->icoordinate+1),p = (pointer->jcoordinate-1); t<=7,p>=0; t++,p--)
                nonrecursed(pointer,t,p,board,enemy,current);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate+1); t>=0,p<=7; t--,p++)
                nonrecursed(pointer,t,p,board,enemy,current);
            for(t = (pointer->icoordinate-1),p = (pointer->jcoordinate-1); t>=0,p>=0; t--,p--)
                nonrecursed(pointer,t,p,board,enemy,current);
        }

        else if(pointer->symbol == 'K')
        {
            for(j = -1; j <= 1; j++)
                for(i = -1; i <= 1; i++)
                    if( pointer->icoordinate+i > -1 && pointer->icoordinate+i < 8 && pointer->jcoordinate+j > -1 && pointer->jcoordinate+j < 8 && (board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'R' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'K' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'B' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'H' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'P' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'Q'))
                        nonrecursed(pointer,pointer->icoordinate+i,pointer->jcoordinate+j,board,enemy,current);
        }

        else if(pointer->symbol == 'H')
        {
            for(j = -2; j <= 2; j++)
                for(i = -2; i <= 2; i++)
                    if( pointer->icoordinate+i > -1 && pointer->icoordinate+i < 8 && pointer->jcoordinate+j > -1 && pointer->jcoordinate+j < 8 && (board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'R' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'K' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'B' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'H' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'P' && board[pointer->icoordinate+i][pointer->jcoordinate+j] != 'Q'))
                        nonrecursed(pointer,pointer->icoordinate+i,pointer->jcoordinate+j,board,enemy,current);
        }

        else if(pointer->symbol == 'P')
        {
            nonrecursed(pointer,pointer->icoordinate-1,pointer->jcoordinate+1,board,enemy,current);
            nonrecursed(pointer,pointer->icoordinate-1,pointer->jcoordinate-1,board,enemy,current);
            nonrecursed(pointer,pointer->icoordinate-1,pointer->jcoordinate,board,enemy,current);
            if(board[pointer->icoordinate-1][pointer->jcoordinate] == ' ')
                nonrecursed(pointer,pointer->icoordinate-2,pointer->jcoordinate,board,enemy,current);
        }
        pointer = pointer->nextpiece;
    }
}

int main()
{

    PIECE bpawn8 = {1,7,'p',NULL};
    PIECE bpawn7 = {1,6,'p',&bpawn8};
    PIECE bpawn6 = {1,5,'p',&bpawn7};
    PIECE bpawn5 = {1,4,'p',&bpawn6};
    PIECE bpawn4 = {1,3,'p',&bpawn5};
    PIECE bpawn3 = {1,2,'p',&bpawn4};
    PIECE bpawn2 = {1,1,'p',&bpawn3};
    PIECE bpawn1 = {1,0,'p',&bpawn2};
    PIECE bbishopr = {0,5,'b',&bpawn1};
    PIECE bbishopl = {0,2,'b',&bbishopr};
    PIECE brookr = {0,7,'r',&bbishopl};
    PIECE brookl = {0,0,'r',&brookr};
    PIECE bhorser = {0,6,'h',&brookl};
    PIECE bhorsel = {0,1,'h',&bhorser};
    PIECE bqueen = {0,3,'q',&bhorsel};
    PIECE bking = {0,4,'k',&bqueen};

    PIECE wpawn8 = {6,7,'P',NULL};
    PIECE wpawn7 = {6,6,'P',&wpawn8};
    PIECE wpawn6 = {6,5,'P',&wpawn7};
    PIECE wpawn5 = {6,4,'P',&wpawn6};
    PIECE wpawn4 = {6,3,'P',&wpawn5};
    PIECE wpawn3 = {6,2,'P',&wpawn4};
    PIECE wpawn2 = {6,1,'P',&wpawn3};
    PIECE wpawn1 = {6,0,'P',&wpawn2};
    PIECE wbishopr = {7,5,'B',&wpawn1};
    PIECE wbishopl = {7,2,'B',&wbishopr};
    PIECE wrookr = {7,7,'R',&wbishopl};
    PIECE wrookl = {7,0,'R',&wrookr};
    PIECE whorser = {7,6,'H',&wrookl};
    PIECE whorsel = {7,1,'H',&whorser};
    PIECE wqueen = {7,3,'Q',&whorsel};
    PIECE wking = {7,4,'K',&wqueen};

    char j,c,tj;
    int i,js,ti,tjs,dead_i,dead_j;
    bool ai_wins = 0, human_wins = 0,draw = 0;
    PIECE* rescuer;

    FILE *log;
    if((log = fopen( "log.txt", "w+")) == NULL)
        MessageBox(0,"Unable to access log file","Alert",MB_OK);
    fclose(log);
    system("cls");
    update(&bking,&wking,board);
    print(board);
    void quit(void)
    {
        if((log = fopen( "log.txt", "a")) == NULL)
            MessageBox(0,"Unable to access log file","Alert",MB_OK);
        fprintf(log,"\n FINAL LOOK\n\n    Active Pieces      Dead Pieces \n\n");
        for(i=0; i<=7; i++)
        {
            fprintf(log," %d  ",7-i+1);
            int j;
            for(j=0; j<=15; j++)
            {
                if(j == 7)
                {
                    fprintf(log,"%c    ",board[i][j]);
                }
                else
                {
                    fprintf(log,"%c ",board[i][j]);
                }
            }
            fprintf(log,"\n");
        }
        fprintf(log,"\n    A B C D E F G H \n");
        fclose( log );
    }
    atexit(quit);
    printf("\n >> Do the best of you can!\n ");
    // HERE STARTS THE MAIN PROCESS LOOP
    while(ai_wins == 0 && human_wins == 0 && draw == 0)
    {
        // the very next loop iterates until check conditions are eliminated.
        while(1)
        {
            bool death = 0;
            bool flag = 0;
            checksearch(&bking,board) == 0 ? (flag = 0) : (flag = 1);
            flag == 0 ? : MessageBox(0,"Your king is under threat!","Check!",MB_OK);
            pref = NULL;
            check_evaluate(&wking,&bking);
            if(pref == NULL && flag == 0)
            {
                draw = 1;
                break;
            }
            if(pref == NULL && flag == 1)
            {
                ai_wins = 1;
                break;
            }
            // Here we have the while statement for "From:"
            while(1)
            {
                printf("\n Turn: User\n From: ");
                scanf("%c%d",&j,&i);
                js = correcter(j);
                i=8-i;
                // Check whether the given coordinate is inside the borders of the table or not
                if(i >= 8 || js == 99 || i < 0)
                {
                    system("cls");
                    print(board);
                    printf("\n >> Out of board. Please type a proper coordinate.\n");
                    continue;
                }
                c = board[i][js];
                // Check if the given coordinate is really suitable
                if(c == 'R' || c == 'K' || c == 'B' || c == 'H' || c == 'P' || c == 'Q')
                {
                    system("cls");
                    print(board);
                    printf("\n >> Appropriate selection. Type 'z0' if you want to change.\n");
                    break;
                }
                else if(c == 'r' || c == 'k' || c == 'b' || c == 'h' || c == 'p' || c == 'q')
                {
                    system("cls");
                    print(board);
                    printf("\n >> This is an enemy piece. You cannot move it.\n");
                    continue;
                }
                else if(c == ' ')
                {
                    system("cls");
                    print(board);
                    printf("\n >> Empty coordinate. Please choose another.\n");
                    continue;
                }
                else
                {
                    system("cls");
                    print(board);
                    printf("\n >> Something goes wrong. Try again.\n");
                    continue;
                }
            }
            // now the "To:" section
            printf("\n Turn: User\n From: %c%d\n To: ",j,8-i);
            PIECE* dead = &bking;
            while(1)
            {
                scanf("%c%d",&tj,&ti);
                if((tj == 'z'  || tj == 'Z') && ti == 0)
                    break;
                tjs = correcter(tj);
                ti=8-ti;
                char tc;
                if((0<=ti) && (ti<=7) && (0<=tjs)  && (tjs<=7))
                {
                    tc = board[ti][tjs];
                }
                else
                {
                    system("cls");
                    print(board);
                    printf(" \n >> Out of board. Type 'z0'if you want to change piece.\n\n Turn: User\n From: %c%d\n To: ",j,8-i);
                    continue;
                }
                if(tc == 'r' || tc == 'b' || tc == 'h' || tc == 'p' || tc == 'q')
                {
                    if(movetest(c,i,js,ti,tjs,board) == 1)
                    {
                        // USE linked list to find the enemy piece. mark it as dead.
                        while(dead != NULL)
                        {
                            if(dead->icoordinate == ti && dead->jcoordinate == tjs)
                                break;
                            dead = dead->nextpiece;
                        }
                        rescuer = kill(ti,tjs,&bking,&wking,board);
                        locate(ti,tjs,search(i,js,board,&wking),board);
                        death = 1;
                        break;
                    }
                    else
                    {
                        system("cls");
                        print(board);
                        printf("\n >> You are not allowed to move here. Type 'z0' if you want to change piece. \n\n Turn: User\n From: %c%d\n To: ",j,8-i);
                        continue;
                    }
                    system("cls");
                    print(board);
                    printf("\n Turn: User\n From: %c%d \n To: %c%d",j,8-i,tj,8-ti);
                    break;
                }
                else if(tc == 'k')
                {
                    system("cls");
                    print(board);
                    printf("\n >> How dare you! Type 'z0' if you want to change piece.\n\n Turn: User\n From: %c%d\n To: ",j,8-i);
                    continue;
                }
                else if(tc == 'R' || tc == 'K' || tc == 'B' || tc == 'H' || tc == 'P' || tc == 'Q')
                {
                    system("cls");
                    print(board);
                    printf("\n >> Your own piece. You can't attack it. Type 'z0' if you want to change piece.\n\n Turn: User\n From: %c%d\n To: ",j,8-i);
                    continue;
                }
                else if(tc == ' ' )
                {
                    if( movetest(c,i,js,ti,tjs,board) == 1)
                    {
                        locate(ti,tjs,search(i,js,board,&wking),board);
                        update(&bking,&wking,board);
                        break;
                    }
                    else // if movetest() returns 0
                    {
                        system("cls");
                        print(board);
                        printf("\n >> You are not allowed to move here. Type 'z0' if you want to change piece.\n\n Turn: User\n From: %c%d\n To: ",j,8-i);
                        continue;
                    }
                }
                else
                {
                    system("cls");
                    print(board);
                    printf("\n >> Something goes wrong. Type 'z0' if you want to change piece.\n");
                    printf("\n Turn: User\n From: %c%d\n To: %c%d",j,8-i,tj,8-ti);
                    continue;
                }
            }
            if(tj == 'z' && ti == 0)
            {
                system("cls");
                print(board);
                continue;
            }
            if(checksearch(&bking,board) == 1)
            {
                //now remove what did you do:
                MessageBox(0,"This move results a check, thus it is considered invalid.","Alert",MB_OK);
                locate(i,js,search(ti,tjs,board,&wking),board);
                update(&bking,&wking,board);
                if(death == 1)
                    revive(dead,&bking,&wking,board,rescuer);;
                MessageBox(0,"Move canceled..","Alert",MB_OK);
                continue;
            }
            break;
        }
        if(draw == 1 || ai_wins == 1 || human_wins == 1)
            break;
        update(&bking,&wking,board);
        if((log = fopen( "log.txt", "a")) == NULL)
            MessageBox(0,"Unable to access log file","Alert",MB_OK);
        fprintf( log, " %c : %c%d -> %c%d\n",c,j,8-i,tj,(8-ti));
        fclose(log);
        system("cls");
        print(board);
        printf("\n >> You've moved the piece on %c%d to %c%d\n\n Turn: Computer (Thinking)\n",j,8-i,tj,(8-ti));
        checksearch(&wking,board) == 0 ?  : MessageBox(0,"Black king is under threat!","Check!",MB_OK);
        ((bking.nextpiece == NULL) && (wking.nextpiece == NULL)) ? (draw = 1) : (draw = 0) ;

        /* NOW THE COMPUTER'S MOVE COMES
        computer will, starting from the first piece of its, evaluate moves and compare
        estimated maximum costs of the possible moves as well.	*/
        // the linked list chooses AI pieces consecutively. there must be a reference PIECE pointer for comparisons.
        // ** actually cost() function will evaluate the input move parameters. possible moves will be generated
        // each time by an upper class switch-loop. once move parameters reach to the cost() funtion, it immediately
        // returns an integer value and it will be compared with the cost reference. if cost() returns a value greater
        // the cost reference, then its value is assigned to the cost reference.
        // the following loop evaluates each piece and does the action whose cost is the greatest.
        // if no move dominates, then the last possible move is reminded.
        bool checkflag;
        checksearch(&wking,board) == 1 ? (checkflag = 1) : (checkflag = 0);
        pieceref = NULL;
        int times=0,timed=0;
        times = clock();
        evaluate(&bking,&wking,SCAN_ORDER,-9999,times,timed);
        if(pieceref == NULL && checkflag == 1)
        {
            human_wins = 1;
            break;
        }
        else if(pieceref == NULL && checkflag == 0)
        {
            draw = 1;
            break;
        }
        int xi=pieceref->icoordinate;
        int xj=pieceref->jcoordinate;
        if(board[ai_i][ai_j] != ' ' && board[ai_i][ai_j] != 'r' && board[ai_i][ai_j] != 'k' && board[ai_i][ai_j] != 'b' && board[ai_i][ai_j] != 'h' && board[ai_i][ai_j] != 'p' && board[ai_i][ai_j] != 'q')
            kill(ai_i,ai_j,&wking,&bking,board);
        update(&bking,&wking,board);
        locate(ai_i,ai_j,pieceref,board);
        system("cls");
        update(&bking,&wking,board);
        print(board);
        printf("\n >> %c from %c%d moved to %c%d in %.3lf seconds\n",pieceref->symbol,reverser(xj),8-xi,reverser(ai_j),8-ai_i,difference/CLOCKS_PER_SEC);
        if((log = fopen( "log.txt", "a")) == NULL)
            MessageBox(0,"Unable to access log file","Alert",MB_OK);
        fprintf(log," %c : %c%d -> %c%d\n",pieceref->symbol,reverser(xj),8-xi,reverser(ai_j),8-ai_i);
        fclose(log);
        ((bking.nextpiece == NULL) && (wking.nextpiece == NULL)) ? (draw = 1) : (draw = 0) ;


    }
    if((log = fopen( "log.txt", "a")) == NULL)
        MessageBox(0,"Unable to access log file","Alert",MB_OK);
    if(human_wins == 1)
    {
        fprintf(log," *** HUMAN WINS ***");
        printf("\a\n Congratulations: Checkmate. So-called *smart* computer loses!\n");
    }
    else if(ai_wins == 1)
    {
        fprintf(log," *** AI WINS ***");
        printf("\a\n Checkmate: Human being loses. The computer doesn't seem to be so *dumb*.\n");
    }
    else if(draw == 1)
    {
        fprintf(log," *** DRAW ***");
        printf("\a\n The game seems to be finished in draw. \n");
        if(pieceref == NULL && (checksearch(&bking,board)==0 && checksearch(&wking,board)==0))
            printf(" Current player has no legal move. \n");
    }
    fclose( log );
    getchar();
    MessageBox(0,"Game Over","The End",MB_OK);
    getchar();
    return 0;
}

