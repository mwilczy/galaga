#include "galaga.h"



void save_binary(SpaceShip player, Ball *aliens, Ball *player_bullets, Ball *alien_bullets)
{
	char *data_stream = malloc(10000);
	int i = 0;
	FILE *plik;
	plik = fopen("save.bin", "wb");
	i = 0;
	memcpy(data_stream + i,&(player.x), sizeof(int));
	i+=sizeof(int);
	memcpy(data_stream + i, &(player.y), sizeof(int));
	i+=sizeof(int);

	go_back(&aliens);
	go_back(&player_bullets);
	go_back(&alien_bullets);
	while(aliens != NULL)
	{
		memcpy(data_stream + i, &(aliens->x), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(aliens->y), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(aliens->velocity), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(aliens->direction), sizeof(int));
		i+=sizeof(int);

		aliens = aliens->next;
	}

	memcpy(data_stream + i, "END", 3);
	i+=3;

	while(player_bullets != NULL)
	{
		memcpy(data_stream + i, &(player_bullets->x), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(player_bullets->y), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(player_bullets->velocity), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(player_bullets->direction), sizeof(int));
		i+=sizeof(int);

		player_bullets = player_bullets->next;
	}

	memcpy(data_stream + i, "END",3);
	i+=3;

	while(alien_bullets != NULL)
	{
		memcpy(data_stream + i, &(alien_bullets->x), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(alien_bullets->y), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(alien_bullets->velocity), sizeof(int));
		i+=sizeof(int);
		memcpy(data_stream + i, &(alien_bullets->direction), sizeof(int));
		i+=sizeof(int);
		alien_bullets = alien_bullets->next;
	}
	memcpy(data_stream + i, "END", 3);
	i+=3;

	memcpy(data_stream + i, &punkty_gracza,sizeof(int));
	i+=sizeof(int);

	memcpy(data_stream + i, &chances, sizeof(int));
	i+=sizeof(int);


	fwrite(data_stream, sizeof(char),i, plik);

	fclose(plik);
}

void Load(Assets *images)
{
	int i = 0;
	int x,y,velocity,direction;
	char *data_stream = malloc(10000);
	FILE *plik;
	plik = fopen("save.bin", "rb");

	int bytes = fread(data_stream, 1, 10000, plik);
	printf("bytes read %d\n", bytes);
	printf("sizeof pointer %lu\n", sizeof(Ball*));
	memcpy(&(images->one), data_stream, sizeof(SpaceShip));
	int *tmp;
	i += sizeof(SpaceShip);
	// aliens
	while(!(data_stream[i] == 'E' && data_stream[i+1] =='N' && data_stream[i+2] == 'D'))
	{
		tmp = (int*)(data_stream + i);
		x = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		y = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		velocity = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		direction = *tmp;
		i+=sizeof(int);
		Insert(&(images->aliens), x,y,velocity,direction);
	}
	i+=3;

	while(!(data_stream[i] == 'E' && data_stream[i+1] =='N' && data_stream[i+2] == 'D'))
	{
		tmp = (int*)(data_stream + i);
		x = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		y = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		velocity = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		direction = *tmp;
		i+=sizeof(int);
		Insert(&(images->bullets), x,y,velocity,direction);
	}
	i+=3;

	while(!(data_stream[i] == 'E' && data_stream[i+1] =='N' && data_stream[i+2] == 'D'))
	{
		tmp = (int*)(data_stream + i);
		x = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		y = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		velocity = *tmp;
		i+=sizeof(int);
		tmp = (int*)(data_stream + i);
		direction = *tmp;
		i+=sizeof(int);
		Insert(&(images->alien_bullets), x,y,velocity,direction);
	}
	i+=3;
	memcpy(&punkty_gracza, data_stream + i, sizeof(int));
	i+=sizeof(int);
	memcpy(&chances, data_stream + i, sizeof(int));
	i+=sizeof(int);
	//free(data_stream);
	printf("bytes loaded: %d\n", i);
	fclose(plik);

	go_back(&(images->aliens));
	go_back(&(images->bullets));
	go_back(&(images->alien_bullets));
}

void save_text(SpaceShip player, Ball *aliens, Ball *player_bullets, Ball *alien_bullets)
{
	FILE *plik;
	plik = fopen("save.txt", "w");
	fprintf(plik, "%d %d\n", player.x,player.y);

	go_back(&aliens);
	go_back(&player_bullets);
	go_back(&alien_bullets);

	while(aliens != NULL)
	{
		fprintf(plik,"%d %d %d %d\n", aliens->x,aliens->y,aliens->velocity,aliens->direction);
		aliens = aliens->next;
	}
	fprintf(plik,"X\n");
	while(player_bullets != NULL)
	{
		fprintf(plik,"%d %d %d %d\n", player_bullets->x,player_bullets->y,player_bullets->velocity,player_bullets->direction);
		player_bullets = player_bullets->next;
	}
	fprintf(plik,"X\n");
	while(alien_bullets != NULL)
	{
		fprintf(plik,"%d %d %d %d\n", alien_bullets->x,alien_bullets->y,alien_bullets->velocity,alien_bullets->direction);
		alien_bullets = alien_bullets->next;
	}
	fprintf(plik,"X\n");

	fprintf(plik,"%d %d\n", punkty_gracza, chances);
	fclose(plik);
}

int LoadText(Assets *images)
{
	FILE *plik = fopen("save.txt", "r");
	if(plik == NULL)
		return -1;
	int i = 0;
	int x,y,velocity,direction;

	if(fscanf(plik,"%d %d\n", &(images->one.x), &(images->one.y)) != 2)
		return -1;
	
	while(1)
	{
		if(fgetc(plik) == 'X')
		{	
			fscanf(plik,"\n");
			break;
		}
		fseek(plik, -1, SEEK_CUR);
		// get aliens
		if(fscanf(plik,"%d %d %d %d\n", &x,&y,&velocity,&direction) != 4)
			return -1;
		//if(x > WIDTH || x < 0 || y > HEIGHT || y < 0)
		//	return -1;

		Insert(&(images->aliens), x,y,velocity,direction);
		printf("%d %d %d %d\n", x,y,velocity,direction);
	}

	while(1)
	{
		if(fgetc(plik) == 'X')
		{
			fscanf(plik,"\n");
			break;
		}
		fseek(plik, -1, SEEK_CUR);
		// get player bullets
		if(fscanf(plik,"%d %d %d %d\n", &x,&y,&velocity,&direction) != 4)
			return -1;
		//if(x > WIDTH || x < 0 || y > HEIGHT || y < 0)
		//	return -1;
		Insert(&(images->bullets), x,y,velocity,direction);
		printf("%d %d %d %d\n", x,y,velocity,direction);
	}
	printf("savin\n");
	while(1)
	{
		// get alien bullets
		if(fgetc(plik) == 'X')
		{
			fscanf(plik,"\n");
			break;
		}
		fseek(plik, -1, SEEK_CUR);

		if(fscanf(plik,"%d %d %d %d\n", &x,&y,&velocity,&direction) != 4)
			return -1;
		Insert(&(images->alien_bullets), x,y,velocity,direction);
		printf("%d %d %d %d\n", x,y,velocity,direction);
	}

	if(fscanf(plik,"%d %d", &punkty_gracza, &chances) != 2)
		return -1;
	fclose(plik);
	return 1;
}
