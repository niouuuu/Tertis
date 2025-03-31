#include<stdio.h>
#include<stdlib.h>
#include<conio.h> // ����x��J
#include<time.h>
#include<windows.h>

#define HEIGHT 24
#define WIDTH 28
#define OFFSET_X 1
#define OFFSET_Y 1
#define block_h 4
#define block_w 4
#define NEXT_HEIGHT 4
#define NEXT_WIDTH 4

enum block_type{
	I,J,L,O,S,T,Z	//�C�|���O�]enumeration type�^�A�Ψөw�q�@�զ��W�٪���Ʊ`��
}; 

int block_type_num = 7;
int cur_x = 0, cur_y = 0;
int color_to_change_pattern;
int over = 0;

// �U�������l�˦�
int BlockI[block_h][block_w] = {
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0}
};

int BlockJ[block_h][block_w] = {
	{0,0,1,0},
	{0,0,1,0},
	{0,1,1,0},
	{0,0,0,0}
};

int BlockL[block_h][block_w] = {
	{0,1,0,0},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};

int BlockO[block_h][block_w] = {
	{0,0,0,0},
	{0,1,1,0},
	{0,1,1,0},
	{0,0,0,0}
};

int BlockS[block_h][block_w] = {
	{0,0,0,0},
	{0,1,1,0},
	{1,1,0,0},
	{0,0,0,0}
};

