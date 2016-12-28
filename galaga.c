#include "galaga.h"

char health[20] = "HEALTH:";
void FatalError(char *text)
{
	printf("%s\n", text);
	exit(-1);
}

int InitGame(GtkWidget *main_window, Assets *images)
{
	int image_width, image_height;
	GtkWidget *drawing_area;
	if(!(images->ship = gdk_pixbuf_new_from_file("ship.png", NULL)))
		FatalError("Can't load ship");
	if(!(images->bullet = gdk_pixbuf_new_from_file("bullet.png", NULL)))
		FatalError("Can't load bullet");
	if(!(images->alien = gdk_pixbuf_new_from_file("alien.png", NULL)))
		FatalError("Can't load alien");

	//printf("%d %d\n",gdk_pixbuf_get_width(images->alien), gdk_pixbuf_get_height(images->alien));
	if(images->aliens == NULL)
		GenerateAliens(&(images->aliens),gdk_pixbuf_get_width(images->alien), gdk_pixbuf_get_height(images->alien));
	image_width = gdk_pixbuf_get_width(images->ship);
	image_height = gdk_pixbuf_get_height(images->ship);

	images->one.x = WIDTH/2 - image_width/2;
	images->one.y = HEIGHT - image_height;
	drawing_area = gtk_drawing_area_new();

	//Insert(images->)

	printf("init game\n");
	gtk_container_add(GTK_CONTAINER(main_window), drawing_area);
	g_signal_connect(drawing_area,"draw", G_CALLBACK(draw_event), images);
	//g_signal_connect(main_window, "key_press_event", G_CALLBACK(key_game_press_event), images);
	g_signal_connect(main_window, "motion-notify-event", G_CALLBACK(motion_event), images);
	g_signal_connect(main_window, "button_press_event", G_CALLBACK(button_game_press_event), images);
	g_signal_connect(main_window, "key_press_event", G_CALLBACK(key_game_press_event), images);
	gtk_widget_show_all(main_window);
	gdk_window_set_cursor(gtk_widget_get_window(main_window),gdk_cursor_new_from_name(gdk_display_get_default(),"none"));
	g_timeout_add(16,(GSourceFunc)timeout,drawing_area);
	g_timeout_add(800, (GSourceFunc)timeout2, images);
	return 0;
}

gboolean timeout2(Assets *images)
{
	int t;
	int nr_bullet = BULLETS_AT_A_TIME;
	while(nr_bullet > 0)
	{
		Ball *tmp = images->aliens;	
		while(tmp != NULL)
		{
			t = rand() % 20;
			if(t == 5)
			{
				Insert(&(images->alien_bullets), tmp->x,tmp->y,VELOCITY, BULLET_DOWN);
				nr_bullet--;
				if(nr_bullet == 0)
					return TRUE;
			}
			tmp = tmp->next;
		}

		tmp = images->aliens;
		while(tmp != NULL)
		{
			t = rand() % 20;
			if(t == 5)
			{
				Insert(&(images->alien_bullets), tmp->x,tmp->y,VELOCITY, BULLET_DOWN);
				nr_bullet--;
				if(nr_bullet ==0)
					return TRUE;
			}
			tmp = tmp->next;
		}
	}
	return TRUE;
}

gboolean timeout(GtkWidget *drawing_area)
{
	gtk_widget_queue_draw(drawing_area);
	return TRUE;
}

gboolean draw_event(GtkWidget *widget, cairo_t *cr, Assets *images)
{
	SpawnMore(&(images->aliens), gdk_pixbuf_get_width(images->alien), gdk_pixbuf_get_height(images->alien));
	DetectLose(images->aliens);

	if(DetectBorder(images->aliens) == -1)
		ChangeAlienDirection(images->aliens);

	cairo_set_source_rgb(cr, 0,0,0);
	cairo_paint(cr);	

	health[7] = chances + 48;
	health[8] = '\0';
	cairo_set_source_rgb(cr, 1,1,1);
	cairo_move_to(cr, 0, 20);
	cairo_set_font_size(cr, 20.0);
	cairo_show_text(cr,health);
	cairo_stroke(cr);

	char points[60] = "Punkty:";
	ascii(punkty_gracza, points + 7); 
	cairo_move_to(cr, 150, 20);
	cairo_show_text(cr, points);
	cairo_stroke(cr);



	
	gdk_cairo_set_source_pixbuf(cr,images->ship, images->one.x,images->one.y);
	cairo_paint(cr);
	Traverse(images->bullets, cr, images->bullet);
	Traverse(images->aliens,cr, images->alien);
	Traverse(images->alien_bullets,cr, images->bullet);
	DetectCollision(images->aliens, images->bullets, &(images->aliens), &(images->bullets));
	DetectCollisionPlayer(images->one.x, images->one.y, images->alien_bullets,&(images->alien_bullets));
	return FALSE;
}

gboolean motion_event(GtkWidget *widget, GdkEventMotion *event, Assets *images)
{
	int ship_width = gdk_pixbuf_get_width(images->ship);
	if(event->x > WIDTH - ship_width)
		return TRUE;
	images->one.x = event->x;
	return TRUE;
}

