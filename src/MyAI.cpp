#include "float.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include "MyAI.h"
using namespace std;

// double F3_1(MyAI &game, double alpha, double beta, int depth);
// double G3_1(MyAI &game, double alpha, double beta, int depth);
// double star1_G3_1(MyAI &game, double alpha, double beta, int depth);
// double star1_F3_1(MyAI &game, double alpha, double beta, int depth);

MyAI::MyAI(void){}

MyAI::~MyAI(void){}

void MyAI::Name(const char* data[], char* response)
{
	strcpy(response, "MyAI");
}

void MyAI::Version(const char* data[], char* response)
{
	strcpy(response, "1.0.0");
}

void MyAI::Time_setting(const char* data[], char* response)
{
	this->red_time = stoi(data[1]);
	this->blue_time = stoi(data[1]);
	strcpy(response, "1");
}

void MyAI::Board_setting(const char* data[], char* response)
{
	this->board_size = stoi(data[1]);
	this->red_piece_num = stoi(data[2]);
	this->blue_piece_num = stoi(data[2]);
	strcpy(response, "1");
}

void MyAI::Ini(const char* data[], char* response)
{
	// set color
	if(!strcmp(data[1], "R"))
	{
		this->color = RED;
	}
	else if(!strcmp(data[1], "B"))
	{
		this->color = BLUE;
	}

	char position[15];
	this->Init_board_state(position);

	sprintf(response, "%c%c %c%c %c%c %c%c %c%c %c%c", position[0], position[1], position[2], position[3],
													   position[4], position[5], position[6], position[7],
													   position[8], position[9], position[10], position[11]);

}

void MyAI::Get(const char* data[], char* response)
{
	// set color
	if(!strcmp(data[1], "R"))
	{
		this->color = RED;
	}
	else if(!strcmp(data[1], "B"))
	{
		this->color = BLUE;
	}

	// set dice & board
	this->dice = stoi(data[2]);
	char position[25];
	sprintf(position, "%s%s%s%s%s%s%s%s%s%s%s%s", data[3], data[4], data[5], data[6], data[7], data[8], 
												  data[9], data[10], data[11], data[12], data[13], data[14]);
	this->Set_board(position);

	// generate move
	char move[4];
	// fprintf(stderr, "GET");
	this->Generate_move(move);
	sprintf(response, "%c%c %c%c", move[0], move[1], move[2], move[3]);
}

void MyAI::Exit(const char* data[], char* response)
{
	fprintf(stderr, "Bye~\n");
}

// *********************** AI FUNCTION *********************** //

void MyAI::Init_board_state(char* position)
{
	int order[PIECE_NUM] = {0, 1, 2, 3, 4, 5};
	string red_init_position = "A1B1C1A2B2A3";
	string blue_init_position = "E3D4E4C5D5E5";

	// assign the initial positions of your pieces in random order
	for(int i = 0; i < PIECE_NUM; i++)
	{
		int j = rand() % (PIECE_NUM - i) + i;
		int tmp = order[i];
		order[i] = order[j];
		order[j] = tmp;
	}

	for(int i = 0; i < PIECE_NUM; i++)
	{
		if(this->color == RED)
		{
			position[order[i] * 2] = red_init_position[i * 2];
			position[order[i] * 2 + 1] = red_init_position[i * 2 + 1];
		}
		else if(this->color == BLUE)
		{
			position[order[i] * 2] = blue_init_position[i * 2];
			position[order[i] * 2 + 1] = blue_init_position[i * 2 + 1];
		}
	}
}

void MyAI::Set_board(char* position)
{
	memset(this->board, 0, sizeof(this->board));
	memset(this->blue_exist, 1, sizeof(this->blue_exist));
	memset(this->red_exist, 1, sizeof(this->red_exist));
	this->blue_piece_num = PIECE_NUM;
	this->red_piece_num = PIECE_NUM;

	int lost_piece_num = 0;
	for(int i = 0; i < PIECE_NUM * 2; i++)
	{
		int index = i * 2 - lost_piece_num;

		// the piece does not exist
		while(position[index] == '0')
		{
			index = i * 2 - lost_piece_num + 1;
			lost_piece_num++;
			// blue
			if(i < PIECE_NUM) 
			{
				this->blue_piece_num --;
				this->blue_exist[i] = 0;
			}
			// red
			else 
			{
				this->red_piece_num --;
				this->red_exist[i - PIECE_NUM] = 0;
			}
			i += 1;
		}
		// 1~6: blue pieces; 7~12: red pieces
		if (i < PIECE_NUM * 2)
		{
			this->board[position[index + 1] - '1'][position[index] - 'A'] = i + 1;
		}
	}
	fprintf(stderr, "\nThe current board:\n");
	this->Print_chessboard();
}

