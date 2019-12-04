#include<iostream>
#include<math.h>
#include<string.h>
#include<vector>
#include<time.h>

int SIZE;
struct tree;

void print(int*);
int count_child(tree*, int);

#define maxthreads 1024

#include"AI.cu"

using namespace std;

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
	*(board+SIZE/2-1) += 3*pow(4, SIZE/2-1) + 2*pow(4, SIZE/2);
	*(board+SIZE/2) += 2*pow(4, SIZE/2-1) + 3*pow(4, SIZE/2);
	print(board);
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

int count_child(tree* root, int return_val){
	if(root->leaf != NULL){
		return ++return_val;
	}
	for(int a = 0; a < 4; a++){
		if(root->next_block[a] != NULL){
			return_val = count_child(root->next_block[a], return_val);
		}
	}
	return return_val;
}

int main(int argc, char* argv[]){
	srand(time(NULL));
	if(argc != 2){
		cout << "Wrong input! Invalid chessboard size." << endl;
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
	}
	char choise;
	cin >> choise;
	int* board = Init();
	tree* root = new tree;
	int* search_data;
	cudaMallocManaged(&search_data, sizeof(int)*SIZE);
	memcpy(search_data, board, sizeof(int)*SIZE);
	make_leaf(root, board);
	search_recu(1, search_data, 0, root, 0);
	/*
	   cout << endcount << endl;
	   cout << error_count << endl;
	   cout << count_child(root, 0) << endl;
	 */
	int chess_num[2] = {2, 2};
	int color = 0;
	int x, y;
	cout << chess_num[0] << " : " << chess_num[1] << endl;
	cout << "● 's turn: ";
	if(choise == '1'){
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
					break;
				case 1:
					cout << "○ 's turn: ";
					break;
			}
			continue;
		}
	}
	else if(choise == '2'){
		while(1){
			vector<int> avamov;
			for(int posi = 0; posi < SIZE*SIZE; posi++){
				if(check(board, posi/SIZE, posi%SIZE, color, 0)){
					avamov.push_back(posi);
				}
			}
			if(color == 0){
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
				if(avamov.size() == 0){
					cout << "Can't make any move!\n\n" << endl;
					color = 1-color;
				}
				cout << "○'s turn: ";
				continue;
			}
			else{		//	AI move.
				if(avamov.size() != 0){
					int sigma;
					bool win = 0;
					for(int a = 0; a < avamov.size(); a++){
						int* board_cpy = new int[SIZE];
						memcpy(board_cpy, board, sizeof(int)*SIZE);
						sigma = check(board, avamov[a]/SIZE, avamov[a]%SIZE, 1, 1);
						if(get_leaf_pointer(root, board)->white_value == 1){
							win = 1;
							delete [] board_cpy;
							break;
						}
						memcpy(board, board_cpy, sizeof(int)*SIZE);
					}
					if(win){
						cout << "Meow." << endl;
					}
					else{
						sigma = check(board, avamov[rand()%avamov.size()]/SIZE, avamov[rand()%avamov.size()]%SIZE, 1, 1);
						cout << "Meow!" << endl;
					}
					chess_num[1] += sigma + 1;
					chess_num[0] -= sigma;
					print(board);
					color = 1-color;
				}
			}
			if(chess_num[0] + chess_num[1] == SIZE*SIZE){
				break;
			}
			if(!(avamov.size())){
				cout << "Can't make any move!\n\n" << endl;
				color = 1-color;
			}
			cout << "●'s turn :";
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
	return 0;
}
