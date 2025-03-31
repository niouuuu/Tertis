#include<stdio.h>
#include<stdlib.h>
#include<conio.h> // 控制台輸入
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
	I,J,L,O,S,T,Z	//列舉型別（enumeration type），用來定義一組有名稱的整數常數
}; 

int block_type_num = 7;
int cur_x = 0, cur_y = 0;
int color_to_change_pattern;
int over = 0;

// 各方塊的原始樣式
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

UINT_PTR timerId; // 這通常用於 Windows API 的計時器功能，例如 SetTimer() 函式會返回一個計時器 ID，用來識別特定的計時器。
int score = 0;
int lines_cleared = 0;

// 設定文字顏色 
void setColor(int color){       
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
}

// 移動游標到指定位置。
void gotoxy(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 在指定位置印出字符串 
void printxy(const char* str, int x, int y){
    gotoxy(x + OFFSET_X, y + OFFSET_Y);
    printf("%s", str);
}

// 設定控制台游標的可見性
void setCursorVisable(int visible){
    CONSOLE_CURSOR_INFO cursorInfo;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(handle, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(handle, &cursorInfo);
}

// 印出方塊，使用當前顏色（由 color_to_change_pattern 決定）
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

// 根據隨機數生成下一個方塊及其顏色
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

// 產生新的 current_block：把 next_block 移至 current_block，再生成下一個 next_block
void make_new_block(){
	//先把 next_block -> current_block
    copy_block(next_block, current_block);
    color_to_change_pattern = nextColor;
    currentType = nextType;
    currentColor = nextColor;
    
    //重設 current_block 的初始位置
    cur_x = (WIDTH - block_w) / 2;
    cur_y = 0;
    
    print_block(current_block, cur_x, cur_y);
    generate_next_block(); 
    erase_print_next_block_area(next_block);
    print_next_block(next_block); // 顯示下一個方塊
}

// 旋轉方塊: 順時針選轉90度 
void rotate_block_with_adjustment() {
    int temp_block[block_h][block_w] = {0};
    // 旋轉公式：new[r][c] = old[block_h - 1 - c][r]
    int height = 0, width = 0;
    for (height = 0; height < block_h; height++) {
        for (width = 0; width < block_w; width++) {
            temp_block[height][width] = current_block[block_h - 1 - width][height];
        }
    }
    // 如果旋轉後不碰撞，直接採用
    if (!isCollision(cur_x, cur_y, temp_block)) {
        copy_block(temp_block, current_block);
    } else {
        // 若碰撞則嘗試調整位置（使用旋轉後的新形狀計算邊界）
        adjust_position_after_rotation(temp_block);
        if (!isCollision(cur_x, cur_y, temp_block)) {
            copy_block(temp_block, current_block);
        }
    }
}

// 檢查是否超出邊界 
int isCollision(int x, int y, int block[block_h][block_w]){
	int row, col;
	for(row = 0; row < block_h; row++){
		for(col = 0; col < block_w; col++){
			if(block[row][col] == 1){
				// 是否超出邊界 
				if(x + col < 0 || x + col >= WIDTH || y + row >= HEIGHT ){
					return 1;
				}
				// 是否與存在的方塊重疊，並且在有效範圍內
                	if(y + row >= 0 && surface[y + row][x + col] != 0){
                  		return 1;
                   	}
                }
           	}
        }
    return 0;
}

// 檢查row 是否已填滿 
int is_line_fill(int h){
	int col;
	for(col = 0; col < WIDTH; col++){
		if(surface[h][col] == 0){
			return 0;
		}
	}
	return 1;
}

// 定義額外加分
int get_line_score(int line_count){
	if(line_count >= 2){
		return 200 * line_count;
	}
	else{
		return 100 * line_count;
	}
}

// 清除已填滿的row
void check_line(){
	int clearedLines = 0; // 記錄消除的行數
	int row = HEIGHT - 1; // 從底部的row檢查 
	while(row >= 0){
		if(is_line_fill(row)){
			clearedLines++;
			int r = row, c= 0;
			for(r = row; r > 0; r--){
				for(c = 0; c < WIDTH; c++){
					surface[r][c] = surface[r - 1][c];
					// 這樣整個遊戲畫面從當前行往上都下移一行 
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

// 當前方塊的填充狀態添加到 surface 陣列中，並清除當前方塊
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

// 重繪整個遊戲區域：根據 surface 陣列中的顏色值來決定
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

// 擦除指定位置的方塊
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

// 根據傳入的 block_arr 計算上下邊界（有填充的最小與最大 row）
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

// 根據傳入的 block_arr 計算左右邊界（有填充的最小與最大 col）
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
 
// 使用旋轉後的方塊來調整 cur_x 與 cur_y，讓旋轉後不會超出邊界
void adjust_position_after_rotation(int rotated_block[][block_w]){
	int left_margin, right_margin, top_margin, bottom_margin;
	// 計算旋轉後方塊的上下左右邊界 
	get_block_top_bottom_margin(rotated_block, &top_margin, &bottom_margin);
	get_block_left_right_margin(rotated_block, &left_margin, &right_margin);
	// 超出左邊界
	if(cur_x + left_margin < 0){
		cur_x = -left_margin;
	}
	// 超出右邊界
	if(cur_x  + right_margin >= WIDTH){
		cur_x = WIDTH - right_margin - 1;
	} 
	// 超出下邊界
	if(cur_y + bottom_margin >=  HEIGHT){
		cur_y = HEIGHT - bottom_margin - 1;
	}
	// 超出上邊界
	if(cur_y + top_margin < 0){
		cur_y = -top_margin ;
	} 
}
 
// 將當前方塊向下移動，若碰撞則落定、檢查消行並生成新方塊
void move_block_down(){
    erase_block(current_block, cur_x, cur_y);  // 清除舊方塊
    cur_y++;  // 先嘗試將方塊移動向下
    
    if(isCollision(cur_x, cur_y, current_block)){  // 如果移動後碰撞了
        cur_y--;  // 回到原來位置
        land_block();  // 將方塊落到底部
        check_line();  // 檢查是否有行被清除
        make_new_block();  // 生成新的方塊
        if(isCollision(cur_x, cur_y, current_block)){  // 如果新方塊也碰撞了，遊戲結束
            over = 1;
        }
    }
    print_surface();  // 更新地面
    print_block(current_block, cur_x, cur_y);  // 顯示新方塊
}

// 鍵盤控制方塊
void key_control() {
    if (kbhit()) {
        char key = getch();
        erase_block(current_block, cur_x, cur_y);  // 清除舊方塊
        switch (key) {
            case 'a':  // 左移
                if (!isCollision(cur_x - 1, cur_y, current_block)) cur_x--;
                break;
            case 'd':  // 右移
                if (!isCollision(cur_x + 1, cur_y, current_block)) cur_x++;
                break;
            case 's':  // 加速下移
                move_block_down();
                break;
            case 'w':  // 旋轉
                rotate_block_with_adjustment();
                break;
            default:
                break;
        }
        // **繪製新的方塊與地面**
		print_surface();
		print_block(current_block, cur_x, cur_y);
    }
}

// 清除顯示下一個方塊區域
void erase_print_next_block_area(int next_block[NEXT_HEIGHT][NEXT_WIDTH]){
	int height, width;
	// 清除 
	for(height = 0; height < NEXT_HEIGHT; height++){
		gotoxy(WIDTH + 3, 5 + height);
		for(width = 0; width < NEXT_WIDTH; width++){
			printf(" ");
		}
	}
}

// 印出下一個方塊，並設定為 nextColor
void print_next_block(int next_block[NEXT_HEIGHT][NEXT_WIDTH]){ 
	int height, width;
	gotoxy(WIDTH + 3, 4);
	printf("Next Block : ");
	setColor(nextColor);
	// 再來印出新的next_block 
	for(height = 0; height < NEXT_HEIGHT; height++){
		for(width = 0; width < NEXT_WIDTH; width++){
			if(next_block[height][width] == 1){
				gotoxy(WIDTH + 3 + width, 5 + height);
				printf("#");
			}
		}
	}
}

// 顯示得分
void print_score() {
	setColor(nextColor); 
    gotoxy(WIDTH + 3, 1);  // 顯示在畫面右上方
    printf("Score: %d ", score);
}

// 遊戲外框
void print_border() {
    int i;
    // 顯示頂部和底部邊框
    for (i = 0; i < WIDTH + 2; i++) {
        gotoxy(i , 0);
        printf("I");
        gotoxy(i, HEIGHT + 1);
        printf("I");
    }

    // 顯示左右邊框
    for (i = 0; i < HEIGHT; i++) {
        gotoxy(0, i + 1);
        printf("I");
        gotoxy(WIDTH + 1, i + 1);
        printf("I");
    }
}

// 顯示遊戲結束畫面
void game_over_screen() {
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2);  // 將光標移至中心
    printf("Game Over");
    
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2 + 1);  // 顯示分數
    printf("Score: %d", score);
    
    gotoxy(WIDTH / 2 - 5, HEIGHT / 2 + 2);  // 顯示提示語
    printf("Press any key to exit.");
}

int main(){
    // 初始化隨機數生成器
    srand(time(NULL));
    
    // 隱藏控制台游標
    setCursorVisable(0);
     
    // 初始化 surface 矩陣
    int row, col;
    for(row = 0; row < HEIGHT; row++){
        for(col = 0; col < WIDTH; col++){
            surface[row][col] = 0;
        }
    }
    // DWORD 是 Windows 中的一種資料型態
	//代表 "Double Word"（雙字），其大小是 32 位元（4 字節）
	// 它通常用來儲存無符號的整數
	const int AUTO_DROP_INTERVAL = 500;   //  每500毫秒自動下落一次
    DWORD lastUpdate = GetTickCount();    // 上次方塊自動下落的時間
    DWORD currentTime;
    const int FRAME_INTERVAL = 30;        // 每次迴圈都Sleep 30ms
    
	printf("\n\n\t\t遊戲說明");
    printf("\n\ttips : \n");
    printf("\t   w為改變方向\n");
    printf("\t   s為加速向下\n");
    printf("\t   a d為橫向移動\n");
    Sleep(5000);
	system("cls");
	
	print_border();
	print_score();    

    // 先生成「第一個」next_block
    generate_next_block();
    // 接著呼叫 make_new_block()，它會把「現在的 next_block」改為 current_block，
    // 然後再生成「下一個」next_block。
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
    // 等待用戶按鍵後退出
    getch();
    system("pause");
    return 0;
} 
