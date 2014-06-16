#define USE_CONSOLE
#include <allegro.h>
#include <iostream>
#include <loadpng.h>
#include <string>
#include <fstream>
using namespace std;

struct statek_kosmiczny {
	int x,y,amunicja;
	int rodzaj_broni;
};
int kl=0, i=0, ilosc_asteroid=10, punkty=0, ile_pozostalo, poziom_gry=1, nowa_gra=1, poziom_trudnosci=1;//1 - latwy, 2- trudny
int pkt50=0, pkt50_y, pkt50_x;//int ile_zestrzelonych=0;
volatile long speed=0;
statek_kosmiczny statek;
//bitmapy ,dzwieki, czcionki
BITMAP *bmp=NULL;
BITMAP *s=NULL;
BITMAP *tlo=NULL;
BITMAP *tlo2=NULL;
BITMAP *gwiazdy=NULL;
BITMAP *gwiazdy2=NULL;
BITMAP *gwiazda_smierci=NULL;
BITMAP *tlo_menu=NULL;
BITMAP *tlo_menu2=NULL;
BITMAP *belka_menu=NULL;
BITMAP *plus_50=NULL;
BITMAP *tlo_tablicy_wynikow=NULL;
BITMAP *bron=NULL;
BITMAP *psk=NULL;
BITMAP *pomoc=NULL;
BITMAP *pomoc2=NULL;
BITMAP *bomba_bmp=NULL;
BITMAP *eksplozja=NULL;
BITMAP *panel=NULL;
BITMAP *wybuch=NULL;
BITMAP *napis=NULL;
SAMPLE *laser=NULL;
SAMPLE *menu_zmiana=NULL;
SAMPLE *wybuch_sample=NULL;
//SAMPLE *ambient;
MIDI *muzyczka=NULL;
FONT *cyberspace=NULL;
FONT *r2014;
PALETTE palette;

struct a{ //asteroida
	int x,y,hp,punkty;
	int rx,ry;
	BITMAP *bmp;
	int lvl;
	int wybuch;
};

struct p { //pocisk
	int x,y,strzelono;
};

struct b { //bron - znajdzka
	int ktora_bron;
	int x,y,czy_jest;
};

struct bomb { //bomba/rakieta/czy coœ w tym stylu
	bool znajdzka,uzyto;
	int ilosc;
	int x,y;
};

a asteroida[50];
p pocisk[20];	
b bron_znajdzka;
bomb bomba;

fstream plik;	//zmienna plikowa
char gracz[11][30];
char nazwa_gracza[30];
bool dzwiek_wlaczony=true;

//-----Potrzebne do timera:
void increment_speed(){
    speed++;}
END_OF_FUNCTION( increment_speed );

LOCK_VARIABLE( speed );
LOCK_FUNCTION( increment_speed );

//====================Inicjalizacja==========================
int inicjalizacja () {
	int ilosc_wczytanych=0,udalo_sie_wczytac=1,los;
	ile_pozostalo=ilosc_asteroid;
	srand(time(NULL));
	//uruchomienie allegro
	allegro_init();
	loadpng_init();
    install_keyboard();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);
    set_palette(default_palette);
	install_sound( DIGI_AUTODETECT, MIDI_AUTODETECT, "" );
	set_volume( 255, 125 );
	bmp=create_bitmap(800,600);//tworzenie bufora an ktory wklejane sa wszystkie bitmapy i tekst

	cout<<"Space shooter v1.0 Beta"<<endl<<endl;
	strcpy_s(nazwa_gracza,"Gracz               ");
		for(i=0;i<50;i++) {  //ustawianie wskaznikow bitmap asteroid na NULL
		asteroida[i].bmp=NULL; }
		for(i=0;i<50;i++) {
			los=(rand()%100)+1;
			if (los<=20) {//20% szansy na wylosowanie twardszych asteroid
				asteroida[i].bmp=load_bmp("obrazy/asteroida2.bmp",default_palette);
				asteroida[i].lvl=2;
				}
			if (los>20 && los<=30) { //10% szansy na najtwardsze asteroidy
				asteroida[i].bmp=load_bmp("obrazy/asteroida3.bmp",default_palette);
				asteroida[i].lvl=3;
			}
			if (los>30) { //70% szansy na wylosowanie zwyklych asteroid
			asteroida[i].bmp=load_bmp("obrazy/asteroida.bmp",default_palette);
			asteroida[i].lvl=1;
			}
		if (!asteroida[i].bmp) {
			allegro_message("Wystapil problem podczas wczytywania bitmap asteroid\nSprawdz, czy w folderze znajduja sie pliki: 'asteroida.bmp','asteroida2.bmp','asteroida3.bmp'");
			break;
			}
		else ilosc_wczytanych++;}
		if (ilosc_wczytanych!=50) {
			allegro_message("Nie udalo sie wczytac wszystkich asteroid");
			return 0;}
	//--Wczytywanie bitmap--
	cout<<"Wczytywanie bitmap: ";

	plus_50=load_bmp("obrazy/50.bmp",default_palette);			
	bron=load_bmp("obrazy/czerwona_bron.bmp",default_palette);	
	bomba_bmp=load_bmp("obrazy/bomba.bmp",default_palette);
	s=load_bmp("obrazy/statek.bmp",default_palette);			
	tlo=load_png("obrazy/tlo.png",default_palette);							
	tlo2=load_png("obrazy/tlo.png",default_palette);			
	tlo_menu=load_png("obrazy/tlo.png",default_palette);
	tlo_menu2=load_png("obrazy/tlo.png",default_palette);
	gwiazdy=load_bmp("obrazy/gwiazdy.bmp",default_palette);
	gwiazdy2=load_bmp("obrazy/gwiazdy.bmp",default_palette);
	gwiazda_smierci=load_bmp("obrazy/gwiazda_smierci.bmp",default_palette);
	psk=load_png("obrazy/psk.png",default_palette);
	pomoc=load_png("obrazy/pomoc.png",default_palette);
	pomoc2=load_png("obrazy/pomoc2.png",default_palette);
	tlo_tablicy_wynikow=load_png("obrazy/tlo_tablicy_wynikow.png",default_palette);
	eksplozja=load_png("obrazy/eksplozja.png",default_palette);
	panel=load_png("obrazy/panel.png",default_palette);
	wybuch=load_png("obrazy/wybuch.png",default_palette);
	napis=load_bmp("obrazy/napis.bmp",default_palette);

	if (!s) {
		allegro_message("Nie udalo sie wczytac bitmapy statku - 'statek.bmp'!");
		return 0;}
	if (!tlo||!tlo2||!tlo_menu||!tlo_menu2) {
		allegro_message("Nie udalo sie wczytac pliku 'tlo.png'!");
		return 0;}
	if (!gwiazdy||!gwiazdy2) {
		allegro_message("Nie udalo sie wczytac pliku 'gwiazdy.bmp'!");
		return 0;}
	if (!plus_50) {
		allegro_message("Nie udalo sie wczytac pliku '50.bmp'!");
		return 0;}
	if (!pomoc) {
		allegro_message("Nie udalo sie wczytac pliku 'pomoc.png'!");
		return 0;}
	if (!pomoc2) {
		allegro_message("Nie udalo sie wczytac pliku 'pomoc2.png'!");
		return 0;}
	if (!eksplozja) {
		allegro_message("Nie udalo sie wczytac pliku 'eksplozja.png'!");
		return 0;}
	if (!wybuch) {
		allegro_message("Nie udalo sie wczytac pliku 'wybuch.png'!");
		return 0;}
	if (!bron) {
		allegro_message("Nie udalo sie wczytac pliku 'czerwona_bron.bmp'!");
		return 0;}
	if (!gwiazda_smierci) {
		allegro_message("Nie udalo sie wczytac pliku 'gwiazda_smierci.bmp'!");
		return 0;}
	if (!psk) {
		allegro_message("Nie udalo sie wczytac pliku 'psk.png'!");
		return 0;}
	if (!tlo_tablicy_wynikow) {
		allegro_message("Nie udalo sie wczytac pliku 'tlo_tablicy_wynikow.png'!");
		return 0;}
	if (!bomba_bmp) {
		allegro_message("Nie udalo sie wczytac pliku 'bomba.bmp'!");
		return 0;}
	if (!panel) {
		allegro_message("Nie udalo sie wczytac pliku 'panel.png'!");
		return 0;}
	if (!napis) {
		allegro_message("Nie udalo sie wczytac pliku 'napis.bmpqq'!");
		return 0;}

		cout<<"OK"<<endl;
	//--Wczytywanie dzwiekow--
		cout<<"Wczytywanie dzwiekow: ";
	laser = load_sample("dzwieki/laser.wav");
	menu_zmiana = load_sample("dzwieki/menu.wav");
	wybuch_sample = load_sample("dzwieki/wybuch.wav");
	muzyczka = load_midi("dzwieki/byob.mid");
	//ambient = load_sample("dzwieki/ambient.wav");
	if( !laser || !menu_zmiana || !muzyczka || !wybuch/*|| !ambient*/) {
    allegro_message("Nie udalo sie zaladowac plikow z dzwiekami");
    return 0;
	}
		cout<<"OK"<<endl;
	//--Wczytywanie czcionek--
		cout<<"Wczytywanie czcionek: ";
	cyberspace=load_font("czcionki/cyberspace.pcx",palette,NULL);
	if(!cyberspace) {
		allegro_message("Nie udalo sie zaladowac czcionki Cyberspace");
		return 0; }
	r2014=load_font("czcionki/r-2014.pcx",palette,NULL);
	if(!r2014) {
		allegro_message("Nie udalo sie zaladowac czcionki R-2014");
		return 0; }
		cout<<"OK"<<endl<<endl;
	cout<<"Inicjalizacja zakonczyla sie powodzeniem"<<endl<<endl;
	return 1;
}

