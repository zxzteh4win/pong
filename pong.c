#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <time.h>

struct game_set {
    int field_width;
    int field_height;
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
    int left_score;
    int right_score;
    int score_limit;
    int game_speed;
    int random_speed_enable;
    int random_pad_size_enable; 
};

char** mem_alloc(char** field, struct game_set* set);
void output_field(char** field, struct game_set* set);
char** fill_field(char** field, struct game_set* set);
struct game_set default_set(struct game_set* set);
struct game_set collision_handler(struct game_set* set);
struct game_set ball_refresh(struct game_set* set);
struct game_set check_out(struct game_set* set);
struct game_set move_pad(struct game_set* set, char input);
struct game_set main_menu(struct game_set* set);
struct game_set speed_menu(struct game_set* set);
struct game_set field_size_menu(struct game_set* set);
struct game_set pad_size_menu(struct game_set* set);
struct game_set throw_ball(struct game_set* set);
int get_random(int lowest, int highest);

int main() {
    char input = '-';
    initscr();
    nodelay(stdscr, TRUE);
    noecho();
    struct game_set set;
    char** field = NULL;
    set = main_menu(&set);
    refresh();
    field = mem_alloc(field, &set);
    while (input != '0') {
        field = fill_field(field, &set);
        output_field(field, &set);
        input = getch();
        set = move_pad(&set, input);
        set = ball_refresh(&set);
        set = collision_handler(&set);
        field = fill_field(field, &set);
        output_field(field, &set);
        set = check_out(&set);
        usleep(300000 / set.game_speed);
    }
    free(field);
    endwin();
}

char** mem_alloc(char** field, struct game_set* set) {
    field = malloc(sizeof(char*) * set->field_height +
                   sizeof(char) * set->field_height * set->field_width);
    char *p = (char*) (field + set->field_height);
    for (int i = 0; i < set->field_height; i++) {
        field[i] = p + i * set->field_width;
    }
    return field;
}

char** fill_field(char** field, struct game_set* set) {
    for (int i = 0 ; i < set->field_height; i++) {
        for (int j = 0; j < set->field_width; j++) {
            if (set->ball_x == j && set->ball_y == i) {
                field[i][j] = '*';
            } else if ((set->left_pad_x == j) &&
                      ((set->left_pad_y + set->left_pad_size / 2) >= i) &&
                      ((set->left_pad_y - set->left_pad_size / 2) <= i)) {
                field[i][j] = '#';
            } else if ((set->right_pad_x == j) &&
                       (set->right_pad_y + set->right_pad_size / 2 >= i) &&
                       (set->right_pad_y - set->right_pad_size / 2 <= i)) {
                field[i][j] = '#';
            } else if (j == set->field_width / 2 || j == set->field_width / 2 + 1) {
                field[i][j] = '|';
            } else if (i == 1 && j == set->field_width / 2 - 3) {
                field[i][j] = (set->left_score / 10) + '0';
                j++;
                field[i][j] = (set->left_score % 10) + '0';
            } else if (i == 1 && j == set->field_width / 2 + 3) {
                field[i][j] = (set->right_score / 10) + '0';
                j++;
                field[i][j] = (set->right_score % 10) + '0';
            } else {
                field[i][j] = ' '; 
            }
        }
    }
    return field;
}

void output_field(char** field, struct game_set* set) {
    clear();
    for (int i = 0; i < set->field_width; i++) {
        printw("-");
    }
    printw("\n");
    for (int i = 0 ; i < set->field_height; i++) {
        for (int j = 0; j < set->field_width; j++) {
            printw("%c", field[i][j]);
        }
        printw("\n");
    }
    for (int i = 0; i < set->field_width; i++) {
        printw("-");
    }
    printw("\n");
    refresh();
}

struct game_set default_set(struct game_set* set) {
    set->field_width = 80;
    set->field_height = 25;
    set->ball_x = set->field_width / 2;
    set->ball_y = set->field_height / 2;
    set->ball_spd_x = 1;
    set->ball_spd_y = 1;
    set->left_pad_x = 1;
    set->left_pad_y = set->field_height / 2;
    set->right_pad_x = 78;
    set->right_pad_y = set->field_height / 2;
    set->left_pad_size = 3;
    set->right_pad_size = 3;
    set->left_score = 0;
    set->right_score = 0;
    set->score_limit = 21;
    set->game_speed = 2;
    set->random_speed_enable = 0;
    set->random_pad_size_enable = 0;
    return *set;
}

