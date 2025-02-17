#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "../include/initSdl.h"
#include "../include/mouvements.h"
#include "../include/menu.h"
#include "../include/joueur.h"
#include "../include/animations.h"
#include "../include/jeu.h"
#include "../include/personnages.h"
#include "../include/map.h"
#include "../include/gui.h"
#include "../include/pause.h"

#define MODE FULLSCREEN
int sec_deb_combat, temps_combat, ancien_temps=-1, temps_deb_pause, temps_fin_pause, temps_pause;
bool quit;
bool pause=false;
SDL_DisplayMode ecran;
int flag_perdu=0;
int numRound=0;

void roundSuivant(Joueur * joueurGagnant, Joueur * joueurPerdant ,TTF_Font * font){
  numRound++;
  SDL_Color blanc = {255, 255, 255};
  SDL_Texture * texture = NULL;
  SDL_Rect rect;
  char texte[50];
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 125);

  snprintf(texte, sizeof(texte), "%s gagne le round %d !", joueurGagnant->perso.nom, numRound);
  creerBouton(renderer, font, texte, blanc, &rect, &texture, ecran.w/2, ecran.h/2+100);
  
  SDL_RenderFillRect(renderer, NULL);
  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_RenderPresent(renderer);

  flag_perdu=0;
  initJoueur(joueurGagnant, 100.0, joueurGagnant->nom, joueurGagnant->texture, gauche);
  initJoueur(joueurPerdant, 600.0, joueurPerdant->nom, joueurPerdant->texture, droite);
  SDL_Delay(5000);
}

