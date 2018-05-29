#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "../include/logicprocess.h"
#include "../include/logicjuego.h"



//Determina el numero de filas de cada proceso en caso de que la division no sea entera
int numero_filas(int filas, int n){
	int num = filas % n;
	return num;

}
//Inicializa todos los pipes de comunicacion entre hijos
void crear_pipes_hijos(struct pipe *arrPipes, int n){
	for (int i=0; i < (n - 1); i++){
		if(pipe(arrPipes[i].fd) < 0)
			perror("Failed to open pipe");
	}
	
}

//Inicializa todos los pipes de comunicacion entre padre e hijos
void crear_pipes_hijos_padre(struct pipe *arrPipes, int n){
	for (int i=0; i < n; i++){
		if(pipe(arrPipes[i].fd) < 0)
			perror("Failed to open pipe");
	}
	
}

//Funcion que se llama desde el proceso padre para determinar el area de trabajo de cada proceso hijo
void crear_area_trabajo(int columna, char area_trabajo[][columna+1], int m, int k, char game[][columna+1]){
	for(int i = 0; i < m; i++) {
		strcpy(area_trabajo[i], game[k]);
		k++;
	}

}

//Se crea el Array con las coordenadas de la primera fila a procesar por cada proceso hijo
void crear_array_turnos(int *array, int n, int m){
	int k = 0;
	for(int i = 0; i < n; i++){
		array[i] = k;
		k = k + m;
	}
}
 
//Funcion logica de la creacion de procesos
void crear_procesos(int columna, int fila, struct pipe *arrPipes_padre_hijo, struct pipe *arrPipes_hijo_padre, struct pipe *arrPipes_abajo, struct pipe *arrPipes_arriba, int n, int m, int *turno, int num_gen){
		
		pid_t p;
		int k = 0;
		
		for(int i = 0; i < n; i++){//Se aplica hasta alcanzar el numero de procesos deseados
			p = fork();//Crea nuevo proceso
			if(p < 0)
				perror("Fork failed");
			else if(p == 0){//Se trata de un proceso hijo
				if(i == 0){//Si se trata del primer proceso
					
					int cont = 0;//Contador para el ciclo de vida del proceso
					char area_trabajo[m][columna+1];//Alamacena area de trabajo del proceso
					char respuesta[m][columna+1];//Alamcena el area de trabajo luego de ser procesada para ser enviada al padre
					char arr[m][columna+1];//Auxiliar
					char fila_inferior_propia[columna+1];//Almacena la ultima fila del area de trabajo para ser enviada al proceso inferior
					char fila_superior_vec[columna+1];//Almacena la informacion de la primera fila del vecino inferior

					//Se cierran todos los lados de los pipes que no van a ser usados(escritura o lectura dependiendo del caso)
					close(arrPipes_padre_hijo[i].fd[1]);
					close(arrPipes_abajo[i].fd[0]);
					close(arrPipes_arriba[i].fd[1]);
					close(arrPipes_hijo_padre[i].fd[0]);
					while(cont < num_gen){//Ciclo de vida
						read(arrPipes_padre_hijo[i].fd[0], area_trabajo, (m*(columna+1))*sizeof(char));//Recibe el area de trabajo proveniente del padre
						strcpy(fila_inferior_propia, area_trabajo[m - 1]);
						write(arrPipes_abajo[i].fd[1], fila_inferior_propia, columna+1);///Envia informacion sobre ultima fila del area de trabajo
						read(arrPipes_arriba[i].fd[0], fila_superior_vec, columna+1);//Recibe informacion sobre la primera fila del proceso inferior
						determinar_estado_cel_prim_pros(columna, area_trabajo, "", fila_superior_vec, m, arr);
						for(int i = 0; i < m; i++){
							strcpy(respuesta[i], arr[i]);//Se crea la respuesta a enviar al padre
						}
				
						write(arrPipes_hijo_padre[i].fd[1], respuesta, (m*(columna+1))*sizeof(char));//Se envia informacion del area de trabajo procesada al padre
						cont++;
					}
				}
				else if((i != 0) && (i != n - 1)){//Aplica lo mismo que el anterior solo que en este caso se envia y recibe informacion tanto al vecino superior como al inferior
					
					int cont = 0;
					char area_trabajo[m][columna+1];
					char respuesta[m][columna+1];
					char arr[m][columna+1];
					char fila_superior_propia[columna+1];
					char fila_inferior_propia[columna+1];
					char fila_superior_vec[columna+1];
					char fila_inferior_vec[columna+1];
					close(arrPipes_padre_hijo[i].fd[1]);
					close(arrPipes_abajo[i-1].fd[1]);
					close(arrPipes_arriba[i-1].fd[0]);
					close(arrPipes_abajo[i].fd[0]);
					close(arrPipes_arriba[i].fd[1]);
					close(arrPipes_hijo_padre[i].fd[0]);
					while(cont < num_gen){	
						read(arrPipes_padre_hijo[i].fd[0], area_trabajo, (m*(columna+1))*sizeof(char));
						strcpy(fila_superior_propia, area_trabajo[0]);
						strcpy(fila_inferior_propia, area_trabajo[m - 1]);
						read(arrPipes_abajo[i-1].fd[0], fila_inferior_vec, columna+1);
						write(arrPipes_arriba[i-1].fd[1], fila_superior_propia, columna+1);	
						write(arrPipes_abajo[i].fd[1], fila_inferior_propia, columna+1);
						read(arrPipes_arriba[i].fd[0], fila_superior_vec, columna+1);
						determinar_estado_cel_prim_pros(columna, area_trabajo, fila_inferior_vec, fila_superior_vec, m, arr);
						for(int i = 0; i < m; i++){
							strcpy(respuesta[i], arr[i]);
						}
					
						write(arrPipes_hijo_padre[i].fd[1], respuesta, (m*(columna+1))*sizeof(char));
						cont++;
					}
				}
				else{//Aplica lo mismo, solo que solo envia y recibe informacion al vecino superior 
					if((fila % n) != 0)
						m = m + numero_filas(fila, n);//Si la division del numero de filas entre el numero de procesos no es entera al ultimo proceso le corresponden todas las filas restantes
					
					int cont = 0;
					char area_trabajo[m][columna+1];
					char respuesta[m][columna+1];
					char arr[m][columna+1];
					char fila_superior_propia[columna+1];
					char fila_inferior_vec[columna+1];
					close(arrPipes_padre_hijo[i].fd[1]);
					close(arrPipes_arriba[i-1].fd[0]);
					close(arrPipes_abajo[i-1].fd[1]);
					close(arrPipes_hijo_padre[i].fd[0]);
					while(cont < num_gen){
						read(arrPipes_padre_hijo[i].fd[0], area_trabajo, (m*(columna+1))*sizeof(char));
						strcpy(fila_superior_propia, area_trabajo[0]);
						write(arrPipes_arriba[i-1].fd[1], fila_superior_propia, columna+1);
						read(arrPipes_abajo[i-1].fd[0], fila_inferior_vec, columna+1);
						determinar_estado_cel_prim_pros(columna,area_trabajo, fila_inferior_vec, "", m, arr);
						for(int i = 0; i < m; i++){
							strcpy(respuesta[i], arr[i]);
						}
				
						write(arrPipes_hijo_padre[i].fd[1], respuesta, (m*(columna+1))*sizeof(char));
						cont++;
					}
				}
				
				exit(0);
			}
			
		}
}