void MyAI::Print_chessboard()
{
	fprintf(stderr, "\n");
	// 1~6 represent blue pieces; A~F represent red pieces
	for(int i = 0;i < BOARD_SIZE; i++)
	{
		fprintf(stderr, "<%d>   ", i + 1);
		for(int j = 0;j < BOARD_SIZE; j++)
		{
			if(this->board[i][j] <= PIECE_NUM) fprintf(stderr, "%d  ", this->board[i][j]);
			else fprintf(stderr, "%c  ", 'A' + (this->board[i][j] - 7)); 
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n     ");
	for (int i = 0;i < BOARD_SIZE; i++)
	{
		fprintf(stderr, "<%c>", 'A' + i);
	}
	fprintf(stderr, "\n\n");
	fprintf(stderr, "The number of blue pieces: %d\nThe number of red pieces: %d\n", this->blue_piece_num, this->red_piece_num);
}

void MyAI::Generate_move(char* move)
{
	int result[100];
	// get legal moves
	// fprintf(stderr, "hello");
	int move_count = this->get_legal_move(result);
	// randomly choose a legal move
	// int rand_move = rand() % move_count;
	// int piece = result[rand_move * 3];
	// int start_point = result[rand_move * 3 + 1];
	// int end_point = result[rand_move * 3 + 2];
	int my_move = this->my_strategy();
	// int my_move = rand() % move_count;
	// fprintf(stderr, "hello2");
	int piece = result[my_move * 3];
	int start_point = result[my_move * 3 + 1];
	int end_point = result[my_move * 3 + 2];
	sprintf(move, "%c%c%c%c", 'A' + start_point % BOARD_SIZE, '1' + start_point / BOARD_SIZE, 'A' + end_point % BOARD_SIZE, '1' + end_point / BOARD_SIZE);
	this->Make_move(piece, start_point, end_point);
	// print the result
	fprintf(stderr, "============================\nMy result:\n");
	if(piece <= PIECE_NUM) fprintf(stderr, "Blue piece %d: (%c%c) -> (%c%c)\n", piece, move[0], move[1], move[2], move[3]);
	else fprintf(stderr, "Red piece %d: (%c%c) -> (%c%c)\n", piece - PIECE_NUM, move[0], move[1], move[2], move[3]);
	this->Print_chessboard();
	// fprintf(stderr, "score: %lf\n", this->f2());
	fprintf(stderr, "============================\n");
}

// transposition table
double table[100000];
int MyAI::hash()
{
	int hash = 5381; 
    const int MOD_VALUE = 99999;

    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            hash = ((hash << 5) + hash) ^ ((i * 5 + j + 1) * (this->board[i][j] + 1));
            hash %= MOD_VALUE;
        }
    }

    return hash;
}

#define INF 99999999
#define LOSE -1000
#define WIN 1000
#define MAX_MOVES 100
#define USAGE 3

int MyAI::my_strategy()
{
	int depth = 4;
	int move_arr[MAX_MOVES];
    int num_moves;
    int best_index = 0;
    double val = -INF;

    num_moves = this->get_legal_move(move_arr);
	// cout << "num+moves" << num_moves << "\n";
	// fprintf(stderr, "num moves: %d\n", num_moves);
    for (int i = 0; i < num_moves; i++) {
		// cout << "i=" << i << "\n";
		// fprintf(stderr, "i: %d\n", i);
		MyAI tempAI;
		tempAI = *this;
		int piece = move_arr[i * 3];
		int start_point = move_arr[i * 3 + 1];
		int end_point = move_arr[i * 3 + 2];
		tempAI.Make_move(piece, start_point, end_point);
		// fprintf(stderr, "================================");
		tempAI.Print_chessboard();
		// fprintf(stderr, "================================");
		if(tempAI.f() == WIN)
		{
			return i;
		}
        double tmp = tempAI.star1_F3_1(-INF, INF, depth - 1);
		fprintf(stderr, "tmp=%lf\n", tmp);
        if (val < tmp) {
            val = tmp;
            best_index = i;
        }
    }
    
    return best_index;
}

