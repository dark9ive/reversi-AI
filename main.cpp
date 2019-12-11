#include<iostream>
#include<math.h>
#include<string.h>
#include<vector>
#include<fstream>
#include<float.h>
#include<algorithm>
#include<unistd.h>
#define Alpha 0.01
#define decay_rate 0.5

using namespace std;
int SIZE;
int* Init();
void print(int*);

#include"AI.cpp"

void play_with_player();

void print(int* board){
	cout << "   |";
	for(int a = 0; a < SIZE; a++){
		cout << " " << a << " |";
	}
	cout << endl;
	for(int a = 0; a < SIZE+1; a++){
		cout << "----";
	}
	cout << endl;
	for(int a = 0; a < SIZE; a++){
		cout << " " << a << " |";
		for(int b = 0; b < SIZE; b++){
			switch((*(board+a) >> (2*b)) & 0x00000003){
				case 2:
					cout << " ● |";
					break;
				case 3:
					cout << " ○ |";
					break;
				default:
					cout << " . |";
					break;
			}
		}
		cout << endl;
		for(int b = 0; b < SIZE+1; b++){
			cout << "----";
		}
		cout << endl;
	}
}

int* Init(){
	int* board = new int[SIZE];
	for(int index = 0; index < SIZE; index++){
		board[index] = 0;
	}
	*(board+SIZE/2-1) += 3*pow(4, SIZE/2-1) + 2*pow(4, SIZE/2);
	*(board+SIZE/2) += 2*pow(4, SIZE/2-1) + 3*pow(4, SIZE/2);
	//print(board);
	return board;
}