//========================Strzelanie==========================
void strzal (int petla){
		int i=0,j;
		int los;
		if (statek.rodzaj_broni==1) {  //jesli bron to zielony laser
		if (petla%25==0) {				//co 25 petle wykonuj...
		while (pocisk[i].strzelono==1) { //i gdy jest wystrzelony to..
			i++; }
		if (pocisk[i].strzelono==0) {
			pocisk[i].x=statek.x;
			pocisk[i].y=statek.y-20;
			pocisk[i].strzelono=1;
			if (dzwiek_wlaczony==true) play_sample( laser, 100, 127, 500, 0 );
		}}}
		
		if (statek.rodzaj_broni==2) {  //czerwony laser(?)
		if (petla%10==0) {
		while (pocisk[i].strzelono==1) {
			i++; }
		if (pocisk[i].strzelono==0) {
			pocisk[i].x=statek.x;
			pocisk[i].y=statek.y-20;
			pocisk[i].strzelono=1; 
			if (dzwiek_wlaczony==true) play_sample( laser, 70, 127, 400, 0 );
		}}}

		if (key[KEY_SPACE]) {	//odpalenie bomby za pomoca spacji
			if (bomba.ilosc>-64 && bomba.uzyto==false) {
				bomba.ilosc-=1;
				bomba.uzyto=true;
				if (dzwiek_wlaczony==true) play_sample(wybuch_sample,130,127,350,0);
				for (i=0;i<ilosc_asteroid;i++) {
					if (asteroida[i].wybuch>=100) continue;
					if (asteroida[i].y>-64  && asteroida[i].x>-100) {
					asteroida[i].hp=0;
					asteroida[i].wybuch=100;
					punkty=punkty+asteroida[i].punkty;
					asteroida[i].x=-1000;
					ile_pozostalo--;
					}
			}
			}}
	
	for (i=0;i<ilosc_asteroid;i++) { //sprawdzenie, czy pocisk uderzyl asteroide
	if (asteroida[i].wybuch>20) continue;
	 int x1=asteroida[i].x, y1=asteroida[i].y, s1=64, w1=64;
	 int x2,y2,s2,w2;
	 for (j=0;j<20;j++) {
		 if (statek.rodzaj_broni==1) {
			 x2=pocisk[j].x-1; y2=pocisk[j].y-5; s2=2; w2=10;}
		 if (statek.rodzaj_broni==2) {
			 x2=pocisk[j].x-2; y2=pocisk[j].y-2; s2=4; w2=4;}
	 
	if(x2>=x1 && x2<=x1+s1 && y2<=y1+w1 && y2>=y1 &&y2>=0) {

		if (pocisk[j].strzelono==0) continue; 
		if (pocisk[j].strzelono==1) {	//odejmowanie hp asteroidy i resetowanie pocisku
			pocisk[j].strzelono=0;
			pocisk[j].x=1000;
			if (statek.rodzaj_broni==1) asteroida[i].hp-=10;
			if (statek.rodzaj_broni==2) asteroida[i].hp-=5;
			
			los=rand()%100;				//10% szans na wylosowanie +50 punktow z zestrzelonej asteroidy
			if (los<10 && pkt50==0 && bron_znajdzka.czy_jest==0 && bomba.znajdzka==false && asteroida[i].hp<=0) {
				pkt50=1;
				pkt50_y=asteroida[i].y;
				pkt50_x=asteroida[i].x;
			}
			if (los>=10 && los<20 && pkt50==0 && bron_znajdzka.czy_jest==0 && bomba.znajdzka==false && asteroida[i].hp<=0) { //10% szans na wylosowanie broni
				bron_znajdzka.czy_jest=1;
				bron_znajdzka.x=asteroida[i].x;
				bron_znajdzka.y=asteroida[i].y;
			}

			if (los>=20 && los<25 && pkt50==0 && bron_znajdzka.czy_jest==0 && bomba.znajdzka==false && asteroida[i].hp<=0) { //5% szans na wylosowanie bomby
				bomba.znajdzka=true;
				bomba.x=asteroida[i].x;
				bomba.y=asteroida[i].y;
			}
		}}
	}
					if (asteroida[i].hp<=0 && asteroida[i].wybuch<=20) {			//jesli udalo sie rozwalic asteroide...
					asteroida[i].wybuch++;
					//asteroida[i].x=-200;
					}
						if (asteroida[i].wybuch>20  && asteroida[i].x>-100) {
						punkty+=asteroida[i].punkty;
						asteroida[i].x=-1000;
						ile_pozostalo--;}
	}
	}