int MyAI::f() {
	// fprintf(stderr, "hello\n");
	if(this -> color == RED)
	{
		if(this->red_piece_num == 0) return LOSE;
		if(this->blue_piece_num == 0) return WIN;
		if(this->board[4][4] > PIECE_NUM) return WIN;
		if(this->board[0][0] <= PIECE_NUM && this->board[0][0] > 0) return LOSE;
		int red_min_dist = 10, blue_min_dist = 10;
		for(int i = 0; i < BOARD_SIZE; i++)
		{
			for(int j = 0; j < BOARD_SIZE; j++)
			{
				if(this->board[i][j] > PIECE_NUM)
				{
					int MM = max(4-i, 4-j);
					red_min_dist = min(red_min_dist, MM);
				}
				else if(this->board[i][j] <= PIECE_NUM && this->board[i][j] > 0)
				{
					int MM = max(i, j);
					blue_min_dist = min(blue_min_dist, MM);
				}
			}
		}
		// fprintf(stderr, "dist:%d %d\n", blue_min_dist, red_min_dist);
		return blue_min_dist - red_min_dist;
	}
	else
	{
		if(this->red_piece_num == 0) return WIN;
		if(this->blue_piece_num == 0) return LOSE;
		if(this->board[4][4] > PIECE_NUM) return LOSE;
		if(this->board[0][0] <= PIECE_NUM && this->board[0][0] > 0) return WIN;
		int red_min_dist = 100, blue_min_dist = 100;
		for(int i = 0; i < BOARD_SIZE; i++)
		{
			for(int j = 0; j < BOARD_SIZE; j++)
			{
				if(this->board[i][j] > PIECE_NUM)
				{
					int MM = max(4-i, 4-j);
					red_min_dist = min(red_min_dist, MM);
				}
				else if(this->board[i][j] <= PIECE_NUM && this->board[i][j] > 0)
				{
					int MM = max(i, j);
					blue_min_dist = min(blue_min_dist, MM);
				}
			}
		}
		// fprintf(stderr, "dist:%d %d\n", blue_min_dist, red_min_dist);
		return red_min_dist - blue_min_dist;
	}
}

double MyAI::f2()
{
	if(this -> color == RED)
	{
		if(this->red_piece_num == 0) return LOSE;
		if(this->blue_piece_num == 0) return WIN;
		if(this->board[4][4] > PIECE_NUM) return WIN;
		if(this->board[0][0] <= PIECE_NUM && this->board[0][0] > 0) return LOSE;
	}
	else
	{
		if(this->red_piece_num == 0) return WIN;
		if(this->blue_piece_num == 0) return LOSE;
		if(this->board[4][4] > PIECE_NUM) return LOSE;
		if(this->board[0][0] <= PIECE_NUM && this->board[0][0] > 0) return WIN;
	}
	int red_steps[7] = {5,5,5,5,5,5,5};
	int blue_steps[7] = {5,5,5,5,5,5,5};
	for(int i = 0; i < BOARD_SIZE; i++)
	{
		for(int j = 0; j < BOARD_SIZE; j++)
		{
			if(this->board[i][j] > PIECE_NUM)
			{
				red_steps[this->board[i][j] - PIECE_NUM] = max(4-i, 4-j);
			}
			else if(this->board[i][j] <= PIECE_NUM && this->board[i][j] > 0)
			{
				blue_steps[this->board[i][j]] = max(i, j);
			}
		}
	}
	double red_dice[7] = {0};
	for(int i = 1; i <= 6; i++)
	{
		if(red_exist[i-1])
			red_dice[i] = 1.0/6.0;
	}
	for(int i = 1; i <= 6; i++)
	{
		if(!red_exist[i-1])
		{
			int l = i - 1;
			int left_index = 0;
			while(l >= 1)
			{
				if(red_exist[l-1]){
					left_index = l;
					break;
				}
				l--;
			}
			int r = i + 1;
			int right_index = 0;
			while(r <= 6)
			{
				if(red_exist[r-1])
				{
					right_index = r;
					break;
				}
				r++;
			}
			if(red_steps[left_index] < red_steps[right_index])
				red_dice[left_index] += 1.0 / 6.0;
			else
				red_dice[right_index] += 1.0 / 6.0;
		}
	}
	double blue_dice[7] = {0};
	for(int i = 1; i <= 6; i++)
	{
		if(blue_exist[i-1])
			blue_dice[i] = 1.0/6.0;
	}
	for(int i = 1; i <= 6; i++)
	{
		if(!blue_exist[i-1])
		{
			int l = i - 1;
			int left_index = 0;
			while(l >= 1)
			{
				if(blue_exist[l-1]){
					left_index = l;
					break;
				}
				l--;
			}
			int r = i + 1;
			int right_index = 0;
			while(r <= 6)
			{
				if(blue_exist[r-1])
				{
					right_index = r;
					break;
				}
				r++;
			}
			if(blue_steps[left_index] < blue_steps[right_index])
				blue_dice[left_index] += 1.0 / 6.0;
			else
				blue_dice[right_index] += 1.0 / 6.0;
		}
	}
	double red_dist[7], blue_dist[7];
	for(int i = 1; i <= 6; i++)
	{
		// red_dist += pow(red_dice[i], red_steps[i]) * red_steps[i];
		// blue_dist += pow(blue_dice[i],  blue_steps[i]) * blue_steps[i];
		red_dist[i] = red_steps[i] / red_dice[i];
		blue_dist[i] = blue_steps[i] / blue_dice[i];
	}
	double red1 = 30.0, red2 = 30.0, blue1 = 30.0, blue2 = 30.0;
	for(int i = 1; i <= 6; i++)
	{
		if(red_dist[i] < red1)
		{
			red2 = red1;
			red1 = red_dist[i];
		}
		else if(red_dist[i] < red2)
		{
			red2 = red_dist[i];
		}
		if(blue_dist[i] < blue1)
		{
			blue2 = blue1;
			blue1 = blue_dist[i];
		}
		else if(blue_dist[i] < blue2)
		{
			blue2 = blue_dist[i];
		}
	}
	// fprintf(stderr, "red exist\n");
	// for(int i = 1; i <= 6; i++)
	// {
	// 	fprintf(stderr, "%d ", red_exist[i-1]);
	// }
	// fprintf(stderr, "\nblue exist\n");
	// for(int i = 1; i <= 6; i++)
	// {
	// 	fprintf(stderr, "%d ", blue_exist[i-1]);
	// }
	// fprintf(stderr, "\nred dice\n");
	// for(int i = 1; i <= 6; i++)
	// {
	// 	fprintf(stderr, "%lf ", red_dice[i]);
	// }
	// fprintf(stderr, "\nred steps\n");
	// for(int i = 1; i <= 6; i++)
	// {
	// 	fprintf(stderr, "%d ", red_steps[i]);
	// }
	// fprintf(stderr, "\nblue dice\n");
	// for(int i = 1; i <= 6; i++)
	// {
	// 	fprintf(stderr, "%lf ", blue_dice[i]);
	// }
	// fprintf(stderr, "\nblue steps\n");
	// for(int i = 1; i <= 6; i++)
	// {
	// 	fprintf(stderr, "%d ", blue_steps[i]);
	// }
	// fprintf(stderr, "\n");
	// fprintf(stderr, "%lf %lf %lf %lf \n", red1, red2, blue1, blue2);
	int coef = 10;
	if(this->color == RED) return blue1 * coef + blue2 - red1 * coef - red2;
	else return red1 * coef + red2 - blue1 * coef - blue2;
}

