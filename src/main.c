#define _POSIX_C_SOURCE 199309L //Macro para la funcion nanosleep
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include "../include/logicprocess.h"//Se incluye el archivo .h de la logica de procesos
#include "../include/logicjuego.h"//Se inlcuye el archivo .h de la logica del juego

//Definiciones para el color de fondo y letra del juego
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_WHITE   "\e[37m"
#define ANSI_BGR_COLOR_WHITE    "\x1b[107m"
#define ANSI_BGR_COLOR_BLACK   "\x1b[40m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD "\e[1;37m"
#define ANSI_UNDERLINE "\e[4;37m"

struct timespec ts = {0, 500000000L}; //Estructura necesaria para la funcion nanosleep

void leer_matriz(int columna, int filas, FILE *fp, char game[][columna+1]){ //Funcion que lee de archivo el mundo a generar
	char buff[255];
	char aux[columna+1];
	for(int i = 0; i < filas; i++){ //Se recorren todas las filas luego de la declaracion de numero de filas y columnas
		int cont =0;
		fgets(buff, 255, (FILE*)fp);//Se almacena cada fila en la variable buff
		for(int j = 0; j< (strlen (buff)-1); j++){//Se verifica que el dato no sea un espacio en blaco
			if(buff[j] != ' '){
				aux[cont] = buff[j];
				cont++;
			}
		}
		strcpy(game[i], aux);

	}

}

void actualizar_matriz(int columna, char game[][columna+1], char game_aux[][columna+1], int fila){//Actualiza la matriz del juego con los datps de la matrz aux
	for(int i = 0; i < fila; i++){
		strcpy(game[i], game_aux[i]);
	}

}

void mostrar_matriz(int columna, char game[][columna+1], int fila){ //Imprime en pantalla el mundo almacenado en la matriz con los respectivos colores para celdas vivas y muertas
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

	if(argc != 5){//Se verifica que se reciban todos los parametros necesarios para la ejecucion
		printf("Se esperan 5 argumentos para la ejecucion: juego <n_procesos>  <n_generaciones> <n_visualizacion> <archivo>\n");
		printf("Ejemplo: ./juego 5 10 1 ./mundo.txt\n");
	}
	else{

		FILE *fp;
   		char buff[255];
   		fp = fopen(argv[4], "r");
   		if(fp == NULL){//Si exite error abriendo el archivo se muestra mensaje y se termina la operacion
			printf("Error al abrir el archivo\n");
			return 0;
		}

   		fgets(buff, 255, (FILE*)fp);//Almacena el numero de filas indicado en archivo en variable filas
		int filas = atoi(buff);

		fgets(buff, 255, (FILE*)fp);//Almacena el numero de columnas indicado en archivo en variable filas
		int columnas = atoi(buff);

		char game[filas][columnas+1];//Se indica el tamano de la matriz del juego

		leer_matriz(columnas, filas, fp, game);//Se almacenan los datos del mundo indicados en archivo en la matriz game
		fclose(fp);

		int n = atoi(argv[1]);//Almacena el numero de procesos indicados en archivo en la variable n

		if(n > filas){
			printf("El número de procesos no puede ser mayor al número de filas.\n");
			return 0;
		}

		int turno[n];//Se crea un array para almacenar la coordenada de la primera fila que le corresponde a cada proceso

		int num_gen = atoi(argv[2]);//Almacena el numero de generaciones en la variable num_gen

		int num_visualizacion = atoi(argv[3]);//Almacena el numero de visualizaciones en la variable num_visualizacion
	
		int m = filas/n;

		int p = 1;//Contador usado para el ciclo principal del juego

		char game_aux[filas][columnas+1];//Se crea matriz aux donde se escribira la informacion enviada por cada proceso luego de procesar su parte del mundo correspondiente

		struct pipe arrPipes_abajo[(n-1)];//Array que almacena los pipes que van de arriba hacia bajo (se resta uno ya que el ultimo proceso no tiene pipe hacia abajo)

		struct pipe arrPipes_arriba[(n-1)];//Array que almacena los pipes que van de abajo hacia arriba (se resta uno ya que el primer proceso no tiene pipe hacia arriba)

		struct pipe arrPipes_hijo_padre[n];//Array que almacena los pipes que van del hijo al padre

		struct pipe arrPipes_padre_hijo[n];//Array que almacena los pipes que van del padre al hijo

		crear_array_turnos(turno, n, m);//Se determina la coordenada de la primera fila qeu le toca procesar a cada proceso y se guarda en array turnos

		//Se inicializan todos los pipas y se almacenan en su respectivo Array
		crear_pipes_hijos(arrPipes_abajo, n);
		crear_pipes_hijos(arrPipes_arriba, n);
		crear_pipes_hijos_padre(arrPipes_hijo_padre, n);
		crear_pipes_hijos_padre(arrPipes_padre_hijo, n);

		//Se llama a la funcion que se encuentra en el modulo de logica de procesos para crear el numero necesario de los mismos
		crear_procesos(columnas, filas, arrPipes_padre_hijo, arrPipes_hijo_padre, arrPipes_abajo, arrPipes_arriba, n, m, turno, num_gen);

		
		//Informacion mostrada al usuario
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
				nanosleep (&ts, NULL);		
			}
			p++;
			
		}

		for(int i = 0; i < n; i++) {
			wait(NULL);
		}	

		return(0);
	}

}