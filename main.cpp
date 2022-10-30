#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include "SDL_draw.h"
#include "SDL_ttf.h"
#undef main

#include "TEXTURES.h"
#include "LOGICS.h"

Uint32 ttFunk(Uint32, void*); //функция для таймера
void convert(char*, int);     //конвертация int числа в строку

void add_score(int*, int);  //добавление рекодра
void read_file(int*, char); //чтение файла с рекордами
void save_file(int*, char); //сохранение файла с рекодрами

short widht_menu = 300, height_menu = 600; //размеры окна для меню
short widht_game = 0, height_game = 0;     //размеры окна игры

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) //инициализация SDL и SDL_ttf
        return 0;
    SDLCALL TTF_Init();

    SDL_Surface *screen = SDL_SetVideoMode(widht_menu, height_menu, 32, SDL_ANYFORMAT); //создание главной поверхности
    SDL_WM_SetCaption("PacMan           Ruslan I596", NULL); //смена названия окна
    SDL_Surface *menu_full = SDL_LoadBMP("menu_full.bmp"); //загрузка bmp со всеми менюшками (потом будет скрин)
    SDL_Event event;
    SDL_Rect dest, dest_2;
    dest_2.h = 25;
    dest_2.w = 25;

    SDL_TimerID tid;
    tid = SDL_AddTimer(10, ttFunk, 0); //установка таймера

    bool KEYS[322] = {0};              //массив хранящий информации о нажатых клавишах

    SDL_Surface *text_rect;      //плоскость для текста
    SDL_Color text_color;        //цвет текста
    TTF_Font *font, *font_small; //2 шрифта

    font = TTF_OpenFont("Pixel.otf", 55); //открытие шрифтов и установка цвета
    font_small = TTF_OpenFont("Pixel.otf", 30);
    text_color.r = 255; text_color.g = 255; text_color.b = 255;

    char *way_level[] ={"level/level_2.bin", "level/level_1.bin"}; //путь к уровням

    LOGICS *logics = NULL;
    TEXTURES *textures = NULL;

    char number_menu = 0; //перeменные для работы меню
    char last_number_menu = 0;
    bool add_number_menu = 0;
    char regim = 0, last_regim = regim;
    bool key_flag = 1, pause;
    int i, j;
    char score_str[10] = {0};
    int score[11] = {0};
    char level = 0;
    char new_dir;

    read_file(score, level);

    while(SDL_WaitEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                SDL_QUIT;
                return 0;
            case SDL_KEYDOWN: //если клавиша нажата
                KEYS[event.key.keysym.sym] = 1; break;
            case SDL_KEYUP:   //если клавиша отпущена
                KEYS[event.key.keysym.sym] = 0; break;
            case SDL_USEREVENT:
                switch(event.user.code) {
                    case 1:
                        if(last_regim != regim) { //запуск игры
                            if(regim == 1) {
                                if(logics) delete logics;
                                if(textures) delete textures;
                                logics = new LOGICS();
                                logics->gen(way_level[level]);
                                textures = new TEXTURES(logics->maze);
                                new_dir = logics->pacman.get_dir();
                                pause = 0;
                            }

                            switch(regim) { //изменение размера главной поверхности в зависимости от того для чего она нужна (меню или игра)
                                case 0:
                                    SDL_FreeSurface(screen);
                                    screen = SDL_SetVideoMode(widht_menu, height_menu, 32, SDL_ANYFORMAT);
                                    break;
                                case 1:
                                    SDL_FreeSurface(screen);
                                    screen = SDL_SetVideoMode(widht_game = logics->maze.get_size().y * scale, height_game = logics->maze.get_size().x * scale, 32, SDL_ANYFORMAT);
                                    SDL_BlitSurface(textures->get_pointer_background(), NULL, screen, NULL);
                                    break;
                            }

                            last_regim = regim;
                        }

                        //если не нажаты кнопки нужные для меня то флаг = 1, нужна для того чтобы не "проскакивать" через меню
                        if(!key_flag && !KEYS[13] && !KEYS[SDLK_w] && !KEYS[SDLK_s] && !KEYS[SDLK_p]) key_flag = 1;

                        switch(regim) {
                            case 0: //отрисовка меню
                                switch(add_number_menu) {
                                    case 0: //отрисовка главного меню
                                        dest.h = 600; dest.w = 300; dest.x = 300; dest.y = 0; //накладываение части поверхности с меню на главную
                                        SDL_BlitSurface(menu_full, &dest, screen, NULL);

                                        Draw_Round(screen, 48, 58 + number_menu * 102, 204, 74, 20, 16711680); //отрисовка рамки вокруг активной кнопки

                                        if(key_flag && KEYS[SDLK_w] && number_menu > 0) { key_flag = 0; number_menu--; } //переключение активной кнопки
                                        else if(key_flag && KEYS[SDLK_s] && number_menu < 4) { key_flag = 0; number_menu++; }

                                        //выбор пункта меню или завершение работы программы
                                        if(key_flag && number_menu == 0 && KEYS[13]) { key_flag = 0; regim = 1; }
                                        else if(key_flag && number_menu == 1 && KEYS[13] && !add_number_menu) {
                                                key_flag = 0; add_number_menu = 1; last_number_menu = number_menu; number_menu = level; }
                                        else if(key_flag && number_menu == 2 && KEYS[13] && !add_number_menu) {
                                            key_flag = 0; add_number_menu = 1; last_number_menu = number_menu; number_menu = 1; }
                                        else if(key_flag && number_menu == 3 && KEYS[13] && !add_number_menu) {
                                            key_flag = 0; add_number_menu = 1; last_number_menu = number_menu; number_menu = 0; }
                                        else if(key_flag && number_menu == 4 && KEYS[13]) {
                                            key_flag = 0; save_file(score, level); SDL_FreeSurface(screen); SDL_FreeSurface(menu_full); SDL_QUIT; return 0; }

                                        break;
                                    case 1: //отрисовка дополнительных меню
                                        switch(last_number_menu) {
                                            case 1: //отрисовка меню выбора уровня
                                                dest.h = 600; dest.w = 300; dest.x = 600; dest.y = 0; //накладываение части поверхности с меню на главную
                                                SDL_BlitSurface(menu_full, &dest, screen, NULL);

                                                switch(number_menu) { //отрисовка рамки активной кнопки
                                                    case 0: Draw_Round(screen, 48, 58, 204, 74, 20, 16711680); break;
                                                    case 1: Draw_Round(screen, 48, 160, 204, 74, 20, 16711680); break;
                                                    case 2: Draw_Round(screen, 48, 466, 204, 74, 20, 16711680); break;
                                                }

                                                if(key_flag && KEYS[SDLK_w] && number_menu > 0) { key_flag = 0; number_menu--; } //переключение активной кнопки
                                                else if(key_flag && KEYS[SDLK_s] && number_menu < 2) { key_flag = 0; number_menu++; }

                                                if(key_flag && KEYS[13] && number_menu < 2) { //смена уровня и таблици очков
                                                    key_flag = 0;
                                                    save_file(score, level);
                                                    level = number_menu;
                                                    number_menu = last_number_menu;
                                                    add_number_menu = 0;
                                                    read_file(score, level);
                                                }
                                                //выход из меню без смены уровня
                                                else if(key_flag && (KEYS[SDLK_ESCAPE] || KEYS[13] && number_menu == 2)) {
                                                        key_flag = 0; number_menu = last_number_menu; add_number_menu = 0; }

                                                break;
                                            case 2: //отрисовка меню score
                                                dest.h = 600; dest.w = 300; dest.x = 900; dest.y = 0; //накладываение части поверхности с меню на главную
                                                SDL_BlitSurface(menu_full, &dest, screen, NULL);

                                                for(i = 0; i < 10; i++) { //отрисовка количесетва очков
                                                    convert(score_str, score[i]);
                                                    text_rect = TTF_RenderText_Solid(font_small, score_str, text_color);

                                                    dest.y = 22 + i * 46;

                                                    switch(strlen(score_str)) {
                                                        case 1: dest.x = i%2 ? 48 + 45 : 148 + 45; break;
                                                        case 3: dest.x = i%2 ? 48 + 26 : 148 + 26; break;
                                                        case 4: dest.x = i%2 ? 48 + 18 : 148 + 18; break;
                                                        case 5: dest.x = i%2 ? 48 + 8 : 148 + 8; break;
                                                    }

                                                    SDL_BlitSurface(text_rect, NULL, screen, &dest);
                                                    SDL_FreeSurface(text_rect);
                                                }

                                                switch(number_menu) { //отрисовка рамки активной кнопки
                                                    case 0: Draw_Round(screen, 128, 483, 144, 42, 20, 16711680); break;
                                                    case 1: Draw_Round(screen, 28, 529, 144, 42, 20, 16711680); break;
                                                }

                                                if(key_flag && KEYS[SDLK_w] && number_menu > 0) { key_flag = 0; number_menu--; } //переключение активной кнопки
                                                else if(key_flag && KEYS[SDLK_s] && number_menu < 1) { key_flag = 0; number_menu++; }

                                                //выход из меню
                                                if(key_flag && (KEYS[SDLK_ESCAPE] || KEYS[13] && number_menu == 1)) {
                                                        key_flag = 0; number_menu = last_number_menu; add_number_menu = 0; }
                                                else if(key_flag && KEYS[13] && !number_menu) { key_flag = 0; for(i=0; i<11; i++) score[i] = 0; } //отчистка таблицы

                                                break;
                                            case 3: //отрисовка help
                                                dest.h = 600; dest.w = 300; dest.x = 1200; dest.y = 0; //накладываение части поверхности с меню на главную
                                                SDL_BlitSurface(menu_full, &dest, screen, NULL);

                                                if(key_flag && KEYS[SDLK_ESCAPE]) { key_flag = 0; number_menu = last_number_menu; add_number_menu = 0; } //выход из меню
                                                break;
                                        }
                                        break;
                                    }
                                break;
                            case 1: //отрисовка игры
                                if(KEYS[SDLK_ESCAPE]) regim = 0; //выход из игры
                                if(key_flag && KEYS[SDLK_p]) {
                                    key_flag = 0;
                                    pause = !pause;
                                    if(pause) {
                                        Draw_FillRound(screen, 10, 10, 200, 73, 20, 0);
                                        Draw_Round(screen, 10, 10, 200, 73, 20, 4294967295);
                                        text_rect = TTF_RenderText_Solid(font, "Pause", text_color);
                                        dest.x = 30;
                                        dest.y = 11;
                                        SDL_BlitSurface(text_rect, NULL, screen, &dest);
                                        SDL_FreeSurface(text_rect);
                                    } else {
                                        Draw_FillRound(screen, 0, 0, widht_game, height_game, 20, 0);
                                        SDL_BlitSurface(textures->get_pointer_background(), NULL, screen, NULL);
                                    }
                                } //отработка паузы

                                if(!pause) {
                                    if(key_flag && KEYS[SDLK_w]) { key_flag = 0; new_dir = 0; }      //смена направления движения пакмена
                                    else if(key_flag && KEYS[SDLK_d]) { key_flag = 0; new_dir = 1; }
                                    else if(key_flag && KEYS[SDLK_s]) { key_flag = 0; new_dir = 2; }
                                    else if(key_flag && KEYS[SDLK_a]) { key_flag = 0; new_dir = 3; }
                                    if(logics->pacman.center() && !logics->maze.check_wall(logics->pacman.get_point(), new_dir)) logics->pacman.set_dir(new_dir);

                                    dest.x = logics->pacman.get_point().x - 12;     //затирание предыдущего расположения пакмена и призраков
                                    dest.y = logics->pacman.get_point().y - 12;
                                    Draw_FillRect(screen, dest.x, dest.y, 25, 25, 0);
                                    for(i = 0; i < 4; i++) {
                                        dest.x = logics->enemy[i].get_point().x - 12;
                                        dest.y = logics->enemy[i].get_point().y - 12;
                                        Draw_FillRect(screen, dest.x, dest.y, 25, 25, 0);
                                    }

                                    for(float i = 0; i < logics->maze.get_size().x; i++)        //отрисовка по новой всех монет и ягод
                                        for(float j = 0; j < logics->maze.get_size().y; j++) {
                                            if(logics->maze.check_value(i, j) == 1) {
                                                dest.x = j * 25;
                                                dest.y = i * 25;
                                                SDL_BlitSurface(textures->get_pointer_money(), NULL, screen, &dest);
                                            }
                                            if(logics->maze.check_value(i, j) == 4) {
                                                dest.x = j * 25;
                                                dest.y = i * 25;
                                                SDL_BlitSurface(textures->get_pointer_cherry(), NULL, screen, &dest);
                                            }
                                        }
                                    for(i = 0; i < 4; i++) {    //отрисовка приведений
                                        dest.x = logics->enemy[i].get_point().x - 12;
                                        dest.y = logics->enemy[i].get_point().y - 12;
                                        if(logics->enemy[i].get_type()) SDL_BlitSurface(textures->get_pointer_enemy(i+1), NULL, screen, &dest);
                                        else SDL_BlitSurface(textures->get_pointer_enemy(0), NULL, screen, &dest);
                                    }

                                    dest.x = logics->pacman.get_point().x - 12; //отрисовка пакмена
                                    dest.y = logics->pacman.get_point().y - 12;
                                    dest_2.x = logics->pacman.get_mouth() * 25;
                                    dest_2.y = logics->pacman.get_dir() * 25;
                                    SDL_BlitSurface(textures->get_pointer_pacman(), &dest_2, screen, &dest);

                                    logics->eat();
                                    logics->die_pacman();
                                    logics->step();

                                    if(logics->regim == 0) { //отображение счета по завершению игры
                                        Draw_FillRound(screen, 10, 10, widht_game - 20, 73, 20, 0);
                                        Draw_Round(screen, 10, 10, widht_game - 20, 73, 20, 4294967295);
                                        text_rect = TTF_RenderText_Solid(font, "You score: ", text_color);
                                        dest.x = 23;
                                        dest.y = 10;
                                        SDL_BlitSurface(text_rect, NULL, screen, &dest);
                                        SDL_FreeSurface(text_rect);

                                        convert(score_str, logics->pacman.get_score());
                                        add_score(score, logics->pacman.get_score());
                                        text_rect = TTF_RenderText_Solid(font, score_str, text_color);
                                        dest.x = 330;
                                        dest.y = 10;
                                        SDL_BlitSurface(text_rect, NULL, screen, &dest);
                                        SDL_FreeSurface(text_rect);

                                        SDL_Flip(screen);
                                        SDL_Delay(1500);
                                        regim = 0;
                                    }
                                }

                                break;
                        }

                        event.user.code = 0;
                        SDL_Flip(screen);

                        break;
                }
        }
    }

    save_file(score, level);
    SDL_FreeSurface(screen);
    SDL_FreeSurface(menu_full);
    SDL_QUIT;

    return 0;
}

