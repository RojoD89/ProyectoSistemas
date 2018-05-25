#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/logicprocess.h"
#include "../include/logicjuego.h"




int numero_filas(int filas, int n){
	int num = filas % n;
	return num;

}

void crear_pipes_hijos(struct pipe *arrPipes, int n){
	for (int i=0; i < (n - 1); i++){
		if(pipe(arrPipes[i].fd) < 0)
			perror("Failed to open pipe");
	}
	
}

void crear_pipes_hijos_padre(struct pipe *arrPipes, int n){
	for (int i=0; i < n; i++){
		if(pipe(arrPipes[i].fd) < 0)
			perror("Failed to open pipe");
	}
	
}


void crear_area_trabajo(int columna, char area_trabajo[][columna+1], int m, int k, char game[][columna+1]){
	for(int i = 0; i < m; i++) {
		strcpy(area_trabajo[i], game[k]);
		k++;
	}

}

void crear_array_turnos(int *array, int n, int m){
	int k = 0;
	for(int i = 0; i < n; i++){
		array[i] = k;
		k = k + m;
	}
}
 

void crear_procesos(int columna, int fila, struct pipe *arrPipes_padre_hijo, struct pipe *arrPipes_hijo_padre, struct pipe *arrPipes_abajo, struct pipe *arrPipes_arriba, int n, int m, int *turno, int num_gen){
		
		pid_t p;
		int k = 0;
		
		for(int i = 0; i < n; i++){
			p = fork();
			if(p < 0)
				perror("Fork failed");
			else if(p == 0){
				if(i == 0){
					
					int cont = 0;
					char area_trabajo[m][columna+1];
					char respuesta[m][columna+1];
					char arr[m][columna+1];
					char fila_inferior_propia[columna+1];
					char fila_superior_vec[columna+1];
					close(arrPipes_padre_hijo[i].fd[1]);
					close(arrPipes_abajo[i].fd[0]);
					close(arrPipes_arriba[i].fd[1]);
					close(arrPipes_hijo_padre[i].fd[0]);
					while(cont < num_gen){
						read(arrPipes_padre_hijo[i].fd[0], area_trabajo, (m*(columna+1))*sizeof(char));
						strcpy(fila_inferior_propia, area_trabajo[m - 1]);
						write(arrPipes_abajo[i].fd[1], fila_inferior_propia, columna+1);
						read(arrPipes_arriba[i].fd[0], fila_superior_vec, columna+1);
						determinar_estado_cel_prim_pros(columna, area_trabajo, "", fila_superior_vec, m, arr);
						for(int i = 0; i < m; i++){
							strcpy(respuesta[i], arr[i]);
						}
				
						write(arrPipes_hijo_padre[i].fd[1], respuesta, (m*(columna+1))*sizeof(char));
						cont++;
					}
				}
				else if((i != 0) && (i != n - 1)){
					
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
				else{
					if((fila % n) != 0)
						m = m + numero_filas(fila, n);
					
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