int MyAI::f3() {
	if(this -> color == RED)
	{
		if(this->red_piece_num == 0) return LOSE;
		if(this->blue_piece_num == 0) return WIN;
		if(this->board[4][4] > PIECE_NUM) return WIN;
		if(this->board[0][0] <= PIECE_NUM && this->board[0][0] > 0) return LOSE;
		int red_min_dist = 10, blue_min_dist = 10;
		// int red_min_index = 0, blue_min_index = 0;
		int red_min_dist2 = 10, blue_min_dist2 = 10;
		// int red_min_index2 = 0, blue_min_index2 = 0;
		for(int i = 0; i < BOARD_SIZE; i++)
		{
			for(int j = 0; j < BOARD_SIZE; j++)
			{
				if(this->board[i][j] > PIECE_NUM)
				{
					int MM = max(4-i, 4-j);
					if(MM < red_min_dist)
					{
						red_min_dist2 = red_min_dist;
						// red_min_index2 = red_min_index;
						red_min_dist = MM;
						// red_min_index = this->board[i][j]-PIECE_NUM;
					}
					else if(MM < red_min_dist2)
					{
						red_min_dist2 = MM;
						// red_min_index2 = this->board[i][j]-PIECE_NUM;
					}
				}
				else if(this->board[i][j] <= PIECE_NUM && this->board[i][j] > 0)
				{
					int MM = max(i, j);
					if(MM < blue_min_dist)
					{
						blue_min_dist2 = blue_min_dist;
						// blue_min_index2 = blue_min_index;
						blue_min_dist = MM;
						// blue_min_index = this->board[i][j];
					}
					else if(MM < blue_min_dist2)
					{
						blue_min_dist2 = MM;
						// blue_min_index2 = this->board[i][j];
					}
				}
			}
		}
		// fprintf(stderr, "dist:%d %d\n", blue_min_dist, red_min_dist);
		if(blue_min_dist2 == 10) blue_min_dist2 = 0;
		if(red_min_dist2 == 10) red_min_dist2 = 0;
		return blue_min_dist * 3 + blue_min_dist2 - red_min_dist * 3 - red_min_dist2;
	}
	else
	{
		if(this->red_piece_num == 0) return WIN;
		if(this->blue_piece_num == 0) return LOSE;
		if(this->board[4][4] > PIECE_NUM) return LOSE;
		if(this->board[0][0] <= PIECE_NUM && this->board[0][0] > 0) return WIN;
		int red_min_dist = 10, blue_min_dist = 10;
		// int red_min_index = 0, blue_min_index = 0;
		int red_min_dist2 = 10, blue_min_dist2 = 10;
		// int red_min_index2 = 0, blue_min_index2 = 0;
		for(int i = 0; i < BOARD_SIZE; i++)
		{
			for(int j = 0; j < BOARD_SIZE; j++)
			{
				if(this->board[i][j] > PIECE_NUM)
				{
					int MM = max(4-i, 4-j);
					if(MM < red_min_dist)
					{
						red_min_dist2 = red_min_dist;
						// red_min_index2 = red_min_index;
						red_min_dist = MM;
						// red_min_index = this->board[i][j]-PIECE_NUM;
					}
					else if(MM < red_min_dist2)
					{
						red_min_dist2 = MM;
						// red_min_index2 = this->board[i][j]-PIECE_NUM;
					}
				}
				else if(this->board[i][j] <= PIECE_NUM && this->board[i][j] > 0)
				{
					int MM = max(i, j);
					if(MM < blue_min_dist)
					{
						blue_min_dist2 = blue_min_dist;
						// blue_min_index2 = blue_min_index;
						blue_min_dist = MM;
						// blue_min_index = this->board[i][j];
					}
					else if(MM < blue_min_dist2)
					{
						blue_min_dist2 = MM;
						// blue_min_index2 = this->board[i][j];
					}
				}
			}
		}
		// fprintf(stderr, "dist:%d %d\n", blue_min_dist, red_min_dist);
		if(blue_min_dist2 == 10) blue_min_dist2 = 0;
		if(red_min_dist2 == 10) red_min_dist2 = 0;
		return red_min_dist * 3 + red_min_dist2 - blue_min_dist * 3 - blue_min_dist2;
	}
}

