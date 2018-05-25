#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/logicprocess.h"
#include "../include/logicjuego.h"

#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_WHITE   "\e[37m"
#define ANSI_BGR_COLOR_WHITE    "\x1b[107m"
#define ANSI_BGR_COLOR_BLACK   "\x1b[40m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD "\e[1;37m"
#define ANSI_UNDERLINE "\e[4;37m"



void leer_matriz(int columna, int filas, FILE *fp, char game[][columna+1]){ 
	char buff[255];
	char aux[columna+1];
	for(int i = 0; i < filas; i++){
		int cont =0;
		fgets(buff, 255, (FILE*)fp);
		for(int j = 0; j< (strlen (buff)-1); j++){
			if(buff[j] != ' '){
				aux[cont] = buff[j];
				cont++;
			}
		}
		strcpy(game[i], aux);

	}

}

void actualizar_matriz(int columna, char game[][columna+1], char game_aux[][columna+1], int fila){
	for(int i = 0; i < fila; i++){
		strcpy(game[i], game_aux[i]);
	}

}

void mostrar_matriz(int columna, char game[][columna+1], int fila){
	for(int i = 0; i < fila; i++){
		for(int j= 0; j < columna; j++){
			if(game[i][j] =='0')
				printf(ANSI_BGR_COLOR_WHITE ANSI_COLOR_WHITE     "%*c"     ANSI_COLOR_RESET ,2, '*');
			else
				printf(ANSI_BGR_COLOR_BLACK ANSI_COLOR_BLACK    "%*c"     ANSI_COLOR_RESET ,2, game[i][j]);
		}
		printf("\n");
	}
}



int main(int argc, char const *argv[]){

	if(argc != 5){
		printf("Se esperan 5 argumentos para la ejecucion: juego <n_procesos>  <n_generaciones> <n_visualizacion> <archivo>\n");
		printf("Ejemplo: ./juego 5 10 1 ./mundo.txt\n");
	}
	else{

		FILE *fp;
   		char buff[255];
   		fp = fopen(argv[4], "r");
   		if(fp == NULL){
			printf("Error al abrir el archivo\n");
			return 0;
		}

   		fgets(buff, 255, (FILE*)fp);
		int filas = atoi(buff);

		fgets(buff, 255, (FILE*)fp);
		int columnas = atoi(buff);

		char game[filas][columnas+1];

		leer_matriz(columnas, filas, fp, game);
		fclose(fp);

		int n = atoi(argv[1]);

		if(n > filas){
			printf("El número de procesos no puede ser mayor al número de filas.\n");
			return 0;
		}

		int turno[n];	

		int num_gen = atoi(argv[2]);

		int num_visualizacion = atoi(argv[3]);
	
		int m = filas/n;

		int i = 0;
		int p = 1;

		char game_aux[filas][columnas+1];

		struct pipe arrPipes_abajo[(n-1)];

		struct pipe arrPipes_arriba[(n-1)];

		struct pipe arrPipes_hijo_padre[n];

		struct pipe arrPipes_padre_hijo[n];

		crear_array_turnos(turno, n, m);

		crear_pipes_hijos(arrPipes_abajo, n);
		crear_pipes_hijos(arrPipes_arriba, n);
		crear_pipes_hijos_padre(arrPipes_hijo_padre, n);
		crear_pipes_hijos_padre(arrPipes_padre_hijo, n);

		crear_procesos(columnas, filas, arrPipes_padre_hijo, arrPipes_hijo_padre, arrPipes_abajo, arrPipes_arriba, n, m, turno, num_gen);

		system("clear");
		printf(ANSI_BOLD ANSI_UNDERLINE "CONWAY'S JUEGO DE LA VIDA\n" ANSI_COLOR_RESET);
		printf("\n");
		printf(	"MUNDO GENERADO DE ARCHIVO:\n");
		printf("\n");

		mostrar_matriz(columnas, game, filas);
		printf("\n");
		printf("Datos Iniciales: \n");
		printf("\n");
		printf("- Número de Procesos: ");
		printf(ANSI_BOLD "%d\n" ANSI_COLOR_RESET ,n);
		printf("- Número de Genereciones: ");
		printf(ANSI_BOLD "%d\n" ANSI_COLOR_RESET ,num_gen);
		printf("- El mundo se actualiza cada ");
		printf(ANSI_BOLD "%d" ANSI_COLOR_RESET ,num_visualizacion);
		printf(" Genereciones\n");
		printf("\n");
		printf("Presione cualquier tecla para iniciar\n");

		getchar();


		while(p <= num_gen){
			enviar_informacion_hijos(filas, columnas, arrPipes_padre_hijo, n, m, turno, game);

			crear_matriz_aux(columnas, filas, arrPipes_hijo_padre, n, m, game_aux, turno);
		
			printf("\n");
		
			actualizar_matriz(columnas, game, game_aux, filas);
			if((p % num_visualizacion) == 0){
				system("clear");
				printf(ANSI_BOLD ANSI_UNDERLINE "CONWAY'S JUEGO DE LA VIDA\n" ANSI_COLOR_RESET);
				printf("\n");
				mostrar_matriz(columnas, game, filas);
				printf("\n");
				printf("Generación número: ");
				printf(ANSI_BOLD "%d\n" ANSI_COLOR_RESET ,p);
				sleep(1);		
			}
			p++;
			
		}

		for(int i = 0; i < n; i++) {
			wait(NULL);
		}	

		return(0);
	}

}