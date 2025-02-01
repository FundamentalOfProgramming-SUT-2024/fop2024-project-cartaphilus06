#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#define MAX_CHAR 200
#define BACKGROUND_COLOR_PAIR 1
#define PLAYER_COLOR_PAIR 2
#define GOLD_COLOR_PAIR 3

typedef struct Player{
    int x;
    int y;
    int hp;
    char color;
    int gold;
    int key;
    int broken_key;
    int food;
    int hunger;
    int sword_count,magic_wand_count,hammer_count,mace_count,dagger_count,arrow_count;
    int hp_curse,speed_curse,damage_curse;
} Player;

typedef struct Monster{
    int x;
    int y;
    char kind;
} Monster;

typedef struct Door{
    int x;
    int y;
    int pass;
    int buttonx;
    int buttony;
    int password;
    int pass_existion;
} Door;

typedef struct person_info{
    char username[MAX_CHAR];
    char password[MAX_CHAR];
    char email[MAX_CHAR];
} person_info;

typedef struct Trap{
    int x;
    int y;
} Trap;

typedef struct item{
    int x;
    int y;
    int existion;
} item;

typedef struct Curse{
    int x;
    int y;
    int existion;
    char kind;
} Curse;

typedef struct weapon{
    int x;
    int y;
    int existion;
} weapon;

typedef struct Obstacle{
    int x;
    int y;
    int existion;
} Obstacle;

typedef struct invisibleDoor{
    int x;
    int y;
} invisibleDoor;

typedef struct Room{
    int x;
    int y;
    int width;
    int height;
    int index;
    int bottom_col,up_col;
    int bottom_row,up_row;
    item food,gold,key,stair;
    item golds[10];
    weapon sword,magic_wand,arrow,hammer,mace,dagger;
    Curse curse;
    Curse indoor_curses[6];
    invisibleDoor inDoor;
    Obstacle obstacle;
    Door doors[2];
} Room;

typedef struct The_whole_map{
    char** dungeon;
    Player* player;
    person_info* user;
    int room_count;
    int** visibility;
} The_whole_map;

typedef struct userScores{
    char username[MAX_CHAR];
    int score;
} userScores;

The_whole_map* whole_map;
Room* rooms_f1;
Room* rooms_f2;
Room* rooms_f3;
Room* rooms_f4;
Trap* traps1;
Trap* traps2;
Trap* traps3;
Trap* traps4;
Room room_indoor;
int pass1=0,pass11=0,pass2=0,pass21=0,pass3=0,pass31=0,pass4=0,pass41=0;

int maxRow,maxCol;
int room_min_size=4;
int room_max_size=10;
int current_floor=1;
int condition_met=0;
int indoor_check=0;

//unicodes
wchar_t hammer_unicode = 0x2692;
wchar_t mace_unicde = 0x1F5E1;
wchar_t magicWand_unicode = 0x1FA84;
wchar_t normalArrow_unicode = 0x27B3;
wchar_t sword_unicode = 0x2694;
time_t start_time;

int menu();
void register_page();
void login_page();
void allocate_everything(int maxRow,int maxCol);
int check_password_format(char* password);
int check_email_format(char* email);
int check_user_existion(char* username);
int authenticate_user(char* username,char* password);
void game_menu();
Player* set_player();
void play_music();
void settings();
void player_color();
void difficulty();
void ingame_screen();
void generate_dungeon(Room* rooms);
void render_dungeon();
int overlap(Room r1,Room r2);
void create_room(Room room);
void generate_doors(Room* rooms);
void generate_random_corridors(Room* rooms);
void handle_status();
void spawn(Room* rooms);
void draw_player();
int check_collision(int target_x,int target_y);
void move_player(int target_x,int target_y);
int is_room(int x,int y);
void reveal_room(int x,int y);
void reveal_corridor(int x,int y,int l);
int esc_menu();
void generate_random_items(Room* rooms);
void check_item(int x,int y);
void generate_bukies(Room* rooms);
void generate_traps(Room* rooms,Trap* traps);
void generate_obstacles(Room* rooms);
void check_trap_existion(int x,int y);
int check_nearby_doors(Room* rooms,int x,int y);
int checkPassDoorExistion(Room* rooms,int x,int y);
void checkPassDoor(int x,int y);
int return_room_index(int x,int y);
int check_anything_existion(Room* rooms,int x,int y);
void generate_pass_doors(Room* rooms);
void display_pass_door(int x,int y);
void onclickPassButton(int x,int y,int number);
void next_floor();
void generate_stair(Room* rooms);
void load_game();
void save();
void calculate_leaderboard();
void food_page();
void show_map(int input);
void check_hunger(time_t* start_time,time_t now);
void bukies_list();
void weapon_menu();
void generate_curse(Room* rooms);
void display_curse(int x,int y);
void display_indoor_curse(int x,int y);
void check_curse(int x,int y);
void generate_invisible_door(Room* rooms);
void check_inDoor(int x,int y);
void check_indoor_curse(int x,int y);
void exit_indoor(int x,int y);
int check_partial_existion(Room room,int x,int y);
int is_wall(int x,int y);
int end_game(int x,int y);
void generate_treasure_room(Room* room);

