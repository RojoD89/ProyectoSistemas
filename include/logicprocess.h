
struct pipe
	{
   		int fd[2];   
	};

int numero_filas(int filas, int n);

void crear_pipes_hijos(struct pipe *arrPipes, int n);

void crear_pipes_hijos_padre(struct pipe *arrPipes, int n);

void crear_area_trabajo(int columna, char area_trabajo[][columna+1], int m, int k, char game[][columna+1]);

void crear_array_turnos(int *array, int n, int m);

void crear_array_turnos(int *array, int n, int m);

void crear_procesos(int columna, int fila, struct pipe *arrPipes_padre_hijo, struct pipe *arrPipes_hijo_padre, struct pipe *arrPipes_abajo, struct pipe *arrPipes_arriba, int n, int m, int *turno, int num_gen);

void crear_matriz_aux(int columna, int fila, struct pipe *arrPipes_hijo_padre, int n, int m, char game_aux[][columna+1], int *turno);

void enviar_informacion_hijos(int fila, int columna, struct pipe *arrPipes_padre_hijo, int n, int m, int *turno, char game[][columna+1]);
		