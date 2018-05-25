#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/logicjuego.h"

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
