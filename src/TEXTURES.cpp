#include "TEXTURES.h"

#define clear  0 //номера для цветов
#define black  255
#define white  4294967295
#define yellow 4294902015
#define field  169908479
Uint32 enemy_color[5] = {2139062271, 4280549887, 16711679, 4225956863, 4289462783};  //серый красный бирюзовый розовый оранжевый

TEXTURES::TEXTURES(MAZE &input) { //конструктор класса
    draw_pacman();
    draw_enemy();
    draw_money();
    draw_cherry();
    draw_background(input);
}

TEXTURES::~TEXTURES() { //декструктор класса
    SDL_FreeSurface(pacman);
    for(int i = 0; i < 5; i++)
        SDL_FreeSurface(enemy[i]);
    SDL_FreeSurface(background);
    SDL_FreeSurface(money);
    SDL_FreeSurface(cherry);
    SDL_QUIT;
}

void TEXTURES::draw_pacman() { //создание поверхности и отрисовка всех возможных состояний пакмена
    pacman = SDL_CreateRGBSurface(0, 13 * scale, 4 * scale, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    int x, y = 12, i, j, k;

    for(i = 0; i <= 12; i++) {
        x = i * scale + 12;
        for(j = 0; j < 4; j++) {
            Draw_FillCircle(pacman, x, y + j * scale, 11, yellow);
            switch(j) {
                case 0:
                    for(k = x - i; k < x + i; k++) Draw_Line(pacman, k, y - 11 + j * scale, x, y + j * scale, clear);
                    break;
                case 1:
                    for(k = y - i; k < y + i; k++) Draw_Line(pacman, x + 11, k + j * scale, x, y + j * scale, clear);
                    break;
                case 2:
                    for(k = x - i; k < x + i; k++) Draw_Line(pacman, k, y + 11 + j * scale, x, y + j * scale, clear);
                    break;
                case 3:
                    for(k = y - i; k < y + i; k++) Draw_Line(pacman, x - 11, k + j * scale, x, y + j * scale, clear);
                    break;
            }
        }
    }
}

void TEXTURES::draw_enemy() { //создание поверхностией и отрисовка привидений
    for(int i = 0; i < 5; i++) {
        enemy[i] = SDL_CreateRGBSurface(0, scale, scale, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

        Draw_FillCircle(enemy[i], 12, 12,  9, enemy_color[i]);
        Draw_FillRect  (enemy[i],  3, 12, 19, 9, enemy_color[i]);

        Draw_FillRect(enemy[i], 11, 20, 3, 1, clear);
        Draw_FillRect(enemy[i],  6, 20, 3, 1, clear);
        Draw_FillRect(enemy[i], 16, 20, 3, 1, clear);

        Draw_Pixel(enemy[i], 12, 19, clear);
        Draw_Pixel(enemy[i], 17, 19, clear);
        Draw_Pixel(enemy[i],  7, 19, clear);
        Draw_Pixel(enemy[i], 21, 20, clear);
        Draw_Pixel(enemy[i],  3, 20, clear);

        Draw_FillCircle(enemy[i],  8, 11, 3, white);
        Draw_FillCircle(enemy[i], 16, 11, 3, white);
        Draw_FillRect  (enemy[i], 15, 10, 2, 2, black);
        Draw_FillRect  (enemy[i],  7, 10, 2, 2, black);
    }
}

void TEXTURES::draw_money() { //создане поверхности и отрисовка монетки
    money = SDL_CreateRGBSurface(0, scale, scale, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    Draw_FillCircle(money, 12, 12, 3, yellow);
}

void TEXTURES::draw_cherry() { //создание поерхности и отрисовка ягодки
    cherry = SDL_CreateRGBSurface(0, scale, scale, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    Draw_FillCircle(cherry, 12+4, 12+2, 5, SDL_MapRGB (cherry->format, 237, 28, 36));
    Draw_FillCircle(cherry, 12-5, 12+3, 5, SDL_MapRGB (cherry->format, 237, 28, 36));
    Draw_Line(cherry, 12+2, 12-10, 12+4, 12-3, SDL_MapRGB (cherry->format, 136, 0, 21));
    Draw_Line(cherry, 12+2, 12-10, 12-5, 12-2, SDL_MapRGB (cherry->format, 136, 0, 21));
}

void TEXTURES::draw_background(MAZE &input) { //создание поверхности и отрисока стенок лабиринта на основе входных данных
    int i, j;
    DOT size = input.get_size();
    background = SDL_CreateRGBSurface(0, size.y * scale, size.x * scale, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    Draw_FillRect(background, 0, 0, size.y * scale, size.x * scale, black);

    for(i = 0; i < size.y; i++)
        for(j = 0; j < size.x; j++){
            if(i < size.y-1 && input.check_value(j, i+1) == 0 && input.check_value(j, i) == 0)
                Draw_FillRect(background, scale*i+6, scale*j+6, scale*2-12, scale-12, field);
            if(j < size.x-1 && input.check_value(j+1, i) == 0 && input.check_value(j, i) == 0)
                Draw_FillRect(background, scale*i+6, scale*j+6, scale-12, scale*2-12, field);
            if(i < size.y-1 && j < size.x-1 && input.check_value(j, i) == 0 && input.check_value(j+1, i+1) == 0)
                if(input.check_value(j, i+1) == 0 && input.check_value(j+1, i) == 0)
                    Draw_FillRect(background, scale*i+16, scale*j+16, 16, 16, field);
        }
}

SDL_Surface *TEXTURES::get_pointer_pacman() { //получние указателя на поверхность с пакменом
    return pacman;
}

SDL_Surface *TEXTURES::get_pointer_enemy(char number) { //получние указателя на поверхность с одиним из привидений
    return enemy[number];
}

SDL_Surface *TEXTURES::get_pointer_money() { //получние указателя на поверхность с монеткой
    return money;
}

SDL_Surface *TEXTURES::get_pointer_cherry() { //получние указателя на поверхность с ягодкой
    return cherry;
}

SDL_Surface *TEXTURES::get_pointer_background() { //получние указателя на поверхность с фоном
    return background;
}