//potega 2 stopnia
int kwadrat(int liczba) {
	liczba=liczba*liczba;
	return liczba;
}

/*============================Detekcja kolizji asteroid (rownanie okregu)==================*/
void kolizja_asteroid2 (int a1, int a2) {
	double x1,x2,y1,y2;
	int pom;
	x1=double(asteroida[a1].x);
	x2=double(asteroida[a2].x);
	y1=double(asteroida[a1].y);
	y2=double(asteroida[a2].y);
	//sprawdzenie, czy nastapila kolizja na podstawie rownania okregu
	if(sqrt(double(abs(kwadrat(x1-x2))) + double(abs(kwadrat(y1-y2)))) <55) {
		if(asteroida[a1].x>asteroida[a2].x) {	//asteroida 1 z lewej
			asteroida[a1].x++;
			asteroida[a2].x--; 
			pom=asteroida[a1].rx;
			asteroida[a1].rx=asteroida[a2].rx;
		    asteroida[a2].rx=pom;}
		else {									//asteroida 1 z prawej
			asteroida[a1].x--;
			asteroida[a2].x++;
			pom=asteroida[a1].rx;
			asteroida[a1].rx=asteroida[a2].rx;
		    asteroida[a2].rx=pom;}
		if(asteroida[a1].y>asteroida[a2].y) {	//asteroida 1 u dolu
			asteroida[a1].y++;
			asteroida[a2].y--; 
			pom=asteroida[a1].ry;
			asteroida[a1].ry=asteroida[a2].ry;
		    asteroida[a2].ry=pom;
		}
		else {									//asteroida 1 u gory
			asteroida[a1].y--;
			asteroida[a2].y++;
			pom=asteroida[a1].ry;
			asteroida[a1].ry=asteroida[a2].ry;
		    asteroida[a2].ry=pom;
		}
	}
}

//=========================Kolizja statku=================
int kolizja_statku (void){
	int i,kolizja=0,kolizja_ze_znajdzka=0;
	for (i=0;i<ilosc_asteroid;i++) {
	//Tutaj kolizja jest sprawdzana na podstawie kolizji prostokatow
		if ((statek.x-10<asteroida[i].x+64 && statek.x-10>asteroida[i].x) && (statek.y-24<asteroida[i].y+64 && statek.y-24>asteroida[i].y)) {
			kolizja=1;  //statek z prawej u dolu
			break; }
		if ((statek.x+24<asteroida[i].x+64 && statek.x+24>asteroida[i].x) && (statek.y+24<asteroida[i].y+64 && statek.y+24>asteroida[i].y)) {
			kolizja=1;  //statek z lewej u gory
			break; }
		if ((statek.x+10<asteroida[i].x+64 && statek.x+10>asteroida[i].x) && (statek.y-24<asteroida[i].y+64 && statek.y-24>asteroida[i].y)) {
			kolizja=1;  //statek z leewj u dolu
			break; }
		if ((statek.x-24<asteroida[i].x+64 && statek.x-24>asteroida[i].x) && (statek.y+24<asteroida[i].y+64 && statek.y+24>asteroida[i].y)) {
			kolizja=1;  //statek z prawej u gory
			break; }
	}
		if (pkt50==1) {
		if ((pkt50_x+20>statek.x-24 && pkt50_x+20<statek.x+24) && (pkt50_y+20>statek.y-24 && pkt50_y+20<statek.y+24)) {
			kolizja_ze_znajdzka=1;  //statek z prawej u dolu
			}
		if ((pkt50_x<statek.x+24 && pkt50_x>statek.x-24) && (pkt50_y>statek.y-24 && pkt50_y<statek.y+24)) {
			kolizja_ze_znajdzka=1;  //statek z lewej u gory
			}
		if ((pkt50_x<statek.x+24 && pkt50_x>statek.x-24) && (pkt50_y+20>statek.y-24 && pkt50_y+20<statek.y+24)) {
			kolizja_ze_znajdzka=1;  //statek z leewj u dolu
			}
		if ((pkt50_x+20>statek.x-24 && pkt50_x+20<statek.x+24) && (pkt50_y>statek.y-24 && pkt50_y<statek.y+24)) {
			kolizja_ze_znajdzka=1;  //statek z prawej u gory
		}}

		if (bron_znajdzka.czy_jest==1) {
			if ((bron_znajdzka.x+20>statek.x-24 && bron_znajdzka.x+20<statek.x+24) && (bron_znajdzka.y+20>statek.y-24 && bron_znajdzka.y+20<statek.y+24)) {
			kolizja_ze_znajdzka=2;  //statek z prawej u dolu
			}
		if ((bron_znajdzka.x<statek.x+24 && bron_znajdzka.x>statek.x+24) && (bron_znajdzka.y>statek.y-24 && bron_znajdzka.y<statek.y+24)) {
			kolizja_ze_znajdzka=2;  //statek z lewej u gory
			}
		if ((bron_znajdzka.x<statek.x+24 && bron_znajdzka.x>statek.x+24) && (bron_znajdzka.y+20>statek.y-24 && bron_znajdzka.y+20<statek.y+24)) {
			kolizja_ze_znajdzka=2;  //statek z leewj u dolu
			}
		if ((bron_znajdzka.x+20>statek.x-24 && bron_znajdzka.x+20<statek.x+24) && (bron_znajdzka.y>statek.y-24 && bron_znajdzka.y<statek.y+24)) {
			kolizja_ze_znajdzka=2;  //statek z prawej u gory
		}}

		if (bomba.znajdzka==true) {
			if ((bomba.x+20>statek.x-24 && bomba.x+20<statek.x+24) && (bomba.y+20>statek.y-24 && bomba.y+20<statek.y+24)) {
			kolizja_ze_znajdzka=10;  //statek z prawej u dolu
			}
		if ((bomba.x<statek.x+24 && bomba.x>statek.x-24) && (bomba.y>statek.y-24 && bomba.y<statek.y+24)) {
			kolizja_ze_znajdzka=10;  //statek z lewej u gory
			}
		if ((bomba.x<statek.x+24 && bomba.x>statek.x-24) && (bomba.y+20>statek.y-24 && bomba.y+20<statek.y+24)) {
			kolizja_ze_znajdzka=10;  //statek z lewej u dolu
			}
		if ((bomba.x+20>statek.x-24 && bomba.x+20<statek.x+24) && (bomba.y>statek.y-24 && bomba.y<statek.y+24)) {
			kolizja_ze_znajdzka=10;  //statek z prawej u gory
		}}
	if (kolizja==1) return 1;					//gdy statek zderzy sie z asteroida
	if (kolizja_ze_znajdzka==1) return 50;		//gdy gracz zlapie "znajdzke"
	if (kolizja_ze_znajdzka==2) {				
		if (statek.rodzaj_broni==2) bron=load_bmp("obrazy/czerwona_bron.bmp",default_palette);
		if (statek.rodzaj_broni==1) bron=load_bmp("obrazy/zielona_bron.bmp",default_palette);
		if (!bron) {
			allegro_message("Wystapil blad z zaladowaniem bitmapy!");
			return 0;
		}
		return 2;
	}
	if (kolizja_ze_znajdzka==10) return 10;
	else return 0;
}