struct game_set collision_handler(struct game_set* set) {
    int dif = 255;
    int bounced = 0;
    int near_left = 0;
    int near_right = 0;
    if (set->ball_y == 0 || set->ball_y == set->field_height - 1) {
        set->ball_spd_y = 0 - set->ball_spd_y;
        bounced = 1;
    }
    if (set->ball_x == 2) {
        dif = set->ball_y - set->left_pad_y;
        if (dif >= -1 * set->left_pad_size / 2 && dif <= set->left_pad_size / 2) {
            set->ball_spd_x = 0 - set->ball_spd_x;
        } else if ((dif == set->left_pad_size / 2 + 1 && set->ball_spd_y < 0) ||
                   (dif == -1 * set->left_pad_size / 2 - 1 && set->ball_spd_y > 0)) {
            set->ball_spd_x = 0 - set->ball_spd_x;
            set->ball_spd_y = 0 - set->ball_spd_y;
        }
        near_left = 1;
    }
    if (set->ball_x == set->field_width - 3) {
        dif = set->ball_y - set->right_pad_y;
        if (dif >= -1 * set->right_pad_size / 2 && dif <= set->right_pad_size / 2) {
            set->ball_spd_x = 0 - set->ball_spd_x;
        } else if ((dif == set->right_pad_size / 2 + 1 && set->ball_spd_y < 0) ||
                   (dif == -1 * set->right_pad_size / 2 - 1 && set->ball_spd_y > 0)) {
            set->ball_spd_x = 0 - set->ball_spd_x;
            set->ball_spd_y = 0 - set->ball_spd_y;
        }
        near_right = 1;
    }
    if ((dif != 255 || bounced) && set->random_speed_enable) {
        set->game_speed = get_random(1, 10);
    }
    if (dif != 255 && set->random_pad_size_enable) {
        if (near_left) {
            set->left_pad_size = get_random(1, 10);
        } else if (near_right) {
            set->right_pad_size = get_random(1, 10);            
        }
    }
    return *set;
}

struct game_set ball_refresh(struct game_set* set) {
    set->ball_x += set->ball_spd_x;
    set->ball_y += set->ball_spd_y;
    return *set;
}

struct game_set check_out(struct game_set* set) {
    int out = 0;
    if (set->ball_x == 0) {
        set->right_score++;
        out = 1;
    }
    if (set->ball_x == set->field_width - 1) {
        set->left_score++;
        out = 1;
    }
    if (out == 1) {
        *set = throw_ball(set);
    }
    return *set;
}

struct game_set throw_ball(struct game_set* set) {
    set->ball_x = set->field_width / 2;
    set->ball_y = get_random(0, set->field_height);
    set->ball_spd_x = 0;
    set->ball_spd_y = 0;
    while (set->ball_spd_x == 0) {
        set->ball_spd_x = get_random(-1, 1);
    }
    while (set->ball_spd_y == 0) {
        set->ball_spd_y = get_random(-1, 1);
    }
    return *set;
}

struct game_set move_pad(struct game_set* set, char input) {
    if (input == 'a' && set->left_pad_y > set->left_pad_size / 2) {
        set->left_pad_y--;
    } else if (input == 'z' && set->field_height - set->left_pad_y - 1 > set->left_pad_size / 2) {
        set->left_pad_y++;
    } else if (input == 'k' && set->right_pad_y > set->right_pad_size / 2) {
        set->right_pad_y--;
    } else if (input == 'm' && set->field_height - set->right_pad_y - 1 > set->left_pad_size / 2) {
        set->right_pad_y++;
    }
    return *set;
}