int main(){
    whole_map=(The_whole_map*)malloc(sizeof(The_whole_map));
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    start_color();
    srand(time(NULL));
    start_time=0;
    init_pair(BACKGROUND_COLOR_PAIR,COLOR_WHITE,COLOR_BLACK);
    init_pair(GOLD_COLOR_PAIR,COLOR_YELLOW,COLOR_BLACK);
    bkgd(COLOR_PAIR(1));
    getmaxyx(stdscr,maxRow,maxCol);
    //alocation
    allocate_everything(maxRow,maxCol);
    //start of the program
    menu();
    game_menu();
    //game space
    //
    endwin();
    return 0;
}
int mPressed=0;
void ingame_screen(){
    curs_set(FALSE);
    handle_status();
    generate_dungeon(rooms_f1);
    spawn(rooms_f1);
    generate_random_items(rooms_f1);
    generate_bukies(rooms_f1);
    generate_traps(rooms_f1,traps1);
    generate_obstacles(rooms_f1);
    generate_pass_doors(rooms_f1);
    generate_stair(rooms_f1);
    generate_curse(rooms_f1);
    generate_invisible_door(rooms_f1);
    render_dungeon();
    draw_player();
    time_t hunger_start=time(NULL);
    while(true){
        if(end_game(whole_map->player->x,whole_map->player->y)) break;
        time_t now=time(NULL);
        check_hunger(&hunger_start,now);
        char command=getch();
        if(command=='w' || command=='W'){
            move_player(whole_map->player->x,whole_map->player->y-1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='s' || command=='S'){
            move_player(whole_map->player->x,whole_map->player->y+1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='a' || command=='A'){
            move_player(whole_map->player->x-1,whole_map->player->y);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='d' || command=='D'){
            move_player(whole_map->player->x+1,whole_map->player->y);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='1'){
            move_player(whole_map->player->x-1,whole_map->player->y+1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='2'){
            move_player(whole_map->player->x-1,whole_map->player->y-1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='3'){
            move_player(whole_map->player->x+1,whole_map->player->y-1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='4'){
            move_player(whole_map->player->x+1,whole_map->player->y+1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command==27) {
            int choice=esc_menu();
            if(choice==1) {
                break;
            }
            if(choice==0){
                render_dungeon();
                draw_player();
                handle_status();
            }
        }
        else if(command=='e' || command=='E') {
            food_page();
            render_dungeon();
            handle_status();
            draw_player();
        }
        else if(command=='m' || command=='M'){
            if(!mPressed){
                mPressed++;
                show_map(1);
            }
            else {
                mPressed=0;
                show_map(0);
            }
        }
        else if(command=='i' || command=='I'){
            weapon_menu();
            render_dungeon();
            handle_status();
            draw_player();
        }
    }
}

void show_map(int input){
    if(input){
        for(int i=1;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                if(whole_map->dungeon[i][j]=='G'){
                    attron(COLOR_PAIR(GOLD_COLOR_PAIR));
                    mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
                    attroff(COLOR_PAIR(GOLD_COLOR_PAIR));
                }
                else if(whole_map->dungeon[i][j]=='@') display_pass_door(j,i);
                else mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
            }
        }
        draw_player();
        handle_status();
    }
    else{
        for(int i=1;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                mvprintw(i,j," ");
            }
        }
        render_dungeon();
        handle_status();
        draw_player();
    }
}

void continue_screen(){
    clear();
    curs_set(FALSE);
    noecho();
    handle_status();
    time_t hunger_start=time(NULL);
    if(current_floor==1){
        time_t now=time(NULL);
        check_hunger(&hunger_start,now);
        generate_pass_doors(rooms_f1);
        generate_traps(rooms_f1,traps1);
        generate_stair(rooms_f1);
    }
    if(current_floor==2){
        generate_pass_doors(rooms_f2);
        generate_traps(rooms_f2,traps2);
        generate_stair(rooms_f2);
    }
    if(current_floor==3){
        generate_pass_doors(rooms_f3);
        generate_traps(rooms_f3,traps3);
        generate_stair(rooms_f3);
    }
    if(current_floor==4){
        generate_obstacles(rooms_f4);
        generate_pass_doors(rooms_f4);
        generate_traps(rooms_f4,traps4);
    }
    render_dungeon();
    draw_player();
    while(true){
        char command=getch();
        if(command=='w' || command=='W'){
            move_player(whole_map->player->x,whole_map->player->y-1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='s' || command=='S'){
            move_player(whole_map->player->x,whole_map->player->y+1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='a' || command=='A'){
            move_player(whole_map->player->x-1,whole_map->player->y);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='d' || command=='D'){
            move_player(whole_map->player->x+1,whole_map->player->y);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='1'){
            move_player(whole_map->player->x-1,whole_map->player->y+1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='2'){
            move_player(whole_map->player->x-1,whole_map->player->y-1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='3'){
            move_player(whole_map->player->x+1,whole_map->player->y-1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command=='4'){
            move_player(whole_map->player->x+1,whole_map->player->y+1);
            handle_status();
            render_dungeon();
            draw_player();
        }
        else if(command==27) {
            int choice=esc_menu();
            if(choice==1) {
                break;
            }
            if(choice==0){
                render_dungeon();
                draw_player();
                handle_status();
            }
        }
        else if(command=='e' || command=='E') {
            food_page();
            render_dungeon();
            handle_status();
            draw_player();
        }
        else if(command=='m' || command=='M'){
            if(!mPressed){
                mPressed++;
                show_map(1);
            }
            else {
                mPressed=0;
                show_map(0);
            }
        }
        else if(command=='i' || command=='I'){}
    }
}

void generate_dungeon(Room* rooms){
    for(int i=1;i<maxRow-1;i++){
        for(int j=0;j<maxCol;j++){
            whole_map->dungeon[i][j]=' ';
        }
    }
    rooms[0].bottom_col=0,rooms[0].up_col=maxCol/3;
    rooms[0].bottom_row=1,rooms[0].up_row=maxRow/2;
    rooms[0].width=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[0].height=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[0].x=rand()%(rooms[0].up_col-rooms[0].bottom_col-rooms[0].width-3)+rooms[0].bottom_col+2;
    rooms[0].y=rand()%(rooms[0].up_row-rooms[0].bottom_row-rooms[0].height-3)+rooms[0].bottom_row+2;
    create_room(rooms[0]);
    for(int i=rooms[0].y-1;i<=rooms[0].y+rooms[0].height;i++){
        for(int j=rooms[0].x-1;j<=rooms[0].x+rooms[0].width;j++) whole_map->visibility[i][j]=1;
    }

    rooms[1].bottom_col=maxCol/3,rooms[1].up_col=2*(maxCol/3);
    rooms[1].bottom_row=1,rooms[1].up_row=maxRow/2;
    rooms[1].width=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[1].height=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[1].x=rand()%(rooms[1].up_col-rooms[1].bottom_col-rooms[1].width-3)+rooms[1].bottom_col+2;
    rooms[1].y=rand()%(rooms[1].up_row-rooms[1].bottom_row-rooms[1].height-3)+rooms[1].bottom_row+2;
    create_room(rooms[1]);

    rooms[2].bottom_col=(maxCol/3)*2,rooms[2].up_col=maxCol;
    rooms[2].bottom_row=0,rooms[2].up_row=maxRow/2;
    rooms[2].width=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[2].height=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[2].x=rand()%(rooms[2].up_col-rooms[2].bottom_col-rooms[2].width-3)+rooms[2].bottom_col+2;
    rooms[2].y=rand()%(rooms[2].up_row-rooms[2].bottom_row-rooms[2].height-3)+rooms[2].bottom_row+2;
    create_room(rooms[2]);

    rooms[3].bottom_col=2*(maxCol/3),rooms[3].up_col=maxCol;
    rooms[3].bottom_row=maxRow/2,rooms[3].up_row=maxRow;
    rooms[3].width=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[3].height=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[3].x=rand()%(rooms[3].up_col-rooms[3].bottom_col-rooms[3].width-3)+rooms[3].bottom_col+2;
    rooms[3].y=rand()%(rooms[3].up_row-rooms[3].bottom_row-rooms[3].height-3)+rooms[3].bottom_row+2;
    create_room(rooms[3]);

    rooms[4].bottom_col=maxCol/3,rooms[4].up_col=(maxCol/3)*2;
    rooms[4].bottom_row=maxRow/2,rooms[4].up_row=maxRow;
    rooms[4].width=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[4].height=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[4].x=rand()%(rooms[4].up_col-rooms[4].bottom_col-rooms[4].width-3)+rooms[4].bottom_col+2;
    rooms[4].y=rand()%(rooms[4].up_row-rooms[4].bottom_row-rooms[4].height-3)+rooms[4].bottom_row+2;
    create_room(rooms[4]);

    rooms[5].bottom_col=0,rooms[5].up_col=maxCol/3;
    rooms[5].bottom_row=maxRow/2,rooms[5].up_row=maxRow;
    rooms[5].width=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[5].height=rand()%(room_max_size-room_min_size+1)+room_min_size;
    rooms[5].x=rand()%(rooms[5].up_col-rooms[5].bottom_col-rooms[5].width-3)+rooms[5].bottom_col+2;
    rooms[5].y=rand()%(rooms[5].up_row-rooms[5].bottom_row-rooms[5].height-3)+rooms[5].bottom_row+2;
    create_room(rooms[5]);

    generate_doors(rooms);
}

void generate_doors(Room* rooms){
    rooms[0].doors[0].x=rooms[0].x+rooms[0].width;
    rooms[0].doors[0].y=rand()%((rooms[0].height/2)-1)+rooms[0].y+rooms[0].height/2;
    whole_map->dungeon[rooms[0].doors[0].y][rooms[0].doors[0].x]='+';

    rooms[1].doors[0].x=rooms[1].x-1;
    rooms[1].doors[0].y=rand()%((rooms[1].height/2)-1)+rooms[1].y;
    rooms[1].doors[1].x=rooms[1].x+rooms[1].width;
    rooms[1].doors[1].y=rand()%((rooms[1].height/2)-1)+rooms[1].y+rooms[1].height/2;
    whole_map->dungeon[rooms[1].doors[0].y][rooms[1].doors[0].x]='+';
    whole_map->dungeon[rooms[1].doors[1].y][rooms[1].doors[1].x]='+';

    rooms[2].doors[0].x=rooms[2].x-1;
    rooms[2].doors[0].y=rand()%((rooms[2].height/2)-1)+rooms[2].y+rooms[2].height/2;
    rooms[2].doors[1].x=rand()%(rooms[2].width/2 -1)+rooms[2].x+rooms[2].width/2;
    rooms[2].doors[1].y=rooms[2].y+rooms[2].height;
    whole_map->dungeon[rooms[2].doors[0].y][rooms[2].doors[0].x]='+';
    whole_map->dungeon[rooms[2].doors[1].y][rooms[2].doors[1].x]='+';

    rooms[3].doors[0].x=rand()%(rooms[3].width/2 -1)+rooms[3].x;
    rooms[3].doors[0].y=rooms[3].y-1;
    rooms[3].doors[1].x=rooms[3].x-1;
    rooms[3].doors[1].y=rand()%(rooms[3].height/2-1)+rooms[3].y+rooms[3].height/2;
    whole_map->dungeon[rooms[3].doors[0].y][rooms[3].doors[0].x]='+';
    whole_map->dungeon[rooms[3].doors[1].y][rooms[3].doors[1].x]='+';

    rooms[4].doors[1].x=rooms[4].x-1;
    rooms[4].doors[1].y=rand()%((rooms[4].height/2)-1)+rooms[4].y+rooms[4].height/2;
    rooms[4].doors[0].x=rooms[4].x+rooms[4].width;
    rooms[4].doors[0].y=rand()%((rooms[4].height/2)-1)+rooms[4].y;
    whole_map->dungeon[rooms[4].doors[0].y][rooms[4].doors[0].x]='+';
    whole_map->dungeon[rooms[4].doors[1].y][rooms[4].doors[1].x]='+';

    rooms[5].doors[0].x=rooms[5].x+rooms[5].width;
    rooms[5].doors[0].y=rand()%(rooms[5].height/2 -1)+rooms[5].y;
    whole_map->dungeon[rooms[5].doors[0].y][rooms[5].doors[0].x]='+';

    generate_random_corridors(rooms);
}

void generate_random_corridors(Room* rooms){
    //room 0 to 1
    int start_y=rooms[0].doors[0].y;
    int end_y=rooms[1].doors[0].y;
    int start_x=rooms[0].doors[0].x+1;
    int end_x=rooms[1].doors[0].x;
    for(int i=0;i<3;i++) whole_map->dungeon[rooms[0].doors[0].y][rooms[0].doors[0].x+1+i]='#';
    start_x+=3;
    if(start_y > end_y){
        while(start_y+1!=end_y) whole_map->dungeon[start_y--][start_x]='#';
    }
    else{
        while(start_y-1!=end_y) whole_map->dungeon[start_y++][start_x]='#';
    }
    while(start_x!=end_x) whole_map->dungeon[end_y][start_x++]='#';
    //room 1 to 2
    start_y=rooms[1].doors[1].y;
    end_y=rooms[2].doors[0].y;
    start_x=rooms[1].doors[1].x+1;
    end_x=rooms[2].doors[0].x;
    for(int i=0;i<2;i++) whole_map->dungeon[rooms[1].doors[1].y][rooms[1].doors[1].x+1+i]='#';
    start_x+=2;
    if(start_y > end_y){
        while(start_y+1!=end_y) whole_map->dungeon[start_y--][start_x]='#';
    }
    else{
        while(start_y-1!=end_y) whole_map->dungeon[start_y++][start_x]='#';
    }
    while(start_x!=end_x) whole_map->dungeon[end_y][start_x++]='#';
    //room 2 to 3
    start_y=rooms[2].doors[1].y+1;
    end_y=rooms[3].doors[0].y;
    start_x=rooms[2].doors[1].x;
    end_x=rooms[3].doors[0].x;
    for(int i=0;i<2;i++) whole_map->dungeon[start_y+i][start_x]='#';
    start_y+=2;
    if(start_x>end_x){
        while(start_x+1!=end_x) whole_map->dungeon[start_y][start_x--]='#';
    }
    else {
        while(start_x-1!=end_x) whole_map->dungeon[start_y][start_x++]='#';
    }
    while(start_y!=end_y) whole_map->dungeon[start_y++][end_x]='#';
    //room 3 to 4
    start_y=rooms[3].doors[1].y;
    end_y=rooms[4].doors[0].y;
    start_x=rooms[3].doors[1].x-1;
    end_x=rooms[4].doors[0].x;
    for(int i=0;i<2;i++) whole_map->dungeon[start_y][start_x-i]='#';
    start_x-=2;
    if(start_y > end_y){
        while(start_y+1!=end_y) whole_map->dungeon[start_y--][start_x]='#';
    }
    else{
        while(start_y-1!=end_y) whole_map->dungeon[start_y++][start_x]='#';
    }
    while(start_x!=end_x) whole_map->dungeon[end_y][start_x--]='#';
    //room 4 to 5
    start_y=rooms[4].doors[1].y;
    end_y=rooms[5].doors[0].y;
    start_x=rooms[4].doors[1].x-1;
    end_x=rooms[5].doors[0].x;
    for(int i=0;i<2;i++) whole_map->dungeon[start_y][start_x-i]='#';
    start_x-=2;
    if(start_y > end_y){
        while(start_y+1!=end_y) whole_map->dungeon[start_y--][start_x]='#';
    }
    else{
        while(start_y-1!=end_y) whole_map->dungeon[start_y++][start_x]='#';
    }
    while(start_x!=end_x) whole_map->dungeon[end_y][start_x--]='#';
}

void create_room(Room room){
    for(int i=room.y;i<room.y+room.height;i++){
        for(int j=room.x;j<room.x+room.width;j++){ 
            if(i>=0 && i<maxRow && j>=0 && j<maxCol) whole_map->dungeon[i][j]='.';
        }
    }
    for(int i=room.y;i<=room.y+room.height;i++) {
        if(room.x>0) whole_map->dungeon[i][room.x-1]='|';
        if(room.x+room.width<maxCol) whole_map->dungeon[i][room.x+room.width]='|';
    }
    for(int i=room.x-1;i<=room.x+room.width;i++){
        if(room.y>0) whole_map->dungeon[room.y-1][i]='_';
        if(room.y+room.height<maxRow && i!=room.x-1 && i!=room.x+room.width) whole_map->dungeon[room.y+room.height][i]='_';
    }
}

void render_dungeon(){
    mvprintw(0,maxCol-2*strlen(whole_map->user->username),"%s",whole_map->user->username);
    for(int i=1;i<maxRow-1;i++){
        for(int j=0;j<maxCol;j++){
            if(whole_map->visibility[i][j]) {
                if(whole_map->dungeon[i][j]=='G' || whole_map->dungeon[i][j]=='K'){
                    attron(COLOR_PAIR(GOLD_COLOR_PAIR));
                    mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
                    attroff(COLOR_PAIR(GOLD_COLOR_PAIR));
                }
                else if(whole_map->dungeon[i][j]=='C') {
                    if(indoor_check) display_indoor_curse(j,i);
                    else display_curse(j,i);
                }
                else if(whole_map->dungeon[i][j]=='@') display_pass_door(j,i);
                else {
                    if(current_floor==4){
                        int index=return_room_index(j,i);
                        if(is_wall(j,i)){
                            init_pair('y',COLOR_YELLOW,COLOR_BLACK);
                            attron(COLOR_PAIR('y'));
                            mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
                            attroff(COLOR_PAIR('y'));
                        }
                        else if(is_room(j,i) && index==5){
                            init_pair('r',COLOR_RED,COLOR_BLACK);
                            attron(COLOR_PAIR('r'));
                            mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
                            attroff(COLOR_PAIR('r'));
                        }
                        else mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
                    }else mvprintw(i,j,"%c",whole_map->dungeon[i][j]);
                }
            }
        }
    }
}

int end_game(int x,int y){
    if(current_floor==4 && whole_map->dungeon[y][x]=='<'){
        clear();
        init_pair('y',COLOR_YELLOW,COLOR_BLACK);
        attron(COLOR_PAIR('y'));
        int order=(maxCol-strlen("Congrats! You won the game! Press any key to return"))/2;
        mvprintw(maxRow/2,order,"Congrats! You won the game!");
        mvprintw(maxRow/2+1,order,"You score is %d",whole_map->player->gold);
        getch();
        return 1;
    }
    return 0;
}

void spawn(Room* rooms){
    whole_map->player->x=rand()%(rooms[0].width) +rooms[0].x;
    whole_map->player->y=rand()%(rooms[0].height) + rooms[0].y;
}

void handle_status(){
    char* status[]={"Floor:","Health:","Keys:","Golds:","Foods:"};
    mvprintw(maxRow-1,maxCol/6-strlen(status[0]),"%s %d",status[0],current_floor);
    mvprintw(maxRow-1,2*(maxCol/6)-strlen(status[1]),"%s %d",status[1],whole_map->player->hp);
    mvprintw(maxRow-1,3*(maxCol/6)-strlen(status[2]),"%s %d",status[2],whole_map->player->key);
    mvprintw(maxRow-1,4*(maxCol/6)-strlen(status[3]),"%s %d",status[3],whole_map->player->gold);
    mvprintw(maxRow-1,5*(maxCol/6)-strlen(status[4]),"%s %d",status[4],whole_map->player->food);
}

void allocate_everything(int maxRow,int maxCol){
    whole_map->dungeon=(char**)malloc(sizeof(char*)*maxRow);
    for(int i=0;i<maxRow;i++) whole_map->dungeon[i]=(char*)malloc(sizeof(char)*maxCol);
    whole_map->player=(Player*)malloc(sizeof(Player));
    whole_map->visibility=(int**)malloc(sizeof(int*)*maxRow);
    whole_map->user=(person_info*)malloc(sizeof(person_info));
    whole_map->player->color='d';
    whole_map->player->gold=0;
    whole_map->player->hp=16;
    whole_map->player->food=0;
    whole_map->player->key=0;
    whole_map->player->broken_key=0;
    whole_map->player->hunger=2;
    whole_map->player->hp_curse=0,whole_map->player->speed_curse=0,whole_map->player->damage_curse=0;
    rooms_f1=(Room*)malloc(sizeof(Room)*6);
    rooms_f2=(Room*)malloc(sizeof(Room)*6);
    rooms_f3=(Room*)malloc(sizeof(Room)*6);
    rooms_f4=(Room*)malloc(sizeof(Room)*6);
    traps1=(Trap*)malloc(sizeof(Trap)*6);
    traps2=(Trap*)malloc(sizeof(Trap)*6);
    traps3=(Trap*)malloc(sizeof(Trap)*6);
    traps4=(Trap*)malloc(sizeof(Trap)*6);
    for(int i=0;i<6;i++){
        rooms_f1[i].doors[1].pass=0;
        rooms_f1[i].doors[1].password=-1;
        rooms_f1[i].doors[1].pass_existion=0;
        rooms_f1[i].curse.existion=0;
        rooms_f1[i].curse.x=-1;
        rooms_f1[i].curse.y=-1;
        rooms_f2[i].doors[1].pass=0;
        rooms_f2[i].doors[1].password=-1;
        rooms_f2[i].doors[1].pass_existion=0;
        rooms_f2[i].curse.existion=0;
        rooms_f2[i].curse.x=-1;
        rooms_f2[i].curse.y=-1;
        rooms_f3[i].doors[1].pass=0;
        rooms_f3[i].doors[1].password=-1;
        rooms_f3[i].doors[1].pass_existion=0;
        rooms_f3[i].curse.existion=0;
        rooms_f3[i].curse.x=-1;
        rooms_f3[i].curse.y=-1;
        rooms_f4[i].doors[1].pass=0;
        rooms_f4[i].doors[1].password=-1;
        rooms_f4[i].doors[1].pass_existion=0;
        rooms_f4[i].curse.existion=0;
        rooms_f4[i].curse.x=-1;
        rooms_f4[i].curse.y=-1;
    }
    whole_map->player->sword_count=0,whole_map->player->mace_count=1,whole_map->player->dagger_count=0,whole_map->player->hammer_count=0,whole_map->player->magic_wand_count=0,whole_map->player->arrow_count=0;
    //first row is asigned to player stats
    for(int i=0;i<maxRow;i++){
        whole_map->visibility[i]=(int*)malloc(sizeof(int)*maxCol);
        for(int j=0;j<maxCol;j++) whole_map->visibility[i][j]= i==0 ? 1 : 0;
    }
}

int menu(){
    char* buttons[]={"login","register","exit"};
    int num_buttons=3;
    int current_selection=0;
    while(true){
        mvprintw(0,(maxCol-strlen("use w and s to select the option you want"))/2,"use w and s to select the option you want");
        for(int i=0;i<num_buttons;i++){
            if(i==current_selection) attron(A_REVERSE);
            mvprintw((maxRow-num_buttons)/2 + i,(maxCol-strlen(buttons[i]))/2,"%s",buttons[i]);
            if(i==current_selection) attroff(A_REVERSE);
        }
        char command=getch();
        if(command=='w' || command=='W'){
            current_selection=(current_selection-1+num_buttons)%num_buttons;
        }
        else if(command=='s' || command=='S'){
            current_selection=(current_selection+1)%num_buttons;
        }
        else if(command=='\n'){
            if(current_selection==0) { 
                login_page();
                break;
            }
            if(current_selection==1) {
                register_page();
                break;
            }
            if(current_selection==2) return current_selection;
        }
        else {
            mvprintw(0,(maxCol-strlen("invalid command! press any key to continue"))/2,"invalid command! press any key to continue");
            getch();
            clear();
        }
    }
}

void game_menu(){
    curs_set(FALSE);
    noecho();
    clear();
    char* buttons[]={"New game","continue","log out"};
    int num_buttons=3;
    int currect_selection=0;
    while(true){
        for(int i=0;i<num_buttons;i++){
            if(i==currect_selection) attron(A_REVERSE);
            mvprintw((maxRow-num_buttons)/2+i,(maxCol-strlen(buttons[i]))/2,"%s",buttons[i]);
            if(i==currect_selection) attroff(A_REVERSE);
        }
        char command=getch();
        if(command=='w' || command=='W'){
            currect_selection=(currect_selection-1+num_buttons)%num_buttons;
        }
        else if(command=='S' || command=='s'){
            currect_selection=(currect_selection+1)%num_buttons;
        }
        else if(command=='\n'){
            if(currect_selection==0){
                settings();
                break;
            }
            if(currect_selection==1){
                load_game();
                continue_screen();
                break;
            }
            if(currect_selection==2){
                break;
            }
        }
        else {
            mvprintw(0,(maxCol-strlen("invalid command! press any key to continue"))/2,"invalid command! press any key to continue");
            getch();
            clear();
        }
    }
}

void settings(){ 
    curs_set(FALSE);
    clear();
    char* buttons[]={"select difficulty","enable music","disable music","select player color","start game"};
    int num_buttons=5;
    int current_selection=0;
    while(true){
        for(int i=0;i<num_buttons;i++){
            if(i==current_selection) attron(A_REVERSE);
            mvprintw(maxRow/2+i,(maxCol-strlen(buttons[0]))/2,"%s",buttons[i]);
            if(i==current_selection) attroff(A_REVERSE);
        }
        char command=getch();
        if(command=='w' || command=='W'){
            current_selection=(current_selection-1+num_buttons)%num_buttons;
        }
        else if(command=='s' || command=='S'){
            current_selection=(current_selection+1)%num_buttons; 
        }
        else if(command=='\n'){
            if(current_selection==0){
                difficulty();
                clear();
                continue;
            }
            if(current_selection==1){
                play_music("background_music.mp3");
            }
            if(current_selection==2){
                Mix_HaltMusic();
            }
            if(current_selection==3){
                player_color();
                clear();
                continue;
            }
            if(current_selection==4){
                clear();
                ingame_screen();
                break;
            }
        }
        else {
            mvprintw(0,(maxCol-strlen("invalid command! press any key to continue"))/2,"invalid command! press any key to continue");
            getch();
            clear();
        }
    }
}

void difficulty(){
    curs_set(FALSE);
    clear();
    char* buttons[]={"easy","medium","hard"};
    int num_buttons=3;
    int current_selection=0;
    while(true){
        for(int i=0;i<num_buttons;i++){
            if(i==current_selection) attron(A_REVERSE);
            mvprintw((maxRow-num_buttons)/2+i,(maxCol-strlen(buttons[i]))/2,"%s",buttons[i]);
            if(i==current_selection) attroff(A_REVERSE);
        }
        char command=getch();
        if(command=='w' || command=='W'){
            current_selection=(current_selection-1+num_buttons)%num_buttons;
        }
        else if(command=='s' || command=='S'){
            current_selection=(current_selection+1)%num_buttons;
        }
        else if(command=='\n'){
            if(current_selection==0){
                break;
            }
            if(current_selection==1){
                break;
            }
            if(current_selection==2){
                break;
            }
        }
    }
}

void player_color(){
    clear();
    char* buttons[]={"red","green","blue","yellow"};
    int num_buttons=4;
    int current_selection=0;
    while(true){
        for(int i=0;i<num_buttons;i++){
            if(i==current_selection) attron(A_REVERSE);
            mvprintw((maxRow-num_buttons)/2+i,(maxCol-strlen(buttons[0]))/2,"%s",buttons[i]);
            if(i==current_selection) attroff(A_REVERSE);
        }
        char command=getch();
        if(command=='w' || command=='W'){
            current_selection=(current_selection-1+num_buttons)%num_buttons;
        }
        else if(command=='s' || command=='S'){
            current_selection=(current_selection+1)%num_buttons;
        }
        else if(command=='\n'){
            if(current_selection==0){
                whole_map->player->color='r';
                init_pair(PLAYER_COLOR_PAIR,COLOR_RED,COLOR_BLACK);
                break;
            }
            if(current_selection==1){
                whole_map->player->color='g';
                init_pair(PLAYER_COLOR_PAIR,COLOR_GREEN,COLOR_BLACK);
                break;
            }
            if(current_selection==2){
                whole_map->player->color='b';
                init_pair(PLAYER_COLOR_PAIR,COLOR_BLUE,COLOR_BLACK);
                break;
            }
            if(current_selection==3){
                whole_map->player->color='y';
                init_pair(PLAYER_COLOR_PAIR,COLOR_YELLOW,COLOR_BLACK);
                break;
            }
        }
        else {
            mvprintw(0,(maxCol-strlen("invalid command! press any key to continue"))/2,"invalid command! press any key to continue");
            getch();
            clear();
        }
    }
}

void register_page(){
    clear();
    char* buttons[]={"username: ","password: ","confirm password: ","email: "};
    int num_buttons=4;
    char temp_username[MAX_CHAR];
    char temp_password[MAX_CHAR];
    char temp_confirm[MAX_CHAR];
    char temp_email[MAX_CHAR];
    while(true){
        clear();
        echo();
        curs_set(TRUE);
        mvprintw((maxRow-num_buttons)/2,(maxCol-strlen(buttons[0]))/2,"%s",buttons[0]);
        mvprintw((maxRow-num_buttons)/2+1,(maxCol-strlen(buttons[0]))/2,"%s",buttons[1]);
        mvprintw((maxRow-num_buttons)/2+2,(maxCol-strlen(buttons[0]))/2,"%s",buttons[2]);
        mvprintw((maxRow-num_buttons)/2+3,(maxCol-strlen(buttons[0]))/2,"%s",buttons[3]);
        move((maxRow-num_buttons)/2,(maxCol-strlen(buttons[0]))/2 + strlen(buttons[0]));
        getstr(temp_username);
        if(check_user_existion(temp_username)){
            mvprintw(0,(maxCol-strlen("user already exists! press any key to continue"))/2,"user already exists! press any key to continue");
            getch();
            continue;
        }
        move((maxRow-num_buttons)/2+1,(maxCol-strlen(buttons[0]))/2 + strlen(buttons[1]));
        noecho();
        getstr(temp_password);
        if(!check_password_format(temp_password)){
            mvprintw(0,(maxCol-strlen("password format is wrong! Press any key to continue"))/2,"password format is wrong! Press any key to continue");
            getch();
            continue;
        }
        move((maxRow-num_buttons)/2+2,(maxCol-strlen(buttons[0]))/2 + strlen(buttons[2]));
        getstr(temp_confirm);
        if(strcmp(temp_password,temp_confirm)){
            mvprintw(0,(maxCol-strlen("Passwords do not match! press any key to continue"))/2,"Passwords do not match! press any key to continue");
            getch();
            continue;
        }
        move((maxRow-num_buttons)/2+3,(maxCol-strlen(buttons[0]))/2 + strlen(buttons[3]));
        echo();
        getstr(temp_email);
        if(!check_email_format(temp_email)){
            mvprintw(0,(maxCol-strlen("email format is wrong! press any key to continue"))/2,"email format is wrong! press any key to continue");
            getch();
            continue;
        }
        break;
    }
    curs_set(FALSE);
    strcpy(whole_map->user->username,temp_username);
    strcpy(whole_map->user->password,temp_password);
    strcpy(whole_map->user->email,temp_email);
    strcat(temp_username,".txt");
    FILE* file=fopen(temp_username,"w");
    fprintf(file,"%s %s %s",whole_map->user->username,whole_map->user->password,whole_map->user->email);
    fclose(file);
}

void login_page(){
    char* buttons[]={"username: ","password: "};
    int num_buttons=2;
    int current_selection=0;
    char temp_username[MAX_CHAR];
    char temp_password[MAX_CHAR];
    while(true){
        clear();
        echo();
        mvprintw((maxRow-2)/2,(maxCol-strlen(buttons[0]))/2,"%s",buttons[0]);
        mvprintw((maxRow-2)/2+1,(maxCol-strlen(buttons[0]))/2,"%s",buttons[1]);
        move((maxRow-2)/2,(maxCol-strlen(buttons[0]))/2+strlen(buttons[0]));
        getstr(temp_username);
        noecho();
        move((maxRow-2)/2+1,(maxCol-strlen(buttons[0]))/2+strlen(buttons[1]));
        curs_set(TRUE);
        getstr(temp_password);
        if(!check_user_existion(temp_username)){
            mvprintw(0,(maxCol-strlen("user not found! press any key to continue"))/2,"user not found! press any key to continue");
            getch();
            continue;
        }
        if(!authenticate_user(temp_username,temp_password)){
            mvprintw(0,(maxCol-strlen("password is wrong! press any key to continue"))/2,"password is wrong! press any key to continue");
            getch();
            continue;
        }
        break;
    }
    clear();
    strcpy(whole_map->user->username,temp_username);
    strcpy(whole_map->user->password,temp_password);
}

int authenticate_user(char* username,char* password){
    char filename[MAX_CHAR];
    strcpy(filename,username);
    strcat(filename,".txt");
    FILE* userinfo=fopen(filename,"r");
    if(userinfo==NULL) return 0;
    char fileUsername[MAX_CHAR],filePassword[MAX_CHAR],fileEmail[MAX_CHAR];
    fscanf(userinfo,"%s %s %s",fileUsername,filePassword,fileEmail);
    if(!strcmp(fileUsername,username) && !strcmp(filePassword,password)){
        fclose(userinfo);
        return 1;
    }
    fclose(userinfo);
    return 0;
}

int check_user_existion(char* username){
    char filename[MAX_CHAR];
    strcpy(filename,username);
    strcat(filename,".txt");
    FILE* userinfo=fopen(filename,"r");
    if(userinfo==NULL) return 0;
    fclose(userinfo);
    return 1;
}

int check_password_format(char* password){
    int size=strlen(password);
    if(size<7) return 0;
    int check_upper=0;
    int check_lower=0;
    int check_number=0;
    for(int i=0;i<size;i++){
        if(password[i]>='A' && password[i]<='Z') check_upper=1;
        if(password[i]>='a' && password[i]<='z') check_lower=1;
        if(password[i]>='1' && password[i]<='9') check_number=1;
    }
    if(check_lower && check_upper && check_number) return 1;
    return 0;
}

int check_email_format(char* email){
    int size=strlen(email)-1;
    //char @ char . char
    int checkat=1,checkdot=1;
    int at_index,dot_index;
    int at_counter=0;
    for(int i=0;i<size;i++){
        if(email[i]=='@'){
            if(i==0) return 0;
            checkat=0;
            at_index=i;
            at_counter++;
        }
        if(email[i]=='.'){
            checkdot=0;
            dot_index=i;
        }
    }
    int dot_counter=0;
    if(checkat && checkdot) return 0;
    if(at_counter>1) return 0;
    if(at_index>dot_index) return 0;
    for(int i=at_index;i<size;i++){
        if(email[i]=='.') dot_counter++;
    }
    if(dot_counter>1) return 0;
    if(dot_index==size-1) return 0;
    if(dot_index==at_index+1 || at_index==0) return 0;
    return 1;
}

Mix_Music* bgm = NULL;
void play_music(char* file) {
    //initialize
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        SDL_Quit();
        exit(1);
    }
    if (bgm != NULL) {
        Mix_FreeMusic(bgm);
        bgm = NULL;
    }
    bgm = Mix_LoadMUS(file);
    if (!bgm) {
        printf("Failed to load music: %s\n", Mix_GetError());
        return;
    }
    if (Mix_PlayMusic(bgm, -1) == -1) {
        printf("Failed to play music: %s\n", Mix_GetError());
        Mix_FreeMusic(bgm);
        bgm = NULL;
    }
}

void draw_player(){
    if(whole_map->player->color!='d'){
        attron(COLOR_PAIR(PLAYER_COLOR_PAIR));
        mvprintw(whole_map->player->y,whole_map->player->x,"P");
        attroff(COLOR_PAIR(PLAYER_COLOR_PAIR));
    }
    else {
        mvprintw(whole_map->player->y,whole_map->player->x,"P");
    }
}
int number=0;
void move_player(int target_x,int target_y){
    if(whole_map->dungeon[target_y][target_x]=='@') checkPassDoor(target_x,target_y);
    if(check_collision(target_x,target_y)){
        whole_map->visibility[whole_map->player->y][whole_map->player->x]=1;
        whole_map->visibility[target_y][target_x]=1;
        whole_map->player->y=target_y;
        whole_map->player->x=target_x;
        move(target_y,target_x);
        if(!condition_met && whole_map->dungeon[target_y][target_x]=='&'){
            condition_met=1;
            start_time=time(NULL);
            int d1=rand()%10,d2=rand()%10,d3=rand()%10,d4=rand()%10;
            int digits[]={d1,d2,d3,d4};
            while(d1!=0) d1=rand()%10; while(d2!=0) d2=rand()%10; while(d3!=0) d3=rand()%10; while(d4!=0) d4=rand()%10;
            int power=1;
            for(int i=0;i<4;i++){
                number+=digits[i]*power;
                power*=10;
            }
            int index=return_room_index(target_x,target_y);
            if(current_floor==1) rooms_f1[index].doors[1].password=number;
            if(current_floor==2) rooms_f2[index].doors[1].password=number;
            if(current_floor==3) rooms_f3[index].doors[1].password=number;
            if(current_floor==4) rooms_f4[index].doors[1].password=number;
        }
        onclickPassButton(target_x,target_y,number);
        check_item(target_x,target_y);
        check_trap_existion(target_x,target_y);
        if(indoor_check) check_indoor_curse(target_x,target_y);
        else check_curse(target_x,target_y);
        check_inDoor(target_x,target_y);
        exit_indoor(target_x,target_y);
        if(is_room(target_x,target_y)) reveal_room(target_x,target_y);
        if(whole_map->dungeon[target_y][target_x]=='<') next_floor();
        reveal_corridor(target_x,target_y,0);
    }
}
char tempRoom[200][200];
int tempVis[200][200];
int xpp,ypp;
void exit_indoor(int x,int y){
    if(whole_map->dungeon[y][x]=='e'){
        indoor_check=0;
        clear();
        whole_map->player->x=xpp;
        whole_map->player->y=ypp;
        for(int i=1;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                whole_map->dungeon[i][j]=tempRoom[i][j];
            }
        }
        for(int i=0;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                whole_map->visibility[i][j]=tempVis[i][j];
            }
        }
        if(current_floor==1){
            rooms_f1[4].inDoor.x=-1;
            rooms_f1[4].inDoor.y=-1;
        }
    }
}

void check_indoor_curse(int x,int y){
    if(whole_map->dungeon[y][x]=='C'){
        whole_map->dungeon[y][x]='.';
        for(int i=0;i<6;i++){
            if(room_indoor.indoor_curses[i].kind=='D' && room_indoor.indoor_curses[i].x==x && room_indoor.indoor_curses[i].y==y){
                whole_map->player->damage_curse++;
                clear();
                mvprintw(0,0,"You earned a damage curse!");
                render_dungeon();
                draw_player();
                handle_status();
            }
            if(room_indoor.indoor_curses[i].kind=='S' && room_indoor.indoor_curses[i].x==x && room_indoor.indoor_curses[i].y==y){
                whole_map->player->speed_curse++;
                clear();
                mvprintw(0,0,"You earned a speed curse!");
                render_dungeon();
                draw_player();
                handle_status();
            }
            if(room_indoor.indoor_curses[i].kind=='H' && room_indoor.indoor_curses[i].x==x && room_indoor.indoor_curses[i].y==y){
                whole_map->player->hp_curse++;
                clear();
                mvprintw(0,0,"You earned a health curse!");
                render_dungeon();
                draw_player();
                handle_status();
            }
        }
    }
}

void check_curse(int x,int y){
    if(whole_map->dungeon[y][x]=='C'){
        whole_map->dungeon[y][x]='.';
        int index=return_room_index(x,y);
        Room* rooms=current_floor==1?rooms_f1:current_floor==2?rooms_f2:current_floor==3?rooms_f3:rooms_f4;
        rooms[index].curse.x=-1;
        rooms[index].curse.y=-1;
        if(rooms[index].curse.kind=='D') {
            whole_map->player->damage_curse++;
            clear();
            mvprintw(0,0,"You earned a damage curse!");
            render_dungeon();
            draw_player();
            handle_status();
        }
        if(rooms[index].curse.kind=='S'){ 
            whole_map->player->speed_curse++;
            clear();
            mvprintw(0,0,"You earned a speed curse!");
            render_dungeon();
            draw_player();
            handle_status();
        }
        if(rooms[index].curse.kind=='H') {
            whole_map->player->hp_curse++;
            clear();
            mvprintw(0,0,"You earned a health curse!");
            render_dungeon();
            draw_player();
            handle_status();
        }
    }
}

void onclickPassButton(int x,int y,int number){
    if(condition_met){
        clear();
        time_t elapsed=time(NULL)-start_time;
        int remaining=30-elapsed;
        if(remaining>=0){
            mvprintw(0,0,"Password is %d and the remaining time until disappearing is %d",number,remaining);
        }else {
            clear();
            condition_met=0;
        }
    }
}

void checkPassDoor(int x,int y){
    Room* rooms= current_floor==1 ? rooms_f1 : current_floor==2 ? rooms_f2 : current_floor==3 ? rooms_f3 : rooms_f4;
    int index=return_room_index(x,y);
    int pass= current_floor==1?(index==1 ? pass1 : pass11):current_floor==2?(index==1 ? pass2 : pass21):current_floor==3?(index==1 ? pass3 : pass31):(index==1 ? pass4 : pass41);
    if(rooms[index].doors[1].pass==1 || pass) {
        start_time-=30;
        return;
    }
    if(!condition_met && whole_map->player->key>0){
        int a=rand()%10;
        int b=rand()%10;
        whole_map->player->key--;
        if(a==b){
            whole_map->player->broken_key++;
            if(whole_map->player->broken_key==2){
                whole_map->player->broken_key=0;
                whole_map->player->key++;
            }
        }
        rooms[index].doors[1].pass=1;
        if(current_floor==1){
            if(index==1) pass1=1;
            else pass11=1;
        }
        if(current_floor==2){
            if(index==1) pass2=1;
            else pass21=1;
        }
        if(current_floor==3){
            if(index==1) pass3=1;
            else pass31=1;
        }
        if(current_floor==4){
            if(index==1) pass4=1;
            else pass41=1;
        }
        return;
    }
    clear();
    echo();
    int entered_pass;
    init_pair('y',COLOR_YELLOW,COLOR_BLACK);
    init_pair('r',COLOR_RED,COLOR_BLACK);
    for(int i=0;i<3;i++){
        clear();
        mvprintw(maxRow/2,(maxCol-strlen("Enter the passkey: ")-4)/2,"Enter the passkey: ");
        mvscanw(maxRow/2,(maxCol+strlen("Enter the passkey: ")-4)/2,"%d",&entered_pass);
        if(entered_pass==rooms[index].doors[1].password){
            rooms[index].doors[1].pass=1;
            if(current_floor==1) {if(index==1) pass1=1; else pass11=1;} if(current_floor==2) {if(index==1) pass2=1; else pass21=1;} if(current_floor==3) {if(index==1) pass3=1; else pass31=1;} if(current_floor==4) {if(index==1) pass4=1; else pass41=1;}
            start_time-=30;
            break;
        }
        else{
            if(i==0){
                attron(COLOR_PAIR('y'));
                mvprintw(0,(maxCol-strlen("Password is wrong! Press any key to continue"))/2,"Password is wrong! Press any key to continue");
                attroff(COLOR_PAIR('y'));
                getch();
            }
            if(i==1){
                attron(COLOR_PAIR('r'));
                mvprintw(0,(maxCol-strlen("Password is wrong! Press any key to continue"))/2,"Password is wrong! Press any key to continue");
                attroff(COLOR_PAIR('r'));
                getch();
            }
        }
    }
    noecho();
    clear();
}

int check_collision(int target_x,int target_y){
    int index=return_room_index(target_x,target_y);
    int pass= current_floor==1?(index==1 ? pass1 : pass11):current_floor==2?(index==1 ? pass2 : pass21):current_floor==3?(index==1 ? pass3 : pass31):(index==1 ? pass4 : pass41);
    return !(whole_map->dungeon[target_y][target_x]=='_' || whole_map->dungeon[target_y][target_x]=='|' || 
    whole_map->dungeon[target_y][target_x]=='O' || whole_map->dungeon[target_y][target_x]==' ' ||
    (whole_map->dungeon[target_y][target_x]=='@' && pass==0));
}

void reveal_corridor(int x,int y,int l){
    if(x<1 || y<1 || x>maxCol-1 || y>maxRow-1) return;
    if(l==6) return;
    int dx[]={1,0,-1,0};
    int dy[]={0,1,0,-1};
    if(whole_map->dungeon[y][x]=='#'){
        whole_map->visibility[y][x]=1;
        for(int i=0;i<4;i++){
            int nx=x+dx[i];
            int ny=y+dy[i];
            if(whole_map->dungeon[ny][nx]=='#'){
                reveal_corridor(nx,ny,l+1);
            }
        }
    }
}

void reveal_room(int x,int y){
    int room_index=return_room_index(x,y);
    if(current_floor==1){
        int xR=rooms_f1[room_index].x;
        int yR=rooms_f1[room_index].y;
        for(int i=xR-1;i<=xR+rooms_f1[room_index].width;i++){
            for(int j=yR-1;j<=yR+rooms_f1[room_index].height;j++) whole_map->visibility[j][i]=1;
        }
    }
    if(current_floor==2){
        int xR=rooms_f2[room_index].x;
        int yR=rooms_f2[room_index].y;
        for(int i=xR-1;i<=xR+rooms_f2[room_index].width;i++){
            for(int j=yR-1;j<=yR+rooms_f2[room_index].height;j++) whole_map->visibility[j][i]=1;
        }
    }
    if(current_floor==3){
        int xR=rooms_f3[room_index].x;
        int yR=rooms_f3[room_index].y;
        for(int i=xR-1;i<=xR+rooms_f3[room_index].width;i++){
            for(int j=yR-1;j<=yR+rooms_f3[room_index].height;j++) whole_map->visibility[j][i]=1;
        }
    }
    if(current_floor==4){
        int xR=rooms_f4[room_index].x;
        int yR=rooms_f4[room_index].y;
        for(int i=xR-1;i<=xR+rooms_f4[room_index].width;i++){
            for(int j=yR-1;j<=yR+rooms_f4[room_index].height;j++) whole_map->visibility[j][i]=1;
        }
    }
}

int is_wall(int x,int y){
    return (whole_map->dungeon[y][x]=='+' || whole_map->dungeon[y][x]=='|' || whole_map->dungeon[y][x]=='_');
}

int is_room(int x,int y){
    return (whole_map->dungeon[y][x]=='+' || whole_map->dungeon[y][x]=='@' || whole_map->dungeon[y][x]=='|' || 
    whole_map->dungeon[y][x]=='_' || whole_map->dungeon[y][x]=='.');
}

void load_game(){
    for(int i=0;i<maxRow;i++){
        for(int j=0;j<maxCol;j++){
            whole_map->dungeon[i][j]=' ';
        }
    }
    char filename[MAX_CHAR];
    strcpy(filename,whole_map->user->username);
    strcat(filename,".txt");
    char userinfo[300];
    FILE* file=fopen(filename,"r");
    fgets(userinfo,300,file);
    fscanf(file,"%d %d %d %d %d",&current_floor,&whole_map->player->hp,&whole_map->player->key,&whole_map->player->gold,&whole_map->player->food);
    fscanf(file,"%d %d %c",&whole_map->player->x,&whole_map->player->y,&whole_map->player->color);

    if(whole_map->player->color=='r') init_pair(PLAYER_COLOR_PAIR,COLOR_RED,COLOR_BLACK);
    if(whole_map->player->color=='g') init_pair(PLAYER_COLOR_PAIR,COLOR_GREEN,COLOR_BLACK);
    if(whole_map->player->color=='b') init_pair(PLAYER_COLOR_PAIR,COLOR_BLUE,COLOR_BLACK);
    if(whole_map->player->color=='y') init_pair(PLAYER_COLOR_PAIR,COLOR_YELLOW,COLOR_BLACK);

    Room* rooms=current_floor==1?rooms_f1:current_floor==2?rooms_f2:current_floor==3?rooms_f3:rooms_f4;
    fscanf(file,"%d %d %d %d %d %d",&rooms_f1[0].x,&rooms_f1[0].y,&rooms_f1[0].width,&rooms_f1[0].height,&rooms_f1[0].doors[0].x,&rooms_f1[0].doors[0].y);
    fscanf(file,"%d %d %d %d %d %d %d %d",&rooms_f1[1].x,&rooms_f1[1].y,&rooms_f1[1].width,&rooms_f1[1].height,&rooms_f1[1].doors[0].x,&rooms_f1[1].doors[0].y,&rooms_f1[1].doors[1].x,&rooms_f1[1].doors[1].y);
    fscanf(file,"%d",&pass1); rooms_f1[1].doors[1].pass=pass1;
    fscanf(file,"%d %d %d %d %d %d %d %d",&rooms_f1[2].x,&rooms_f1[2].y,&rooms_f1[2].width,&rooms_f1[2].height,&rooms_f1[2].doors[0].x,&rooms_f1[2].doors[0].y,&rooms_f1[2].doors[1].x,&rooms_f1[2].doors[1].y);
    fscanf(file,"%d %d %d %d %d %d %d %d",&rooms_f1[3].x,&rooms_f1[3].y,&rooms_f1[3].width,&rooms_f1[3].height,&rooms_f1[3].doors[0].x,&rooms_f1[3].doors[0].y,&rooms_f1[3].doors[1].x,&rooms_f1[3].doors[1].y);
    fscanf(file,"%d %d %d %d %d %d %d %d",&rooms_f1[4].x,&rooms_f1[4].y,&rooms_f1[4].width,&rooms_f1[4].height,&rooms_f1[4].doors[0].x,&rooms_f1[4].doors[0].y,&rooms_f1[4].doors[1].x,&rooms_f1[4].doors[1].y);
    fscanf(file,"%d",&rooms_f1[4].doors[1].pass);
    fscanf(file,"%d %d %d %d %d %d",&rooms_f1[5].x,&rooms_f1[5].y,&rooms_f1[5].width,&rooms_f1[5].height,&rooms_f1[5].doors[0].x,&rooms_f1[5].doors[0].y);
    fscanf(file,"%d %d",&rooms_f1[4].inDoor.x,&rooms_f1[4].inDoor.y);
    fscanf(file,"%d",&whole_map->player->gold);
    for(int i=0;i<6;i++){
        rooms[i].x=rooms_f1[i].x;
        rooms[i].y=rooms_f1[i].y;
        rooms[i].width=rooms_f1[i].width;
        rooms[i].height=rooms_f1[i].height;
        if(i==0 || i==5) {
            rooms[i].doors[0].x=rooms_f1[i].doors[0].x;
            rooms[i].doors[0].y=rooms_f1[i].doors[0].y;
        }
        else{
            rooms[i].doors[0].x=rooms_f1[i].doors[0].x;
            rooms[i].doors[0].y=rooms_f1[i].doors[0].y;
            rooms[i].doors[1].x=rooms_f1[i].doors[1].x;
            rooms[i].doors[1].y=rooms_f1[i].doors[1].y;
        }
    }
    rooms[1].doors[1].pass=rooms_f1[1].doors[1].pass;
    rooms[4].doors[1].pass=rooms_f1[4].doors[1].pass;
    for(int i=0;i<6;i++) create_room(rooms[i]);
    whole_map->dungeon[rooms[0].doors[0].y][rooms[0].doors[0].x]='+';
        
    whole_map->dungeon[rooms[1].doors[0].y][rooms[1].doors[0].x]='+';
    whole_map->dungeon[rooms[1].doors[1].y][rooms[1].doors[1].x]='+';

    whole_map->dungeon[rooms[2].doors[0].y][rooms[2].doors[0].x]='+';
    whole_map->dungeon[rooms[2].doors[1].y][rooms[2].doors[1].x]='+';

    whole_map->dungeon[rooms[3].doors[0].y][rooms[3].doors[0].x]='+';
    whole_map->dungeon[rooms[3].doors[1].y][rooms[3].doors[1].x]='+';

    whole_map->dungeon[rooms[4].doors[0].y][rooms[4].doors[0].x]='+';
    whole_map->dungeon[rooms[4].doors[1].y][rooms[4].doors[1].x]='+';

    whole_map->dungeon[rooms[5].doors[0].y][rooms[5].doors[0].x]='+';
    generate_random_corridors(rooms);
    for(int i=0;i<6;i++){
        int x,y;
        char kind;
        fscanf(file,"%d %d %c",&x,&y,&kind);
        if((~x && ~y) && (x && y)){
            rooms[i].curse.x=x;
            rooms[i].curse.y=y;
            rooms[i].curse.kind=kind;
            whole_map->dungeon[y][x]='C';
        }
    }
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        rooms[i].obstacle.x=x;
        rooms[i].obstacle.y=y;
        whole_map->dungeon[y][x]='O';
    }
    fscanf(file,"%d %d",&whole_map->player->food,&whole_map->player->hunger);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if(~x && ~y){
            whole_map->dungeon[y][x]='F';
        }
        rooms[i].food.x=x;
        rooms[i].food.y=y;
    }
    fscanf(file,"%d %d",&whole_map->player->key,&whole_map->player->broken_key);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if((~x && ~y) && (x && y)){
            whole_map->dungeon[y][x]='K';
        }
        rooms[i].key.x=x;
        rooms[i].key.y=y;
    }
    fscanf(file,"%d",&whole_map->player->sword_count);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if((~x && ~y) && (x && y)){
            whole_map->dungeon[y][x]='!';
        }
        rooms[i].sword.x=x;
        rooms[i].sword.y=y;
    }
    fscanf(file,"%d",&whole_map->player->dagger_count);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if((~x && ~y) && (x && y)){
            whole_map->dungeon[y][x]='~';
        }
        rooms[i].dagger.x=x;
        rooms[i].dagger.y=y;
    }
    fscanf(file,"%d",&whole_map->player->arrow_count);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if((~x && ~y) && (x && y)){
            whole_map->dungeon[y][x]='>';
        }
        rooms[i].arrow.x=x;
        rooms[i].arrow.y=y;
    }
    fscanf(file,"%d",&whole_map->player->magic_wand_count);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if((~x && ~y) && (x && y)){
            whole_map->dungeon[y][x]='$';
        }
        rooms[i].magic_wand.x=x;
        rooms[i].magic_wand.y=y;
    }
    fscanf(file,"%d",&whole_map->player->mace_count);
    for(int i=0;i<6;i++){
        int x,y;
        fscanf(file,"%d %d",&x,&y);
        if((~x && ~y) && (x && y)){
            whole_map->dungeon[y][x]='*';
        }
        rooms[i].mace.x=x;
        rooms[i].mace.y=y;
    }

    for(int i=0;i<maxRow;i++){
        for(int j=0;j<maxCol;j++){
            fscanf(file,"%d",&whole_map->visibility[i][j]);
        }
    }
    fclose(file);
}

