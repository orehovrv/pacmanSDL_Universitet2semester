#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL.h>
#include "SDL_draw.h"

#include "DOT.h"
#include "MAZE.h"

#define scale 25

class TEXTURES {
    private:
        SDL_Surface *pacman = NULL;
        SDL_Surface *enemy[5] = {0};
        SDL_Surface *background = NULL;
        SDL_Surface *money = NULL;
        SDL_Surface *cherry = NULL;
        //указатели на поверхности с уже готовыми текстурами пакмена,
        //привидений, монетки, ягоды и фона

        void draw_pacman();
        void draw_enemy();
        void draw_money();
        void draw_cherry();
        void draw_background(MAZE&);
        //методы для отрисовки текстур с помощью примитивов
    public:
        TEXTURES(MAZE&);
        ~TEXTURES();

        SDL_Surface *get_pointer_pacman();
        SDL_Surface *get_pointer_enemy(char);
        SDL_Surface *get_pointer_money();
        SDL_Surface *get_pointer_cherry();
        SDL_Surface *get_pointer_background();
        //методы для получения указателей на поверхности
};

#endif // TEXTURES_H