double MyAI::F3_1(double alpha, double beta, int depth) {
    // Implementation of Algorithm F3.1
    // Node node; 
	int move_arr[MAX_MOVES];
	int num_moves = this->get_legal_move(move_arr);
	// fprintf(stderr, "num moves2: %d\n", num_moves);

	// for(int i = 0; i < num_moves; i++)
	// {
	// 	fprintf(stderr, "%d %d\n", i, move_arr[i]);
	// 	fprintf(stderr, "%d %d\n", i, move_arr[i]);
	// 	fprintf(stderr, "%d %d\n", i, move_arr[i]);
	// }
	if(this->board[4][4]>=7)
	{
		// fprintf(stderr, "case1\n");
		// fprintf(stderr, "%d\n", board[5][5]);
		if(this->color == RED) return WIN;
		else return LOSE;
	}
	if(this->board[0][0]>=1 && this->board[0][0]<=6)
	{
		// fprintf(stderr, "case2\n");
		// fprintf(stderr, "%d\n", board[0][0]);
		if(this->color == RED) return LOSE;
		else return WIN;
	}

	int value = this->hash();
	// if(table[value] != 0)
	// {
	// 	fprintf(stderr, "collision\n");
	// 	return table[value];
	// }


    if (num_moves == 0 || depth == 0) {
		// fprintf(stderr, "case3\n");
		double eval_double;
		if(USAGE == 3){
			int eval = this->f3();
			eval_double = eval;
		}
        // int eval = this->f3();
		// fprintf(stderr, "output=%d", eval);
		// double eval_double = eval;
		if(USAGE == 2)
			eval_double = this->f2();
		// fprintf(stderr, "evalF=%lf\n", eval_double);
		return eval_double;
    }

    double m = -INF; // Initializing m as negative infinity

	MyAI tempAI_0 = *this;
	int piece = move_arr[0];
	int start_point = move_arr[1];
	int end_point = move_arr[2];
	tempAI_0.Make_move(piece, start_point, end_point);
	m = max(m, tempAI_0.star1_F3_1(alpha, beta, depth-1));

    for (int i = 1; i < num_moves; ++i) {
        double t;
        MyAI tempAI = *this;
		int piece = move_arr[i * 3];
		int start_point = move_arr[i * 3 + 1];
		int end_point = move_arr[i * 3 + 2];
		tempAI.Make_move(piece, start_point, end_point);
        // t = tempAI.star1_F3_1(max(alpha, m), beta, depth - 1);
		t = tempAI.star1_F3_1(m, m+1, depth - 1);
		// fprintf(stderr, "t=%lf\n", t);
        if (t > m) {
			if(depth < 3 || t >= beta)
            	m = t;
			else
				m = tempAI.star1_F3_1(t, beta, depth-1);
        }

        if (m >= beta) {
			// fprintf(stderr, "m=%lf beta=%lf\n", m, beta);
            return m; // Beta cut-off
        }
    }
	table[value] = m;
	// fprintf(stderr, "m=%lf\n", m);
    return m;
}

