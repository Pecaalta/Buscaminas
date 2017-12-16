#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>
#include <string>
#include <windows.h>
#include <iostream>
#include <math.h> 
#include <fstream>

using namespace std;

#define CoMARCA 88		//Simbolo que se pondra en las marcas  		88=X
#define CoiNCOGNITO 63	//Simbolo que se pondra en las ocultas 		63=?
#define ComBOMBAS 66	//Simbolo que se pondra en las explosiones	66=B 

#define Co_e 'e'		//Tecla presionada para explorar
#define Co_b 'b'		//Tecla presionada para Buscar
#define Co_m 'm'		//Tecla presionada para Marcar

//	Convierte las teclas en minusculas en Mayusculas (No tocar)
#define Co_E Co_e - 32 
#define Co_B Co_b - 32
#define Co_M Co_m - 32

#define MaximosUsusarios 100	//Tope Usuarios registrables

//	::struct
struct Celdas {
	int 	Cal;  		// Para hacer Calculos
	char	Mos;  		// Para mostrar en pantalla
} Panel[8][8];
struct Usuario {		// Datos Cada usuario
	char 	Edad[3] = "";
	char 	Nombre[20] = "";
	char 	Nickname[20]= "";
	int 	Puntos = 0;
} Usuarios[MaximosUsusarios];
int NUsers = 0,			//Numero actual de usuarios en memeoria
	UserActive = 0;		//Usuario iniciado
	
int DimMatris=8	;		//Para dimensionar las matrices
int CantMinas=16;		//Cantidad de minas q seran plantadas
int CantMarcas=16;		//Limita marcas

//	::Variables determinan el fin del juego
int		GameWin  = (DimMatris * DimMatris) - CantMinas; 	// Celdas restantes						
bool 	GameOver = false;									// Situacion actual de la partida


int  DimCelda = 6;
char imp[6];
char ImpOculto[3][6]  	= {{201,205,205,205,187},{186,' ','?',' ',186},{200,205,205,205,188}};
char ImpNormal[3][6]  	= {{218,196,196,196,191},{179,' ','?',' ',179},{192,196,196,196,217}};
char ImpMarco[4] 				= {223,220,238,219};
char MDesplasar[] 		= "                             ";
char MDerrota[] 		= "\n                                             +----------------+\n                                             |   Game  over   |\n                                             +----------------+\n\n";
char MVictoria[] 		= "\n                                             +----------------+\n                                             |    Victoria    |\n                                             +----------------+\n\n";
char MFinalD[] 			= "                                    Suerte para la proxima, se nota que\n                                               la necesitas  \n                                             \n";
char MFinalV[] 			= "                              Fue pura suerte la proxima \n                              NO TENDRAS TANTA... SEGURO !\n                                                   \n";

//	::Funciones Impresion Letras grandes
void ImpBuscaminas();
void ImpMiCuenta();
void ImpVerperfil();
void ImpRegistrarse();
void ImpInicioSesion();
void ImpListo();
void ImpFallido();
void ImpJugar();

//	::Funciones usadas en el juego
int  Max(int x);					//Calcula si se encuntra en un limite superopr
int  Min(int x);					//Calcula si se encuntra en un limite inferior
int  ContarMinas(int x, int y);		//Retorna candidad de minas
bool BuscaErrores(int x, int y, char espacio);	//Busca errores en la for
void NuevaPartida();				//resetea la parte de matris a mostrar en partida
void rebelaceros(int a, int b);		//recusiva para la cascada de 0
int  Leeedad();						//Leera edad valida

//	::Funciones de cuerpo
void CiclosPartidas();				// Ciclos de juego
void Resultado();					// Sumara puntos si gano he imprimira resultados
void ImpInstrucciones();			// Imprimira Instrucciones
void ImpJuga(int fin = 0);			// Imprime panel 

//	::Menus
void Inicio();
void Login();
void Registrar();
void VerPerfil();
void Cuentas(int m = 0);
void Jugar();