//=================Tablica wynikow=========================
int tablica_wynikow (){
	int j;
			//Otwarcie pliku
	plik.open("tablica_wynikow.txt",ios::in);
	if( plik.is_open() == false ) { 
		allegro_message("Nie udalo sie otworzyc pliku z najlepszymi wynikami"); 
		return 0; 
	}
	else { //gdy uda sie otworzyc plik to wykonuj...
	i=1;
	char tmp[30];
	while(!plik.eof() && i<=10) {
		sprintf_s(gracz[i], "%d", i);
		if (i<10) strcat_s(gracz[i],". ");
		if (i==10) strcat_s(gracz[i],".");
		plik.getline(tmp,27);		//Odczyt linii z pliku
		strcat_s(gracz[i],tmp);
		i++;
	}

	blit(tlo_tablicy_wynikow,bmp,0,0,0,0,800,600);
	for (j=1;j<i;j++) {	//Wypisywanie poszczegolnych wynikow
		textout_ex(bmp,r2014,gracz[j],150,200+(j*20),makecol(0,255,0),-1); }
	textout_ex(bmp,r2014,"Wciscnij ESC, zeby wyjsc do glownego menu",135,230+(i*20),makecol(0,255,0),-1);
	blit(bmp,screen,0,0,0,0,800,600);
	while(!key[KEY_ESC]);
	plik.close();
	}
return 0;
}

//================Dodawanie wyniku do tablicy wynikow=================
int dodaj_wynik_do_tablicy_wynikow (int wynik) {
	fstream plik;
	int wyniki[11],j;
	//char aktualny_gracz[30];
	char tmp[30],liczba[6];

	//strcpy_s(aktualny_gracz,nazwa_gracza);
	sprintf_s(tmp, "%d", wynik);
	wyniki[0]=wynik;
	//strcat_s(nazwa_gracza,tmp);
	strcpy_s(gracz[0],nazwa_gracza);
	strcat_s(gracz[0],tmp);
	//allegro_message(gracz[0]);
	plik.open("tablica_wynikow.txt", ios::in);		//--otwarcie pliku
	if( plik.is_open() == false ) 
		allegro_message("Nie udalo sie otworzyc pliku z najlepszymi wynikami");
	else {		//--gdy uda sie otworzyc plik to:
		i=1;
		char tmp[25];
		while(!plik.eof() && i<=10) {
		plik.getline(tmp,27);	//wczytywanie linii z pliku
		strcpy_s(gracz[i],tmp);
		for(j=0;j<6;j++) liczba[j]=tmp[j+19];
		liczba[6]='\0';
		wyniki[i]=atoi(liczba);
		//printf("Wynik: %d",wyniki[i]);
		//allegro_message(liczba);
		i++;
		}
		plik.close(); }
		for (i=0;i<=10;i++) {	//sortowanie wynikow
			for (j=0;j<=10;j++) {
				if (wyniki[j]<wyniki[j+1] && wyniki[j]>0 && wyniki[j+1]>0) {
					int pom=wyniki[j+1];
					char pom_str[30];
					strcpy_s(pom_str,gracz[j+1]);
					wyniki[j+1]=wyniki[j];
					wyniki[j]=pom;
					strcpy_s(gracz[j+1],gracz[j]);
					strcpy_s(gracz[j],pom_str);
				}}}
		//--Otwarcie pliku do zapisu
		plik.open("tablica_wynikow.txt", ios::out );
		if (plik.is_open()==false) {
			allegro_message("Nie udalo sie otworzyc pliku do zapisania tablicy wynikow");
			return 0; 
			}
		else {
		for (i=1;i<=10;i++) { //zapis do pliku
			plik<<gracz[i]<<endl;
			if (plik.fail()) {
				allegro_message("Nieudany zapis do pliku");
				return 0; }
			} 
		plik.close();
		strcpy_s(nazwa_gracza,"Gracz               ");		//Podstawienie po nazwe gracza tej nazwy by uniknac bledow
		//allegro_message("Plik zamkniety");
		}	
	//}
return 0;
}

//===========================Glowna procedura gry=========================

