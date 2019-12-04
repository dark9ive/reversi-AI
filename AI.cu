//
//	AI.cu
//

using namespace std;
struct leaves;

struct tree{
	tree* next_block[4] = {NULL, NULL, NULL, NULL};
	leaves* leaf = NULL;
};
struct leaves{
	int black_value = -1;
	int white_value = -1;
};
int error_count = 0;
int endcount = 0;

int make_leaf(tree* root, int* board){
	for(int x = 0; x < SIZE; x++){
		for(int y = 0; y < SIZE; y++){
			if(root->next_block[board[x] >> (2*y) & 0x00000003] == NULL){
				root->next_block[board[x] >> (2*y) & 0x00000003] = new tree;
			}
			root = root->next_block[board[x] >> (2*y) & 0x00000003];
		}
	}
	if(root->leaf == NULL){
		root->leaf = new leaves;
		return 1;
	}
	else{
		return 0;
	}
}

__global__ void cuda_check(int* board, int* cuda_data_out, int SIZE, int color){
	board += SIZE*threadIdx.x;
	int board_cpy[8];
	memcpy(board_cpy, board, sizeof(int)*SIZE);
	int anscount = 0;
	int used;
	for(int counter = 0; counter < 2; counter++){
		used = 0;
		for(int index = 0; index < SIZE*SIZE; index++){
			int posi_x = index/SIZE;
			int posi_y = index%SIZE;
			int return_val = 0;
			if((board[posi_x] >> (2*posi_y) & 0x00000003) != 0){
				used++;
				continue;
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
							board[x__] += (2*color-1)*pow(4, y__);
							x__ += sigmax;
							y__ += sigmay;
							return_val++;
						}
						break;
					}
					else{
						printf("ERROR! %d %d %d\n", x_, y_, threadIdx.x);
						return;
					}
				}
			}
			if(return_val){
				board[posi_x] += (color+2)*pow(4, posi_y);
				memcpy(cuda_data_out+(SIZE*SIZE*SIZE+1)*threadIdx.x+anscount*SIZE+1, board, sizeof(int)*SIZE);
				memcpy(board, board_cpy, sizeof(int)*SIZE);
				anscount++;
			}
		}
		if(anscount == 0){
			if(used == SIZE*SIZE || counter == 1){
				break;
			}
			color = 1 - color;
			continue;
		}
		if(counter == 1){
			anscount = 0;
			memcpy(cuda_data_out+(SIZE*SIZE*SIZE+1)*threadIdx.x+anscount*SIZE+1, board, sizeof(int)*SIZE);
			anscount = 1;
		}
		break;
	}
	*(cuda_data_out+(SIZE*SIZE*SIZE+1)*threadIdx.x) = anscount;
	return;
}

void set_end_value(tree* root, int* board){
	int white = 9;
	int black = 0;
	for(int x = 0; x < SIZE; x++){
		for(int y = 0; y < SIZE; y++){
			switch(board[x] >> (2*y) & 0x00000003){
				case 2:
					black++;
					root = root->next_block[2];
					break;
				case 3:
					white++;
					root = root->next_block[3];
					break;
				default:
					root = root->next_block[0];
					break;
			}
		}
	}
	if(black > white){
		root->leaf->black_value = 1;
		root->leaf->white_value = 0;
	}
	else if(white > black){
		root->leaf->black_value = 0;
		root->leaf->white_value = 1;
	}
	else{
		root->leaf->black_value = 1;
		root->leaf->white_value = 1;
	}
	return;
}

leaves* get_leaf_pointer(tree* root, int* board){
	for(int x = 0; x < SIZE; x++){
		for(int y = 0; y < SIZE; y++){
			root = root->next_block[board[x] >> (2*y) & 0x00000003];
		}
	}
	return root->leaf;
}

void search_recu(int thread, int* cuda_data_in, int color, tree* root, int level){
	//cout << "in." << endl;
	int* cuda_data_out;
	cudaMallocManaged(&cuda_data_out, sizeof(int)*(SIZE*SIZE*SIZE+1)*thread);
	cuda_check <<<1, thread>>> (cuda_data_in, cuda_data_out, SIZE, color);
	int* next_level_data;
	cudaMallocManaged(&next_level_data, sizeof(int)*SIZE*maxthreads);
	cudaDeviceSynchronize();
	int data_count = 0;
	for(int thread_num = 0; thread_num < thread; thread_num++){
		for(int a = 0; a < *(cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num); a++){
			make_leaf(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a);
			if(get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->white_value == -1){
				memcpy(next_level_data+SIZE*data_count, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a, sizeof(int)*SIZE);
				//print(next_level_data+SIZE*data_count);
				data_count++;
				if(data_count == maxthreads){
					search_recu(maxthreads, next_level_data, 1-color, root, level+1);
					data_count = 0;
				}
			}
			else if(get_leaf_pointer(root, cuda_data_in+SIZE*thread_num) == get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)){
				memcpy(next_level_data+SIZE*data_count, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a, sizeof(int)*SIZE);
				//print(next_level_data+SIZE*data_count);
				data_count++;
				if(data_count == maxthreads){
					search_recu(maxthreads, next_level_data, 1-color, root, level+1);
					data_count = 0;
				}
			}
		}
	}
	if(data_count != 0){
		search_recu(data_count, next_level_data, 1-color, root, level+1);
	}
	for(int thread_num = 0; thread_num < thread; thread_num++){
		if(*(cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num) == 0){
			set_end_value(root, cuda_data_in+SIZE*thread_num);
			/*
			print(cuda_data_in+SIZE*thread_num);
			char meow;
			cin >> meow;
			*/
			endcount++;
		}
		else{
			get_leaf_pointer(root, cuda_data_in+SIZE*thread_num)->black_value = 1;
			for(int a = 0; a < *(cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num); a++){
				if(get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->white_value == 1){
					get_leaf_pointer(root, cuda_data_in+SIZE*thread_num)->black_value = 0;
					break;
				}
				if(get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->white_value == -1){
					
					cout << "ERROR! in white value" << endl;
					cout << get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->black_value << " " << get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->white_value << endl;
					print(cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a);
					char meow;
					cin >> meow;
					
					error_count++;
				}
			}
			get_leaf_pointer(root, cuda_data_in+SIZE*thread_num)->white_value = 1;
			for(int a = 0; a < *(cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num); a++){
				if(get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->black_value == 1){
					get_leaf_pointer(root, cuda_data_in+SIZE*thread_num)->white_value = 0;
					break;
				}
				if(get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->black_value == -1){
					
					cout << "ERROR! in black value" << endl;
					cout << get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->black_value << " " << get_leaf_pointer(root, cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a)->white_value << endl;
					print(cuda_data_out+(SIZE*SIZE*SIZE+1)*thread_num+1+SIZE*a);
					char meow;
					cin >> meow;
					
					error_count++;
				}
			}
		}
	}
	cudaFree(&cuda_data_out);
	cudaFree(&next_level_data);
	if(level >= SIZE*SIZE){
		//cout << level << " " << count_child(root, 0) << endl;
	}
	else{
		//cout << level << endl;
	}
	//cout << "return." << endl;
	return;
}