//	::Persistencias
void LeerPrersistencias();
bool guardar(Usuario Nuevo);
void LeerPrersistencias();
void Modificar(Usuario BufferNuevo);
int  Localisar(Usuario BufferNuevo);
bool CambiarPuntos(Usuario Nuevo,int ubicasion);


main(){
	srand (time(NULL));
	system("Title Buscaminas");
	system ("color f0");
	ImpBuscaminas();
	LeerPrersistencias();
	system("pause");
	Inicio();
}


bool CambiarPuntos(Usuario Nuevo,int ubicasion){
	try{
		ofstream archivoguradar("perfiles.txt");
		archivoguradar.seekp((ubicasion-1)*sizeof(Nuevo),ios::beg);
		archivoguradar.write((char *)&Nuevo, sizeof(Nuevo));
		archivoguradar.clear();
		return true;
	}catch(int e){
		return false;
	}
}
int  Localisar(Usuario BufferNuevo){
	int ubicado = 0;
	string Comparable, Busqueda;
	Busqueda = BufferNuevo.Nickname;
	Usuario Nuevo;
	
	ifstream archivoLeer("perfiles.txt");
	archivoLeer.read((char *)&Nuevo, sizeof(Nuevo));
	while(archivoLeer && !archivoLeer.eof()){
		ubicado++;
		Comparable = Nuevo.Nickname;
		if (Comparable == Busqueda){
			archivoLeer.clear();
			return ubicado;
		}
		archivoLeer.read((char *)&Nuevo, sizeof(Nuevo));
	}
	archivoLeer.clear();
	return 0;
}
void Modificar(Usuario BufferNuevo){	
	int ubicasion = 0;
	ubicasion = Localisar(BufferNuevo);
	if (ubicasion){
		if(CambiarPuntos(BufferNuevo,ubicasion)){
			cout << "Puntos guardados\n";
		}else{
			cout << "Error al guardar\n";
		}
	}else{
		cout << "No encontrar\n";
	}

}
void LeerPrersistencias(){
	Usuario Nuevo;
	ifstream archivoLeer("perfiles.txt");
	archivoLeer.read((char *)&Nuevo, sizeof(Nuevo));
	bool repetido = false;
	while(archivoLeer && !archivoLeer.eof()){
		Usuarios[NUsers] = Nuevo;
		NUsers++;
		archivoLeer.read((char *)&Nuevo, sizeof(Nuevo));
	}	
	archivoLeer.clear();
}
bool guardar(Usuario Nuevo){
	try{
		ofstream archivoguradar("perfiles.txt",ios::app);
		archivoguradar.write((char *)&Nuevo, sizeof(Nuevo));
		archivoguradar.clear();
		return true;
	}catch(int e){
		return false;
	}

}