void save(){
    char filename[MAX_CHAR];
    strcpy(filename,whole_map->user->username);
    strcat(filename,".txt");
    FILE* file=fopen(filename,"w");
    fprintf(file,"%s %s %s\n",whole_map->user->username,whole_map->user->password,whole_map->user->email);
    fprintf(file,"%d %d %d %d %d\n",current_floor,whole_map->player->hp,whole_map->player->key,whole_map->player->gold,whole_map->player->food);
    //player color pair
    fprintf(file,"%d %d %c\n",whole_map->player->x,whole_map->player->y,whole_map->player->color);
    Room* rooms=current_floor==1?rooms_f1:current_floor==2?rooms_f2:current_floor==3?rooms_f3:rooms_f4;
    fprintf(file,"%d %d %d %d %d %d\n",rooms[0].x,rooms[0].y,rooms[0].width,rooms[0].height,rooms[0].doors[0].x,rooms[0].doors[0].y);
    fprintf(file,"%d %d %d %d %d %d %d %d\n",rooms[1].x,rooms[1].y,rooms[1].width,rooms[1].height,rooms[1].doors[0].x,rooms[1].doors[0].y,rooms[1].doors[1].x,rooms[1].doors[1].y);
    fprintf(file,"%d\n",rooms[1].doors[1].pass);
    fprintf(file,"%d %d %d %d %d %d %d %d\n",rooms[2].x,rooms[2].y,rooms[2].width,rooms[2].height,rooms[2].doors[0].x,rooms[2].doors[0].y,rooms[2].doors[1].x,rooms[2].doors[1].y);
    fprintf(file,"%d %d %d %d %d %d %d %d\n",rooms[3].x,rooms[3].y,rooms[3].width,rooms[3].height,rooms[3].doors[0].x,rooms[3].doors[0].y,rooms[3].doors[1].x,rooms[3].doors[1].y);
    fprintf(file,"%d %d %d %d %d %d %d %d\n",rooms[4].x,rooms[4].y,rooms[4].width,rooms[4].height,rooms[4].doors[0].x,rooms[4].doors[0].y,rooms[4].doors[1].x,rooms[4].doors[1].y);
    fprintf(file,"%d\n",rooms[4].doors[1].pass);
    fprintf(file,"%d %d %d %d %d %d\n",rooms[5].x,rooms[5].y,rooms[5].width,rooms[5].height,rooms[5].doors[0].x,rooms[5].doors[0].y);
    fprintf(file,"%d %d\n",rooms[4].inDoor.x,rooms[4].inDoor.y);
    fprintf(file,"%d\n",whole_map->player->gold);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d %c\n",rooms[i].curse.x,rooms[i].curse.y,rooms[i].curse.kind);
    }
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].obstacle.x,rooms[i].obstacle.y);
    }
    fprintf(file,"%d %d\n",whole_map->player->food,whole_map->player->hunger);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].food.x,rooms[i].food.y);
    }
    fprintf(file,"%d %d\n",whole_map->player->key,whole_map->player->broken_key);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].key.x,rooms[i].key.y);
    }
    fprintf(file,"%d\n",whole_map->player->sword_count);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].sword.x,rooms[i].sword.y);
    }
    fprintf(file,"%d\n",whole_map->player->dagger_count);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].dagger.x,rooms[i].dagger.y);
    }
    fprintf(file,"%d\n",whole_map->player->arrow_count);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].arrow.x,rooms[i].arrow.y);
    }
    fprintf(file,"%d\n",whole_map->player->magic_wand_count);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].magic_wand.x,rooms[i].magic_wand.y);
    }
    fprintf(file,"%d\n",whole_map->player->mace_count);
    for(int i=0;i<6;i++){
        fprintf(file,"%d %d\n",rooms[i].mace.x,rooms[i].mace.y);
    }
    for(int i=0;i<maxRow;i++){
        for(int j=0;j<maxCol;j++){
            fprintf(file,"%d",whole_map->visibility[i][j]);
            if(j!=maxCol-1) fprintf(file," ");
        }
        fprintf(file,"\n");
    }
    fclose(file);
    //save the updated datas to leaderboard
    FILE* score_file=fopen("leaderboard.txt","r");
    char users[100][50];
    int scores[100];
    int index=0;
    while(~fscanf(score_file,"%s",users[index])){
        fscanf(file,"%d",&scores[index++]);
    }
    fclose(score_file);
    FILE* put_score=fopen("leaderboard.txt","w");
    for(int i=0;i<index;i++){
        if(!strcmp(users[i],whole_map->user->username)) fprintf(file,"%s %d\n",users[i],whole_map->player->gold);
        else fprintf(file,"%s %d\n",users[i],scores[i]);
    }
    fclose(put_score);
}

