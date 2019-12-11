//***********************************************************************************************
//
//
//					     AI.cpp
//
//					  training AI
//
//
//***********************************************************************************************

using namespace std;

void fillvalue(double* blackvalue, double* whitevalue, double* file){
	int index = 0;
	for(int a = 0; a < SIZE/2; a++){
		for(int b = a; b < SIZE/2; b++){
			*(blackvalue+a*SIZE+b) = *(file+index);
			*(blackvalue+b*SIZE+a) = *(file+index);
			*(blackvalue+a*SIZE+SIZE-b-1) = *(file+index);
			*(blackvalue+b*SIZE+SIZE-1-a) = *(file+index);
			*(blackvalue+(SIZE-1-a)*SIZE+b) = *(file+index);
			*(blackvalue+(SIZE-1-b)*SIZE+a) = *(file+index);
			*(blackvalue+(SIZE-1-a)*SIZE+SIZE-b-1) = *(file+index);
			*(blackvalue+(SIZE-1-b)*SIZE+SIZE-1-a) = *(file+index);
			index++;
		}
	}
	for(int a = 0; a < SIZE/2; a++){
		for(int b = a; b < SIZE/2; b++){
			*(whitevalue+a*SIZE+b) = *(file+index);
			*(whitevalue+b*SIZE+a) = *(file+index);
			*(whitevalue+a*SIZE+SIZE-b-1) = *(file+index);
			*(whitevalue+b*SIZE+SIZE-1-a) = *(file+index);
			*(whitevalue+(SIZE-1-a)*SIZE+b) = *(file+index);
			*(whitevalue+(SIZE-1-b)*SIZE+a) = *(file+index);
			*(whitevalue+(SIZE-1-a)*SIZE+SIZE-b-1) = *(file+index);
			*(whitevalue+(SIZE-1-b)*SIZE+SIZE-1-a) = *(file+index);
			index++;
		}
	}
	return;
}

void fillvalue2(double* blackvalue, double* whitevalue, double* file){
	memcpy(blackvalue, file, sizeof(double)*SIZE*SIZE);
	memcpy(whitevalue, file+SIZE*SIZE, sizeof(double)*SIZE*SIZE);
	return;
}

int AIcheck(int* board, int posi_x, int posi_y, int color, bool fswitch){
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

double get_value(int* board, int posi_x, int posi_y, double* blackvalue, double* whitevalue, int color){
	vector<double> value;
	double return_val = 0.0;
	//return_val += 1.0;
	value.push_back(*(blackvalue+posi_x*SIZE+posi_y)+*(whitevalue+posi_x*SIZE+posi_y));
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
				bool lock = true;
				while(x__ != x_ || y__ != y_){
					if(lock){
						value.push_back(*(blackvalue+x_*SIZE+y_) + *(whitevalue+x_*SIZE+y_));
						//return_val += 1.0;
					}
					value.push_back(*(blackvalue+x__*SIZE+y__) + *(whitevalue+x__*SIZE+y__));
					//return_val += 1.0;
					x__ += sigmax;
					y__ += sigmay;
				}
				break;
			}
			else{
				cout << "Error!" << x_ << y_ << endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	sort(value.begin(), value.end());
	reverse(value.begin(), value.end());
	for(int a = 0; a < value.size(); a++){
		return_val += value[a]*pow(decay_rate, a);
	}
	return return_val;
}

void TD_check(int* board, double* sigma, int color){
	for(int dire = 0; dire < 9; dire++){
		if(dire == 4){
			continue;
		}
		int sigmax = (dire/3)-1;
		int sigmay = (dire%3)-1;
		int posi_x = *(board+SIZE)/SIZE;
		int posi_y = *(board+SIZE)%SIZE;
		int x_ = posi_x + sigmax;
		int y_ = posi_y + sigmay;
		double value = 0.0;
		while(x_ != -1 && x_ != SIZE && y_ != -1 && y_ != SIZE){
			int target = ((board[x_] >> (2*y_)) & 0x00000003);
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
				bool lock = true;
				while(x__ != x_ || y__ != y_){
					if(lock){
						value += 1.0;
					}
					value += *(sigma+x__*SIZE+y__);
					value += 1.0;
					x__ += sigmax;
					y__ += sigmay;
				}
				*(sigma+posi_x*SIZE+posi_y) += value/2.0;
				*(sigma+x_*SIZE+y_) += value/2.0;
				break;
			}
			else{
				cout << "Error!" << x_ << y_ << endl;
				exit(EXIT_FAILURE);
			}
		}
	}
	delete [] board;
	return;
}