void Inicio(){
	while(true){
		system ("cls");
		ImpBuscaminas ();
		cout << endl<< endl;
		cout << "\t\t\t\t   1 - Iniciar Sesi"<< char(162) <<"n" << endl;
		cout << "\t\t\t\t   2 - Registrarse." << endl;
		cout << "\t\t\t\t   > ";
		char opsion = 3;
		
		
		cin >> opsion;
		cin.ignore();
		fflush( stdin );
		while(opsion!=49 and opsion!=50){
			system ("cls");
			ImpBuscaminas ();
			cout << endl;
			cout << "\t\t\t\tError opsion no validad" << endl<< endl;
			cout << "\t\t\t\t   1 - Iniciar Sesi"<< char(162) <<"n" << endl;
			cout << "\t\t\t\t   2 - Registrarse." << endl;
			cout << "\t\t\t\t   > ";
			
			fflush( stdin );
			cin >> opsion;
			fflush( stdin );
		}
		if (opsion == 49)
			Login();
		else 
			Registrar();
	}
}
void Registrar(){
	system ("cls");
	ImpRegistrarse();
	
	Usuario Nuevo;
	string compara1,compara2;
	int Duplicados = 0;
	char EdadProbisional[3];
	if (NUsers < MaximosUsusarios){
		cout << endl << "\t\t\t\t   Nickname: ";
		cin.getline(Nuevo.Nickname,20);
		compara1 = Nuevo.Nickname;
		fflush( stdin );
		do{
			Duplicados = 0;
			for(int i = 0; i < NUsers;i++){
				compara2 = Usuarios[i].Nickname;
				if (compara2 == compara1){
					Duplicados++;
				}
			}
			
			if (Duplicados != 0 ){
				system ("cls");
				Duplicados = 0;
				ImpRegistrarse ();
				cout << endl;
				cout << "\t\t\t\t   Nickname ya existe,proba con otro." << endl;
				cout << "\t\t\t\t   Nickname: ";
				cin.getline(Nuevo.Nickname,20);
				fflush( stdin );
			}
		}while(Duplicados > 0);
		cout<< endl << "\t\t\t\t   Nombre: ";
		cin.getline(Nuevo.Nombre,20);
		fflush( stdin );
		cout<< endl << "\t\t\t\t   edad: ";
		cin.getline(Nuevo.Edad,3);
		
		fflush( stdin );
		Usuarios[NUsers]= Nuevo;
		
		NUsers++;
		system ("cls");
		if (guardar(Nuevo)){
			ImpListo();
			cout << endl;
			cout << "\t\t\t\t   " << Usuarios[NUsers-1].Nickname << " ya puedes jugar !!\n";
			cout << "\t\t\t\t   Suerte!!\n";	
			cout << "\t\t\t\t      ";
			system ("pause > null");
		}else{
			ImpFallido();
			cout << endl;
			cout << "\t\t\t\t   Error al guardar persistencia\n";
			cout << "\t\t\t\t      ";
			system ("pause > null");
			
		}	
	}else{
		system ("cls");
		ImpFallido();
		cout << endl;
		cout << "\t\t\t\t   Demasiados Usuarios\n";
		cout << "\t\t\t\t      ";
		system ("pause > null");
	}
}
void Login(){
	int i = 0;
	string Nickname;
	system ("cls");
	ImpInicioSesion();
	cout << endl;
	cout<< "\t\t\t\t   Nickname: ";
	cin >> Nickname;
	do{
		for(i = 0; i < NUsers;i++){
			if (Nickname == Usuarios[i].Nickname){
				UserActive = i;
				break;
			}
		}
		
		if (i == NUsers){
			system ("cls");
			ImpInicioSesion();
			cout << endl;
			cout << "\t\t\t\t   El nickname no existe" << endl;
			cout << "\t\t\t\t   Presione cualquier tecla para volver...";
			system("pause > null");
			Inicio();
		}else{
			Cuentas(1);
		}
	}while(i == NUsers);	
}
void Cuentas(int m){
	char opsion = 3;
	while(true){
		system ("cls");
 		ImpMiCuenta();
		cout << endl;
		if (m == 1)
			cout << "\t\t\t\tBienbenido " << Usuarios[UserActive].Nombre<< endl<< endl;
		cout << "\t\t\t\t   1 - Jugar" << endl;
		cout << "\t\t\t\t   2 - Ver Perfil" << endl;
		cout << "\t\t\t\t   3 - Cerrar Sesi"<< char(162) <<"n" << endl;
		cout << "\t\t\t\t   >";
		cin >> opsion;
		fflush( stdin );
		while(opsion<49 or opsion>51){
			system ("cls");
			ImpMiCuenta ();
			cout << endl;
			cout << "\t\t\t\tError opsion no validad" << endl<< endl;
			cout << "\t\t\t\t   1 - Jugar" << endl;
			cout << "\t\t\t\t   2 - Ver Perfil" << endl;
			cout << "\t\t\t\t   3 - Cerrar Sesi"<< char(162) <<"n" << endl;
			cout << "\t\t\t\t   >";
			
			cin >> opsion;
			fflush( stdin );
		}
		switch(opsion){
			case 49:
				Jugar();
				break;
			case 50:
				VerPerfil();
				break;
			case 51:
				return;
		}
	}
}
void Jugar(){
	system ("cls");
	ImpJugar();
	char opsion ;
	cout << endl;
	cout << "\t\t\t\t   1 - 4x4  4 Minas  10 Puntos" << endl;
	cout << "\t\t\t\t   2 - 6x6  9 Minas  40 Puntos" << endl;
	cout << "\t\t\t\t   3 - 8x8 16 Minas 100 Puntos" << endl;	
	cout << "\t\t\t\t   >";
	cin >> opsion;
	fflush( stdin );
	while(opsion<49 or opsion>51){
		system ("cls");
		ImpJugar ();
		cout << endl;
		cout << "\t\t\t\t   Error opsion no validad" << endl;
		cout << "\t\t\t\t   1 - 4x4  4 Minas  10 Puntos" << endl;
		cout << "\t\t\t\t   2 - 6x6  9 Minas  40 Puntos" << endl;
		cout << "\t\t\t\t   3 - 8x8 16 Minas 100 Puntos" << endl;	
		cout << "\t\t\t\t   > ";
		
		cin >> opsion;
		fflush( stdin );
	}
	
	switch(opsion){
		case 49:
			CantMinas 	= CantMarcas = 4;
			DimMatris	= 4;
			break;
		case 50:
			CantMinas 	= CantMarcas = 9;
			DimMatris	= 6;
			break;
		case 51:
			CantMinas 	= CantMarcas = 1;
			DimMatris	= 8;
			break;
	}
	GameWin  = (DimMatris * DimMatris) - CantMinas;						
	GameOver = false;
	NuevaPartida();
	ImpInstrucciones();
	ImpJuga();
	CiclosPartidas();
	Resultado();
}
void VerPerfil(){
	system ("cls");
	ImpVerperfil();
	cout << endl;
	cout << "\t\t\t\t   Jugador " 	<< Usuarios[UserActive].Nickname 	<< endl<< endl;
	cout << "\t\t\t\t     Nombre " 	<< Usuarios[UserActive].Nombre 		<< endl;
	cout << "\t\t\t\t     Edad   " 	<< Usuarios[UserActive].Edad 		<< endl;
	cout << "\t\t\t\t     Puntos " 	<< Usuarios[UserActive].Puntos 		<< endl<< endl;
	cout << "\t\t\t\t     Presione cualquier tecla para volver..." 		;
	system("pause > null");
}


