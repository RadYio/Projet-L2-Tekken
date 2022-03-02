#include "../include/joueur.h"

#define IDLE 0
#define COURIR 1
#define POING 2
#define PARER 3

void renderAnimation(Joueur * joueur, SDL_RendererFlip flip);
void resetAnimation(Joueur * joueur);
void jouerAnimation(Joueur * joueur, int anim);
