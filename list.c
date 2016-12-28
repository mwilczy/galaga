#include "galaga.h"

Ball *Remove(Ball **pointer)
{
	if((*pointer) == NULL)
		return NULL;
	if((*pointer)->prev == NULL && (*pointer)->next == NULL)
	{
		free(*pointer);
		return NULL;
	}
	Ball *t;
	if((*pointer)->next == NULL)
	{
		t = (*pointer)->prev;
		(*pointer)->prev->next = NULL;
		free(*pointer);
		(*pointer) = t;
		return t;
	}
	if((*pointer)->prev == NULL)
	{
		t = (*pointer)->next;
		(*pointer)->next->prev = NULL;
		free(*pointer);
		(*pointer) = t;
		return t;
	}


	t = (*pointer)->next;

	(*pointer)->prev->next = (*pointer)->next;

	(*pointer)->next->prev = (*pointer)->prev;
	
	free(*pointer);

	*pointer = t;
	return t;
}


Ball *Insert(Ball **pointer, int x,int y, int velocity,int direction)
{
	if(*pointer == NULL)
	{
		*pointer = malloc(sizeof(Ball));
		(*pointer)->prev = NULL;
		(*pointer)->next = NULL;
		(*pointer)->x = x;
		(*pointer)->y = y;
		(*pointer)->velocity = velocity;
		(*pointer)->direction = direction;
		return *pointer;
	}

	Ball *t = malloc(sizeof(Ball));
	t->x = x;
	t->y = y;
	t->velocity = velocity;
	t->direction = direction;
	if((*pointer)->prev == NULL)
	{
		(*pointer)->prev = t;
		t->prev = NULL;
		t->next = *pointer;
		*pointer = t;
		return t;
	}

	if((*pointer)->next == NULL)
	{
		(*pointer)->next = t;
		t->prev = *pointer;
		t->next = NULL;
		*pointer = t;
		return t;
	}

	Ball *tmp = (*pointer)->next;
	(*pointer)->next = t;
	tmp->prev = t;
	t->prev = *pointer;
	t->next = tmp;
	*pointer = t;
	return t;
}


void Movement(Ball *pointer)
{
	if(pointer->direction == BULLET_UP)
		pointer->y -= pointer->velocity;
	else if(pointer->direction == BULLET_DOWN)
		pointer->y += pointer->velocity;
	else if(pointer->direction ==HORIZONTAL_LEFT)
		pointer->x -= pointer->velocity;
	else if(pointer->direction == HORIZONTAL_RIGHT)
		pointer->x += pointer->velocity;
}

void Traverse(Ball *pointer, cairo_t *cr, GdkPixbuf *object)
{
	if(pointer == NULL)
		return;
	Ball *tmp = pointer;
	while(pointer != NULL)
	{
		gdk_cairo_set_source_pixbuf(cr,object, pointer->x,pointer->y);
		cairo_paint(cr);
		Movement(pointer);
		pointer = pointer->next;
	}

	pointer = tmp->prev;

	while(pointer != NULL)
	{
		gdk_cairo_set_source_pixbuf(cr,object, pointer->x,pointer->y);
		cairo_paint(cr);
		Movement(pointer);
		pointer = pointer->prev;
	}

}

void GenerateAliens(Ball **aliens,int width, int height)
{
	int i;
	for(i=0;i<10;i++)
	{
		Insert(&(*aliens),i*(width + 20),height + 10,1,HORIZONTAL_RIGHT);
	}
	for(i=0;i<10;i++)
	{
		Insert(&(*aliens),i*(width + 20),(height + 10) *2,1,HORIZONTAL_RIGHT);
	}
	for(i=0;i<10;i++)
	{
		Insert(&(*aliens),i*(width + 20),(height+10) * 3,1,HORIZONTAL_RIGHT);
	}
}


void ChangeAlienDirection(Ball *pointer)
{
	if(pointer == NULL)
		return;
	Ball *tmp = pointer;
	while(pointer != NULL)
	{
		if(pointer->direction == HORIZONTAL_RIGHT)
			pointer->direction = HORIZONTAL_LEFT;
		else
			pointer->direction = HORIZONTAL_RIGHT;
		pointer->y += 30;
		pointer = pointer->next;
	}

	pointer = tmp->prev;

	while(pointer != NULL)
	{
		if(pointer->direction == HORIZONTAL_RIGHT)
			pointer->direction = HORIZONTAL_LEFT;
		else
			pointer->direction = HORIZONTAL_RIGHT;
		pointer->y += 30;
		pointer = pointer->prev;
	}
}