int BlockT[block_h][block_w] = {
	{0,0,0,0},
	{1,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};

int BlockZ[block_h][block_w] = {
	{0,0,0,0},
	{1,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};

int current_block[block_h][block_w];
int surface[HEIGHT][WIDTH] = {0};
int next_block[NEXT_HEIGHT][NEXT_WIDTH] = {0};

void setColor(int color);
void gotoxy(int x, int y);
void printxy(const char* str, int x, int y);
void setCursorVisable(int visible);
void print_block(int block[][block_w],int x, int y);
void copy_block(int block_t[][block_w],int block_c[][block_w]);
void generate_next_block();
void make_new_block();
void rotate_block_with_adjustment();
int isCollision(int x, int y, int block[block_h][block_w]);
int is_line_fill(int h);
void land_block();
void adjust_position_after_rotation(int rotated_block[][block_w]);
int get_line_score(int line_count);
void check_line();
void print_surface();
void erase_block(int block[][block_w], int x, int y);
void get_block_top_bottom_margin(int block_arr[][block_w], int *top_margin, int *bottom_margin);
void get_block_left_right_margin(int block_arr[][block_w], int *left_margin, int *right_margin);
void move_block_down();
void key_control();
void erase_print_next_block_area(int next_block[NEXT_HEIGHT][NEXT_WIDTH]);
void print_next_block(int next_block[NEXT_HEIGHT][NEXT_WIDTH]);
void print_border();
void print_score();
void game_over_screen();

enum block_type currentType;
enum block_type nextType;

int currentColor;
int nextColor;

UINT_PTR timerId; // �o�q�`�Ω� Windows API ���p�ɾ��\��A�Ҧp SetTimer() �禡�|��^�@�ӭp�ɾ� ID�A�Ψ��ѧO�S�w���p�ɾ��C
int score = 0;
int lines_cleared = 0;

// �]�w��r�C�� 
void setColor(int color){       
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
}

// ���ʴ�Ш���w��m�C
void gotoxy(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// �b���w��m�L�X�r�Ŧ� 
void printxy(const char* str, int x, int y){
    gotoxy(x + OFFSET_X, y + OFFSET_Y);
    printf("%s", str);
}

// �]�w����x��Ъ��i����
void setCursorVisable(int visible){
    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(handle, &cursorInfo);
}

// �L�X����A�ϥη�e�C��]�� color_to_change_pattern �M�w�^
void print_block(int block[][block_w],int x, int y){
	setColor(color_to_change_pattern);
	int width, height;
	for(height = 0; height < block_h; height++) {
    		for(width = 0; width < block_w; width++) {
			if(block[height][width] == 1 && over == 0){
				printxy("#",x+width,y+height);
            }
        }
    }
}

void copy_block(int block_t[][block_w],int block_c[][block_w]){
	int width = 0, height = 0;
	for(width = 0; width < block_w; width++){
		for(height = 0; height < block_h; height++){
			block_c[height][width] = block_t[height][width];
		}
	}
}

// �ھ��H���ƥͦ��U�@�Ӥ���Ψ��C��
void generate_next_block(){
	nextType = (enum block_type)(rand() % block_type_num);
	switch(nextType){
		case I:
			nextColor = 11;
			copy_block(BlockI,next_block);
			break;
		case J:
			nextColor = 9;
			copy_block(BlockJ,next_block);
			break;
		case L:
			nextColor = 15;
			copy_block(BlockL,next_block);
			break;
		case O:
			nextColor = 14;
			copy_block(BlockO,next_block);
			break;
		case S:
			nextColor = 10;
			copy_block(BlockS,next_block);
			break;
		case T:
			nextColor = 13;
			copy_block(BlockT,next_block);
			break;
		case Z:
			nextColor = 12;
			copy_block(BlockZ,next_block);
			break;
		default:
			break;
	}
}

// ���ͷs�� current_block�G�� next_block ���� current_block�A�A�ͦ��U�@�� next_block
void make_new_block(){
	//���� next_block -> current_block
    copy_block(next_block, current_block);
    color_to_change_pattern = nextColor;
    currentType = nextType;
    currentColor = nextColor;
    
    //���] current_block ����l��m
    cur_x = (WIDTH - block_w) / 2;
    cur_y = 0;
    
    print_block(current_block, cur_x, cur_y);
    generate_next_block(); 
    erase_print_next_block_area(next_block);
    print_next_block(next_block); // ��ܤU�@�Ӥ��
}

// ������: ���ɰw����90�� 
void rotate_block_with_adjustment() {
    int temp_block[block_h][block_w] = {0};
    // ���ऽ���Gnew[r][c] = old[block_h - 1 - c][r]
    int height = 0, width = 0;
    for (height = 0; height < block_h; height++) {
        for (width = 0; width < block_w; width++) {
            temp_block[height][width] = current_block[block_h - 1 - width][height];
        }
    }
    // �p�G����ᤣ�I���A�����ĥ�
    if (!isCollision(cur_x, cur_y, temp_block)) {
        copy_block(temp_block, current_block);
    } else {
        // �Y�I���h���սվ��m�]�ϥα���᪺�s�Ϊ��p����ɡ^
        adjust_position_after_rotation(temp_block);
        if (!isCollision(cur_x, cur_y, temp_block)) {
            copy_block(temp_block, current_block);
        }
    }
}

// �ˬd�O�_�W�X��� 
int isCollision(int x, int y, int block[block_h][block_w]){
	int row, col;
	for(row = 0; row < block_h; row++){
		for(col = 0; col < block_w; col++){
			if(block[row][col] == 1){
				// �O�_�W�X��� 
				if(x + col < 0 || x + col >= WIDTH || y + row >= HEIGHT ){
					return 1;
				}
				// �O�_�P�s�b��������|�A�åB�b���Ľd��
                	if(y + row >= 0 && surface[y + row][x + col] != 0){
                  		return 1;
                   	}
                }
           	}
        }
    return 0;
}

// �ˬdrow �O�_�w�� 
int is_line_fill(int h){
	int col;
	for(col = 0; col < WIDTH; col++){
		if(surface[h][col] == 0){
			return 0;
		}
	}
	return 1;
}

// �w�q�B�~�[��
int get_line_score(int line_count){
	if(line_count >= 2){
		return 200 * line_count;
	}
	else{
		return 100 * line_count;
	}
}

// �M���w�񺡪�row
void check_line(){
	int clearedLines = 0; // �O�����������
	int row = HEIGHT - 1; // �q������row�ˬd 
	while(row >= 0){
		if(is_line_fill(row)){
			clearedLines++;
			int r = row, c= 0;
			for(r = row; r > 0; r--){
				for(c = 0; c < WIDTH; c++){
					surface[r][c] = surface[r - 1][c];
					// �o�˾�ӹC���e���q��e�橹�W���U���@�� 
				}
			}
			for(c = 0; c < WIDTH; c++){
				surface[0][c] = 0;
			}
		}
		else{
			row--;
		}
	}
	if(clearedLines > 0){
		score += (clearedLines >= 2 ? 200 * clearedLines : 100 * clearedLines);
		lines_cleared += clearedLines;
	}
} 

// ��e�������R���A�K�[�� surface �}�C���A�òM����e���
void land_block(){
    int row, col;
    for(row = 0; row < block_h; row++){
        for(col = 0; col < block_w; col++){
            if(current_block[row][col] == 1){
                int surface_x = cur_x + col;
                int surface_y = cur_y + row;
                if(surface_y >= 0 && surface_y < HEIGHT && surface_x >=0 && surface_x < WIDTH){
                    surface[surface_y][surface_x] = currentColor;
                }
            }
        }
    }
}

// ��ø��ӹC���ϰ�G�ھ� surface �}�C�����C��ȨӨM�w
void print_surface(){
	int row, col;
	for(row = 0; row < HEIGHT; row++){
		for(col = 0; col < WIDTH; col++){
			if(surface[row][col] != 0){
				setColor(surface[row][col]);
				printxy("#",col,row);
			}
			else{
				printxy(" ",col,row);
			}
		}
	}
} 

// �������w��m�����
void erase_block(int block[][block_w], int x, int y){
	int width, height;
	for(height = 0; height < block_h; height++){
		for(width = 0; width < block_w; width++){
			if(block[height][width] == 1){
				printxy(" ", x + width, y + height);
			}
		}
	}	
}

// �ھڶǤJ�� block_arr �p��W�U��ɡ]����R���̤p�P�̤j row�^
void get_block_top_bottom_margin(int block_arr[][block_w], int *top_margin, int *bottom_margin) {
    *top_margin = block_h;
    *bottom_margin = 0;
    int height = 0 , col = 0;
    for (height = 0; height < block_h; height++) {
        int rowFilled = 0;
        for (col = 0; col < block_w; col++) {
            if (block_arr[height][col] != 0) {
                rowFilled = 1;
                break;
            }
        }
        if (rowFilled) {
            if (height < *top_margin)
                *top_margin = height;
            if (height > *bottom_margin)
                *bottom_margin = height;
        }
    }
}

// �ھڶǤJ�� block_arr �p�⥪�k��ɡ]����R���̤p�P�̤j col�^
void get_block_left_right_margin(int block_arr[][block_w], int *left_margin, int *right_margin) {
    *left_margin = block_w;
    *right_margin = 0;
    int col = 0, row = 0;
    for (col = 0; col < block_w; col++) {
        int colFilled = 0;
        for (row = 0; row < block_h; row++) {
            if (block_arr[row][col] != 0) {
                colFilled = 1;
                break;
            }
        }
        if (colFilled) {
            if (col < *left_margin)
                *left_margin = col;
            if (col > *right_margin)
                *right_margin = col;
        }
    }
}
 
// �ϥα���᪺����ӽվ� cur_x �P cur_y�A������ᤣ�|�W�X���
void adjust_position_after_rotation(int rotated_block[][block_w]){
	int left_margin, right_margin, top_margin, bottom_margin;
	// �p�����������W�U���k��� 
	get_block_top_bottom_margin(rotated_block, &top_margin, &bottom_margin);
	get_block_left_right_margin(rotated_block, &left_margin, &right_margin);
	// �W�X�����
	if(cur_x + left_margin < 0){
		cur_x = -left_margin;
	}
	// �W�X�k���
	if(cur_x  + right_margin >= WIDTH){
		cur_x = WIDTH - right_margin - 1;
	} 
	// �W�X�U���
	if(cur_y + bottom_margin >=  HEIGHT){
		cur_y = HEIGHT - bottom_margin - 1;
	}
	// �W�X�W���
	if(cur_y + top_margin < 0){
		cur_y = -top_margin ;
	} 
}
 
// �N��e����V�U���ʡA�Y�I���h���w�B�ˬd����åͦ��s���
void move_block_down(){
    erase_block(current_block, cur_x, cur_y);  // �M���¤��
    cur_y++;  // �����ձN������ʦV�U
    
    if(isCollision(cur_x, cur_y, current_block)){  // �p�G���ʫ�I���F
        cur_y--;  // �^���Ӧ�m
        land_block();  // �N������쩳��
        check_line();  // �ˬd�O�_����Q�M��
        make_new_block();  // �ͦ��s�����
        if(isCollision(cur_x, cur_y, current_block)){  // �p�G�s����]�I���F�A�C������
            over = 1;
        }
    }
    print_surface();  // ��s�a��
    print_block(current_block, cur_x, cur_y);  // ��ܷs���
}

// ��L������
void key_control() {
    if (kbhit()) {
        char key = getch();
        erase_block(current_block, cur_x, cur_y);  // �M���¤��
        switch (key) {
            case 'a':  // ����
                if (!isCollision(cur_x - 1, cur_y, current_block)) cur_x--;
                break;
            case 'd':  // �k��
                if (!isCollision(cur_x + 1, cur_y, current_block)) cur_x++;
                break;
            case 's':  // �[�t�U��
                move_block_down();
                break;
            case 'w':  // ����
                rotate_block_with_adjustment();
                break;
            default:
                break;
        }
        // **ø�s�s������P�a��**
		print_surface();
		print_block(current_block, cur_x, cur_y);
    }
}

// �M����ܤU�@�Ӥ���ϰ�
void erase_print_next_block_area(int next_block[NEXT_HEIGHT][NEXT_WIDTH]){
	int height, width;
	// �M�� 
	for(height = 0; height < NEXT_HEIGHT; height++){
		gotoxy(WIDTH + 3, 5 + height);
		for(width = 0; width < NEXT_WIDTH; width++){
			printf(" ");
		}
	}
}

// �L�X�U�@�Ӥ���A�ó]�w�� nextColor
void print_next_block(int next_block[NEXT_HEIGHT][NEXT_WIDTH]){ 
	int height, width;
	gotoxy(WIDTH + 3, 4);
	printf("Next Block : ");
	setColor(nextColor);
	// �A�ӦL�X�s��next_block 
	for(height = 0; height < NEXT_HEIGHT; height++){
		for(width = 0; width < NEXT_WIDTH; width++){
			if(next_block[height][width] == 1){
				gotoxy(WIDTH + 3 + width, 5 + height);
				printf("#");
			}
		}
	}
}

// ��ܱo��
void print_score() {
	setColor(nextColor); 
    gotoxy(WIDTH + 3, 1);  // ��ܦb�e���k�W��
    printf("Score: %d ", score);
}

// �C���~��
void print_border() {
    int i;
    // ��ܳ����M�������
    for (i = 0; i < WIDTH + 2; i++) {
        gotoxy(i , 0);
        printf("I");
        gotoxy(i, HEIGHT + 1);
        printf("I");
    }

    // ��ܥ��k���
    for (i = 0; i < HEIGHT; i++) {
        gotoxy(0, i + 1);
        printf("I");
        gotoxy(WIDTH + 1, i + 1);
        printf("I");
    }
}

// ��ܹC�������e��
void game_over_screen() {
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2);  // �N���в��ܤ���
    printf("Game Over");
    
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2 + 1);  // ��ܤ���
    printf("Score: %d", score);
    
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2 + 2);  // ��ܴ��ܻy
    printf("Press any key to exit.");
}