int esc_menu(){
    char* buttons[]={"save","back to game","leader board","resources","quit"};
    int num_buttons=5;
    int current_selection=0;
    while(true){
        clear();
        for(int i=0;i<num_buttons;i++){
            if(i==current_selection) attron(A_REVERSE);
            mvprintw(maxRow/2+i,(maxCol-strlen(buttons[i]))/2,"%s",buttons[i]);
            if(i==current_selection) attroff(A_REVERSE);
        }
        char command=getch();
        if(command=='w'){
            current_selection=(current_selection-1+num_buttons)%num_buttons;
        }
        else if(command=='s'){
            current_selection=(current_selection+1)%num_buttons;
        }
        else if(command=='\n'){
            if(current_selection==0) {
                save();
            }
            if(current_selection==1){
                clear();
                return 0;
            }
            if(current_selection==2){
                calculate_leaderboard();
            }
            if(current_selection==4){
                endwin();
                return 1;
            }
        }
        else {
            mvprintw(0,(maxCol-strlen("invalid command! press any key to continue"))/2,"invalid command! press any key to continue");
            getch();
            clear();
        }
    }
}

void check_item(int x,int y){
    char item=whole_map->dungeon[y][x];
    int index=return_room_index(x,y);
    Room* rooms=current_floor==1?rooms_f1:current_floor==2?rooms_f2:current_floor==3?rooms_f3:rooms_f4;
    if(item=='F'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].food.x=-1;
        rooms[index].food.y=-1;
        if(whole_map->player->hunger<=11){
            if(whole_map->player->hunger==11) whole_map->player->hunger++;
            else whole_map->player->hunger +=2;
            if(whole_map->player->hp<20) whole_map->player->hp++;
        }
        if(whole_map->player->food<5){
            whole_map->player->food++;
            mvprintw(0,0,"You ate a food!");
        }else mvprintw(0,0,"You are full so you can't eat foods anymore!");
    }
    if(item=='G'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].gold.x=-1;
        rooms[index].gold.y=-1;
        int rand_gold=rand()%4+1;
        whole_map->player->gold+=rand_gold;
        if(rand_gold==1) mvprintw(0,0,"You earned 1 gold!");
        else mvprintw(0,0,"Congrants! You earned %d golds!",rand_gold);
    }
    if(item=='~'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].dagger.x=-1;
        rooms[index].dagger.y=-1;
        whole_map->player->dagger_count++;
        mvprintw(0,0,"You earned a dagger!");
    }
    if(item=='$'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].magic_wand.x=-1;
        rooms[index].magic_wand.y=-1;
        whole_map->player->magic_wand_count++;
        mvprintw(0,0,"You earned a magic wand!");
    }
    if(item=='!'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].sword.x=-1;
        rooms[index].sword.y=-1;
        whole_map->player->sword_count++;
        mvprintw(0,0,"You earned a sword!");
    }
    if(item=='*'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].mace.x=-1;
        rooms[index].mace.y=-1;
        whole_map->player->mace_count++;
        mvprintw(0,0,"You earned a mace!");
    }
    if(item=='>'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].arrow.x=-1;
        rooms[index].arrow.y=-1;
        whole_map->player->arrow_count++;
        mvprintw(0,0,"You earned an arrow!");
    }
    if(item=='H'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].hammer.x=-1;
        rooms[index].hammer.y=-1;
        whole_map->player->hammer_count++;
        mvprintw(0,0,"You earned a hammer!");
    }
    if(item=='K'){
        clear();
        whole_map->dungeon[y][x]='.';
        rooms[index].key.x=-1;
        rooms[index].key.y=-1;
        whole_map->player->key++;
        mvprintw(0,0,"You earned an ancient key!");
    }
}

