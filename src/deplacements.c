#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "../include/deplacements.h"
#include "../include/initSdl.h"
#include "../include/joueur.h"
#include "../include/animations.h"
#include "../include/jeu.h"
#include "../include/son.h"

#define VITESSE 1

typedef struct {
	Joueur * joueur;
	int anim;
} params;

void sauter(Joueur * joueur){
        /*if(joueur->action==SAUTER)
           if(estTropHaut(joueur))
              if(!estAuSol(joueur))
	         joueur->position.y -= 1.5;
	   else
	         joueur->position.y += 1.5;*/
}

void attaquer(Joueur * jAttaquant, Joueur * j2){
	jAttaquant->action=POING;
	if(checkCollisions(jAttaquant, j2)){
		if(j2->action!=PARER)
			j2->vie-=20;
	}
}

void deplacements(Joueur * j1, Joueur * j2) {
	hitbox(j1);
	hitbox(j2);
	direction(j1, j2);
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT:
		quit = true;
		break;
		if(j2->perso.frame==0){
			case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button) {
				case SDL_BUTTON_RIGHT:
					j2->action=PARER;
					break;
				case SDL_BUTTON_LEFT:
					attaquer(j2, j1);
					break;
			}
			break;

			case SDL_MOUSEBUTTONUP:
			switch (event.button.button) {
				/*case SDL_BUTTON_RIGHT:
					j2->action=IDLE;
					break;*/
				case SDL_BUTTON_LEFT:
					j2->action=IDLE;
					break;
			}
			break;

		}
		
			case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				if(j1->perso.frame==0){
					case SDLK_a:
						if(!(event.key.repeat)){
							attaquer(j1, j2);
						}
						break;
				}
				/*if(j2->perso.frame==0){ marche pas
					case SDLK_1:
						if(!(event.key.repeat)){
							attaquer(j2, j1);
						}
						break;
				}*/
			}
			break;
		
		case SDL_KEYUP:
		switch (event.key.keysym.sym) {
			// event J1 
			if((j1->perso.frame)==0){
			case SDLK_q:
				j1->action=IDLE;
				break;
			case SDLK_d:
				j1->action=IDLE;
				break;
			case SDLK_SPACE:
				j1->action=IDLE;
				break;
			case SDLK_e:
				j1->action=IDLE;
				break;
			}
			// event J2 
			if((j2->perso.frame)==0){
			case SDLK_0:
				j2->action=IDLE;
				break;
			case SDLK_LEFT:
				j2->action=IDLE;
				break;
			case SDLK_RIGHT:
				j2->action=IDLE;
				break;
			}
		}
		break;
	}
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		quit=true;
	}
	/* verif touches J1 */
	if(j1->perso.frame==0){
		if (state[SDL_SCANCODE_A]) { //recule : touche q
			if(j1->hitbox.x>0){
				j1->position.x -= VITESSE;
				j1->action=COURIR;
			}
		}
		if (state[SDL_SCANCODE_D]){ // avance
			if(j1->hitbox.x<ecran.w-j1->perso.taille_hitbox.w){
				j1->position.x += VITESSE;
				j1->action=COURIR;
			}
		}
		if (state[SDL_SCANCODE_E]) { // parer
			j1->action=PARER;
		}
		if (state[SDL_SCANCODE_SPACE]) { //sauter
			j1->action=SAUTER;
		}
	/* verif touches J2 */
	}
	if(j2->perso.frame==0){
		if (state[SDL_SCANCODE_LEFT]) {
			if(j2->hitbox.x>0){
			j2->position.x -= VITESSE;
			j2->action=COURIR;
			}
		}
		if (state[SDL_SCANCODE_RIGHT]) {
			if(j2->hitbox.x<ecran.w-j2->perso.taille_hitbox.w){
				j2->position.x += VITESSE;
				j2->action=COURIR;
			}
		}
		if (state[SDL_SCANCODE_KP_0]) {
			j2->action=SAUTER;
		}
	}
}