void ImpBuscaminas (){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t                        Mauro y Maximiliano Presentan... \n";
	cout << "\t\t _______ __   __ _______ _______ _______ __   __ ___ __    _ _______ _______ \n";
	cout << "\t\t|  _    |  | |  |       |       |   _   |  |_|  |   |  |  | |   _   |       |\n";
	cout << "\t\t| |_|   |  | |  |  _____|       |  |_|  |       |   |   |_| |  |_|  |  _____|\n";
	cout << "\t\t|       |  |_|  | |_____|       |       |       |   |       |       | |_____ \n";
	cout << "\t\t|  _   ||       |_____  |      _|       |       |   |  _    |       |_____  |\n";
	cout << "\t\t| |_|   |       |_____| |     |_|   _   | ||_|| |   | | |   |   _   |_____| |\n";
	cout << "\t\t|_______|_______|_______|_______|__| |__|_|   |_|___|_|  |__|__| |__|_______|\n";
	cout << "\n\t\t\t   ";
}
void ImpVerperfil(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t8888888b.                    .d888 d8b 888 \n";
	cout << "\t\t888   Y88b                  d88P   Y8P 888 \n";
	cout << "\t\t888    888                  888        888 \n";
	cout << "\t\t888   d88P  .d88b.  888d888 888888 888 888 \n";
	cout << "\t\t8888888P   d8P  Y8b 888P    888    888 888 \n";
	cout << "\t\t888        88888888 888     888    888 888 \n";
	cout << "\t\t888        Y8b.     888     888    888 888 \n";
	cout << "\t\t888          Y8888  888     888    888 888 \n"; 
	cout << "\n\t\t\t\t   ";
}
void ImpMiCuenta(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t888b     d888 d8b        .d8888b.                             888           d8888 \n";
	cout << "\t\t8888b   d8888 Y8P       d88P  Y88b                            888          d88888 \n";
	cout << "\t\t88888b.d88888           888    888                            888         d88P888 \n";
	cout << "\t\t888Y88888P888 888       888        888  888  .d88b.  88888b.  888888     d88P 888 \n";
	cout << "\t\t888 Y888P 888 888       888        888  888 d8P  Y8b 888  88b 888       d88P  888 \n";
	cout << "\t\t888  Y8P  888 888       888    888 888  888 88888888 888  888 888      d88P   888 \n";
	cout << "\t\t888       888 888       Y88b  d88P Y88b 888 Y8b.     888  888 Y88b.   d8888888888 \n";
	cout << "\t\t888       888 888         Y8888P     Y88888   Y8888  888  888   Y888 d88P     888 \n";	
	cout << "\n\t\t\t\t   ";
}
void ImpRegistrarse(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t888b    888                                     \n";
	cout << "\t\t8888b   888                                     \n";
	cout << "\t\t88888b  888                                     \n";
	cout << "\t\t888Y88b 888 888  888  .d88b.  888  888  .d88b.  \n";
	cout << "\t\t888 Y88b888 888  888 d8P  Y8b 888  888 d88  88b \n";
	cout << "\t\t888  Y88888 888  888 88888888 Y88  88P 888  888 \n";
	cout << "\t\t888   Y8888 Y88b 888 Y8b.      Y8bd8P  Y88..88P \n";
	cout << "\t\t888    Y888   Y88888   Y8888    Y88P     Y88P   \n";
	cout << "\n\t\t\t\t   ";
}
void ImpInicioSesion(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t8888888          d8b          d8b          \n";
	cout << "\t\t  888            Y8P          Y8P          \n";
	cout << "\t\t  888                                      \n";
	cout << "\t\t  888   88888b.  888  .d8888b 888  .d88b.  \n";
	cout << "\t\t  888   888  88b 888 d88P     888 d88  88b \n";
	cout << "\t\t  888   888  888 888 888      888 888  888 \n";
	cout << "\t\t  888   888  888 888 Y88b.    888 Y88..88P \n";
	cout << "\t\t8888888 888  888 888   Y8888P 888   Y88P   \n";
	cout << "\n\t\t\t\t   ";
}
void ImpListo(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t888      d8b          888                   888 \n";
	cout << "\t\t888      Y8P          888                   888 \n";
	cout << "\t\t888                   888                   888 \n";
	cout << "\t\t888      888 .d8888b  888888  .d88b.        888 \n";
	cout << "\t\t888      888 88K      888    d88  88b       888 \n";
	cout << "\t\t888      888  Y8888b. 888    888  888       Y8P \n";
	cout << "\t\t888      888      X88 Y88b.  Y88..88P           \n";
	cout << "\t\t88888888 888  88888P'   Y888   Y88P         888 \n";
	cout << "\n\t\t\t\t   ";
}
void ImpFallido(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t8888888888       888 888          \n";
	cout << "\t\t888              888 888          \n";
	cout << "\t\t888              888 888          \n";
	cout << "\t\t8888888  8888b.  888 888  .d88b.  \n";
	cout << "\t\t888          88b 888 888 d88  88b \n";
	cout << "\t\t888     .d888888 888 888 888  888 \n";
	cout << "\t\t888     888  888 888 888 Y88..88P \n";
	cout << "\t\t888      Y888888 888 888   Y88P   \n";
	cout << "\n\t\t\t\t   ";
}
void ImpJugar(){
	cout << "\n\n\n\n\n\n\n";
	cout << "\t\t       d8888         888888                                    \n";
	cout << "\t\t      d88888            88b                                    \n";
	cout << "\t\t     d88P888            888                                    \n";
	cout << "\t\t    d88P 888            888 888  888  .d88b.   8888b.  888d888 \n";
	cout << "\t\t   d88P  888            888 888  888 d88P 88b      88b 888P    \n";
	cout << "\t\t  d88P   888            888 888  888 888  888 .d888888 888     \n";
	cout << "\t\t d8888888888            88P Y88b 888 Y88b 888 888  888 888     \n";
	cout << "\t\td88P     888            888   Y88888   Y88888  Y888888 888     \n";
	cout << "\t\t                      .d88P               888                  \n";
	cout << "\t\t                    .d88P            Y8b d88P                  \n";
	cout << "\t\t                  888P                Y88P                     \n";
	cout << "\n\t\t\t\t   ";
}
void ImpInstrucciones(){
	system ("cls");
	// Imprimir instrucciones y limpiara pantalla una vez presionada una tecla
	printf("\n");
	printf("\tJugadas  Disponibles\n");
	printf("\n");
	printf("\t\t   %c(B)uscar                    > Buscar                               \n",179);
	printf("\t\t   %c(E)xplorar                     Explorar todas las celdas que rodean  \n",179);
	printf("\t\t   %c(M)arcar      %cCoordenadas     las coordenadas solo si el numero   \n",179,179);
	printf("\t\t   %c           %c%c%c%c%c%c%c             de marcas es igual a de la          \n",179,218,196,196,193,196,196,191);
	printf("\t\t %c%c%c%c%c %c%c%c%c%c %c%c%c%c%c %c%c%c%c%c           coordenada.                         \n",218,196,193,196,191,218,196,196,196,191,218,196,193,196,191,218,196,193,196,191);
	printf("\t\t %c M %c %c   %c %c A %c %c 1 %c        > Explorar                             \n",179,179,179,179,179,179,179,179);
	printf("\t\t %c%c%c%c%c %c%c%c%c%c %c%c%c%c%c %c%c%c%c%c           Desvela contenido de la coordenada  \n",192,196,196,196,217,192,196,194,196,217,192,196,196,196,217,192,196,196,196,217);
	printf("\t\t         %c   Fila-Colimna       > Marcar                               \n",179);
	printf("\t\t         %c                         Marcar o desmarcar una coordenada   \n",179);
	printf("\t\t         %cEspacio                                                      \n",179);
	printf("\n\n");
	printf("\tObjetivo \n");
	printf("\n");
	printf("\t\t\t Despeja todas las celdas libres del mapa!!                          \n");
	printf("\n");
	printf("\tSuertre\n");
	printf("\n\t\t\t ");
    system("pause");
	system ("cls");
}