int gra (void) {
	install_timer();
	install_int_ex( increment_speed, BPS_TO_TIMER( 60 ) ); //ilosc klatek na sekunde
	speed=0;
	int pozycja_napisu_z_poziomem=-45;
	int polozenie_tla; int polozenie_tla2; int polozenie_gwiazd; int polozenie_gwiazd2; int polozenie_gwiazdy_smierci;
	int petla=0,czy_kolizja,j,los;
	char str_punkty[10],str_ile_pozostalo[10],str_poziom_gry[10];
	int przezroczystosc_eksplozji,przezroczystosc_napisu=0;
	char napis_z_poziomem[20];

	if (nowa_gra==1) {		//Gdy gracz rozpoczyna nowa gre
		statek.rodzaj_broni=1;
		bron=load_bmp("obrazy/czerwona_bron.bmp",default_palette);
		punkty=0;
		poziom_gry=1;
		ilosc_asteroid=10;
		bomba.ilosc=1;
		bomba.znajdzka=false;
		bomba.uzyto=false;
		pkt50=0;
		bron_znajdzka.czy_jest=0;
		polozenie_tla=-600;
		polozenie_tla2=-1800;
		polozenie_gwiazdy_smierci=-600;
		polozenie_gwiazd=-600;
		polozenie_gwiazd2=-1800;
		statek.x=400; statek.y=550;
		if (nowa_gra==1 && poziom_trudnosci==2) ilosc_asteroid+=10;
		if (dzwiek_wlaczony==true) play_midi(muzyczka,1);	//odtworz muzyczke w tle
	}

	napis=load_bmp("obrazy/napis.bmp",default_palette);
	_itoa_s(poziom_gry,str_poziom_gry,10);
	strcpy_s(napis_z_poziomem,"Poziom gry: ");
	strcat_s(napis_z_poziomem,str_poziom_gry);
	textout_ex(napis,cyberspace,napis_z_poziomem,0,0,makecol(0,255,0),-1);

	cout<<"Poziom gry: "<<poziom_gry<<endl;
	for(i=0;i<ilosc_asteroid;i++) {
		los=(rand()%100)+1;
		if (poziom_trudnosci==1) {	//gdy ustawiono latwy poziom
			if (los<=20) {//20% szansy na wylosowanie twardszych asteroid
				asteroida[i].bmp=load_bmp("obrazy/asteroida2.bmp",default_palette);
				asteroida[i].lvl=2;
				asteroida[i].wybuch=0;
				}
			if (los>20 && los<=25) {//5% szansy na wylosowanie najtwardszych asteroid
				asteroida[i].bmp=load_bmp("obrazy/asteroida3.bmp",default_palette);
				asteroida[i].lvl=3;
				asteroida[i].wybuch=0;
				}
			if (los>25) {
			asteroida[i].bmp=load_bmp("obrazy/asteroida.bmp",default_palette);
			asteroida[i].lvl=1;
			asteroida[i].wybuch=0;
			}}
		if (poziom_trudnosci==2) {	//gdy ustawiono trudny poziom
			if (los<=50) {//50% szansy na wylosowanie twardszych asteroid na trudniejszym poziomie
				asteroida[i].bmp=load_bmp("obrazy/asteroida2.bmp",default_palette);
				asteroida[i].lvl=2;
				asteroida[i].wybuch=0;
				}
			if (los>50 && los<=60) {//10% szansy na wylosowanie najtwardszych asteroid
				asteroida[i].bmp=load_bmp("obrazy/asteroida3.bmp",default_palette);
				asteroida[i].lvl=3;
				asteroida[i].wybuch=0;
				}
			if (los>60) {	//40% szans na wylosowanie zwyklych asteroid
			asteroida[i].bmp=load_bmp("obrazy/asteroida.bmp",default_palette);
			asteroida[i].lvl=1;
			asteroida[i].wybuch=0;
			}}
		if (!asteroida[i].bmp) {
			allegro_message("Wystapil blad podczas wczytywania %d asteroidy",i);
			return 0; }
	}

	ile_pozostalo=ilosc_asteroid;
	bomba.uzyto=false;
	przezroczystosc_eksplozji=255;
	//punkty=0;
	for(i=0;i<20;i++) {  //"zerowanie pociskow"
		pocisk[i].strzelono=0;
		pocisk[i].y=-10; 
	}
	
	srand(time(NULL));	   
	for(i=0;i<ilosc_asteroid;i++) {		//losowanie danych asteroid(wspolrzedne)
		asteroida[i].x=rand()%500+10;
		asteroida[i].y=rand()%50-150;
		if (poziom_trudnosci==1) asteroida[i].ry=rand()%3+1;
		else asteroida[i].ry=rand()%5+1;
		asteroida[i].rx=rand()%5-2;
		if (asteroida[i].lvl==1) asteroida[i].hp=10;	//podstawianie hp do asteroid w zaleznosci od ich poziomu
		if (asteroida[i].lvl==2) asteroida[i].hp=20;	
		if (asteroida[i].lvl==3) asteroida[i].hp=30;	
		asteroida[i].punkty=asteroida[i].hp;			//ilosc punktow za zestrzelenie asteroidy
		if (poziom_trudnosci==2) asteroida[i].punkty*=2;
		if (i>4) asteroida[i].y=asteroida[i].y-50*i; }
	
	do  //------glowna petla------
	{
	while (speed>0) {		//speed - od timera - utrzymuje stala predkosc
	blit(tlo,bmp,0,0,0,polozenie_tla,800,1200);	//"wklejanie" bitmap na ekran
	blit(tlo,bmp,0,0,0,polozenie_tla2,800,1200);
	masked_blit(gwiazda_smierci,bmp,0,0,0,polozenie_gwiazdy_smierci,800,1200);
	masked_blit(gwiazdy,bmp,0,0,0,polozenie_gwiazd,800,1200);
	masked_blit(gwiazdy2,bmp,0,0,0,polozenie_gwiazd2,800,1200);
	if (pozycja_napisu_z_poziomem <150) {
		if (przezroczystosc_napisu<255 && petla%1==0) przezroczystosc_napisu++;
		set_trans_blender( 0, 0, 0, przezroczystosc_napisu);
		draw_trans_sprite( bmp, napis, 120, pozycja_napisu_z_poziomem );
		pozycja_napisu_z_poziomem++; }
	else {
		if (przezroczystosc_napisu>=0) {
			if (petla%1==0) przezroczystosc_napisu--;
			set_trans_blender( 0, 0, 0, przezroczystosc_napisu);
			draw_trans_sprite( bmp, napis, 120, pozycja_napisu_z_poziomem );}}
	if (petla%5==0) {polozenie_tla+=1; polozenie_tla2+=1;}	//przesuwanie bitmap w dol
	if (petla%2==0) polozenie_gwiazdy_smierci++;
	polozenie_gwiazd+=3; 
	polozenie_gwiazd2+=3;
	if (polozenie_gwiazdy_smierci>600) polozenie_gwiazdy_smierci=-1200;
	if (polozenie_tla>=600) polozenie_tla=-1800;
	if (polozenie_tla2>=600) polozenie_tla2=-1800;
	if(polozenie_gwiazd>600) polozenie_gwiazd=-1800;
	if(polozenie_gwiazd2>600) polozenie_gwiazd2=-1800;

	for(i=0;i<ilosc_asteroid;i++) { 
		if (asteroida[i].hp<=0) continue;
		if(asteroida[i].x<=0||asteroida[i].x>=534) asteroida[i].rx=asteroida[i].rx*-1; //kolizja asteroidy ze sciana
		if(asteroida[i].y>600) asteroida[i].y=-64;		//teleportacja asteroidy
		asteroida[i].x=asteroida[i].x+asteroida[i].rx;	//przesuwanie asteroidy
		asteroida[i].y=asteroida[i].y+asteroida[i].ry; 
	}
	for (i=0;i<ilosc_asteroid;i++) {
		for (j=0;j<ilosc_asteroid;j++) {
		if (i==j) continue;
		else
			kolizja_asteroid2(i,j);		//kolizja pomiedzy asteroidami
		}}
	if (pkt50==1) pkt50_y+=2;			//przesuwanie "znajdziek"
	if (bron_znajdzka.czy_jest==1) bron_znajdzka.y+=2;
	if (bomba.znajdzka==true) bomba.y+=2;
	if (pkt50_y>630) pkt50=0;
	if (bron_znajdzka.y>630) bron_znajdzka.czy_jest=0;
	if (bomba.y>630) bomba.znajdzka=false;

	czy_kolizja=kolizja_statku();     //kolizja statku z innymi obiektami  
	if (czy_kolizja==1) return 0;
	if (czy_kolizja==50) {
		punkty+=50;
		pkt50=0; }
	if (czy_kolizja==2) {
		bool sprawdzono=false;
		if (statek.rodzaj_broni==1 && sprawdzono==false) {
			statek.rodzaj_broni=2;
			punkty+=20;
			sprawdzono=true;}
		if (statek.rodzaj_broni==2 && sprawdzono==false) {
			statek.rodzaj_broni=1;
			punkty+=20;
			sprawdzono=true;}
		bron_znajdzka.czy_jest=0;
	}
	if (czy_kolizja==10) {
		punkty+=20;
		bomba.znajdzka=false;
		bomba.ilosc++;
	}
    //reagowanie na klawisze
	if(key[KEY_Q]) {
		dzwiek_wlaczony=true;
		play_midi(muzyczka,0);}
	if(key[KEY_W]) {
		dzwiek_wlaczony=false;
		stop_midi();}

	if((key[KEY_LEFT])&&(statek.x>24)) {
		statek.x-=3; }
	if((key[KEY_RIGHT])&&(statek.x<576)) {
		statek.x+=3; }
	if((key[KEY_UP])&&(statek.y>24)) {
		statek.y-=3; }
	if((key[KEY_DOWN])&&(statek.y<576)) {
		statek.y+=3; }
	if(key[KEY_ESC]) {
		int przycisk;
		speed=0;
		przycisk=alert("Czy chcesz wyjsc z gry?","T/ENTER - Tak","N/ESC - Nie","Tak","Nie",84,78); 
		if (przycisk==1) return 0; 
		speed=0;
		//if (przycisk==2) {kl=0; return 5;}
	}

		strzal(petla);	//strzelanie

	for(i=0;i<20;i++) {  //rysowanie pociskow
	if (pocisk[i].strzelono==1) pocisk[i].y=pocisk[i].y-4;
	if (pocisk[i].y<-10) pocisk[i].strzelono=0;
	if (statek.rodzaj_broni==1) rectfill(bmp,pocisk[i].x-1,pocisk[i].y-5,pocisk[i].x+1,pocisk[i].y+5,makecol(0,255,0)); 
	if (statek.rodzaj_broni==2) rectfill(bmp,pocisk[i].x-2,pocisk[i].y-2,pocisk[i].x+2,pocisk[i].y+2,makecol(255,0,0)); 
	} 
	if (bomba.uzyto==true && przezroczystosc_eksplozji>0) {
		przezroczystosc_eksplozji--;
		set_trans_blender( 0, 0, 0, przezroczystosc_eksplozji);
		draw_trans_sprite( bmp, eksplozja, 0, 0 );}
	masked_blit(s,bmp,0,0,statek.x-24,statek.y-24,48,48);	//"wklejanie" bitmapy statku
	set_trans_blender( 0, 0, 0, 100);
	draw_trans_sprite( bmp, panel, 600, 0 );
	if (pkt50==1) masked_blit(plus_50,bmp,0,0,pkt50_x,pkt50_y,20,20);
	if (bron_znajdzka.czy_jest==1) masked_blit(bron,bmp,0,0,bron_znajdzka.x,bron_znajdzka.y,20,20);
	if (bomba.znajdzka==true) masked_blit(bomba_bmp,bmp,0,0,bomba.x,bomba.y,20,20);
	for(i=0;i<ilosc_asteroid;i++) {
		if (asteroida[i].wybuch>20) continue;
		masked_blit(asteroida[i].bmp,bmp,0,0,asteroida[i].x,asteroida[i].y,64,64); }
			
		for (i=0;i<ilosc_asteroid;i++) {
		if (asteroida[i].wybuch>0  && asteroida[i].x>-100) {
			set_trans_blender( 0, 0, 0, (asteroida[i].wybuch*8));
			draw_trans_sprite( bmp, wybuch, asteroida[i].x, asteroida[i].y );}
		}
		//"panel" z prawej strony ekranu
		vline(bmp,600,0,600,makecol(0,255,0));
		textout_ex(bmp, r2014, "Punkty", 610, 30,makecol(0, 255, 0), -1);
		_itoa_s(punkty,str_punkty,10);
		textout_ex(bmp, r2014, str_punkty, 610, 50,makecol(0, 255, 0), -1);
		_itoa_s(ile_pozostalo,str_ile_pozostalo,10);
		
		if (ile_pozostalo>4) {
		textout_ex(bmp, r2014, "Pozostalo", 610, 100,makecol(0, 255, 0), -1);
		textout_ex(bmp, r2014, str_ile_pozostalo, 610, 120,makecol(0, 255, 0), -1);
		textout_ex(bmp, r2014, "asteroid", 610, 140,makecol(0, 255, 0), -1);
		}
		if (ile_pozostalo>1 && ile_pozostalo <=4) {
		textout_ex(bmp, r2014, "Pozostaly", 610, 100,makecol(255, petla*2, 0), -1);
		textout_ex(bmp, r2014, str_ile_pozostalo, 610, 120,makecol(255, petla*2, 0), -1);
		textout_ex(bmp, r2014, "asteroidy", 610, 140,makecol(255, petla*2, 0), -1);
		}
		if (ile_pozostalo<=1) {
		textout_ex(bmp, r2014, "Pozostala", 610, 100,makecol(255, petla*5, 0), -1);
		textout_ex(bmp, r2014, str_ile_pozostalo, 610, 120,makecol(255, petla*5, 0), -1);
		textout_ex(bmp, r2014, "asteroida", 610, 140,makecol(255, petla*5, 0), -1);
		}
		
		_itoa_s(poziom_gry,str_poziom_gry,10);
		textout_ex(bmp, r2014, "Poziom gry", 610, 200, makecol(poziom_gry*10,255-(poziom_gry*10),0),-1);
		textout_ex(bmp, r2014, str_poziom_gry, 610, 220, makecol(poziom_gry*10,255-(poziom_gry*10),0),-1);
		if (bomba.uzyto==true || bomba.ilosc<=0) {
			textout_ex(bmp, r2014, "Bomby:", 610, 280, makecol(255,255-petla*5,0),-1);
			for (i=0;i<bomba.ilosc;i++) rectfill(bmp,610+i*20,300,630+i*20,320,makecol(255,255-petla*5,0));
		}		
		else textout_ex(bmp, r2014, "Bomby:", 610, 280, makecol(0,255,0),-1);
		for (i=0;i<bomba.ilosc;i++) {
			masked_blit(bomba_bmp,bmp,0,0,610+i*20,300,20,20);
		}
		textout_ex(bmp, r2014, "Dzwiek:", 610, 500, makecol(50,150,50),-1);
		textout_ex(bmp, r2014, "Q - wlaczony", 610, 520, makecol(50,150,50),-1);
		textout_ex(bmp, r2014, "W - wylaczony", 610, 540, makecol(50,150,50),-1);
	blit(bmp,screen,0,0,0,0,800,600);
	//rest(10);
	petla++;
	if (ile_pozostalo<=0  && bron_znajdzka.czy_jest==0 && pkt50==0 && bomba.znajdzka==false) {		//gdy wszystkie asteroidy zostaly zestrzelone
		poziom_gry++;
		if (poziom_gry<10 && ilosc_asteroid<50) ilosc_asteroid+=5;
		return poziom_gry;
	}
	speed--; 
	}}
	while (!key[KEY_BACKSPACE]);	//wykonuj petle, dopoki sa jeszcze asteroidy do zestrzelenia
		remove_int( increment_speed );
				stop_sample(laser);
				stop_midi();
	return 0;
}

