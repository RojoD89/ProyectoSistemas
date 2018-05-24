#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/wait.h>
#include <string.h>

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_WHITE   "\x1b[0;37m"
#define ANSI_BGR_COLOR_WHITE    "\x1b[47m"
#define ANSI_BGR_COLOR_GREEN   "\x1b[42m"
#define ANSI_COLOR_RESET   "\x1b[0m"

	

struct pipe
	{
   		int fd[2];   
	};

void crear_pipes_hijos(struct pipe arrPipes[], int n){
	for (int i=0; i < (n - 1); i++){
		if(pipe(arrPipes[i].fd) < 0)
			perror("Failed to open pipe");
	}
	
}

void crear_pipes_hijos_padre(struct pipe arrPipes[], int n){
	for (int i=0; i < n; i++){
		if(pipe(arrPipes[i].fd) < 0)
			perror("Failed to open pipe");
	}
	
}

void crear_pipes_padre_hijos(int pipe_padre_hijo[], int pipe_hijo_padre[]){
	if(pipe(pipe_padre_hijo) < 0)
		perror("Failed to open pipe");
	if(pipe(pipe_hijo_padre) < 0)
		perror("Failed to open pipe");
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
 


void determinar_estado_cel_prim_pros(int columna, char area_trabajo[][columna+1], char fila_inferior_vec[columna+1], char fila_superior_vec[columna+1], int m, char arr[][columna+1]) {
	int vecinos;
	char aux[columna+1];
	for(int i = 0; i < m; i++){
		for(int j = 0; j < columna; j++){
			vecinos = 0;
			if(j-1 >= 0) {
				if(area_trabajo[i][j-1] == '1') vecinos += 1;	
			}
			if(j+1 <= (columna - 1)){
				if(area_trabajo[i][j+1] == '1') vecinos += 1;
			}
			if(i == 0){
				if(strcmp(fila_inferior_vec,"") != 0){
					if(fila_inferior_vec[j] == '1') vecinos += 1;
				
					if(j-1 >= 0){
						if(fila_inferior_vec[j-1] == '1') vecinos += 1;
					}
					if(j+1 <= (columna - 1)){
						if(fila_inferior_vec[j+1] == '1') vecinos += 1;
					}
				}
			}
			else{
				if(area_trabajo[i-1][j] == '1') vecinos += 1;
				if(j-1 >= 0){
					if(area_trabajo[i-1][j-1] == '1') vecinos += 1;
				}
				if(j+1 <= (columna - 1)){
					if(area_trabajo[i-1][j+1] == '1') vecinos += 1;
				}
			}
			if(i == (m-1)){
				if(strcmp(fila_superior_vec,"") != 0){
					if(fila_superior_vec[j] == '1') vecinos += 1;

					if(j-1 >= 0){
						if(fila_superior_vec[j-1] == '1') vecinos +=1;
					}
					if(j+1 <= (columna -1)){
						if(fila_superior_vec[j+1] == '1') vecinos += 1;
					}
				}
			}
			else{
				if(area_trabajo[i+1][j] == '1') vecinos += 1;
				if(j-1 >= 0){
					if(area_trabajo[i+1][j-1] == '1') vecinos += 1;
				}
				if(j+1 <= (columna - 1)){
					if(area_trabajo[i+1][j+1] == '1') vecinos += 1;
				}
			}

			if((area_trabajo[i][j] == '0') && (vecinos == 3)){
				aux[j] = '1';
			}
			else if((area_trabajo[i][j] == '1') && ((vecinos == 3) || (vecinos == 2))){
				aux[j] = '1';
			}
			else{
				aux[j] = '0';
			}
		}

		strcpy(arr[i], aux);

	}
	
}



void crear_procesos(int columna, struct pipe arrPipes_padre_hijo[], struct pipe arrPipes_hijo_padre[], struct pipe arrPipes_abajo[], struct pipe arrPipes_arriba[], int n, int m, int *turno, int num_gen){
		
		pid_t p;
		int k = 0;
		
		for(int i = 0; i < n; i++){
			p = fork();
			if(p < 0)
				perror("Fork failed");
			else if(p == 0){
				//printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid());
				if(i == 0){
					
					int cont = 0;
					char area_trabajo[m][columna+1];
					char respuesta[m][columna+1];
					//str_hijo_padre s1;
					char arr[m][columna+1];
					char fila_inferior_propia[columna+1];
					char fila_superior_vec[columna+1];
					close(arrPipes_padre_hijo[i].fd[1]);
					close(arrPipes_abajo[i].fd[0]);
					close(arrPipes_arriba[i].fd[1]);
					close(arrPipes_hijo_padre[i].fd[0]);
					while(cont < num_gen){
						read(arrPipes_padre_hijo[i].fd[0], area_trabajo, (m*(columna+1))*sizeof(char));
						/*printf("Area de Trabajo\n");
						for(int i = 0; i < m; i++){
							printf("%s\n", area_trabajo[i]);
						}*/
						strcpy(fila_inferior_propia, area_trabajo[m - 1]);
						write(arrPipes_abajo[i].fd[1], fila_inferior_propia, columna+1);
						read(arrPipes_arriba[i].fd[0], fila_superior_vec, columna+1);
						//printf(" Fila superior Vecino: %s\n", fila_superior_vec);
						determinar_estado_cel_prim_pros(columna, area_trabajo, "", fila_superior_vec, m, arr);
						//s1.fila_inicial = turno[i];
						for(int i = 0; i < m; i++){
							strcpy(respuesta[i], arr[i]);
							//printf("%s\n", arr[i]);
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
					//str_hijo_padre s1;
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
						/*printf("Area de Trabajo\n");
						for(int i = 0; i < m; i++){
							printf("%s\n", area_trabajo[i]);
						}*/
						strcpy(fila_superior_propia, area_trabajo[0]);
						strcpy(fila_inferior_propia, area_trabajo[m - 1]);
						read(arrPipes_abajo[i-1].fd[0], fila_inferior_vec, columna+1);
						write(arrPipes_arriba[i-1].fd[1], fila_superior_propia, columna+1);	
						write(arrPipes_abajo[i].fd[1], fila_inferior_propia, columna+1);
						read(arrPipes_arriba[i].fd[0], fila_superior_vec, columna+1);
						//printf(" Fila superior Vecino: %s\n", fila_superior_vec);
						//printf(" Fila inferior Vecino: %s\n", fila_inferior_vec);
						determinar_estado_cel_prim_pros(columna, area_trabajo, fila_inferior_vec, fila_superior_vec, m, arr);
						//s1.fila_inicial = turno[i];
						for(int i = 0; i < m; i++){
							strcpy(respuesta[i], arr[i]);
							//printf("%s\n", arr[i]);
						}
					
						write(arrPipes_hijo_padre[i].fd[1], respuesta, (m*(columna+1))*sizeof(char));
						cont++;
					}
				}
				else{
					
					int cont = 0;
					char area_trabajo[m][columna+1];
					//str_hijo_padre s1;
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
						/*printf("Area de Trabajo\n");
						for(int i = 0; i < m; i++){
							printf("%s\n", area_trabajo[i]);
						}*/
						strcpy(fila_superior_propia, area_trabajo[0]);
						write(arrPipes_arriba[i-1].fd[1], fila_superior_propia, columna+1);
						read(arrPipes_abajo[i-1].fd[0], fila_inferior_vec, columna+1);
						//printf(" Fila inferior Vecino: %s\n", fila_inferior_vec);
						determinar_estado_cel_prim_pros(columna,area_trabajo, fila_inferior_vec, "", m, arr);
						//s1.fila_inicial = turno[i];
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

void crear_matriz_aux(int columna, struct pipe arrPipes_hijo_padre[], int n, int m, char game_aux[][columna+1], int *turno){
	int p;
	//str_hijo_padre s;
	char respuesta[m][columna+1];
	for(int i = 0; i < n; i++){
		p = 0;
		close(arrPipes_hijo_padre[i].fd[1]);
		read(arrPipes_hijo_padre[i].fd[0], respuesta, (m*(columna+1))*sizeof(char));
		for(int j = turno[i]; j < (turno[i] + m); j++){
			strcpy(game_aux[j],respuesta[p]);
			p++;
		}

	}
}

void enviar_informacion_hijos(int columna, struct pipe arrPipes_padre_hijo[], int n, int m, int *turno, char game[][columna+1]){
	for(int i = 0; i < n; i++){
		char area_trabajo[m][columna+1];
		crear_area_trabajo(columna, area_trabajo, m, turno[i], game);
		close(arrPipes_padre_hijo[i].fd[0]);
		write(arrPipes_padre_hijo[i].fd[1], area_trabajo, (m*(columna+1))*sizeof(char));
	}

}

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
				printf(ANSI_BGR_COLOR_WHITE     "%*c"     ANSI_COLOR_RESET ,2, game[i][j]);
			else
				printf(ANSI_BGR_COLOR_GREEN ANSI_COLOR_GREEN    "%*c"     ANSI_COLOR_RESET ,2, game[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char const *argv[]){

	FILE *fp;
   	char buff[255];
   	fp = fopen("/home/daniel/Desktop/Proyecto/mundo.txt", "r");

   	fgets(buff, 255, (FILE*)fp);
	int filas = atoi(buff);

	fgets(buff, 255, (FILE*)fp);
	int columnas = atoi(buff);

	char game[filas][columnas+1];

	leer_matriz(columnas, filas, fp, game);
	fclose(fp);

	//mostrar_matriz(columnas, game, filas);
	
	int n = 5;
	int m = filas/n;
	int turno[n];
	
	int i = 0;

	int num_gen = 10;



	int p = 0;

	 
    

/*
	strcpy(game[0], "00000000000000000000");
	strcpy(game[1], "00000000000000000000");
	strcpy(game[2], "00000000000000000000");
	strcpy(game[3], "00000000000000000000");
	strcpy(game[4], "00000000000000000000");
	strcpy(game[5], "00000000000000000000");
	strcpy(game[6], "00000000000000000000");
	strcpy(game[7], "00000000111110000000");
	strcpy(game[8], "00000000000000000000");
	strcpy(game[9], "00000000000000000000");
	strcpy(game[10], "00000000000000000000");
	strcpy(game[11], "00000000000000000000");
	strcpy(game[12], "00000000000000000000");
	strcpy(game[13], "00000000000000000000");
	strcpy(game[14], "00000000000000000000");
	strcpy(game[15], "00000000000000000000");
	strcpy(game[16], "00000000000000000000");
	strcpy(game[17], "00000000000000000000");
	strcpy(game[18], "00000000000000000000");
	strcpy(game[19], "00000000000000000000");

	//mostrar_matriz(columnas, game, filas);
*/

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

	crear_procesos(columnas, arrPipes_padre_hijo, arrPipes_hijo_padre, arrPipes_abajo, arrPipes_arriba, n, m, turno, num_gen);


	while(p < num_gen){
		enviar_informacion_hijos(columnas, arrPipes_padre_hijo, n, m, turno, game);

		crear_matriz_aux(columnas, arrPipes_hijo_padre, n, m, game_aux, turno);

	
	//mostrar_matriz(game_aux, 6);
		
		printf("\n");
		
		actualizar_matriz(columnas, game, game_aux, filas);
		system("clear");
		mostrar_matriz(columnas, game, filas);
		p++;
		sleep(1);
		
		
		printf("\n");
	}




	for(int i = 0; i < n; i++) {
		wait(NULL);
	}	

	



	return(0);

}