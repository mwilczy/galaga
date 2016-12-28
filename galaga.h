#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define WIDTH 640
#define HEIGHT 840
#define MENU_DISTANCE 10
#define HORIZONTAL_RIGHT 4
#define HORIZONTAL_LEFT 3
#define BULLET_UP 1
#define BULLET_DOWN 0
#define VELOCITY 15
#define BULLETS_AT_A_TIME 1
typedef struct MenuHolder 
{
	GtkWidget *main_window;
	GtkWidget *menu_box;
}MenuHolder;


typedef struct SpaceShip 
{
	int x;
	int y;
}SpaceShip;



typedef struct Ball 
{
	int x;
	int y;
	int velocity;
	int direction; // 1 = up 0 = down
	struct Ball *next;
	struct Ball *prev;
}Ball;

typedef struct Assets
{
	GdkPixbuf *bullet;
	GdkPixbuf *ship;
	GdkPixbuf *alien;
	SpaceShip one;
	Ball *bullets;
	Ball *aliens;
	Ball *alien_bullets;
	int alien_direction;

}Assets;




int player_bullet_timer;// ms
struct timespec prev_time;
int punkty_gracza;
int chances;
/*
typedef struct Bullets 
{
	int x;
	int y;
	int velocity;
	int direction; // 1 - up // 0 - down
	struct Bullets *prev;
	struct Bullets *next;

}Bullets;
*/

int InitGame(GtkWidget *main_window, Assets *images);
void FatalError(char *text);



gboolean timeout(GtkWidget *drawing_area);
gboolean timeout2(Assets *images);
gboolean draw_event(GtkWidget *widget, cairo_t *cr, Assets *images);
gboolean button_exit_press_event(GtkWidget *widget, GdkEventButton *event);
gboolean button_start_press_event(GtkWidget *widget, GdkEventButton *event, MenuHolder *menu);
gboolean motion_event(GtkWidget *widget, GdkEventMotion *event, Assets *images);
gboolean button_game_press_event(GtkWidget *widget, GdkEventButton *event, Assets *images);
gboolean key_game_press_event(GtkWidget *widget, GdkEventKey *event, Assets *images);

Ball *Insert(Ball **pointer, int x,int y, int velocity, int direction);
void Traverse(Ball *pointer, cairo_t *cr, GdkPixbuf *object);
Ball *Remove(Ball **pointer);

void GenerateAliens(Ball **aliens,int width, int height);
void ChangeAlienDirection(Ball *pointer);
int DetectBorder(Ball *pointer);
void DetectBorderBullets(Ball **pointer);

int DetectCollision(Ball *aliens, Ball *pociski, Ball **wynik1, Ball **wynik2);

int DetectCollisionPlayer(int x,int y, Ball *pociski, Ball **wynik2);

void ascii(int liczba, char *buffer);
void DetectLose(Ball *aliens);
void SpawnMore(Ball **aliens,int width, int height);
void go_back(Ball **pointer);

void save_binary(SpaceShip player, Ball *aliens, Ball *player_bullets, Ball *alien_bullets);
void Load(Assets *images);

void save_text(SpaceShip player, Ball *aliens, Ball *player_bullets, Ball *alien_bullets);
int LoadText(Assets *images);