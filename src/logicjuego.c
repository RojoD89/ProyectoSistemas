#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/logicjuego.h" 

//Funcion de logica de funcionamiento del juego

void determinar_estado_cel_prim_pros(int columna, char area_trabajo[][columna+1], char fila_inferior_vec[columna+1], char fila_superior_vec[columna+1], int m, char arr[][columna+1]) {
	//Se recibe el area de trabajo de cada proceso, la ultima fila del proceso superior y la primera fila del proceso inferior (en caso de que existan)
	int vecinos;//Almacena el numero de vecinos vivos
	char aux[columna+1];
	for(int i = 0; i < m; i++){
		for(int j = 0; j < columna; j++){//Se recorre toda el area de trabajo celda por celda
			vecinos = 0;
			if(j-1 >= 0) {//Se analizan las celdas dereha e izquierda
				if(area_trabajo[i][j-1] == '1') vecinos += 1;	
			}
			if(j+1 <= (columna - 1)){
				if(area_trabajo[i][j+1] == '1') vecinos += 1;
			}
			if(i == 0){//Si se trata de la primera fila se verifica que la ultima fila del proceso superior exista(si es el primer proceso esto no ocurre), si existe se anailzan los 3 vecinos superiores de la celda
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
			else{//Caso para cuando la fila en cuestion no se encuentra en el borde superior del area de trabajo
				if(area_trabajo[i-1][j] == '1') vecinos += 1;
				if(j-1 >= 0){
					if(area_trabajo[i-1][j-1] == '1') vecinos += 1;
				}
				if(j+1 <= (columna - 1)){
					if(area_trabajo[i-1][j+1] == '1') vecinos += 1;
				}
			}
			if(i == (m-1)){//Si se trara de la ultima fila se verifica que la primera fila del proceso inferior exista(si es el ultimo proceso esto no ocurre), si existe se analizan los 3 vecinos inferiores de la celda
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
			else{//Caso para cuando la fila en cuestion no se encuentra en el borde inferior del area de trabajo
				if(area_trabajo[i+1][j] == '1') vecinos += 1;
				if(j-1 >= 0){
					if(area_trabajo[i+1][j-1] == '1') vecinos += 1;
				}
				if(j+1 <= (columna - 1)){
					if(area_trabajo[i+1][j+1] == '1') vecinos += 1;
				}
			}
			//Se aplican las reglas del Juego y la fila procesada se almacena en aux
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

		strcpy(arr[i], aux);//Se copia la informacion de cada fila procesada en la matriz de resultado de cada proceso

	}
	
}
