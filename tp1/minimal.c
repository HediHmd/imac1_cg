#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 400;
static unsigned int WINDOW_HEIGHT = 400;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

/* Struct */

typedef struct Point{
    float x, y; //position 2D du point
    unsigned char r, g, b; // Couleur du point
    struct Point* next; // Point suivant à dessiner
} Point, *PointList;

typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
} Primitive, *PrimitiveList;

/* Points */

Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b){
    Point* point = (Point*) malloc(sizeof(point));
    if(!point) {
        return NULL;
    }
    point->x = x;
    point->y = y;
    point->r = r;
    point->g = g;
    point->b = b;
    point->next = NULL;
    return point;
}

void addPointToList(Point* point, PointList* list){
    if(*list == NULL) {
        *list = point;
    }

    else {
    addPointToList(point, &(*list)->next);
    }
}

void drawPoints(PointList list){
    while(list!=NULL){
        glColor3ub(list->r, list->g, list->b);
        glVertex2f(list->x, list->y);
        list=list->next;
    }
}

void deletePoints(PointList* list){
PointList* tmp;
    while(list!=NULL){
        tmp = &(*list)->next; 
        free(*list);
        list = tmp;
    }
}


/* Primitives */

Primitive* allocPrimitive(GLenum primitiveType);

void addPrimitive(Primitive* primitive, PrimitiveList* list);

void drawPrimitives(PrimitiveList list);

void deletePrimitive(PrimitiveList* list);

/* Resize */

void resize(int WINDOW_WIDTH, int WINDOW_HEIGHT){
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);
}

int main(int argc, char** argv) {

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    
    /* Titre de la fenêtre */
    SDL_WM_SetCaption("OpenGL de Hédi", NULL);

    SDL_GL_SwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);

    /* Boucle d'affichage */
    int loop = 1;
    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();

        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            /* L'utilisateur ferme la fenêtre : */
            if(e.type == SDL_QUIT) {
                loop = 0;
                break;
            }
            /* Quelques exemples de traitement d'evenements : */
            switch(e.type) {

                /* Clic souris */
                case SDL_MOUSEBUTTONUP:
                        glBegin(GL_POINTS);
                        glColor3ub(0, 255, 0);
                        glVertex2f(-1 + 2. * e.button.x / WINDOW_WIDTH, - (-1 + 2. * e.button.y / WINDOW_HEIGHT));
                        glEnd();
                        SDL_GL_SwapBuffers();
                    break;

                /* Touche clavier */
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == 112){
                        return 0; 
                    }
                    printf("touche pressée (code = %d)\n", e.key.keysym.sym);
                    break;

                /* Mouvement souris */
                /*
                case SDL_MOUSEMOTION:
                    printf("clic en (%d, %d)\n", e.motion.x, e.motion.y);
                    float r1, v1, b1;
                    r1 = e.button.x / (float)WINDOW_WIDTH;
                    v1 = e.button.y / (float)WINDOW_HEIGHT;
                    b1 = 0;
                    glClearColor(r1, v1, b1, 1);
                    break;
                */

                /* Resize fenetre */
                case SDL_VIDEORESIZE:
                    resize(e.resize.w, e.resize.h);
                    break;

                default:
                    break;
            }
        }

        /* Calcul du temps écoulé */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}