//==================Wczytanie nazwy gracza============================

void wczytaj_nazwe_uzytkownika (void) {
	char znak=0;
	int i,j=0;
	for (i=0;i<20;i++) {
		if (nazwa_gracza[i]==' ' && nazwa_gracza[i+1]==' ') break;
	}
			blit(tlo,bmp,0,0,0,0,800,600);
			textout_ex(bmp,cyberspace,"Wpisz nazwe gracza:",100,150,makecol(0,255,0),-1);
			textout_ex(bmp,cyberspace,nazwa_gracza,50,250,makecol(0,255,0),-1);
			blit(bmp,screen,0,0,0,0,800,600);
	while (znak!=13) {	//enter
		blit(tlo,bmp,0,0,0,0,800,600);
		//rectfill(bmp,50+i*31,300,81+i*31,305,makecol(0,255,0));
	znak=readkey();
	if (dzwiek_wlaczony==true) play_sample(menu_zmiana,255,127,1000,0);
	if (znak==8) {	//8 - backspace
		nazwa_gracza[i-1]=0;
		i-=2;
	}
	if (znak>=32 && znak<127) nazwa_gracza[i]=znak; //32-127 zakres znakow tekstowych w ASCII
	textout_ex(bmp,cyberspace,"Wpisz nazwe gracza:",100,150,makecol(0,255,0),-1);
	textout_ex(bmp,cyberspace,nazwa_gracza,50,250,makecol(0,255,0),-1);
	blit(bmp,screen,0,0,0,0,800,600);
	i++;
	}
	nazwa_gracza[i-1]=0; //usuwanie znaku entera;
	for (j=i-1;j<30;j++) {
		nazwa_gracza[j]=32;	//spacja
	}
	for (i=20;i<30;i++) {
		nazwa_gracza[i]=0; }
	allegro_message("Zapisano nazwe gracza jako: %s",nazwa_gracza);
}