void CargaTipoCelda(char ori[]){
	for (int e = 0; e < DimCelda;e++)
		imp[e] = ori[e];
}
void ImpCeld(int fila, char c){
	if (c == '?' or c == 'X')
		CargaTipoCelda(ImpOculto[fila]);
	else
		CargaTipoCelda(ImpNormal[fila]);
	if (fila == 1) imp[2] = c; 
	
	switch(fila){
		case 0:
			cout <<imp;
			break;
		case 1:
			cout <<imp;
			break;
		case 2:
			cout <<imp;			
			break;
	}
}
void ImpJuga(int fin){
	int Cont = 97, a , b, c;
	//Var para cortar la impresion al final del juego
	int  cortarMatris = DimMatris,
		 Lininferior  = (DimMatris*(DimCelda-1)+4);
	char DespGia[2] = {' ',' '},
		 imp = ImpMarco[0];
	cout << MDesplasar << "    ";
	for( a = 1 ; a <= DimMatris ; a++ ){
		cout << "  " << a << "  ";
	}
	cout << endl << MDesplasar << "  ";
	for (a = 0;a < (DimMatris*(DimCelda-1)+4);a++ )
		cout << ImpMarco[1];
	cout << endl;
	
	if (fin != 0){
		cortarMatris = fin;
		Lininferior+=2;
		imp = ImpMarco[2];
		DespGia[0] = imp;
		DespGia[1] = imp;
	}
	for( a = 0 ; a < cortarMatris ; a++ ){
		for (b = 0;b < 3;b++ ){
			if (b == 1) cout << MDesplasar << char(Cont++) << " ";
			else cout << MDesplasar << "  ";
			cout << ImpMarco[3] <<" ";
			for( c = 0 ; c < DimMatris ; c++ ){
				ImpCeld(b,Panel[a][c].Mos);
			}
			cout << " " << ImpMarco[3] << endl;
		}
	}
	cout << MDesplasar << DespGia[0] << DespGia[1];
	for (a = 0;a < Lininferior;a++ )
		cout << imp;
}