//Almacena la informacion recibida de cada hijo en una matriz aux
void crear_matriz_aux(int columna, int fila, struct pipe *arrPipes_hijo_padre, int n, int m, char game_aux[][columna+1], int *turno){
	int p;
	char respuesta[m][columna+1];
	for(int i = 0; i < n; i++){
		p = 0;
		close(arrPipes_hijo_padre[i].fd[1]);
		if(((fila % n) != 0) && (i == n-1)){
			int num = m + numero_filas(fila, n);
			char respuesta_alt[num][columna+1];
			read(arrPipes_hijo_padre[i].fd[0], respuesta_alt, (num*(columna+1))*sizeof(char));
			for(int j = turno[i]; j < (turno[i] + num); j++){
				strcpy(game_aux[j],respuesta_alt[p]);
				p++;
			}

		}
		else{
			read(arrPipes_hijo_padre[i].fd[0], respuesta, (m*(columna+1))*sizeof(char));
			for(int j = turno[i]; j < (turno[i] + m); j++){
				strcpy(game_aux[j],respuesta[p]);
				p++;
			}
		}

	}
}

//Envia informacion del area de trabajo a cada hijo
void enviar_informacion_hijos(int fila, int columna, struct pipe *arrPipes_padre_hijo, int n, int m, int *turno, char game[][columna+1]){
	for(int i = 0; i < n; i++){
		char area_trabajo[m][columna+1];
		close(arrPipes_padre_hijo[i].fd[0]);
		if(((fila % n) != 0) && (i == n-1)){
			int num = m + numero_filas(fila, n);
			char area_trabajo_alt[num][columna+1];
			crear_area_trabajo(columna, area_trabajo_alt, num, turno[i], game);
			write(arrPipes_padre_hijo[i].fd[1], area_trabajo_alt, (num*(columna+1))*sizeof(char));
		}
		else{
			crear_area_trabajo(columna, area_trabajo, m, turno[i], game);
			write(arrPipes_padre_hijo[i].fd[1], area_trabajo, (m*(columna+1))*sizeof(char));
		}
	}

}