Uint32 ttFunk(Uint32 interval, void *param){
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 1;
    userevent.data1 = 0;
    userevent.data2 = 0;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

void convert(char *number, int count) { //конвертация int числа в строку для отображения очков в конце игры и меню рекордов
    int temp = count, j = 0;
    while(temp) {
        temp = temp / 10;
        j++;
    }

    number[j] = 0;
    number[1] = 0;
    number[0] = '0';
    while(count) {
        number[j-1] = count % 10 + '0';
        count /= 10;
        j--;
    }
}

void add_score(int *mass, int score) { //добавление нового рекорда и проверка рекорд ли это
    mass[10] = score;
    int i, j;

	for (i = 1; i < 11; i++)
		for (j = 10; j >= i; j--)
			if (mass[j-1] < mass[j]) {
				score = mass[j-1];
				mass[j-1] = mass[j];
				mass[j] = score;
			}
}

void read_file(int *mass, char level) { //чтение файла
    FILE *file = fopen(level ? "score_1.bin" : "score_2.bin", "rb");
    fread(mass, sizeof(int), 10, file);
    fclose(file);
}

void save_file(int *mass, char level) { //сохранение файла
    FILE *file = fopen(level ? "score_1.bin" : "score_2.bin", "wb");
    fwrite(mass, sizeof(int), 10, file);
    fclose(file);
}