void generate_random_items(Room* rooms){
    for(int i=0;i<6;i++){
        rooms[i].key.existion=0;
        rooms[i].gold.existion=0;
        rooms[i].food.existion=0;
        rooms[i].stair.existion=0;
    }
    for(int i=0;i<6;i++){
        while(true){
            int xf1=rand()%(rooms[i].width) + rooms[i].x;
            int yf1=rand()%(rooms[i].height) + rooms[i].y;
            if(check_anything_existion(rooms,xf1,yf1)){
                whole_map->dungeon[yf1][xf1]='F';
                rooms[i].food.x=xf1;
                rooms[i].food.y=yf1;
                rooms[i].food.existion=1;
                break;
            }
        }
    }
    while(true){
        int x=rand()%(rooms[1].width)+rooms[1].x;
        int y=rand()%rooms[1].height+rooms[1].y;
        if(check_anything_existion(rooms,x,y)){
            whole_map->dungeon[y][x]='G';
            rooms[1].gold.x=x;
            rooms[1].gold.y=y;
            rooms[1].gold.existion=1;
            break;
        }
    }
    while(true){
        int x=rand()%(rooms[5].width)+rooms[5].x;
        int y=rand()%rooms[5].height+rooms[5].y;
        if(check_anything_existion(rooms,x,y)){
            whole_map->dungeon[y][x]='G';
            rooms[5].gold.x=x;
            rooms[5].gold.y=y;
            rooms[5].gold.existion=1;
            break;
        }
    }
    while(true){
        int x=rand()%(rooms[1].width)+rooms[1].x;
        int y=rand()%rooms[1].height+rooms[1].y;
        if(check_anything_existion(rooms,x,y)){
            whole_map->dungeon[y][x]='K';
            rooms[1].key.x=x;
            rooms[1].key.y=y;
            rooms[1].key.existion=1;
            break;
        }
    }
}