//===================================Menu glowne======================

int menu (void) {
	install_timer();
	install_int_ex( increment_speed, BPS_TO_TIMER( 15 ) ); //ilosc klatek na sekunde
	speed=0;
	clear_keybuf();
	int petla=0, kod, menu_item=0, polozenie_tla=-600, polozenie_tla2=-1800; int polozenie_gwiazd=-600; int polozenie_gwiazd2=-1800; int polozenie_gwiazdy_smierci=-600;
	int pozycja_napisow=605, pozycja_tytulu=-50;
	char item[7][40];
	char poz_tr[10];
	bool wcisnieto=false;
	int x_napisow=100;
	bool x_napisow_rosnace=true;

	//wpisywanie tekstu do zmiennych char[], ktore sa elementami menu glownego
	if (poziom_trudnosci==1) strcpy_s(poz_tr,"Latwy");
	if (poziom_trudnosci==2) strcpy_s(poz_tr,"Trudny");
	strcpy_s(item[0],"Rozpocznij gre"); strcpy_s(item[1],"Poziom trudnosci: "); strcpy_s(item[2],"Tablica wynikow"); strcpy_s(item[3],"Nazwa gracza"); 
	strcpy_s(item[4],"Pomoc"); strcpy_s(item[5],"Dzwiek: "); strcpy_s(item[6],"Wyjscie");
	strcat_s(item[1],poz_tr);

	//if (dzwiek_wlaczony==true) play_midi(muzyczka,1);
			//play_sample(laser,155,127,50,1);

	while (kl!=KEY_ENTER) {
			clear_keybuf();
				while (speed>0) {
					petla++;
		//dodatkowe informacje przy elementach menu
		if (poziom_trudnosci==1) strcpy_s(poz_tr,"Latwy");
		if (poziom_trudnosci==2) strcpy_s(poz_tr,"Trudny");
		strcpy_s(item[1],"Poziom trudnosci: ");
		strcat_s(item[1],poz_tr);
		strcpy_s(item[3],"Nazwa gracza: ");
		strcat_s(item[3],nazwa_gracza);
			if (dzwiek_wlaczony==true) {
				strcpy_s(item[5],"Dzwiek: ");
				strcat_s(item[5],"wlaczony"); }
			if (dzwiek_wlaczony==false) {
				strcpy_s(item[5],"Dzwiek: ");
				strcat_s(item[5],"wylaczony"); }
		if (petla%4==0) {	//przesuwanie tla
			polozenie_tla++;
			polozenie_tla2++; }
		if (petla%2==0) {
			polozenie_gwiazdy_smierci++; }
				polozenie_gwiazd+=3; polozenie_gwiazd2+=3;
		if (polozenie_tla>600) polozenie_tla=-1800;		//"teleportowanie" bitmap po ich 
		if (polozenie_tla2>600) polozenie_tla2=-1800;	//calkowitym wyjechaniu poza ekran
		if (polozenie_gwiazdy_smierci>600) polozenie_gwiazdy_smierci=-1800;
		
		blit(tlo_menu,bmp,0,0,0,polozenie_tla,800,1200);//wklejanie bitmap na ekran
		blit(tlo_menu2,bmp,0,0,0,polozenie_tla2,800,1200);
		masked_blit(gwiazda_smierci,bmp,0,0,0,polozenie_gwiazdy_smierci,800,1200);
		masked_blit(gwiazdy,bmp,0,0,0,polozenie_gwiazd,800,1200);
		masked_blit(gwiazdy2,bmp,0,0,0,polozenie_gwiazd2,800,1200);
		    if (pozycja_napisow>475) pozycja_napisow-=2;
		blit(psk,bmp,0,0,100,pozycja_napisow,114,125);
			//Inforamcje u dolu
			textout_ex(bmp,r2014,"Politechnika Swietokrzyska w Kielcach",225,pozycja_napisow,makecol(50,150,50),-1);
			textout_ex(bmp,r2014,"Projekt z programowania w jezyku C",225,pozycja_napisow+25,makecol(50,150,50),-1);
			textout_ex(bmp,r2014,"Rok akademicki: 2012/2013",225,pozycja_napisow+50,makecol(50,150,50),-1);
			textout_ex(bmp,r2014,"Prowadzacy: dr inz. Robert Tomaszewski",225,pozycja_napisow+75,makecol(50,150,50),-1);
			textout_ex(bmp,r2014,"Wykonawcy: Bebenek Rafal i Bartosinski Adrian",225,pozycja_napisow+100,makecol(50,150,50),-1);

		if (polozenie_gwiazd>600) polozenie_gwiazd=-1800;
		if (polozenie_gwiazd2>600) polozenie_gwiazd2=-1800;
		//Napis Space shooter
		if (x_napisow_rosnace==true && petla%5==0) {
			x_napisow++;
			if (x_napisow>=120) x_napisow_rosnace=false; }
		if (x_napisow_rosnace==false && petla%5==0) {
			x_napisow--;
			if (x_napisow<=80) x_napisow_rosnace=true; }

		if (pozycja_tytulu<40) pozycja_tytulu+=2;
		textout_ex(bmp,cyberspace,"Space shooter",200,pozycja_tytulu,makecol(0,255,0),-1);
		for (i=0;i<7;i++) {			//wyswietlanie poszczegolnych elementow menu
		if (i!=menu_item) textout_ex(bmp, r2014, item[i], x_napisow, 150+(i*20),makecol(50, 100, 50), -1);
		if (i==menu_item) {			//Podswietlenie konkretnego elementu menu
		textout_ex(bmp, r2014, item[i], x_napisow+20, 150+(i*20),makecol(0, 255, 0), -1);
		if (i==1) textout_ex(bmp,r2014,"--ENTER--",x_napisow+400,170,makecol(0,255,0),-1);
		if (i==3) textout_ex(bmp,r2014,"--ENTER--",x_napisow+450,210,makecol(0,255,0),-1);
		if (i==5) textout_ex(bmp,r2014,"--ENTER--",x_napisow+400,250,makecol(0,255,0),-1);
		}}
		blit(bmp,screen,0,0,0,0,800,600);
		//reagowanie na wcisniecie klawiszy
		if (key[KEY_DOWN]) {
			menu_item++;
			if (dzwiek_wlaczony==true) play_sample(menu_zmiana,255,127,1000,0);
			if (menu_item>6) menu_item=0; 
			}
		if (key[KEY_UP]) {
			menu_item--;
			if (dzwiek_wlaczony==true) play_sample(menu_zmiana,255,127,1000,0);
			if (menu_item<0) menu_item=6;}
		//if (key[KEY_ESC]) return 0;
		if (key[KEY_ENTER]) {
			if(menu_item==0) {
				kod=gra(); //wywolanie nowej gry
				nowa_gra=0;
				
				while(kod!=0) {	//gdy gracz przechodzi do nastepnego poziomu
					if (kod>1) { /*allegro_message("Gratulacje - przechodzisz do %d poziomu",poziom_gry);*/ kod=gra();}}
				if (kod==0) {	//koniec gry
					stop_sample(laser);
					allegro_message("Koniec Gry - ilosc zdobytych punktow: %d",punkty);
					if (punkty>0) dodaj_wynik_do_tablicy_wynikow(punkty);
					stop_midi();
					nowa_gra=1;
					speed=0;
					install_int_ex( increment_speed, BPS_TO_TIMER( 15 ) ); //ilosc klatek na sekunde
				}
			}
			if (menu_item==1) {	//zmiana poziomu trudnosci
				if (poziom_trudnosci==1) poziom_trudnosci=2;
				else poziom_trudnosci=1;
			}
			if (menu_item==2) {	//tablica najlepszych wynikow
				tablica_wynikow();
				speed=0; 
			}
			if (menu_item==3) {	//zmiana nazwy gracza
				wczytaj_nazwe_uzytkownika();
				speed=0;
			}
			if (menu_item==4) {	//pomoc do gry
				int ktora_strona=1; int petla=0;
				while(!key[KEY_ESC]) {
				petla++;
				if (ktora_strona==1) blit(pomoc,bmp,0,0,0,0,800,600);
				else blit(pomoc2,bmp,0,0,0,0,800,600);
				if (key[KEY_LEFT] || key[KEY_RIGHT]) {
					rest(100);
					if (ktora_strona==1) ktora_strona=2;
					else ktora_strona=1; }
				textout_ex(bmp,r2014,"Strzalka w lewo lub prawo - przelaczenie pomiedzy stronami pomocy",25,560,makecol(0,petla,0),-1);
				textout_ex(bmp,r2014,"ESC - wyjscie do glownego menu",200,580,makecol(0,petla,0),-1);
				blit(bmp,screen,0,0,0,0,800,600);
				}
				speed=0;
			}
			if (menu_item==5) {	//wlaczenie/wylaczenie dzieku w grze
				if (dzwiek_wlaczony==true) dzwiek_wlaczony=false;
				else dzwiek_wlaczony=true;
			}
			if(menu_item==6) return 0;} //wyjscie z gry
		//rest(5);
				speed--;}}
		remove_int( increment_speed );
	return 0;
}