int  Min(int x){
	x--;
	if (x < 0 ){
		return x+1;
	}else{
		return x;
	}
}
int  Max(int x){
	x++;
	if (x >= DimMatris ){
		return x-1;
	}else{
		return x;
	}
}
int  ContarMinas(int x, int y){
	int contador = 0;
	for(int a = Min(x) ; a <= Max(x) ; a++ ){
		for(int b = Min(y) ; b <= Max(y) ; b++ ){
			if (Panel[a][b].Mos == CoMARCA){
				contador++;
			}
		}
	}
	return contador;
}
bool BuscaErrores(int x, int y, char espacio){
	//Buscara errores en la forma o coordenadas invalidad
	if (y >= DimMatris or y < 0)
		return true;
	if (espacio != ' ')
		return true;
	if (x >= DimMatris or x < 0)
		return true;
	return false;
}

void rebelaceros(int a, int b){
	for(int c = Min(a) ; c <= Max(a) ; c++ ){
		for(int d = Min(b) ; d <= Max(b) ; d++ ){
			if (Panel[c][d].Mos != CoMARCA ){
				if (Panel[c][d].Mos == CoiNCOGNITO){
					Panel[c][d].Mos = (char)Panel[c][d].Cal + 48;
					if (Panel[c][d].Cal == 0){
						rebelaceros(c,d);
					}
					GameWin--;
				}
				if (Panel[c][d].Cal == 9){
					GameOver = true;
				}
			}
		}
	}
}
void NuevaPartida(){
 	// Limpia matrices
	int a, b, c, x, y; //x y para ubicarse en matrices y a b c para los for
	for( a = 0 ; a < DimMatris ; a++ ){
		for( b = 0 ; b < DimMatris ; b++ ){
			Panel[a][b].Cal = 0;
			Panel[a][b].Mos = CoiNCOGNITO;
		}
	}

	//	Recore matris
	for( a = 0 ; a < CantMinas ; a++){
		x = rand() % (DimMatris - 1);//Mandara numeros al azar
		y = rand() % (DimMatris - 1);//Mandara numeros al azar
		if (Panel[x][y].Cal != 9) {	//Compruebo si hay mina, de haberla agrego un ciclo
			Panel[x][y].Cal = 9;	//Si no hay mina la planto y sumo uno alrededor
			for( b = Min(x) ; b <= Max(x) ; b++ ){
				for( c = Min(y) ; c <= Max(y) ; c++ ){
					if (Panel[b][c].Cal != 9){
						Panel[b][c].Cal++;
					}
				}
			}
		}else {
			a--;
		}
    }
}