void generate_curse(Room* rooms){
    while(true){
        int x=rand()%(rooms[0].width)+rooms[0].x;
        int y=rand()%rooms[0].height+rooms[0].y;
        if(check_anything_existion(rooms,x,y)){
            whole_map->dungeon[y][x]='C';
            rooms[0].curse.x=x;
            rooms[0].curse.y=y;
            rooms[0].curse.existion=1;
            rooms[0].curse.kind='H';
            break;
        }
    }
    while(true){
        int x=rand()%(rooms[2].width)+rooms[2].x;
        int y=rand()%rooms[2].height+rooms[2].y;
        if(check_anything_existion(rooms,x,y)){
            whole_map->dungeon[y][x]='C';
            rooms[2].curse.x=x;
            rooms[2].curse.y=y;
            rooms[2].curse.existion=1;
            rooms[2].curse.kind='D';
            break;
        }
    }
    while(true){
        int x=rand()%(rooms[5].width)+rooms[5].x;
        int y=rand()%rooms[5].height+rooms[5].y;
        if(check_anything_existion(rooms,x,y)){
            whole_map->dungeon[y][x]='C';
            rooms[5].curse.x=x;
            rooms[5].curse.y=y;
            rooms[5].curse.existion=1;
            rooms[5].curse.kind='S';
            break;
        }
    }
}

