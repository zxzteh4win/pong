#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

struct game_set{
    int ball_x;
    int ball_y;
    int ball_spd_x;
    int ball_spd_y;
    int left_pad_x;
    int left_pad_y;
    int right_pad_x;
    int right_pad_y;
    int left_pad_size;
    int right_pad_size;
    int game_speed;
};
char** mem_alloc(char** field, int row, int col);
void default_pos(char** field, int row, int col);
void output_field(char** field, int row, int col, struct game_set* set);
struct game_set* init_set(struct game_set* set);

int row = 25;
int col = 80;

int main() {
    struct game_set set;
    char** field = NULL;

    set = init_set(&set);

    start_screen();
    field = mem_alloc(field, row, col);
    default_pos(field, row, col);
    output_field(field, row, col);
    free(field);

}


char** mem_alloc(char** field, int row, int col) {
    field = malloc(sizeof(char*) * row + sizeof(char) * row * col);
    char *p = (char*) (field + row);
    for (int i = 0; i < row; i++) {
        field[i] = p + i * col;
    }
    return field;
}

char** fill_field(char** field, int row, int col, struct game_set* set) {
    for (int i = 0 ; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (set->ball_x == j && set->ball_spd_y == i) {
                field[i][j] = '*';
            } else if (set->left_pad_x == i && set->left_pad_y >= j + set->left_pad_size / 2 && set->left_pad_y)
            
        }
    }
}


void output_field(char** field, int row, int col, struct game_set* set) {
    for (int i = 0 ; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%c", field[i][j]);
        }
        printf("\n");
    }
}

void default_pos(char** field, int row, int col) {
    for (int i = 0 ; i < row; i++) {
        for (int j = 0; j < col; j++) {
            field[i][j] = '0';
        }
    }
}

struct game_set* init_set(struct game_set* set) {
    set->ball_x = col / 2;
    set->ball_y = row / 2 + 1;
    set->ball_spd_x = 1;
    set->ball_spd_y = 1;
    set->left_pad_x = 1;
    set->left_pad_y = row / 2 + 1;
    set->right_pad_x = 78;
    set->right_pad_y = row / 2 + 1;
    int left_pad_size = 3;
    int right_pad_size =  3;
    int game_speed = 1000;
}

// void ErrorHandler()