void TD_update(vector<int*> &black, vector<int*> &white, double* sigma){
	for(int a = black.size()-1; a >= 0; a--){
		TD_check(black[a], sigma, 0);
	}
	for(int a = white.size()-1; a >= 0; a--){
		TD_check(white[a], sigma+SIZE*SIZE, 1);
	}
	return;
}

void traingame(double* blackvalue, double* whitevalue, double* sigma, double playcount){
	int* board = Init();
	vector<int*> replay_board_black;
	vector<int*> replay_board_white;
	for(int a = 0; a < SIZE*SIZE*2; a++){
		*(sigma+a) = 0.0;
	}
	while(1){
		//
		//	here start black.
		//
		int max_index = -1;
		double max_value = -DBL_MAX;
		bool black_no_move = false;
		for(int index = 0; index < SIZE*SIZE; index++){
			if(AIcheck(board, index/SIZE, index%SIZE, 0, 0) != 0){
				double value = get_value(board, index/SIZE, index%SIZE, blackvalue, whitevalue, 0);
				if(value > max_value){
					max_index = index;
					max_value = value;
				}
			}
		}
		if(max_index == -1){
			black_no_move = true;
		}
		else{
			int* board_cpy = new int[SIZE+1];
			memcpy(board_cpy, board, sizeof(int)*SIZE);
			*(board_cpy+SIZE) = max_index;
			replay_board_black.push_back(board_cpy);
			AIcheck(board, max_index/SIZE, max_index%SIZE, 0, 1);
		}
		//
		//	here start white.
		//
		max_index = -1;
		max_value = -DBL_MAX;
		for(int index = 0; index < SIZE*SIZE; index++){
			if(AIcheck(board, index/SIZE, index%SIZE, 1, 0) != 0){
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
				continue;
			}
		}
		else{
			int* board_cpy = new int[SIZE+1];
			memcpy(board_cpy, board, sizeof(int)*SIZE);
			*(board_cpy+SIZE) = max_index;
			replay_board_white.push_back(board_cpy);
			AIcheck(board, max_index/SIZE, max_index%SIZE, 1, 1);
		}
	}
	if((int)playcount%1000 == 0){
		print(board);
	}
	TD_update(replay_board_black, replay_board_white, sigma);
	delete [] board;
	return;
}