int return_room_index(int x,int y){
    int room_index;
    if(y<maxRow/2){
        if(x<maxCol/3) room_index=0;
        if(x>maxCol/3 && x<(maxCol/3)*2) room_index=1;
        if(x>(maxCol/3)*2) room_index=2;
    }
    else{
        if(x<maxCol/3) room_index=5;
        if(x>maxCol/3 && x<(maxCol/3)*2) room_index=4;
        if(x>(maxCol/3)*2) room_index=3;
    }
    return room_index;
}

int check_anything_existion(Room* rooms,int x,int y){
    int index=return_room_index(x,y);
    return !((x==rooms[index].arrow.x && y==rooms[index].arrow.y) || 
    (x==rooms[index].sword.x && y==rooms[index].sword.y) ||
    (x==rooms[index].mace.x && y==rooms[index].mace.y) || 
    (x==rooms[index].dagger.x && y==rooms[index].dagger.y) ||
    (x==rooms[index].magic_wand.x && y==rooms[index].magic_wand.y) ||
    (x==rooms[index].hammer.x && y==rooms[index].hammer.y) ||
    (x==rooms[index].food.x && y==rooms[index].food.y) ||
    (x==rooms[index].gold.x && y==rooms[index].gold.y) ||
    (x==rooms[index].key.x && y==rooms[index].key.y) ||
    (x==rooms[index].obstacle.x && y==rooms[index].obstacle.y) ||
    (x==rooms[index].doors[1].buttonx && y==rooms[index].doors[1].buttony) ||
    (x==rooms[index].stair.x && y==rooms[index].stair.y) ||
    (x==whole_map->player->x && y==whole_map->player->y));
}

void generate_bukies(Room* rooms){
    while(true){
        int x=rand()%(rooms[0].width)+rooms[0].x;
        int y=rand()%(rooms[0].height)+rooms[0].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[0].sword.x=x;
            rooms[0].sword.y=y;
            rooms[0].sword.existion=1;
            whole_map->dungeon[y][x]='!';
            break;
        }
    }
    while(true){
        int x=rand()%(rooms[0].width)+rooms[0].x;
        int y=rand()%(rooms[0].height)+rooms[0].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[0].hammer.x=x;
            rooms[0].hammer.y=y;
            rooms[0].hammer.existion=1;
            whole_map->dungeon[y][x]='H';
            break;
        }
    }
    while(true){
        int x=rand()%rooms[1].width + rooms[1].x;
        int y=rand()%rooms[1].height + rooms[1].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[1].magic_wand.x=x;
            rooms[1].magic_wand.y=y;
            rooms[1].magic_wand.existion=1;
            whole_map->dungeon[y][x]='$';
            break;
        }
    }
    while(true){
        int x=rand()%rooms[2].width + rooms[2].x;
        int y=rand()%rooms[2].height + rooms[2].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[2].mace.x=x;
            rooms[2].mace.y=y;
            rooms[2].mace.existion=1;
            whole_map->dungeon[y][x]='*';
            break;
        }
    }
    while(true){
        int x=rand()%rooms[3].width + rooms[3].x;
        int y=rand()%rooms[3].height + rooms[3].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[3].dagger.x=x;
            rooms[3].dagger.y=y;
            rooms[3].dagger.existion=1;
            whole_map->dungeon[y][x]='~';
            break;
        }
    }
    while(true){
        int x=rand()%rooms[4].width + rooms[4].x;
        int y=rand()%rooms[4].height + rooms[4].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[4].hammer.x=x;
            rooms[4].hammer.y=y;
            rooms[4].hammer.existion=1;
            whole_map->dungeon[y][x]='H';
            break;
        }
    }
    while(true){
        int x=rand()%rooms[4].width + rooms[4].x;
        int y=rand()%rooms[4].height + rooms[4].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[4].arrow.x=x;
            rooms[4].arrow.y=y;
            rooms[4].arrow.existion=1;
            whole_map->dungeon[y][x]='>';
            break;
        }
    }
}

int check_nearby_doors(Room* rooms,int x,int y){
    int dx[]={1,0,-1,0};
    int dy[]={0,1,0,-1};
    int index=return_room_index(x,y);
    for(int i=0;i<4;i++){
        if(x+dx[i]==rooms[index].doors[0].x && y+dy[i]==rooms[index].doors[0].y) return 0;
        if(index!=0 && index!=5){
            if(x+dx[i]==rooms[index].doors[1].x && y+dy[i]==rooms[index].doors[1].y) return 0;
        }
    }
    return 1;
}

void generate_obstacles(Room* rooms){
    for(int i=0;i<6;i++){
        while(true){
            int x=rand()%(rooms[i].width) + rooms[i].x;
            int y=rand()%(rooms[i].height) + rooms[i].y;
            if(check_anything_existion(rooms,x,y) && check_nearby_doors(rooms,x,y)){
                rooms[i].obstacle.x=x;
                rooms[i].obstacle.y=y;
                rooms[i].obstacle.existion=1;
                whole_map->dungeon[y][x]='O';
                break;
            }
        }
    }
}

void generate_traps(Room* rooms,Trap* traps){
    for(int i=0;i<6;i++){
        traps[i].x=rand()%(rooms[i].width)+rooms[i].x;
        traps[i].y=rand()%(rooms[i].height)+rooms[i].y;
    }
}

