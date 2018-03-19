#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 800;

/* Nombre de segments pour former le cercle */
static const unsigned int NB_SEG= 120;
static const float PI = 3.1415926535897932384626433832795;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

unsigned int lastTime = 0, currentTime;

typedef struct Point {
    float x, y;
    unsigned char r, g, b;
    struct Point* next;
} Point, *PointList;

Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b) {
    /*
    On alloue un espace mémoire suffisant pour pouvoir stocker un point
    Attention : la fonction malloc() renvoie un void* qu'il faut impérativement caster en Point*.
    */
    Point* point = (Point*) malloc(sizeof(Point));
    if (!point) {
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

void addPointToList(Point* point, PointList* list) {
    assert(point);
    assert(list);
    if (*list == NULL) {
        /* Si le pointeur sur la liste est vide, on le modifie pour le faire pointer sur le point donné en paramètre */
        *list = point;
    } else {
        /* Sinon, on réapplique la même fonction mais sur le point suivant */
        addPointToList(point, &(*list)->next);
    }
}

void drawPoints(PointList list) {
    while(list) {
        glColor3ub(list->r, list->g, list->b);
        glVertex2f(list->x, list->y);
        list = list->next;
    }
}

void deletePoints(PointList* list) {
    assert(list);
    while (*list) {
        Point* next = (*list)->next;
        free(*list);
        *list = next;
    }
}

typedef struct Primitive{
    GLenum primitiveType;
    PointList points;
    struct Primitive* next;
} Primitive, *PrimitiveList;

Primitive* allocPrimitive(GLenum primitiveType) {
    /*
    On alloue un espace mémoire suffisant pour pouvoir stocker une primitive
    Attention : la fonction malloc() renvoie un void* qu'il faut impérativement caster en Primitive*.
    */
    Primitive* primitive = (Primitive*) malloc(sizeof(Primitive));
    if (!primitive) {
        return NULL;
    }
    primitive->primitiveType = primitiveType;
    primitive->points = NULL;
    primitive->next = NULL;
    return primitive;
}

void addPrimitive(Primitive* primitive, PrimitiveList* list) {
    assert(primitive);
    assert(list);
    primitive->next = *list;
    (*list) = primitive;
}

void drawPrimitives(PrimitiveList list) {
    while(list) {
        glBegin(list->primitiveType);
        drawPoints(list->points);
        glEnd();
        list = list->next;
    }
}

void deletePrimitive(PrimitiveList* list) {
    assert(list);
    while(*list) {
        Primitive* next = (*list)->next;
        deletePoints(&(*list)->points);
        free(*list);
        *list = next;
    }
}

void resizeViewport() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-4., 4., -3., 3.);
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}

void drawSquare(float r, float g, float b){
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5,-0.5);
    glVertex2f(-0.5,-0.5);
    glEnd();
}

void drawLandmark(){
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-1, 0);
    glVertex2f(1, 0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(0,1);
    glVertex2f(0,-1);
    glEnd();

        /*Dessiner points repere*/
    glPointSize(5);
    glBegin(GL_POINTS);
    for (int i = -4; i <= 4; i++)
    {
        glColor3f(1.0, 0.0, 0.0);
        glVertex2f(i, 0);
    }
    for (int j = -3; j <= 3; j++)
    {
        glColor3f(0.0, 1.0, 0.0);
        glVertex2f(0, j);
    }
        glEnd();
}

}

// Pour remplir le cercle : GL_POLYGON
void drawCircle(float r, float g, float b){
    int i;
    float angle;
    angle = 2*PI/NB_SEG;

    glBegin(GL_LINE_STRIP);
    glColor3f(r, g, b);
    for (i = 0; i <= NB_SEG; i++)
    {
        glVertex2f(cos(angle*i), sin(angle*i));
    }
    glEnd();
}