int difference_ms(struct timespec one, struct timespec two)
{
	return labs(1000*(two.tv_sec - one.tv_sec)) + labs((two.tv_nsec - one.tv_nsec)/1000000);
}

gboolean button_game_press_event(GtkWidget *widget, GdkEventButton *event, Assets *images)
{
	if(event->type == GDK_BUTTON_PRESS)
	{
		struct timespec czas;
		clock_gettime(CLOCK_REALTIME, &czas);
		if(difference_ms(prev_time, czas) < player_bullet_timer)
			return TRUE;
		prev_time = czas;
		Insert(&(images->bullets), images->one.x + 22,images->one.y,VELOCITY, BULLET_UP);
	}
	return TRUE;
}


gboolean key_game_press_event(GtkWidget *widget, GdkEventKey *event, Assets *images)
{
	if(event->type == GDK_KEY_PRESS)
	{
		if(event->keyval == GDK_KEY_s)
		{
			printf("time to save\n");
			save_text(images->one, images->aliens, images->bullets, images->alien_bullets);
			save_binary(images->one, images->aliens, images->bullets, images->alien_bullets);
		}
		if(event->keyval == GDK_KEY_Escape)
		{
			exit(1);
		}
	}
	return TRUE;
}

gboolean button_exit_press_event(GtkWidget *widget, GdkEventButton *event)
{
	if(event->type == GDK_BUTTON_PRESS)
		exit(1);
	return TRUE;
}	

gboolean button_start_press_event(GtkWidget *widget, GdkEventButton *event, MenuHolder *menu)
{
	gtk_widget_destroy(menu->menu_box);
	Assets *images = malloc(sizeof(Assets));
	images->aliens = NULL;
	images->alien_direction = HORIZONTAL_RIGHT;
	images->bullets = NULL;
	images->alien_bullets = NULL;
	InitGame(menu->main_window, images);
	return TRUE;
}
gboolean button_load_press_event(GtkWidget *widget, GdkEventButton *event, MenuHolder *menu)
{
	Assets *images = malloc(sizeof(Assets));
	images->aliens = NULL;
	images->alien_direction = HORIZONTAL_RIGHT;
	images->bullets = NULL;
	images->alien_bullets = NULL;
	Load(images);
	gtk_widget_destroy(menu->menu_box);
	InitGame(menu->main_window, images);
	return TRUE;
}

gboolean button_load_text_press_event(GtkWidget *widget, GdkEventButton *event, MenuHolder *menu)
{
	Assets *images = malloc(sizeof(Assets));
	images->aliens = NULL;
	images->alien_direction = HORIZONTAL_RIGHT;
	images->bullets = NULL;
	images->alien_bullets = NULL;
	if(LoadText(images) == -1)
		return TRUE;
	gtk_widget_destroy(menu->menu_box);
	InitGame(menu->main_window, images);
	return TRUE;
}

gboolean exit_cleanly()
{
	//fprintf(stderr,"exiting");
	exit(1);
	return TRUE;
}

int main()
{
	srand(time(NULL));
	chances = 3;
	punkty_gracza = 0;
	prev_time.tv_sec = 0;
	prev_time.tv_nsec = 0;
	player_bullet_timer = 700;

	GtkWidget *main_window;
	GtkWidget *button_start, *button_exit, *button_load,*button_load_text;
	GtkWidget *menu_box;
	MenuHolder *menu = malloc(sizeof(MenuHolder));
	gtk_init(0,NULL);

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	button_start = gtk_button_new_with_label("START");
	button_exit = gtk_button_new_with_label("EXIT");
	button_load = gtk_button_new_with_label("LOAD");
	button_load_text = gtk_button_new_with_label("LOAD_TEXT");

	menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MENU_DISTANCE);

	gtk_window_set_resizable((GtkWindow*)main_window, FALSE);
	gtk_widget_set_events(main_window, GDK_POINTER_MOTION_MASK);

	menu->main_window = main_window;
	menu->menu_box = menu_box;

	gtk_widget_set_size_request(main_window, WIDTH, HEIGHT);

	g_signal_connect(button_exit, "button_press_event", G_CALLBACK(button_exit_press_event), NULL);
	g_signal_connect(button_start, "button_press_event", G_CALLBACK(button_start_press_event), menu);
	g_signal_connect(button_load, "button_press_event", G_CALLBACK(button_load_press_event), menu);
	g_signal_connect(button_load_text, "button_press_event", G_CALLBACK(button_load_text_press_event), menu);
	g_signal_connect(main_window,"delete_event",G_CALLBACK(exit_cleanly),NULL);
	printf("%p\n", menu_box);
	gtk_container_add(GTK_CONTAINER(menu_box), button_start);
	gtk_container_add(GTK_CONTAINER(menu_box), button_load);
	gtk_container_add(GTK_CONTAINER(menu_box), button_load_text);
	gtk_container_add(GTK_CONTAINER(menu_box), button_exit);
	gtk_container_add(GTK_CONTAINER(main_window), menu_box);
	gtk_widget_show_all(main_window);


	gtk_main();
	return 0;
}