double MyAI::G3_1(double alpha, double beta, int depth) {
    // Implementation of Algorithm G3.1
    // Node node; 
	int move_arr[MAX_MOVES];
	int num_moves = this->get_legal_move(move_arr);

	if(this->board[4][4]>=7)
	{
		// fprintf(stderr, "case1\n");
		// fprintf(stderr, "%d\n", board[4][4]);
		if(this->color == RED) return WIN;
		else return LOSE;
	}
	if(this->board[0][0]>=1 && this->board[0][0]<=6)
	{
		// fprintf(stderr, "case2\n");
		// fprintf(stderr, "%d\n", board[0][0]);
		if(this->color == RED) return LOSE;
		else return WIN;
	}

    if (num_moves == 0 || depth == 0) {
		// fprintf(stderr, "case3\n");
		double eval_double;
		if(USAGE == 3){
			int eval = this->f3();
			eval_double = eval;
		}
        // int eval = this->f3();
		// fprintf(stderr, "output=%d", eval);
		// double eval_double = eval;
		if(USAGE == 2)
			eval_double = this->f2();
		// fprintf(stderr, "evalF=%lf\n", eval_double);
		return eval_double;
    }

    double m = INF; // Initializing m as infinity
	MyAI tempAI_0 = *this;
	int piece = move_arr[0];
	int start_point = move_arr[1];
	int end_point = move_arr[2];
	tempAI_0.Make_move(piece, start_point, end_point);
	m = min(m, tempAI_0.star1_G3_1(alpha, beta, depth-1));
	if(m <= alpha)
	{
		return m;
	}
    for (int i = 1; i < num_moves; ++i) {
        double t;
		MyAI tempAI;
		tempAI = *this;
		int piece = move_arr[i * 3];
		int start_point = move_arr[i * 3 + 1];
		int end_point = move_arr[i * 3 + 2];
        t = tempAI.star1_G3_1(m-1, m, depth - 1);

        if (t < m) {
			if(depth < 3 || t <= alpha)
            	m = t;
			else
				m = tempAI.star1_G3_1(alpha, t, depth - 1);
        }

        if (m <= alpha) {
            return m; // Alpha cut-off
        }
    }
	table[this->hash()] = m;
    return m;
}

double MyAI::star1_F3_1(double alpha, double beta, int depth) {
	int c = 6;
    double vmin = -1000;
    double vmax = 1000;

	double A[10], B[10], m[10], M[10];
    
    A[0] = c * (alpha - vmax) + vmax;
    B[0] = c * (beta - vmin) + vmin;
    m[0] = vmin;
    M[0] = vmax;
    double vsum = 0.0;
	for (int i = 1; i <= c; i++) {
		MyAI tempAI;
		tempAI = *this;
		tempAI.dice = i;
        double t = tempAI.G3_1(std::max(A[i-1], vmin), std::min(B[i-1], vmax), depth);
        
        m[i] = m[i-1] + (t - vmin) / c;
        M[i] = M[i-1] + (t - vmax) / c;
        
        if (t >= B[i-1]) return m[i]; // Failed high, chance node cutoff I
        if (t <= A[i-1]) return M[i]; // Failed low, chance node cutoff II
        
        vsum += 1.0/6.0 * t;
        A[i] = A[i-1] + vmax - t;
        B[i] = B[i-1] + vmin - t;
    }
	return vsum;
	
	// int c = 6;
    // double vmin = -100;
    // double vmax = 100;

	// double A[10], B[10], m[10], M[10];
    
    // A[0] = c * (alpha - vmax) + vmax;
    // B[0] = c * (beta - vmin) + vmin;
    // m[0] = vmin;
    // M[0] = vmax;
    // double vsum = 0.0;
	// double t[7];
	// t[0] = -99999999.0;
	// for (int i = 1; i <= c; i++) {
	// 	MyAI tempAI;
	// 	tempAI = *this;
	// 	tempAI.dice = i;

	// 	if(tempAI.color == RED)
	// 	{
	// 		if(tempAI.red_exist[i])
	// 		{
	// 			t[i] = tempAI.F3_1(std::max(A[i-1], vmin), std::min(B[i-1], vmax), depth);
	// 		}
	// 		else
	// 		{
	// 			t[i] = t[i-1];
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if(tempAI.blue_exist[i])
	// 		{
	// 			t[i] = tempAI.F3_1(std::max(A[i-1], vmin), std::min(B[i-1], vmax), depth);
	// 		}
	// 		else
	// 		{
	// 			t[i] = t[i-1];
	// 		}
	// 	}
        
        
    //     m[i] = m[i-1] + (t[i] - vmin) / c;
    //     M[i] = M[i-1] + (t[i] - vmax) / c;
        
    //     if (t[i] >= B[i-1]) return m[i]; // Failed high, chance node cutoff I
    //     if (t[i] <= A[i-1]) return M[i]; // Failed low, chance node cutoff II
        
    //     // vsum += 1.0/6.0 * t;
    //     A[i] = A[i-1] + vmax - t[i];
    //     B[i] = B[i-1] + vmin - t[i];
    // }

	// for(int i = 5; i >= 1; i--)
	// {
	// 	if(this->color == RED)
	// 	{
	// 		if(this->red_exist[i] == 0)
	// 		{
	// 			t[i] = max(t[i], t[i+1]);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if(this->blue_exist[i] == 0)
	// 		{
	// 			t[i] = max(t[i], t[i+1]);
	// 		}
	// 	}
	// }
	// for(int i = 1; i <= 6; i++)
	// {
	// 	vsum += 1.0/6.0 * t[i];
	// }
	// return vsum;
}