int main(int argc, char** argv) {

    /* Initialisation de la SDL */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    
    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    SDL_WM_SetCaption("TP2", NULL);
    resizeViewport();
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    /* On créé une première primitive par défaut */
    PrimitiveList primitives = allocPrimitive(GL_LINE_STRIP);

    int loop = 1;
    float x, y;
    x=0;
    y=0;

    //Carré Drag & drop
    float xCarre, yCarre, rotateCarre;
    int mode = 0;
    xCarre=0;
    yCarre=0;
    rotateCarre=0;

    //Cercle qui se balade
    float xRond=0;
    float yRond=0;

    /* Boucle d'affichage */
    while(loop) {

        /* Récupération du temps au début de la boucle */
        Uint32 startTime = SDL_GetTicks();
        
        /* Code de dessin */
        glClear(GL_COLOR_BUFFER_BIT); // Toujours commencer par clear le buffer
        glLoadIdentity();

        drawPrimitives(primitives);
        //Carré blanc centre
        drawSquare(1.0,1.0,1.0);
        //Reperes
        drawLandmark();
        //Cercle bleu centre
        drawCircle(0.0,0.0,1.0);
        //Cercle orange haut droite
        glTranslatef(1,2,0);
        drawCircle(1.0,0.4,0.4);

        // Carré rouge
        glLoadIdentity();
        glTranslatef(2,0,0);
        glRotatef(45, 0.0,0.0,1.0);
        drawSquare(1.0,0.0,0.0);

        // Carré Violet
        glLoadIdentity();
        glRotatef(45, 0.0,0.0,1.0);
        glTranslatef(2,0,0);
        drawSquare(1.0,0.0,1.0);

        // Carré Jaune
        glLoadIdentity();
        glTranslatef(xCarre,yCarre,0);
        glRotatef(rotateCarre,0.0,0.0,1.0);
        drawSquare(0.0,0.0,1.0);

        // Cercle qui se ballade
        currentTime = SDL_GetTicks();
        glLoadIdentity();
        if (currentTime > lastTime + 1000){
        xRond = rand()%(4 - (-4)) + (-4);
        yRond = rand()%(3 - (-3)) + (-3);
        lastTime = currentTime;
        }
        glTranslatef(xRond,yRond,0);
        drawCircle(1.0,1.0,0.0);
        
        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)){

            /* L'utilisateur ferme la fenêtre : */
            if(e.type == SDL_QUIT){
                loop = 0;
                break;
            }
            
            /* Quelques exemples de traitement d'evenements : */
            switch(e.type) {

                /* Touche clavier */
                case SDL_KEYDOWN:
                    printf("touche pressée (code = %d)\n", e.key.keysym.sym);
                break;

                case SDL_MOUSEBUTTONUP:
                    /* Transformation des coordonnées du clic souris en coordonnées OpenGL */
                    x = -4 + 8. * e.button.x / WINDOW_WIDTH;
                    y = -(-3 + 6. * e.button.y / WINDOW_HEIGHT);
                    xCarre = -4 + 8. * e.button.x / WINDOW_WIDTH;
                    yCarre = -(-3 + 6. * e.button.y / WINDOW_HEIGHT);
                    /* On ajoute un nouveau point à la liste de la primitive courante */
                    addPointToList(allocPoint(x, y, 255, 255, 255), &(primitives->points));
                    if(e.button.button == SDL_BUTTON_RIGHT){
                        primitives->primitiveType = GL_LINE_LOOP;
                        //primitives->points = primitives->next->points;
                        addPrimitive(allocPrimitive(GL_LINE_STRIP),&primitives);
                    }
                    mode=0;
                break;

                case SDL_MOUSEBUTTONDOWN:
                    if(e.button.button == SDL_BUTTON_RIGHT){
                        mode=1;
                    }
                break;

                case SDL_MOUSEMOTION:
                    if(mode==1){
                        xCarre= -4 + 8. * e.motion.x / WINDOW_WIDTH;
                        yCarre=-(-3 + 6. * e.motion.y / WINDOW_HEIGHT);
                        rotateCarre++;
                    }
                break;

                case SDL_VIDEORESIZE:
                    WINDOW_WIDTH = e.resize.w;
                    WINDOW_HEIGHT = e.resize.h;
                    resizeViewport();
                    glMatrixMode(GL_MODELVIEW);
                break;

                default:
                break;
            }
        }

        /* Echange du front et du back buffer : mise à jour de la fenêtre */
        SDL_GL_SwapBuffers();

        /* Calcul du temps écoulé */
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    deletePrimitive(&primitives);

    /* Liberation des ressources associées à la SDL */ 
    SDL_Quit();

    return EXIT_SUCCESS;
}