void initSdl(Joueur * j1, Joueur * j2, int num_map, int drip) { //Créer la fenêtre et l'environnement (pour l'instant)
  temps_combat = 0;
  sec_deb_combat = 0;
  temps_fin_pause = 0;
  temps_deb_pause = 0;
  temps_pause = 0;
  quit = false;

  SDL_Surface * surface_hitbox_coupj1 = IMG_Load("res/rectangle_bleu.png");
  SDL_Texture * texture_hitbox_coupj1;
  SDL_Surface * surface_hitbox_coupj2 = IMG_Load("res/rectangle_bleu.png");
  SDL_Texture * texture_hitbox_coupj2;
  SDL_Surface * surface_hitbox_piedj1 = IMG_Load("res/rectangle_rouge.png");
  SDL_Texture * texture_hitbox_piedj1;
  SDL_Surface * surface_hitbox_piedj2 = IMG_Load("res/rectangle_rouge.png");
  SDL_Texture * texture_hitbox_piedj2;
  SDL_Texture * tex_menu_Principal = NULL;
  SDL_Texture * texture_carre_jaune = NULL;
  SDL_Texture * texture_carre_rouge = NULL;
  SDL_Texture * texture_barre_de_vie = NULL;
  SDL_Texture * texture_nomj1 = NULL;
  SDL_Texture * texture_nomj2 = NULL;
  SDL_Texture * texture_temps = NULL;

  SDL_Rect srcBg, dstBg, rect_sprite_pv_j1, rect_sprite_pv_j2,rect_nom_j1, rect_nom_j2,rect_temps;

  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);
  SDL_GetDesktopDisplayMode(0, &ecran);

  switch(MODE){
    case FULLSCREEN: window = SDL_CreateWindow("SDL2 Tekken",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ecran.w, ecran.h, SDL_WINDOW_FULLSCREEN);
    break;
    case BORDERLESS: window = SDL_CreateWindow("SDL2 Tekken",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ecran.w, ecran.h, SDL_WINDOW_BORDERLESS);
    break;
    default: window = SDL_CreateWindow("SDL2 Tekken",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ecran.w / 2, ecran.h / 2, 0);
  }
  renderer = SDL_CreateRenderer(window, -1, 0);
  texture_hitbox_coupj1 = SDL_CreateTextureFromSurface(renderer, surface_hitbox_coupj1);
  texture_hitbox_coupj2 = SDL_CreateTextureFromSurface(renderer, surface_hitbox_coupj2);

  texture_hitbox_piedj1 = SDL_CreateTextureFromSurface(renderer, surface_hitbox_piedj1);
  texture_hitbox_piedj2 = SDL_CreateTextureFromSurface(renderer, surface_hitbox_piedj2);


  char j1sprite[50], j2sprite[50];
  snprintf(j1sprite, sizeof(j1sprite), "res/sprites/%s.png", j1->perso.nom);
  snprintf(j2sprite, sizeof(j2sprite), "res/sprites/%s.png", j2->perso.nom);

  SDL_Surface * perso1 = IMG_Load(j1sprite);
  SDL_Surface * perso2 = IMG_Load(j2sprite);

  chargerMap(num_map, renderer, 1, drip);

  SDL_Texture * texture_joueur1 = SDL_CreateTextureFromSurface(renderer, perso1);
  SDL_Texture * texture_joueur2 = SDL_CreateTextureFromSurface(renderer, perso2);
  SDL_FreeSurface(perso1);
  SDL_FreeSurface(perso2);

  TTF_Init();

  initJoueur(j1, 100.0, "Shrekleouinouin", texture_joueur1, gauche);
  initJoueur(j2, 600.0, "PINGU", texture_joueur2, droite);
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, 0); //Couleur des hitboxes

  init_sprite_pv(&rect_sprite_pv_j1, 1);
  init_sprite_pv(&rect_sprite_pv_j2, 2);

  SDL_Surface * sprite_barre_de_vie = IMG_Load("res/barre_de_vie.png");
  init_gui(&texture_carre_jaune, &texture_carre_rouge, sprite_barre_de_vie, &texture_barre_de_vie);

  TTF_Font * font = NULL;
  font = TTF_OpenFont("res/fonts/Sans.ttf", 50);

  init_afficher_nom_joueur(j1, font, &rect_sprite_pv_j1, &rect_nom_j1, &texture_nomj1,1);
  init_afficher_nom_joueur(j2, font, &rect_sprite_pv_j2, &rect_nom_j2, &texture_nomj2,2);
  
  initPause();

  int sec_anim;

  sec_deb_combat = SDL_GetTicks();

  while (!quit) {
    SDL_Event event;
	  SDL_PollEvent(&event);
    
    if(!pause)
      temps_combat = (SDL_GetTicks() - (temps_pause*1000))/1000;
    sec_anim = SDL_GetTicks()/75;
    jouerAnimationBackground(&srcBg, &dstBg,1);
    
    switch (event.type) {
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          if(pause)
            temps_deb_pause = (sec_deb_combat - SDL_GetTicks());
          if(!pause)
            temps_fin_pause = (sec_deb_combat - SDL_GetTicks());
          temps_pause+=(temps_fin_pause - temps_deb_pause)/1000;
          pause = !pause;
        break;
      }
    }

    sauter(j1);
    sauter(j2);
    jouerAnimation(j1,sec_anim,j2);
    jouerAnimation(j2,sec_anim,j1);

    if(j1->perso.frame==0){
      jouerAnimationContinu(j1,sec_anim);
    }
    if(j2->perso.frame==0){
      jouerAnimationContinu(j2,sec_anim);
    }

    flag_perdu = checkPerdu(j1, j2);
    if(!flag_perdu)
      SDL_RenderClear(renderer);
    renderMap(&srcBg, &dstBg, renderer);

    hitbox(j1, texture_hitbox_coupj1,1);
	  hitbox(j2, texture_hitbox_coupj2,1);
    hitbox(j1, texture_hitbox_piedj1,0);
    hitbox(j2, texture_hitbox_piedj2,0);

    SDL_RenderFillRect(renderer, &(j1->hitbox));
    SDL_RenderFillRect(renderer, &(j2->hitbox));

    renderAnimation(j1);
    renderAnimation(j2);
    barre_de_vie(j1, &rect_sprite_pv_j1, texture_barre_de_vie, texture_carre_rouge, texture_carre_jaune, font);
    barre_de_vie(j2, &rect_sprite_pv_j2, texture_barre_de_vie, texture_carre_rouge, texture_carre_jaune, font);
    
    init_affichage_temps(temps_combat, font, &rect_sprite_pv_j1, &texture_temps, &rect_temps);

    SDL_RenderCopy(renderer, texture_temps, NULL ,&rect_temps);

    SDL_DestroyTexture(texture_temps);

    SDL_RenderCopy(renderer, texture_nomj1, NULL ,&rect_nom_j1);
    SDL_RenderCopy(renderer, texture_nomj2, NULL ,&rect_nom_j2);

    checkmort(j1,j2);

    if(!pause)
      deplacements(j1, j2, &event);
    else {
      selectionPause(event, drip);
      renderPause();
    }

  if(flag_perdu == 1 && j1->perso.frame+1>=j1->perso.nb_frame[MORT]){
    roundSuivant(j2, j1, font);
    sec_deb_combat=SDL_GetTicks();
  } else if(flag_perdu == 2 && j2->perso.frame+1>=j2->perso.nb_frame[MORT]){
    roundSuivant(j1, j2, font);
    sec_deb_combat=SDL_GetTicks();
  }
    SDL_RenderPresent(renderer);
  }

  SDL_RenderClear(renderer);
  SDL_DestroyTexture(texture_hitbox_coupj1);
  SDL_DestroyTexture(texture_hitbox_coupj2);
  SDL_DestroyTexture(texture_nomj1);
  SDL_DestroyTexture(texture_nomj2);
  SDL_DestroyTexture(texture_carre_rouge);
  SDL_DestroyTexture(texture_carre_jaune);
  SDL_DestroyTexture(texture_barre_de_vie);
  SDL_DestroyTexture(tex_menu_Principal);
  SDL_DestroyTexture(texture_joueur1);
  SDL_DestroyTexture(texture_joueur2);
  SDL_FreeSurface(sprite_barre_de_vie);
  SDL_FreeSurface(surface_hitbox_coupj1);
  SDL_FreeSurface(surface_hitbox_coupj2);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}