/*======================Program g³ówny==============================
====================================================================*/ 
int main(void)
{
	int kod_menu;
	nowa_gra=1;

	int kod_bledu=inicjalizacja(); //inicjalizacja i sprawdzenie czy wszystko w porzadku
	if (kod_bledu==0) {
		allegro_message("Wystapil blad podczas inicjalizacji\nProgram zostanie zamkniety\nSprawdz, czy wszystkie pliki znajduja sie w folderze z gra");
		allegro_exit();
		return 0; }
	
	//gra();
	kod_menu=menu();
	if (kod_menu==5) menu();
	cout<<"Usuwanie bitmap, czcionek i dzwiekow";
		//if (kod_menu==0) return 0;
	//Usuwanie bitmap, czcionek i dzwiekow, zeby uniknac "wyciekow pamieci"
	destroy_bitmap(bmp);
	destroy_bitmap(s);
	destroy_bitmap(tlo);
	destroy_bitmap(tlo2);
	destroy_bitmap(tlo_menu);
	destroy_bitmap(tlo_menu2);
	destroy_bitmap(gwiazda_smierci);
	destroy_bitmap(pomoc);
	destroy_bitmap(pomoc2);
	destroy_bitmap(eksplozja);
	destroy_bitmap(wybuch);
	for (i=0;i<50;i++) destroy_bitmap(asteroida[i].bmp);
	destroy_font(cyberspace);
	destroy_font(r2014);
	destroy_midi(muzyczka);
	destroy_sample(laser);
	destroy_sample(menu_zmiana);
	destroy_sample(wybuch_sample);
    allegro_exit();
	return 0;
}