double MyAI::star1_G3_1(double alpha, double beta, int depth) {
	int c = 6;
    double vmin = -1000;
    double vmax = 1000;

	double A[10], B[10], m[10], M[10];
    
    A[0] = c * (alpha - vmax) + vmax;
    B[0] = c * (beta - vmin) + vmin;
    m[0] = vmin;
    M[0] = vmax;
    double vsum = 0.0;
	for (int i = 1; i <= c; i++) {
		MyAI tempAI;
		tempAI = *this;
		tempAI.dice = i;
        double t = tempAI.F3_1(std::max(A[i-1], vmin), std::min(B[i-1], vmax), depth);
        
        m[i] = m[i-1] + (t - vmin) / c;
        M[i] = M[i-1] + (t - vmax) / c;
        
        if (t >= B[i-1]) return m[i]; // Failed high, chance node cutoff I
        if (t <= A[i-1]) return M[i]; // Failed low, chance node cutoff II
        
        vsum += 1.0/6.0 * t;
        A[i] = A[i-1] + vmax - t;
        B[i] = B[i-1] + vmin - t;
    }
	return vsum;
	// int c = 6;
    // double vmin = -100;
    // double vmax = 100;

	// double A[10], B[10], m[10], M[10];
    
    // A[0] = c * (alpha - vmax) + vmax;
    // B[0] = c * (beta - vmin) + vmin;
    // m[0] = vmin;
    // M[0] = vmax;
    // double vsum = 0.0;
	// double t[7];
	// t[0] = -99999999.0;
	// for (int i = 1; i <= c; i++) {
	// 	MyAI tempAI;
	// 	tempAI = *this;
	// 	tempAI.dice = i;

	// 	if(tempAI.color == RED)
	// 	{
	// 		if(tempAI.red_exist[i])
	// 		{
	// 			t[i] = tempAI.G3_1(std::max(A[i-1], vmin), std::min(B[i-1], vmax), depth);
	// 		}
	// 		else
	// 		{
	// 			t[i] = t[i-1];
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if(tempAI.blue_exist[i])
	// 		{
	// 			t[i] = tempAI.F3_1(std::max(A[i-1], vmin), std::min(B[i-1], vmax), depth);
	// 		}
	// 		else
	// 		{
	// 			t[i] = t[i-1];
	// 		}
	// 	}
        
        
    //     m[i] = m[i-1] + (t[i] - vmin) / c;
    //     M[i] = M[i-1] + (t[i] - vmax) / c;
        
    //     if (t[i] >= B[i-1]) return m[i]; // Failed high, chance node cutoff I
    //     if (t[i] <= A[i-1]) return M[i]; // Failed low, chance node cutoff II
        
    //     // vsum += 1.0/6.0 * t;
    //     A[i] = A[i-1] + vmax - t[i];
    //     B[i] = B[i-1] + vmin - t[i];
    // }

	// for(int i = 5; i >= 1; i--)
	// {
	// 	if(this->color == RED)
	// 	{
	// 		if(this->red_exist[i] == 0)
	// 		{
	// 			t[i] = max(t[i], t[i+1]);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if(this->blue_exist[i] == 0)
	// 		{
	// 			t[i] = max(t[i], t[i+1]);
	// 		}
	// 	}
	// }
	// for(int i = 1; i <= 6; i++)
	// {
	// 	vsum += 1.0/6.0 * t[i];
	// }
	// return vsum;
}