struct game_set main_menu(struct game_set* set) {
    *set = default_set(set);
    char input = '-';
    int chosen = 0;
    while (chosen != 1) {
        clear();
        printw("*PONG GAME*\n\n");
        printw("[1] START GAME\n");
        printw("[2] SELECT SPEED\n");
        printw("[3] SELECT FIELD SIZE\n");
        printw("[4] SELECT PAD SIZE\n");
        printw("[5] SPECIAL MODES\n");
        printw("[0] EXIT\n");       
        refresh();
        char input = getch();
        usleep(200000);
        if (input == '1') {
            chosen = 1;
        } else if (input == '2') {
            *set = speed_menu(set);
        } else if (input == '3') {
            *set = field_size_menu(set);
        } else if (input == '4') {
            *set = pad_size_menu(set);
        }
    }
    return *set;
}

struct game_set speed_menu(struct game_set* set) {
    char input = '-';
    int chosen = 0;
    while (chosen != 1) {
        clear();
        printw("*SELECT SPEED*\n\n");
        printw("[1] SLOW\n");
        printw("[2] MEDIUM\n");
        printw("[3] GOTTA GO FAST\n");
        printw("[4] OVER9000\n");
        printw("[5] RANDOM SPEED AFTER BOUNCE\n");
        printw("[0] EXIT\n");       
        refresh();
        char input = getch();
        usleep(200000);
        if (input >= '1' && input <= '4') {
            set->game_speed = 0;
            for (char i = '0'; i < input; i++) {
                set->game_speed++;
            }
            set->random_speed_enable = 0;
            chosen = 1;
        } else if (input == '5') {
            set->random_speed_enable = 1;
            chosen = 1;
        } else if (input == '0') {
            chosen = 1;
        }
    }
    return *set;
}

struct game_set field_size_menu(struct game_set* set) {
    char input = '-';
    int chosen = 0;
    while (chosen != 1) {
        clear();
        printw("*SELECT FIELD SIZE*\n\n");
        printw("[1] SMALL (13 x 40)\n");
        printw("[2] STANDART (25 x 80)\n");
        printw("[3] BIG (50 x 160)\n");
        printw("[4] WIDE (25 x 180)\n");
        printw("[5] NARROW 50 x 50\n");
        printw("[0] EXIT\n");       
        refresh();
        char input = getch();
        usleep(200000);
        if (input >= '0' && input <= '5' || input == '0') {
            if (input == '1') {
                set->field_height = 13;
                set->field_width = 40;
            } else if (input == '2') {
                set->field_height = 25;
                set->field_width = 80;
            } else if (input == '3') {
                set->field_height = 50;
                set->field_width = 160;
            } else if (input == '4') {
                set->field_height = 25;
                set->field_width = 180;
            } else if (input == '5') {
                set->field_height = 50;
                set->field_width = 50;
            }
        set-> right_pad_x = set->field_width - 2;
        chosen = 1;
        }
    }
    return *set;
}

struct game_set pad_size_menu(struct game_set* set) {
    char input = '-';
    int chosen = 0;
    while (chosen != 1) {
        clear();
        printw("*SELECT PAD SIZE*\n\n");
        printw("[1] DOT (1)\n");
        printw("[2] STANDART (3)\n");
        printw("[3] WIDE (5)\n");
        printw("[4] VERY WIDE (7)\n");
        printw("[5] RANDOM AFTER BOUNCE (1 - 10)\n");
        printw("[0] EXIT\n");       
        refresh();
        char input = getch();
        usleep(200000);
        if (input >= '0' && input <= '5' || input == '0') {
            if (input == '1') {
                set->left_pad_size = 1;
                set->right_pad_size = 1;
            } else if (input == '2') {
                set->left_pad_size = 3;
                set->right_pad_size = 3;
            } else if (input == '3') {
                set->left_pad_size = 5;
                set->right_pad_size = 5;
            } else if (input == '4') {
                set->left_pad_size = 7;
                set->right_pad_size = 7;
            } else if (input == '5') {
                set->random_pad_size_enable = 1;
            }
        chosen = 1;
        }
    }
    return *set;
}

int get_random(int lowest, int highest) {
    srand(time(NULL));
    int rand_num = rand() % (highest - lowest + 1) + lowest;
    return rand_num;
}


// void ErrorHandler()