void AIupdate(double* blackvalue, double* whitevalue, double* sigma){
	for(int a = 0; a < SIZE/2; a++){
		for(int b = a; b < SIZE/2; b++){
			*(sigma+a*SIZE+b) += *(sigma+b*SIZE+a);
			*(sigma+a*SIZE+b) += *(sigma+a*SIZE+SIZE-b-1);
			*(sigma+a*SIZE+b) += *(sigma+b*SIZE+SIZE-1-a);
			*(sigma+a*SIZE+b) += *(sigma+(SIZE-1-a)*SIZE+b);
			*(sigma+a*SIZE+b) += *(sigma+(SIZE-1-b)*SIZE+a);
			*(sigma+a*SIZE+b) += *(sigma+(SIZE-1-a)*SIZE+SIZE-b-1);
			*(sigma+a*SIZE+b) += *(sigma+(SIZE-1-b)*SIZE+SIZE-1-a);
			*(sigma+a*SIZE+b) = ((*(sigma+a*SIZE+b))/8 - (*(blackvalue+a*SIZE+b)))*Alpha;

			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+b*SIZE+a+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+a*SIZE+SIZE-b-1+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+b*SIZE+SIZE-1-a+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+(SIZE-1-a)*SIZE+b+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+(SIZE-1-b)*SIZE+a+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+(SIZE-1-a)*SIZE+SIZE-b-1+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) += *(sigma+(SIZE-1-b)*SIZE+SIZE-1-a+SIZE*SIZE);
			*(sigma+a*SIZE+b+SIZE*SIZE) = ((*(sigma+a*SIZE+b+SIZE*SIZE))/8 - *(whitevalue+a*SIZE+b))*Alpha;

			/*
			   if(abs(*(blackvalue+a*SIZE+b)) <= 5){
			 *(sigma+a*SIZE+b) = (*(sigma+a*SIZE+b))*Alpha;
			 }
			 else{
			 *(sigma+a*SIZE+b) = (*(sigma+a*SIZE+b)/(*(blackvalue+a*SIZE+b))-1)*Alpha*(abs(*(blackvalue+a*SIZE+b))/(*(blackvalue+a*SIZE+b)));

			 }
			 */

			*(blackvalue+a*SIZE+b) += *(sigma+a*SIZE+b);
			*(blackvalue+b*SIZE+a) += *(sigma+a*SIZE+b);
			*(blackvalue+a*SIZE+SIZE-b-1) += *(sigma+a*SIZE+b);
			*(blackvalue+b*SIZE+SIZE-1-a) += *(sigma+a*SIZE+b);
			*(blackvalue+(SIZE-1-a)*SIZE+b) += *(sigma+a*SIZE+b);
			*(blackvalue+(SIZE-1-b)*SIZE+a) += *(sigma+a*SIZE+b);
			*(blackvalue+(SIZE-1-a)*SIZE+SIZE-b-1) += *(sigma+a*SIZE+b);
			*(blackvalue+(SIZE-1-b)*SIZE+SIZE-1-a) += *(sigma+a*SIZE+b);

			/*
			   if(abs(*(whitevalue+a*SIZE+b)) <= 5){
			 *(sigma+a*SIZE+b+SIZE*SIZE) = (*(sigma+a*SIZE+b+SIZE*SIZE))*Alpha;
			 }
			 else{
			 *(sigma+a*SIZE+b+SIZE*SIZE) = ((*(sigma+a*SIZE+b+SIZE*SIZE)/(*(whitevalue+a*SIZE+b)))-1)*Alpha*(abs(*(whitevalue+a*SIZE+b))/(*(whitevalue+a*SIZE+b)));
			 }
			 */

			*(whitevalue+a*SIZE+b) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+b*SIZE+a) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+a*SIZE+SIZE-b-1) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+b*SIZE+SIZE-1-a) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+(SIZE-1-a)*SIZE+b) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+(SIZE-1-b)*SIZE+a) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+(SIZE-1-a)*SIZE+SIZE-b-1) += *(sigma+a*SIZE+b+SIZE*SIZE);
			*(whitevalue+(SIZE-1-b)*SIZE+SIZE-1-a) += *(sigma+a*SIZE+b+SIZE*SIZE);
		}
	}
	return;
}

void AIupdate2(double* blackvalue, double* whitevalue, double* sigma){
	for(int a = 0; a < SIZE; a++){
		for(int b = 0; b < SIZE; b++){
			*(sigma+a*SIZE+b) = (*(sigma+a*SIZE+b) - (*(blackvalue+a*SIZE+b)))*Alpha;
			*(sigma+a*SIZE+b+SIZE*SIZE) = (*(sigma+a*SIZE+b+SIZE*SIZE) - *(whitevalue+a*SIZE+b))*Alpha;
			
			*(blackvalue+a*SIZE+b) += *(sigma+a*SIZE+b);
			*(whitevalue+a*SIZE+b) += *(sigma+a*SIZE+b+SIZE*SIZE);
		}
	}
	return;
}

void makefile(double* blackvalue, double* whitevalue, double playcount, double* file){
	int index = 0;
	for(int a = 0; a < SIZE/2; a++){
		for(int b = a; b < SIZE/2; b++){
			*(file+index) = *(blackvalue+a*SIZE+b);
			index++;
		}
	}
	for(int a = 0; a < SIZE/2; a++){
		for(int b = a; b < SIZE/2; b++){
			*(file+index) = *(whitevalue+a*SIZE+b);
			index++;
		}
	}
	*(file+index) = playcount;
	return;
}

void makefile2(double* blackvalue, double* whitevalue, double playcount, double* file){
	memcpy(file, blackvalue, sizeof(double)*SIZE*SIZE);
	memcpy(file+SIZE*SIZE, whitevalue, sizeof(double)*SIZE*SIZE);
	*(file+SIZE*SIZE*2) = playcount;
	return;
}