int main(){
    // ��l���H���ƥͦ���
    srand(time(NULL));
    
    // ���ñ���x���
    setCursorVisable(0);
     
    // ��l�� surface �x�}
    int row, col;
    for(row = 0; row < HEIGHT; row++){
        for(col = 0; col < WIDTH; col++){
            surface[row][col] = 0;
        }
    }
    // DWORD �O Windows �����@�ظ�ƫ��A
	//�N�� "Double Word"�]���r�^�A��j�p�O 32 �줸�]4 �r�`�^
	// ���q�`�Ψ��x�s�L�Ÿ������
	const int AUTO_DROP_INTERVAL = 500;   //  �C500�@��۰ʤU���@��
    DWORD lastUpdate = GetTickCount();    // �W������۰ʤU�����ɶ�
    DWORD currentTime;
    const int FRAME_INTERVAL = 30;        // �C���j�鳣Sleep 30ms
    
	printf("\n\n\t\t�C������");
    printf("\n\ttips : \n");
    printf("\t   w�����ܤ�V\n");
    printf("\t   s���[�t�V�U\n");
    printf("\t   a d����V����\n");
    Sleep(5000);
	system("cls");
	
	print_border();
	print_score();    

    // ���ͦ��u�Ĥ@�ӡvnext_block
    generate_next_block();
    // ���۩I�s make_new_block()�A���|��u�{�b�� next_block�v�אּ current_block�A
    // �M��A�ͦ��u�U�@�ӡvnext_block�C
    make_new_block();
    
	while(!over) {
    	key_control(); 
		currentTime = GetTickCount();
		if(currentTime - lastUpdate >= AUTO_DROP_INTERVAL) {
        move_block_down();
        lastUpdate = currentTime;
    }
        print_score();
        print_next_block(next_block);
        Sleep(FRAME_INTERVAL);
    }

	game_over_screen();
    // ���ݥΤ�����h�X
    getch();
    system("pause");
    return 0;
} 
