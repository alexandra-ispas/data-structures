#include "utils.h"
#include <limits.h>
#include <math.h>
#include <time.h>

void* (* Abilities[4])(void* x) = {RotateMatrix, DecodeString, InvertArray, KillPlayer};

char *fromEnumtoString(PlayerRoles playerRole)
{
	char *str = calloc(MAX_LEN_STR_ATR, sizeof(char));
	switch(playerRole)
	{
		case Rotator:
			str = strcpy(str, "Rotator");
			break;
		case Decoder:
			str = strcpy(str, "Decoder");
			break;
		case Invertor:
			str = strcpy(str, "Invertor");
			break;
		case Impostor:
			str = strcpy(str, "Impostor");
			break;
	}
	return str;
}

void *RotateMatrix(void *input)
{
	int *nr = input;
	int n = *nr;
	
	//output string
	char *s = (char*) calloc(MAX_LEN_STR_OUT, sizeof(char));
	if(s == NULL){
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	//index for the new string
	int idx = 0;

	for(int i = 1; i <= n; i++){

		for(int j = n; j >= 1; j--){

			char aux[10] = "";
			sprintf(aux, "%d%d", j, i);
			memcpy(s + idx, aux, strlen(aux));

			idx += strlen(aux);
			if(j != 1)
				s[idx++] = ' ';
		}
		if(i != n)
			s[idx++] = '\n';
	}
	return (void *)s;

}

void *DecodeString(void *input)
{
	char sep[2] = "_";
	char *p;
	char *str = input;

	int sum = 0;

	p = strtok(str, sep);
	while(p != NULL){
		sum += atoi(p);
		p = strtok(NULL, sep);
	}

	char *string = (char*) calloc(1000, sizeof(char));

	sprintf(string, "%d", sum);

	return(void*)string;
}

void *InvertArray(void *input)
{
	int *array = input;

	//get length of the array
	int length = array[0];

	//if the array has an even number of elements 
	if(length % 2 == 0){
		for(int i = 1; i <= length; i += 2){

			//interchange elements 2 by 2
			int aux = array[i];
			array[i] = array[i + 1];
			array[i + 1] = aux;
		}
	} else {
		for(int i = 1; i <= length / 2; i++){

			//reverse array
			int aux = array[i];
			array[i] = array[length - i + 1];
			array[length - i + 1] = aux;
		}
	}

	//output string
	char *output = (char*)calloc(MAX_LEN_STR_OUT, sizeof(char));
	if(output == NULL){
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	//create the output string
	for(int i = 1; i <= length; i++){
		char aux[10] = "";
		//convert array element to string
		sprintf(aux, "%d", array[i]);
		//concatenate value to output string
		strcat(output, aux);
		//add space in output
		strcat(output, " ");
	}

	return (void*)output;
}

Player *allocPlayer()
{
	Player *player = (Player *)malloc(1 * sizeof(Player));
	if(player == NULL){
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	player->name = (char*)malloc(MAX_LEN_STR_ATR * sizeof(char));
	if(player->name == NULL){
		free(player);
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	player->color = (char *)malloc(MAX_LEN_STR_ATR * sizeof(char));
	if(player->color == NULL){
		free(player);
		free(player->name);
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}
	player->hat = (char *)malloc(MAX_LEN_STR_ATR * sizeof(char));
	if(player->hat == NULL){
		free(player);
		free(player->name);
		free(player->color);
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	//intialize values
	player->alive = 1;
	player->indexOfLocation = 0;
	player->numberOfLocations = 0;
	player->locations = NULL;

	return player;
}

Game *allocGame()
{
	Game *game = (Game*)malloc(1 * sizeof(Game));
	if(game == NULL){
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	game->name = (char*)malloc(MAX_LEN_STR_ATR * sizeof(char));
	if(game->name == NULL){
		free(game);
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	//initialize values
	game->killRange = 0;
	game->numberOfCrewmates = 0;
	game->crewmates = NULL;
	game->impostor = NULL;

	return game;
}

Player *ReadPlayer(FILE *inputFile)
{
	
	Player *player = allocPlayer();
	fscanf(inputFile, "%s", player->name);
	fscanf(inputFile, "%s", player->color);
	fscanf(inputFile, "%s", player->hat);
	fscanf(inputFile, "%d", &(player->numberOfLocations));

	//allocate memory for array of locations
	player->locations = (Location*)malloc(player->numberOfLocations
		* sizeof(Location));

	for(int j = 0; j < player->numberOfLocations; j++){

		char aux[10] = "", sep[] = ",()", *x, *y;

		fscanf(inputFile, "%s", aux);
		x = strtok(aux, sep);
		y = strtok(NULL, sep);

		player->locations[j].x = atoi(x);
		player->locations[j].y = atoi(y);
	}

	//add the role of the player
	char role[100];
	fscanf(inputFile, "%s", role);

	if(strcmp(role, "Rotator") == 0){
		player->playerRole = Rotator;
		player->ability = Abilities[0];
	}
	if(strcmp(role, "Decoder") == 0){
		player->playerRole = Decoder;
		player->ability = Abilities[1];
	}
	if(strcmp(role, "Invertor") == 0){	
		player->playerRole = Invertor;
		player->ability = Abilities[2];
	}
	if(strcmp(role, "Impostor") == 0){	
		player->playerRole = Impostor;
		player->ability = Abilities[3];
	}

	return player;
}

Game *ReadGame(FILE *inputFile)
{

	Game *game = allocGame();
	fscanf(inputFile, "%s", game->name);
	fscanf(inputFile, "%d", &(game->killRange));
	fscanf(inputFile, "%d", &(game->numberOfCrewmates));

	//allocate memory for array of crewmates
	game->crewmates = (Player**)malloc(game->numberOfCrewmates 
	* sizeof(Player*));
	if(game->crewmates == NULL){
		printf("Allocation error\n");
		free(game);
		exit(EXIT_FAILURE);
	}

	//read data for all the crewmates
	for(int i = 0; i < game->numberOfCrewmates; i++){
		game->crewmates[i] = ReadPlayer(inputFile);
	}

	//read impostor data
	game->impostor = ReadPlayer(inputFile);

	return game;

}

void WritePlayer(Player *player, FILE *outputFile)
{
	//get the role of the player
	char* str = fromEnumtoString(player->playerRole);

	fprintf(outputFile, 
	"Player %s with color %s, hat %s and role %s has entered the game.\n", 
		player->name, player->color, player->hat, str); 

	fprintf(outputFile, "Player's locations: ");

	//get all the locations for a player
	for(int i = 0; i < player->numberOfLocations; i++){

		fprintf(outputFile, "(%d,%d) ", 
		player->locations[i].x, player->locations[i].y);
	}
	fprintf(outputFile, "\n");

	free(str);
}

void WriteGame(Game *game, FILE *outputFile)
{
	fprintf(outputFile, "Game %s has just started!\n", game->name);
	fprintf(outputFile, "\tGame options:\n");
	fprintf(outputFile, "Kill Range: %d\n", game->killRange);
	fprintf(outputFile, "Number of crewmates: %d\n", game->numberOfCrewmates);
	fprintf(outputFile, "\n");
	fprintf(outputFile, "\tCrewmates:\n");

	//get data for all the crewmates
	for(int i = 0; i < game->numberOfCrewmates; i++){
		WritePlayer(game->crewmates[i], outputFile);
	}
	fprintf(outputFile, "\n");
	fprintf(outputFile, "\tImpostor:\n");
	//get data for the impostor
	WritePlayer(game->impostor, outputFile);
}

void *KillPlayer(void *input)
{	
	Game *game = input;

	int min_distance = INT_MAX;
	Player *closest_player = NULL;

	//get the location of the impostor
	Location impostor_location = 
	game->impostor->locations[game->impostor->indexOfLocation];
	

	for(int i = 0; i < game->numberOfCrewmates; i++){
		//get the location of the crewmate
		Location location = 
		game->crewmates[i]->locations[game->crewmates[i]->indexOfLocation];

		//compute distance
		int distance = abs(impostor_location.x - location.x) + 
		abs(impostor_location.y - location.y);

		//get the minimum distance
		if(distance <= min_distance && game->crewmates[i]->alive == 1){
			closest_player = game->crewmates[i];
			min_distance = distance;
		}
	}

	//output string
	char *s = (char *) malloc(100 * sizeof(char));
	if(s == NULL){
		printf("Allocation error\n");
		exit(EXIT_FAILURE);
	}

	//check if the closest player is within killRange
	if(min_distance > game->killRange){
		sprintf(s, "Impostor %s couldn't kill anybody.", game->impostor->name);
	} else {
		closest_player->alive = 0;
		sprintf(s, "Impostor %s has just killed crewmate %s from distance %d.", 
		game->impostor->name, closest_player->name, min_distance);
	}

	return(void*)s;
}

void CalcuateNextCycleOfGame(Game *game, FILE *outputFile, void **inputMatrix)
{
	int i;

	for(i = 0; i < game->numberOfCrewmates; i++){

		//if crewmate is not alive
		if(game->crewmates[i]->alive == 0){
			fprintf(outputFile, "Crewmate %s is dead.\n", 
			game->crewmates[i]->name);
		} else {

			//get to the next location
			game->crewmates[i]->indexOfLocation++;

			//restart the location cycle
			if(game->crewmates[i]->indexOfLocation ==
			 	game->crewmates[i]->numberOfLocations){

				game->crewmates[i]->indexOfLocation = 0;
			}

			//print the new location
			Location location = 
			game->crewmates[i]->locations[game->crewmates[i]->indexOfLocation];

			fprintf(outputFile, "Crewmate %s went to location (%d,%d).\n", 
				game->crewmates[i]->name, location.x, location.y);

			//use the ability of the player
			fprintf(outputFile, "Crewmate %s's output:\n", 
			game->crewmates[i]->name);

			clock_t start = clock();

			char *s = (char*)game->crewmates[i]->ability(inputMatrix[i]);
			
			fprintf(outputFile, "%s\n", s);

			free(s);
		}
	}
	

	//move impostor to the new location
	game->impostor->indexOfLocation++;
	if(game->impostor->indexOfLocation ==
			 	game->impostor->numberOfLocations){

		game->impostor->indexOfLocation = 0;
	}
	Location location = 
	game->impostor->locations[game->impostor->indexOfLocation];

	fprintf(outputFile, "Impostor %s went to location (%d,%d).\n", 
		game->impostor->name, location.x, location.y);

	fprintf(outputFile, "Impostor %s's output:\n", game->impostor->name);

	//use the impostor's ability
	void *s = game->impostor->ability(inputMatrix[i]);

	fprintf(outputFile, "%s\n", (char*)s);

	free(s);
}

void FreePlayer(Player *player)
{
	free(player->name);
	free(player->color);
	free(player->hat);
	free(player->locations);
	free(player);
}

void FreeGame(Game *game)
{
	free(game->name);
	//free each crewmate
	for(int i = 0; i < game->numberOfCrewmates; i++){
		FreePlayer(game->crewmates[i]);
	}
	//free the array of crewmates
	free(game->crewmates);
	FreePlayer(game->impostor);
	free(game);
}