void check_trap_existion(int x,int y){
    for(int i=0;i<6;i++){
        if(current_floor==1){
            if(whole_map->player->x==traps1[i].x && whole_map->player->y==traps1[i].y){
                int dmg=rand()%2 + 1;
                if(dmg==1) {
                    clear();
                    whole_map->player->hp --;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                if(dmg==2){
                    clear();
                    whole_map->player->hp -= 2;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                traps1[i].x=-1;
                traps1[i].y=-1;
            }
        }
        if(current_floor==2){
            if(whole_map->player->x==traps2[i].x && whole_map->player->y==traps2[i].y){
                int dmg=rand()%2 + 1;
                if(dmg==1) {
                    clear();
                    whole_map->player->hp --;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                if(dmg==2){
                    clear();
                    whole_map->player->hp -= 2;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                traps2[i].x=-1;
                traps2[i].y=-1;
            }
        }
        if(current_floor==3){
            if(whole_map->player->x==traps3[i].x && whole_map->player->y==traps3[i].y){
                int dmg=rand()%2 + 1;
                if(dmg==1) {
                    clear();
                    whole_map->player->hp --;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                if(dmg==2){
                    clear();
                    whole_map->player->hp -= 2;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                traps3[i].x=-1;
                traps3[i].y=-1;
            }
        }
        if(current_floor==4){
            if(whole_map->player->x==traps4[i].x && whole_map->player->y==traps4[i].y){
                int dmg=rand()%2 + 1;
                if(dmg==1) {
                    clear();
                    whole_map->player->hp --;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                if(dmg==2){
                    clear();
                    whole_map->player->hp -= 2;
                    mvprintw(0,0,"You got a damage from a trap");
                }
                traps4[i].x=-1;
                traps4[i].y=-1;
            }
        }
    }
}

void generate_pass_doors(Room* rooms){
    int index=1;
    whole_map->dungeon[rooms[index].doors[1].y][rooms[index].doors[1].x]='@';
    rooms[index].doors[1].pass_existion=1;
    while(true){
        int x=rand()%2 ? rooms[index].x : rooms[index].x + rooms[index].width -1;
        int y=rand()%2 ? rooms[index].y : rooms[index].y + rooms[index].height -1;
        if(check_anything_existion(rooms,x,y)){
            rooms[index].doors[1].buttonx=x;
            rooms[index].doors[1].buttony=y;
            whole_map->dungeon[y][x]='&';
            break;
        }
    }
    index=4;
    whole_map->dungeon[rooms[index].doors[1].y][rooms[index].doors[1].x]='@';
    rooms[index].doors[1].pass_existion=1;
    while(true){
        int x=rand()%2 ? rooms[index].x : rooms[index].x + rooms[index].width -1;
        int y=rand()%2 ? rooms[index].y : rooms[index].y + rooms[index].height -1;
        if(check_anything_existion(rooms,x,y)){
            rooms[index].doors[1].buttonx=x;
            rooms[index].doors[1].buttony=y;
            whole_map->dungeon[y][x]='&';
            break;
        }
    }
}

void display_pass_door(int x,int y){
    int index=return_room_index(x,y);
    Room* rooms=current_floor==1 ? rooms_f1 : current_floor==2 ? rooms_f2 : current_floor==3 ? rooms_f3 : rooms_f4;
    init_pair('r',COLOR_RED,COLOR_BLACK);
    init_pair('g',COLOR_GREEN,COLOR_BLACK);
    if(rooms[index].doors[1].pass==1){
        attron(COLOR_PAIR('g'));
        mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
        attroff(COLOR_PAIR('g'));
    }else{
        attron(COLOR_PAIR('r'));
        mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
        attroff(COLOR_PAIR('r'));
    }
}

void display_indoor_curse(int x,int y){
    init_pair('r',COLOR_RED,COLOR_BLACK);
    init_pair('g',COLOR_GREEN,COLOR_BLACK);
    init_pair('b',COLOR_BLUE,COLOR_BLACK);
    for(int i=0;i<6;i++){
        if(room_indoor.indoor_curses[i].x==x && room_indoor.indoor_curses[i].y==y){
            if(room_indoor.indoor_curses[i].kind=='D'){
                attron(COLOR_PAIR('r'));
                mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
                attroff(COLOR_PAIR('r'));
            }
            if(room_indoor.indoor_curses[i].kind=='H'){
                attron(COLOR_PAIR('g'));
                mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
                attroff(COLOR_PAIR('g'));
            }
            if(room_indoor.indoor_curses[i].kind=='S'){
                attron(COLOR_PAIR('b'));
                mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
                attroff(COLOR_PAIR('b'));
            }
        }
    }
}

void display_curse(int x,int y){
    init_pair('r',COLOR_RED,COLOR_BLACK);
    init_pair('g',COLOR_GREEN,COLOR_BLACK);
    init_pair('b',COLOR_BLUE,COLOR_BLACK);
    int index=return_room_index(x,y);
    Room* rooms=current_floor==1?rooms_f1:current_floor==2?rooms_f2:current_floor==3?rooms_f3:rooms_f4;
    if(rooms[index].curse.kind=='D'){
        attron(COLOR_PAIR('r'));
        mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
        attroff(COLOR_PAIR('r'));
    }
    if(rooms[index].curse.kind=='H'){
        attron(COLOR_PAIR('g'));
        mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
        attroff(COLOR_PAIR('g'));
    }
    if(rooms[index].curse.kind=='S'){
        attron(COLOR_PAIR('b'));
        mvprintw(y,x,"%c",whole_map->dungeon[y][x]);
        attroff(COLOR_PAIR('b'));
    }
}

void generate_stair(Room* rooms){
    while(true){
        int x=rand()%(rooms[5].width)+rooms[5].x;
        int y=rand()%(rooms[5].height)+rooms[5].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[5].stair.x=x;
            rooms[5].stair.y=y;
            whole_map->dungeon[y][x]='<';
            break;
        }
    }
}

void generate_treasure_room(Room* room){
    for(int i=0;i<10;i++){
        while(true){
            int x=rand()%room[5].width+room[5].x;
            int y=rand()%room[5].height+room[5].y;
            if(check_partial_existion(room[5],x,y)){
                whole_map->dungeon[y][x]='G';
                room[5].golds[i].existion=1;
                room[5].golds[i].x=x;
                room[5].golds[i].y=y;
                break;
            }
        }
    }
}

void next_floor(){
    if(current_floor==1){
        current_floor++;
        clear();
        for(int i=0;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                whole_map->visibility[i][j]=0;
                whole_map->dungeon[i][j]=' ';
            }
        }
        generate_dungeon(rooms_f2);
        spawn(rooms_f2);
        generate_traps(rooms_f2,traps2);
        generate_random_items(rooms_f2);
        generate_obstacles(rooms_f2);
        generate_pass_doors(rooms_f2);
        generate_stair(rooms_f2);
        generate_curse(rooms_f2);
        generate_bukies(rooms_f2);
    }
    else if(current_floor==2){
        current_floor++;
        clear();
        for(int i=0;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                whole_map->visibility[i][j]=0;
                whole_map->dungeon[i][j]=' ';
            }
        }
        generate_dungeon(rooms_f3);
        spawn(rooms_f3);
        generate_traps(rooms_f3,traps3);
        generate_random_items(rooms_f3);
        generate_obstacles(rooms_f3);
        generate_pass_doors(rooms_f3);
        generate_stair(rooms_f3);
        generate_curse(rooms_f3);
        generate_bukies(rooms_f3);
    }
    else if(current_floor==3){
        current_floor++;
        clear();
        for(int i=0;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                whole_map->visibility[i][j]=0;
                whole_map->dungeon[i][j]=' ';
            }
        }
        generate_dungeon(rooms_f4);
        spawn(rooms_f4);
        generate_traps(rooms_f4,traps4);
        generate_random_items(rooms_f4);
        generate_obstacles(rooms_f4);
        generate_pass_doors(rooms_f4);
        generate_stair(rooms_f4);
        generate_curse(rooms_f4);
        generate_bukies(rooms_f4);
        generate_treasure_room(rooms_f4);
    }
}

userScores users[100];
void calculate_leaderboard(){
    FILE* file=fopen("leaderboard.txt","r");
    int index=0;
    while(~fscanf(file,"%s",users[index].username)){
        fscanf(file,"%d",&users[index++].score);
    }
    fclose(file);
    for(int i=0;i<index;i++){
        for(int j=0;j<index-1;j++){
            if(users[j].score<users[j+1].score){
                userScores temp;
                temp=users[j];
                users[j]=users[j+1];
                users[j+1]=temp;
            }
        }
    }
    clear();
    init_pair('g',COLOR_YELLOW,COLOR_BLACK);
    init_pair('b',COLOR_BLUE,COLOR_BLACK);
    init_pair('p',COLOR_GREEN,COLOR_BLACK);
    for(int i=0;i<index;i++){
        if(i==0){
            attron(COLOR_PAIR('g'));
            mvprintw(i,(maxCol-strlen(users[0].username)-4)/2,"%d.%s %d",i+1,users[i].username,users[i].score);
            attroff(COLOR_PAIR('g'));
            continue;
        }
        if(i==1){
            attron(COLOR_PAIR('b'));
            mvprintw(i,(maxCol-strlen(users[0].username)-4)/2,"%d.%s %d",i+1,users[i].username,users[i].score);
            attroff(COLOR_PAIR('b'));
            continue;
        }
        if(i==2){
            attron(COLOR_PAIR('p'));
            mvprintw(i,(maxCol-strlen(users[0].username)-4)/2,"%d.%s %d",i+1,users[i].username,users[i].score);
            attroff(COLOR_PAIR('p'));
            continue;
        }
        mvprintw(i,(maxCol-strlen(users[0].username)-4)/2,"%d.%s %d",i+1,users[i].username,users[i].score);
    }
    noecho();
    while(true){
        char command=getch();
        if(command==27) break;
    }
    clear();
}

void food_page(){
    clear();
    int x_order=(maxCol-strlen("You have %d foods"))/2;
    mvprintw(0,(maxCol-strlen("You have %d foods"))/2,"You have %d foods",whole_map->player->food);
    mvprintw(1,x_order,"hunger: ");
    mvprintw(maxRow/2,x_order,"Press any key to continue!");
    x_order+=strlen("hunger: ");
    mvprintw(1,x_order++,"[");
    int hunger=whole_map->player->hunger;
    for(int i=0;i<hunger;i++) mvprintw(1,x_order++,"#");
    for(int i=0;i<(12-hunger);i++) mvprintw(1,x_order++,".");
    mvprintw(1,x_order++,"]");
    getch();
    clear();
}

void check_hunger(time_t* start_time,time_t now){
    int elapsed_time=now-*start_time;
    if(elapsed_time>=10){
        if(whole_map->player->hunger==0){
            whole_map->player->hp -=2;
        }
        else if(whole_map->player->hunger>=0){
            if(whole_map->player->food>0) whole_map->player->food--;
            whole_map->player->hunger--;
        }
        (*start_time) += 10;
    }
}

void weapon_menu(){
    clear();
    noecho();
    int size=strlen("list of weapons");
    mvprintw(0,(maxCol-size)/2,"list of weapons");
    mvprintw(1,(maxCol-size)/2,"%d swords",whole_map->player->sword_count);
    mvprintw(2,(maxCol-size)/2,"%d daggers",whole_map->player->dagger_count);
    mvprintw(3,(maxCol-size)/2,"%d Mace",whole_map->player->mace_count);
    mvprintw(4,(maxCol-size)/2,"%d magic wands",whole_map->player->magic_wand_count);
    mvprintw(5,(maxCol-size)/2,"%d hammer",whole_map->player->hammer_count);
    mvprintw(6,(maxCol-size)/2,"%d arrows",whole_map->player->arrow_count);
    mvprintw(maxRow/2,(maxCol-size)/2,"Press i to return");
    while(true){
        char command=getch();
        if(command=='i' || command=='I') break;
    }
    clear();
}

void generate_invisible_door(Room* rooms){
    int index=4;
    while(true){
        int x=rand()%rooms[index].width+rooms[index].x;
        int y=rand()%rooms[index].height+rooms[index].y;
        if(check_anything_existion(rooms,x,y)){
            rooms[index].inDoor.x=x;
            rooms[index].inDoor.y=y;
            break;
        }
    }
}

int check_partial_existion(Room room,int x,int y){
    return !((x==room.arrow.x && y==room.arrow.y) || 
    (x==room.sword.x && y==room.sword.y) ||
    (x==room.mace.x && y==room.mace.y) || 
    (x==room.dagger.x && y==room.dagger.y) ||
    (x==room.magic_wand.x && y==room.magic_wand.y) ||
    (x==room.hammer.x && y==room.hammer.y) ||
    (x==room.food.x && y==room.food.y) ||
    (x==room.gold.x && y==room.gold.y) ||
    (x==room.key.x && y==room.key.y) ||
    (x==room.obstacle.x && y==room.obstacle.y) ||
    (x==room.doors[1].buttonx && y==room.doors[1].buttony) ||
    (x==room.stair.x && y==room.stair.y) ||
    (x==whole_map->player->x && y==whole_map->player->y));
}

void check_inDoor(int x,int y){
    int index=4;
    Room* rooms=current_floor==1?rooms_f1:current_floor==2?rooms_f2:current_floor==3?rooms_f3:rooms_f4;
    if(rooms[index].inDoor.x==x && rooms[index].inDoor.y==y){
        clear();
        indoor_check=1;
        for(int i=1;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                tempRoom[i][j]=whole_map->dungeon[i][j];
                whole_map->dungeon[i][j]=' ';
            }
        }
        for(int i=0;i<maxRow;i++){
            for(int j=0;j<maxCol;j++){
                tempVis[i][j]=whole_map->visibility[i][j];
                whole_map->visibility[i][j]= 1;
            }
        }
        int X=(maxCol-20)/2;
        int Y=(maxRow-20)/2;
        int width=20;
        int height=20;
        room_indoor.x=X,room_indoor.y=Y,room_indoor.width=width,room_indoor.height=height;
        create_room(room_indoor);
        int exit_x=rand()%(room_indoor.width)+room_indoor.x;
        int exit_y=rand()%(room_indoor.height)+room_indoor.y;
        whole_map->dungeon[exit_y][exit_x]='e';
        room_indoor.stair.x=exit_x,room_indoor.stair.y=exit_y;
        xpp=whole_map->player->x,ypp=whole_map->player->y;
        whole_map->player->x=rand()%room_indoor.width+room_indoor.x;
        whole_map->player->y=rand()%room_indoor.height+room_indoor.y;
        for(int i=0;i<6;i++){
            while(true){
                int x=rand()%room_indoor.width+room_indoor.x;
                int y=rand()%room_indoor.height+room_indoor.y;
                if(check_partial_existion(room_indoor,x,y)){
                    whole_map->dungeon[y][x]='C';
                    room_indoor.indoor_curses[i].x=x;
                    room_indoor.indoor_curses[i].y=y;
                    int choice=rand()%3;
                    if(choice==0){
                        room_indoor.indoor_curses[i].kind='H';
                    }
                    if(choice==1){
                        room_indoor.indoor_curses[i].kind='S';
                    }
                    if(choice==2){
                        room_indoor.indoor_curses[i].kind='D';
                    }
                    break;
                }
            }
        }
    }
}