// get all legal moves
int MyAI::get_legal_move(int* result)
{
	int src, dst[3];
	int movable_piece;
	int move_count = 0;
	int result_count = 0;

	if(this->color == BLUE)
	{
		// the corresponding piece is alive
		if(this->blue_exist[this->dice - 1])
		{
			movable_piece = this->dice;
			move_count = this->referee(movable_piece, &src, dst);
			for(int i = result_count; i < result_count + move_count; i++) 
			{
				result[i * 3] = movable_piece;
				result[i * 3 + 1] = src;
				result[i * 3 + 2] = dst[i];
			}
			result_count += move_count;
		}
		// the corresponding piece does not exist
		else
		{
			// seeking for the next-higher piece
			for(int i = this->dice; i <= PIECE_NUM; ++i)
			{
				if(this->blue_exist[i - 1])
				{
					movable_piece = i;
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}
			// seeking for the next-lower piece
			for(int i = this->dice; i >= 1; --i)
			{
				if(this->blue_exist[i - 1])
				{
					movable_piece = i;
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}
		}
	}

	else if(this->color == RED)
	{
		// the corresponding piece is alive
		if(this->red_exist[this->dice - 1])
		{
			movable_piece = this->dice + PIECE_NUM;
			move_count = this->referee(movable_piece, &src, dst);
			for(int i = result_count; i < result_count + move_count; i++) 
			{
				result[i * 3] = movable_piece;
				result[i * 3 + 1] = src;
				result[i * 3 + 2] = dst[i];
			}
			result_count += move_count;
		}
		// the corresponding piece does not exist
		else
		{
			// seeking for the next-higher piece
			for(int i = this->dice; i <= PIECE_NUM; ++i)
			{
				if(this->red_exist[i - 1])
				{
					movable_piece = i + PIECE_NUM; 
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}
			// seeking for the next-lower piece
			for(int i = this->dice; i >= 1; --i)
			{
				if(this->red_exist[i - 1])
				{
					movable_piece = i + PIECE_NUM; 
					move_count = this->referee(movable_piece, &src, dst);
					int index = 0;
					for(int j = result_count; j < result_count + move_count; j++, index++) 
					{
						result[j * 3] = movable_piece;
						result[j * 3 + 1] = src;
						result[j * 3 + 2] = dst[index];
					}
					result_count += move_count;
					break;
				}
			}			
		}		
	}
	return result_count;
}

// get possible moves of the piece
int MyAI::referee(int piece, int* src, int* dst)
{
	for(int i = 0; i < BOARD_SIZE; i++)
	{
		for(int j = 0; j < BOARD_SIZE; j++)
		{
			if(this->board[i][j] == piece)
			{
				*src = i * BOARD_SIZE + j;
			}
		}
	}
	// blue piece
	if(piece <= PIECE_NUM) 
	{
		// the piece is on the leftmost column
		if(*src % BOARD_SIZE == 0)
		{
			dst[0] = *src - BOARD_SIZE; // up
			return 1;
		}
		// the piece is on the uppermost row
		else if(*src < BOARD_SIZE)
		{
			dst[0] = *src - 1; // left
			return 1;
		}
		else
		{
			dst[0] = *src - 1; // left
			dst[1] = *src - BOARD_SIZE; // up
			dst[2] = *src - BOARD_SIZE - 1; // upper left
			return 3;
		}
	}

	// red piece
	else
	{
		// the piece is on the rightmost column
		if(*src % BOARD_SIZE == 4)
		{
			dst[0] = *src + BOARD_SIZE; // down
			return 1;
		}
		// the piece is on the downmost row
		else if(*src >= BOARD_SIZE * (BOARD_SIZE - 1))
		{
			dst[0] = *src + 1; // right
			return 1;
		}
		else
		{
			dst[0] = *src + 1; // right
			dst[1] = *src + BOARD_SIZE; // down
			dst[2] = *src + BOARD_SIZE + 1; // bottom right
			return 3;
		}
	}	
}

void MyAI::Make_move(const int piece, const int start_point, const int end_point)
{
	int start_row = start_point / BOARD_SIZE;
	int start_col = start_point % BOARD_SIZE;
	int end_row = end_point / BOARD_SIZE;
	int end_col = end_point % BOARD_SIZE;

	this->board[start_row][start_col] = 0;

	// there has another piece on the target sqaure
	if(this->board[end_row][end_col] > 0)
	{
		if(this->board[end_row][end_col] <= PIECE_NUM)
		{
			this->blue_exist[this->board[end_row][end_col] - 1] = 0;
			this->blue_piece_num--;
		}
		else
		{
			this->red_exist[this->board[end_row][end_col] - 7] = 0;
			this->red_piece_num--;			
		}
	}
	this->board[end_row][end_col] = piece;
}