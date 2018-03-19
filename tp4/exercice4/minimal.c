#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 800;
static const unsigned int BIT_PER_PIXEL = 32;
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-5., 5., -5., 5.);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}

GLuint creerImage(char *name){
	//Chargement et traitement de la texture
	SDL_Surface *nb;
	nb = IMG_Load(name);
	
	if(nb == NULL)
		printf("erreur avec l'image\n");
	
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		nb->w,
		nb->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nb->pixels
	);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	SDL_FreeSurface(nb);
	
	return textureID;
}

void afficherHeures(GLuint textureID[], int hourUnite, int hourDizaine){
	glBindTexture(GL_TEXTURE_2D, textureID[hourDizaine]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(-4,1);
		glTexCoord2f(1,0);
		glVertex2f(-3,1);
		glTexCoord2f(1,1);
		glVertex2f(-3,-1);
		glTexCoord2f(0,1);
		glVertex2f(-4,-1);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, textureID[hourUnite]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(-3,1);
		glTexCoord2f(1,0);
		glVertex2f(-2,1);
		glTexCoord2f(1,1);
		glVertex2f(-2,-1);
		glTexCoord2f(0,1);
		glVertex2f(-3,-1);
	glEnd();
}

void afficherMinutes(GLuint textureID[], int minuteUnite, int minuteDizaine){
	glBindTexture(GL_TEXTURE_2D, textureID[minuteDizaine]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(-1,1);
		glTexCoord2f(1,0);
		glVertex2f(0,1);
		glTexCoord2f(1,1);
		glVertex2f(0,-1);
		glTexCoord2f(0,1);
		glVertex2f(-1,-1);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, textureID[minuteUnite]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(0,1);
		glTexCoord2f(1,0);
		glVertex2f(1,1);
		glTexCoord2f(1,1);
		glVertex2f(1,-1);
		glTexCoord2f(0,1);
		glVertex2f(0,-1);
	glEnd();
}

void afficherSecondes(GLuint textureID[], int secondUnite, int secondDizaine){
	glBindTexture(GL_TEXTURE_2D, textureID[secondDizaine]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(2,1);
		glTexCoord2f(1,0);
		glVertex2f(3,1);
		glTexCoord2f(1,1);
		glVertex2f(3,-1);
		glTexCoord2f(0,1);
		glVertex2f(2,-1);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, textureID[secondUnite]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(3,1);
		glTexCoord2f(1,0);
		glVertex2f(4,1);
		glTexCoord2f(1,1);
		glVertex2f(4,-1);
		glTexCoord2f(0,1);
		glVertex2f(3,-1);
	glEnd();
}

void afficherColon(GLuint textureID[]){
	glBindTexture(GL_TEXTURE_2D, textureID[10]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(-2,1);
		glTexCoord2f(1,0);
		glVertex2f(-1,1);
		glTexCoord2f(1,1);
		glVertex2f(-1,-1);
		glTexCoord2f(0,1);
		glVertex2f(-2,-1);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, textureID[10]);//On bind la texture pour pouvoir l'utiliser	
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(1,1);
		glTexCoord2f(1,0);
		glVertex2f(2,1);
		glTexCoord2f(1,1);
		glVertex2f(2,-1);
		glTexCoord2f(0,1);
		glVertex2f(1,-1);
	glEnd();
}

int main(int argc, char** argv) {

    // Initialisation de la SDL
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    // Ouverture d'une fenêtre et création d'un contexte OpenGL
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("td04", NULL);
    resizeViewport();
	
	int i;
	
	//Création de la texture
	GLuint textureID[11];
	
	for(i=0; i<10; i++){
		char idx[100];
		sprintf(idx, "numbers/%d.png", i);
		textureID[i] = creerImage(idx);
	}
	textureID[10] = creerImage("numbers/colon.png");

    // Boucle de dessin (à décommenter pour l'exercice 3)
    int loop = 1;
	
	
    glClearColor(0.1, 0.1, 0.1 ,1.0);
    while(loop) {

        Uint32 startTime = SDL_GetTicks();
		
		//Récupération du temps
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		int hourUnite = (timeinfo->tm_hour)%10;
		int hourDizaine = (timeinfo->tm_hour)/10;

		int minuteUnite = (timeinfo->tm_min)%10;
		int minuteDizaine = (timeinfo->tm_min)/10;

		int secondUnite = (timeinfo->tm_sec)%10;
		int secondDizaine = (timeinfo->tm_sec)/10;	

		printf("h : %d%d, min %d%d, sec %d%d\n", hourUnite, hourDizaine, minuteUnite, minuteDizaine, secondUnite, secondDizaine);
		
		
        // TODO: Code de dessin
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D); //on précise qu'on veut activer la fonctionnalité de texturing
		
		afficherHeures(textureID, hourUnite, hourDizaine);
		afficherMinutes(textureID, minuteUnite, minuteDizaine);
		afficherSecondes(textureID, secondUnite, secondDizaine);
		afficherColon(textureID);
		
		
		//printf("h : %ld m %d s %d\n", sec, min/3600, seconde);
		
		
		glDisable(GL_TEXTURE_2D); //on désactive la fonctionnalité de texturing
       	glBindTexture(GL_TEXTURE_2D, 0);//On débind la texture pour pouvoir l'utiliser
		
	
        // Fin du code de dessin

        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            switch(e.type) {

                case SDL_QUIT:
                    loop = 0;
                    break;

                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();

                default:
                    break;
            }
        }

        SDL_GL_SwapBuffers();
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    // Libération des données GPU
    glDeleteTextures(10, textureID);

    // Liberation des ressources associées à la SDL
    SDL_Quit();

    return EXIT_SUCCESS;
}