void Explorar(int x, int y){
	if (Panel[x][y].Mos == CoiNCOGNITO){
		// Si es bomba pierdes si no resta uno a las casillas restantes
		if (Panel[x][y].Cal == 9){
			GameOver = true;
		}else if (Panel[x][y].Cal == 0) {
			rebelaceros(x, y);
		}else{
			GameWin--;
			Panel[x][y].Mos = (char)Panel[x][y].Cal +48;
		}
	}
}
void Buscar(int x, int y){
	if (Panel[x][y].Mos != CoiNCOGNITO){
		// Comprueba las marcados alrededor si todo sale bien suma uno a contador
		// Comprueba si el numero de la casilla es igual al contador  
		if (Panel[x][y].Cal == ContarMinas(x,y)){
			for(int a = Min(x) ; a <= Max(x) ; a++ ){
				for(int b = Min(y) ; b <= Max(y) ; b++ ){
					if (Panel[a][b].Mos != CoMARCA){
						if (Panel[a][b].Mos != (char)Panel[a][b].Cal + 48){
							Panel[a][b].Mos = (char)Panel[a][b].Cal + 48;
							GameWin--;
						}
						// Desvelar las no marcadas
						switch(Panel[a][b].Cal){
							case 9: // Si esta es una bomba mueres
								GameOver = true;
							break;
							case 0: // Si es cero vuelve a ser el mismo ciclo
								rebelaceros(a,b);
							break;
						}
					}
				}
			}
		}
	}
}
void Marcar(int x, int y){
	if (Panel[x][y].Mos == CoiNCOGNITO and CantMarcas != 0){
		Panel[x][y].Mos = CoMARCA;
		CantMarcas--;
	} else {
		if (Panel[x][y].Mos == CoMARCA){
			Panel[x][y].Mos = CoiNCOGNITO;
			CantMarcas++; 
		}
	}
	
}