int DetectBorder(Ball *pointer)
{
	if(pointer == NULL)
		return 0;
	Ball *tmp = pointer;
	while(pointer != NULL)
	{
		if(pointer->x > WIDTH - 45 || pointer->x < 0)
			return -1;
		pointer = pointer->next;
	}
	pointer = tmp->prev;

	while(pointer != NULL)
	{
		if(pointer->x > WIDTH - 45 || pointer->x < 0)
			return -1;
		pointer = pointer->prev;
	}
	return 0;
}

void go_back(Ball **pointer)
{
	if(*pointer == NULL)
		return;
	while((*pointer)->prev != NULL)
	{
		(*pointer) = (*pointer)->prev;
	}
}

int DetectCollision(Ball *aliens, Ball *pociski, Ball **wynik1, Ball **wynik2)
{
	if(aliens == NULL)
		return 0;
	if(pociski == NULL)
		return 0;
	go_back(&aliens);
	go_back(&pociski);
	while(aliens != NULL)
	{
		while(pociski!=NULL)
		{
			if((abs(aliens->x - pociski->x) < 37) && (abs(aliens->y - pociski->y) < 30))
			{
				punkty_gracza++;
				(*wynik1) = Remove(&aliens);
				(*wynik2) = Remove(&pociski);
				printf("collision %d\n",punkty_gracza);
				if(punkty_gracza % 10 == 0)
					chances++;
				return 0;
			}
			else if(pociski->y > HEIGHT + 30 || pociski->y < -30)
			{
				(*wynik2) = Remove(&pociski);
				return 0;
			}
			pociski = pociski->next;
		}
		pociski = *wynik2;
		go_back(&pociski);
		aliens = aliens->next;
	}
	return 0;
}

void SpawnMore(Ball **aliens,int width, int height)
{
	if(*aliens == NULL)
		GenerateAliens(aliens,width,height);

}


void Przegrales()
{
	GtkWidget *dialog;
	GtkDialogFlags flag = GTK_DIALOG_USE_HEADER_BAR;
	dialog = gtk_message_dialog_new(NULL, flag, GTK_MESSAGE_ERROR, \
		GTK_BUTTONS_CLOSE, "Przegrales");
	gtk_widget_set_size_request(dialog,150,150);
	gtk_dialog_run(GTK_DIALOG(dialog));
}

void DetectLose(Ball *aliens)
{
	go_back(&aliens);


	while(aliens != NULL)
	{
		if(aliens->y > HEIGHT - 100)
		{
			Przegrales();
			exit(-1);
		}

		aliens = aliens->next;
	}
}

int DetectCollisionPlayer(int x,int y, Ball *pociski, Ball **wynik2)
{
	if(pociski == NULL)
		return 0;
	Ball *tmp2 = pociski;
	while(pociski!=NULL)
	{
		if((abs(x - pociski->x) < 35) && (abs(y - pociski->y) < 15))
		{
			(*wynik2) = Remove(&pociski);
			printf("gracz trafiony\n");

			if(chances == 0)
			{
				Przegrales();
				exit(0);
			}
			if(pociski->y > HEIGHT + 30 || pociski->y < -30)
			{
				(*wynik2) = Remove(&pociski);
			}

			chances--;
		}
		pociski = pociski->next;
	}
	pociski = tmp2->prev;
	
	while(pociski!=NULL)
	{
		if((abs(x - pociski->x) < 50) && (abs(y - pociski->y) < 10))
		{
			printf("gracz trafiony\n");
			(*wynik2) = Remove(&pociski);

			if(chances == 0)
			{
				Przegrales();
				exit(0);
			}
			chances--;
		}
		pociski = pociski->prev;
	}
	return 0;
}


void ascii(int liczba, char *buffer)
{
	int i=0;
	int t[30];
	int j;
	while(liczba > 0)
	{
		t[i] = (liczba % 10) + 48;
		//printf("%d\n", t[i]);
		liczba = liczba/10;
		i++;
	}
	buffer[i] = '\0';
	i--;
	int k =i;
	for(j=0;j<=k;j++)
	{	
		buffer[j] = t[i];
		i--;
	}

}