int check(int* board, int posi_x, int posi_y, int color, bool fswitch){
	int return_val = 0;
	if((board[posi_x] >> (2*posi_y) & 0x00000003) != 0){
		return 0;
	}
	for(int dire = 0; dire < 9; dire++){
		if(dire == 4){
			continue;
		}
		int sigmax = (dire/3)-1;
		int sigmay = (dire%3)-1;
		int x_ = posi_x + sigmax;
		int y_ = posi_y + sigmay;
		while(x_ != -1 && x_ != SIZE && y_ != -1 && y_ != SIZE){
			int target = (board[x_] >> (2*y_) & 0x00000003);
			if(target == 3-color){
				x_ += sigmax;
				y_ += sigmay;
				continue;
			}
			else if(target == 0){
				break;
			}
			else if(target == 2+color){
				int x__ = posi_x + sigmax;
				int y__ = posi_y + sigmay;
				while(x__ != x_ || y__ != y_){
					if(fswitch){
						board[x__] += (2*color-1)*pow(4, y__);
					}
					x__ += sigmax;
					y__ += sigmay;
					return_val++;
				}
				break;
			}
			else{
				cout << "Error!" << x_ << y_ << endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	if(fswitch){
		board[posi_x] += (color+2)*pow(4, posi_y);
	}
	return return_val;
}

void training(){
	double* blackvalue = new double[SIZE*SIZE];
	double* whitevalue = new double[SIZE*SIZE];
	double* filevalue = new double[SIZE*SIZE*2+1];
	double playcount;
	string filename("./bin/value" + to_string(SIZE) + ".bin");
	FILE* valfile = fopen((filename).c_str(), "rb");
	if(!valfile){
		cout << "Training file not exist!  Start from 0......" << endl;
		playcount = 0;
	}
	else{
		fread(filevalue, sizeof(double), SIZE*SIZE*2+1, valfile);
		fclose(valfile);
		playcount = filevalue[SIZE*SIZE*2];
		cout << playcount << endl;
		fillvalue2(blackvalue, whitevalue, filevalue);
	}
	for(int a = 0; a < SIZE; a++){
		for(int b = 0; b < SIZE; b++){
			cout << *(blackvalue+a*SIZE+b) << " ";
		}
		cout << endl << endl;
	}
	for(int a = 0; a < SIZE; a++){
		for(int b = 0; b < SIZE; b++){
			cout << *(whitevalue+a*SIZE+b) << " ";
		}
		cout << endl << endl;
	}
	cout << playcount << endl;
	while(1){
		double* sigma = new double[SIZE*SIZE*2];
		traingame(blackvalue, whitevalue, sigma, playcount);
		AIupdate2(blackvalue, whitevalue, sigma);
		playcount++;
		if(!((int)playcount%1000)){
			cout << "Training finished " << playcount << " games." << endl;
			if(!((int)playcount%10000)){
				makefile2(blackvalue, whitevalue, playcount, filevalue);
				valfile = fopen((filename).c_str(), "wb");
				fwrite(filevalue, sizeof(double), SIZE*SIZE*2+1, valfile);
				fclose(valfile);
				return;
			}
		}
		delete [] sigma;
	}
	delete [] blackvalue;
	delete [] whitevalue;
	delete [] filevalue;
	return;
}

void play_with_AI(int roll){
	int* board = Init();
	double* blackvalue = new double[SIZE*SIZE];
	double* whitevalue = new double[SIZE*SIZE];
	double* filevalue = new double[SIZE*SIZE/4+SIZE/2+1];
	string filename("./bin/value" + to_string(SIZE) + ".bin");
	FILE* valfile = fopen((filename).c_str(), "rb");
	if(!valfile){
		cout << "Training file not exist! Program exit..." << endl;
		exit(EXIT_FAILURE);
	}
	else{
		fread(filevalue, sizeof(double), SIZE*SIZE/4+SIZE/2+1, valfile);
		fclose(valfile);
		fillvalue(blackvalue, whitevalue, filevalue);
	}
	if(roll == 0){
		print(board);
		while(1){
			//
			//	player's turn.
			//
			int max_index = -1;
			double max_value = -DBL_MAX;
			bool black_no_move = false;
			vector<int> avamov;
			for(int index = 0; index < SIZE*SIZE; index++){
				if(check(board, index/SIZE, index%SIZE, 0, 0)){
					avamov.push_back(index);
				}
			}
			if(avamov.size() != 0){
				while(1){
					char xi;
					char yi;
					cin >> xi >> yi;
					int x = xi - '0';
					int y = yi - '0';
					bool unlock = false;
					for(int index = 0; index < avamov.size(); index++){
						if(avamov[index] == x*SIZE+y){
							check(board, x, y, 0, 1);
							print(board);
							unlock = true;
							break;
						}
					}
					if(unlock){
						break;
					}
					cout << "Wrong Input!" << endl;
				}
			}
			else{
				cout << "You have no move!" << endl;
				black_no_move = true;
			}
			//
			//	AI's turn.
			//
			for(int index = 0; index < SIZE*SIZE; index++){
				if(check(board, index/SIZE, index%SIZE, 1, 0)){
					double value = get_value(board, index/SIZE, index%SIZE, blackvalue, whitevalue, 1);
					if(value > max_value){
						max_index = index;
						max_value = value;
					}
				}
			}
			if(max_index == -1){
				if(black_no_move){
					break;
				}
				else{
					cout << "No move! Your turn!" << endl;
				}
			}
			else{
				check(board, max_index/SIZE, max_index%SIZE, 1, 1);
				sleep(1);
				print(board);
			}
		}
	}
	else{
		while(1){
			//
			//	AI's turn.
			//
			int max_index = -1;
			double max_value = -DBL_MAX;
			bool black_no_move = false;
			for(int index = 0; index < SIZE*SIZE; index++){
				if(check(board, index/SIZE, index%SIZE, 0, 0)){
					double value = get_value(board, index/SIZE, index%SIZE, blackvalue, whitevalue, 0);
					if(value > max_value){
						max_index = index;
						max_value = value;
					}
				}
			}
			if(max_index == -1){
				black_no_move = true;
				cout << "No move! Your turn!" << endl;
			}
			else{
				check(board, max_index/SIZE, max_index%SIZE, 0, 1);
				sleep(1);
				print(board);
			}
			//
			//	player's turn.
			//
			vector<int> avamov;
			for(int index = 0; index < SIZE*SIZE; index++){
				if(check(board, index/SIZE, index%SIZE, 1, 0)){
					avamov.push_back(index);
				}
			}
			if(avamov.size() != 0){
				while(1){
					char xi;
					char yi;
					cin >> xi >> yi;
					int x = xi - '0';
					int y = yi - '0';
					bool unlock = false;
					for(int index = 0; index < avamov.size(); index++){
						if(avamov[index] == x*SIZE+y){
							check(board, x, y, 1, 1);
							print(board);
							unlock = true;
							break;
						}
					}
					if(unlock){
						break;
					}
					cout << "Wrong Input!" << endl;
				}
			}
			else{
				if(black_no_move){
					break;
				}
				else{
					cout << "You have no move!" << endl;
				}
			}
		}
	}
	int chess_num[2] = {0};
	for(int index = 0; index < SIZE*SIZE; index++){
		switch(board[index/SIZE] >> (2*(index%SIZE)) & 0x00000003){
			case 2:
				chess_num[0]++;
				break;
			case 3:
				chess_num[1]++;
				break;
			default:
				break;
		}
	}
	cout << "Game Over!\nFinal score:\t" << chess_num[0] << " : " << chess_num[1] << endl;
	if(chess_num[0] > chess_num[1]){
		cout << "Black wins!" << endl;
	}
	else if(chess_num[1] > chess_num[0]){
		cout << "White wins!" << endl;
	}
	else{
		cout << "It's a tie!" << endl;
	}
	delete [] board;
	delete [] blackvalue;
	delete [] whitevalue;
	delete [] filevalue;
	return;
}
int main(int argc, char* argv[]){
	if(argc != 3 && argc != 4){
		cout << "Wrong input! Invalid chessboard size or mode" << endl;
		return 0;
	}
	else{
		int buf = 0;
		for(int a = 0; a < strlen(argv[1]); a++){
			if(argv[1][a] - '0' > 9 || argv[1][a] - '0' < 0){
				cout << "Wrong input! Invalid chessboard size." << endl;
				return 0;
			}
			else{
				buf *= 10;
				buf += argv[1][a] - '0';
			}
		}
		if(buf%2){
			cout << "Wrong input! Even numbers only!" << endl;
			return 0;
		}
		SIZE = buf;
		if(argv[2][0] == '0'){
			cout << "Playing Mode." << endl;
			play_with_player();
		}
		else if(argv[2][0] == '1'){
			cout << "Training Mode." << endl;
			training();
		}
		else if(argv[2][0] == '2'){
			if(argv[3][0] != '0' && argv[3][0] != '1'){
				cout << "Please specify black or white!" << endl;
				return 0;
			}
			cout << "Play with AI." << endl;
			play_with_AI(argv[3][0]-'0');
		}
		else{
			cout << "Please specify which running Mode!" << endl;
		}
	}
	return 0;
}
void play_with_player(){
	int* board = Init();
	print(board);
	int chess_num[2] = {2, 2};
	int color = 0;
	int x, y;
	cout << chess_num[0] << " : " << chess_num[1] << endl;
	cout << "● 's turn: ";
	while(1){
		vector<int> avamov;
		for(int posi = 0; posi < SIZE*SIZE; posi++){
			if(check(board, posi/SIZE, posi%SIZE, color, 0)){
				avamov.push_back(posi);
			}
		}
		while(avamov.size() != 0){
			cout << "(" << avamov.size() << " possible moves)"<< endl;
			for(int a = 0; a < avamov.size(); a++){
				cout << avamov[a] << " ";
			}
			cout << endl;
			cin >> x >> y;
			while(x >= SIZE || x <= -1 || y >= SIZE || y <= -1){
				cout << "Wrong input! Out of range." << endl;
				cin >> x >> y;
			}
			int index = x*SIZE+y;
			bool lock = 1;
			for(int a = 0; a < avamov.size(); a++){
				if(index == avamov[a]){
					int sigma = check(board, x, y, color, 1);
					chess_num[color] += sigma + 1;
					chess_num[1-color] -= sigma;
					color = 1-color;
					lock = 0;
					break;
				}
			}
			if(lock){
				cout << "Wrong input! You can't flip any enemy chess." << endl;
				continue;
			}
			else{
				print(board);
				cout << chess_num[0] << " : " << chess_num[1] << endl;
				break;
			}
		}
		if(chess_num[0] + chess_num[1] == SIZE*SIZE){
			break;
		}
		if(!(avamov.size())){
			cout << "Can't make any move!\n\n" << endl;
			color = 1-color;
		}
		switch(color){
			case 0:
				cout << "● 's turn: ";
				continue;
			case 1:
				cout << "○ 's turn: ";
				continue;
		}
	}
	cout << "Game Over!\nFinal score:\t" << chess_num[0] << " : " << chess_num[1] << endl;
	if(chess_num[0] > chess_num[1]){
		cout << "Black wins!" << endl;
	}
	else if(chess_num[1] > chess_num[0]){
		cout << "White wins!" << endl;
	}
	else{
		cout << "It's a tie!" << endl;
	}
	delete [] board;
	return;
}