void CiclosPartidas() {
	char accion, espacio;	// Variables para confirmar una forma accion, espacio
	int x, y; // x e y son para coordenadas
	bool error = false;	// Error en ciclo 

	// While finaliza el juego
	while(!GameOver and GameWin != 0){
		// Comprueba errores previos
		if (error == false) {
			cout << "\n\t\t\t    Falta " << GameWin << " celdas por despegar y tines " << CantMarcas << " marcas para usar, que sigue ?\n\t\t\t    " ;
		} else {
			error = false;
			cout << "\n\t\t\t    No entiendo que queres hacer ? \n\t\t\t    ";
		}
		// Escaneo nueva jugada
		scanf("%c%c%c%d",&accion,&espacio,&x,&y);
		fflush( stdin );
		// minuscula, mayuscula retornara un valor Compatible(numerico)
		if(x > 96){ 
			x-=97;
		} else {
			if(x > 64){
				x-=65;
			}
		}
		y--;
		// Compruebame que los valores se encuntren sean balidos		
		error = BuscaErrores(x,y,espacio);

		// Si ha habido error no hara ninguna
		if (error == false){
			switch(accion){
				case Co_B:case Co_b:
					Buscar(x,y);
					break;
				case Co_E: case Co_e:
					Explorar(x,y);
					break;
				case Co_M: case Co_m:
					Marcar(x,y);
					break;
				default:
					// Si no es ninguna de las teclas deseadas manda error
					error = true;
					break;
			}
		}
       
		//Refresco de tavlero con balores actualisados
		system ("cls");
		ImpJuga();
	}
}
void Resultado(){
	int a, b,sumar=0;// Usadas para for
	// Mostrara todas las bombas
	for( a = 0 ; a < DimMatris ; a++ ){
		for( b = 0 ; b < DimMatris ; b++ ){
			if ( Panel[a][b].Cal == 9 ){
				Panel[a][b].Mos = ComBOMBAS;       
			}
		}
	}
	system ("cls");
	// Si las minas es igual a la cantidad de casilleros ocultos ganas 
	ImpJuga(DimMatris/2+1);
	if ( GameWin == 0 ){
		cout << endl;
		cout << MVictoria << MFinalV;
		cout << "\t\t              Pero " << Usuarios[UserActive].Nickname << " gano ";
		switch(DimMatris){
			case 4:
				cout << " 10 puntos ";
				sumar= 10;
				break;
			case 6:
				cout << " 40 puntos ";
				sumar= 40;
				break;
			case 8:
				cout << "100 puntos ";
				sumar= 100;
				break;
		}
		Usuarios[UserActive].Puntos+= sumar;
		Modificar(Usuarios[UserActive]);
		cout << "ahora tines " << Usuarios[UserActive].Puntos << endl;
	}else{
		cout << MDerrota << MFinalD << endl;
		cout << "\t\t                          Puntos totales de " << Usuarios[UserActive].Nickname << " ";
		cout << Usuarios[UserActive].Puntos << endl; 
	}

	cout << endl << MDesplasar<< " ";

	system("pause");
}


