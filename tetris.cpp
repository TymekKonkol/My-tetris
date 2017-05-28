#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <iostream>
#include <time.h>
 
#include <ctype.h>
 
 
extern "C" {
//#ifdef BIT64
//#include"./sdl64/include/SDL.h"
//#include"./sdl64/include/SDL_main.h"
//#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
//#endif
}
 
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
 
 
//WYSOKOSC POLA NIE MOZE BYC WIEKSZA NIZ 19 BO WYJDZIE POZA DOMYSLNY EKRAN CO ZCRASHUJE PROGRAM (PRZY WIELKOSCI KLOCKA 25)
//MINIMALNA WYSOKOSC TO 6 ABY GRA SIE NIE ZACZELA KONCEM
 
//SZEROKOSC POLA NIE MOZE BYC WIEKSZA NIZ 25 BO WYJDZIE POZA DOMYSLNY EKRAN (PRZY WIELKOSCI KLOCKA 25)
//MINIMALNA SZEROKOSC TO 5 ABY GRA SIE NIE SKONCZYLA KONCEM (KLOCEK NA START W SCIANIE)
 
//DOMYSLNIE 14
#define SZEROKOSC_POLA 14
//DOMYSLNIE 19
#define WYSOKOSC_POLA 19
 
//SZEROKOSC I WYSOKOSC POLA SA PODANE W ILOSCI KLOCKOW, KLOCKI MAJA OKRESLONA WIELKOSC
//DOMYSLNIE 25
#define WIELKOSC_KLOCKA 25
 
// narysowanie napisu txt na powierzchni screen, zaczynaj?c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj?ca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
        int px, py, c;
        SDL_Rect s, d;
        s.w = 8;
        s.h = 8;
        d.w = 8;
        d.h = 8;
        while(*text) {
                c = *text & 255;
                px = (c % 16) * 8;
                py = (c / 16) * 8;
                s.x = px;
                s.y = py;
                d.x = x;
                d.y = y;
                SDL_BlitSurface(charset, &s, screen, &d);
                x += 8;
                text++;
                };
        };
 
 
// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt ?rodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
        SDL_Rect dest;
        dest.x = x - sprite->w / 2;
        dest.y = y - sprite->h / 2;
        dest.w = sprite->w;
        dest.h = sprite->h;
        SDL_BlitSurface(sprite, NULL, screen, &dest);
        };
 
 
// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
        int bpp = surface->format->BytesPerPixel;
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
        *(Uint32 *)p = color;
        };
 
 
// rysowanie linii o d?ugo?ci l w pionie (gdy dx = 0, dy = 1) 
// b?d? poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
        for(int i = 0; i < l; i++) {
                DrawPixel(screen, x, y, color);
                x += dx;
                y += dy;
                };
        };
 
 
// rysowanie prostok?ta o d?ugo?ci bok?w l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
        int i;
        DrawLine(screen, x, y, k, 0, 1, outlineColor);
        DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
        DrawLine(screen, x, y, l, 1, 0, outlineColor);
        DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
        for(i = y + 1; i < y + k - 1; i++)
                DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
        };
 
 
 
 
struct klocki
{
        int x1, y1;
        int x2, y2;
        int x3, y3;
        int x4, y4;
        Uint32 kolor;
        Uint32 kolor_wypelnienia;
        char znak;
};
 
struct rodzaje_klockow
{
                klocki dlugi_poziom, dlugi_pion, bolec_dol, bolec_gora, bolec_praw, bolec_lewo, l_prawo, l_dol, l_lewo, l_gora, odwrotne_l_lewo, odwrotne_l_gora, odwrotne_l_prawo, odwrotne_l_dol, s_poziom, s_pion, wonsz_poziom, wonsz_pion;
}rodzaje;
 
 
//moje funkcje
 
void zapisz_do_pliku(char pole_gry[][SZEROKOSC_POLA], double przeskok, int etap, double punkty)
{
        FILE *plik;
        
        plik = fopen("zapis.txt", "w");
        
        
        //zapisanie predkosci gry
        fprintf(plik, "%lf %d %lf", przeskok, etap, punkty);
        //zapisanie pola gry do pliku
        for (int i = 0; i < WYSOKOSC_POLA; i++)
        {
                for (int j = 0; j < SZEROKOSC_POLA; j++)
                {
                        fputc(pole_gry[i][j], plik);                    
                }
                fputc('\n', plik);
        }
 
        
        
        
 
        fclose(plik);
}
 
 
void wczytaj(char pole_gry[][SZEROKOSC_POLA], int *etap, double *przeskok,  double *punkty, klocki *aktualny)
{
        FILE *plik;
        int i = 0, j = 0, ktory_klocek=0;
        char znak = ' ';
        int etap_z_pliku;
        double przeskok_z_pliku = 0.3;
        double pkty_z_pliku = 333;
        fpos_t pozycja;
 
        plik = fopen("zapis.txt", "r");
        if (plik == NULL)
        { 
        }
        else
        {
                fscanf(plik, "%lf", &przeskok_z_pliku);
                fscanf(plik, "%d", &etap_z_pliku);
                fscanf(plik, "%lf", &pkty_z_pliku);
                
                while (i <18)
                {
                        j = 0;
                        
                                while ((znak = fgetc(plik)) != '\n')
                                {
                                        if (znak == 'D' || znak == 'B' || znak == 'K' || znak == 'L' || znak == 'O' || znak == 'S' || znak == 'W')
                                        {
                                                if (ktory_klocek == 0)
                                                {
                                                        (*aktualny).x1 = i;
                                                        (*aktualny).y1 = j;
                                                        (*aktualny).znak = znak;
                                                        ktory_klocek++;
                                                }
                                                else if (ktory_klocek == 1)
                                                {
                                                        (*aktualny).x2 = i;
                                                        (*aktualny).y2 = j;
                                                        (*aktualny).znak = znak;
                                                        ktory_klocek++;
                                                }
                                                else if (ktory_klocek == 2)
                                                {
                                                        (*aktualny).x3 = i;
                                                        (*aktualny).y3 = j;
                                                        (*aktualny).znak = znak;
                                                        ktory_klocek++;
                                                }
                                                else if (ktory_klocek == 3)
                                                {
                                                        (*aktualny).x4 = i;
                                                        (*aktualny).y4 = j;
                                                        (*aktualny).znak = znak;
                                                }
                                        }
                                        pole_gry[i][j] = znak;
                                        if (znak == EOF)
                                        {
                                                break;
                                        }
                                        j++;
                                }
                                i++;
                        
                }
        }
        
        
 
        *przeskok = przeskok_z_pliku;
        *etap = etap_z_pliku;
        *punkty = pkty_z_pliku;
 
        fclose(plik);
}
 
 
 
void zmiana_wg_obrotow(int liczni_obrotow, klocki *dlugi, klocki *bolec, klocki *l, klocki *odwrotne_l, klocki *s, klocki *wonsz, rodzaje_klockow rodzaje)
{
        if (liczni_obrotow == 0)
        {
                *dlugi = rodzaje.dlugi_pion;
                *bolec = rodzaje.bolec_dol;
                *l = rodzaje.l_prawo;
                *odwrotne_l = rodzaje.odwrotne_l_lewo;
                *s = rodzaje.s_poziom;
                *wonsz = rodzaje.wonsz_poziom;
        }
        else if (liczni_obrotow == 1)
        {
                *dlugi = rodzaje.dlugi_poziom;
                *bolec = rodzaje.bolec_lewo;
                *l = rodzaje.l_dol;
                *odwrotne_l = rodzaje.odwrotne_l_gora;
                *s = rodzaje.s_pion;
                *wonsz = rodzaje.wonsz_pion;
        }
        else if (liczni_obrotow == 2)
        {
                *dlugi = rodzaje.dlugi_pion;
                *bolec = rodzaje.bolec_gora;
                *l = rodzaje.l_lewo;
                *odwrotne_l = rodzaje.odwrotne_l_prawo;
                *s = rodzaje.s_poziom;
                *wonsz = rodzaje.wonsz_poziom;
        }
        else if (liczni_obrotow == 3)
        {
                *dlugi = rodzaje.dlugi_poziom;
                *bolec = rodzaje.bolec_praw;
                *l = rodzaje.l_gora;
                *odwrotne_l = rodzaje.odwrotne_l_dol;
                *s = rodzaje.s_pion;
                *wonsz = rodzaje.wonsz_pion;
 
        }
}
 
void przesuniecie_klocka(klocki *zastepczy, int spadek_w_dol, int przesuniecie_w_bok)
{
        (*zastepczy).x1 += spadek_w_dol;
        (*zastepczy).x2 += spadek_w_dol;
        (*zastepczy).x3 += spadek_w_dol;
        (*zastepczy).x4 += spadek_w_dol;
 
        (*zastepczy).y1 += przesuniecie_w_bok;
        (*zastepczy).y2 += przesuniecie_w_bok;
        (*zastepczy).y3 += przesuniecie_w_bok;
        (*zastepczy).y4 += przesuniecie_w_bok;
}
 
void podmiana_klocka_zas(klocki aktualny, klocki dlugi, klocki kwadrat, klocki bolec, klocki l, klocki odwrotne_l, klocki s, klocki wonsz, klocki *zastepczy)
{
        if (aktualny.znak == 'D')
                *zastepczy = dlugi;
        else if (aktualny.znak == 'B')
                *zastepczy = bolec;
        else if (aktualny.znak == 'K')
                *zastepczy = kwadrat;
        else if (aktualny.znak == 'L')
                *zastepczy = l;
        else if (aktualny.znak == 'O')
                *zastepczy = odwrotne_l;
        else if (aktualny.znak == 'S')
                *zastepczy = s;
        else if (aktualny.znak == 'W')
                *zastepczy = wonsz;
}
 
void podmiana_klocka_losowym(klocki *aktualny, int losowa_liczba, klocki dlugi, klocki kwadrat, klocki bolec, klocki l, klocki odwrotne_l, klocki s, klocki wonsz)
{
        if (losowa_liczba == 1)
                *aktualny = dlugi;
        else if (losowa_liczba == 2)
                *aktualny = bolec;
        else if (losowa_liczba == 3)
                *aktualny = kwadrat;
        else if (losowa_liczba == 4)
                *aktualny = l;
        else if (losowa_liczba == 5)
                *aktualny = odwrotne_l;
        else if (losowa_liczba == 6)
                *aktualny = s;
        else if (losowa_liczba == 7)
                *aktualny = wonsz;
}
 
void zmien_pole_pod_aktualnym(char pole_gry[][SZEROKOSC_POLA], klocki aktualny,char znak)
{
        pole_gry[aktualny.x1][aktualny.y1] = znak;
        pole_gry[aktualny.x2][aktualny.y2] = znak;
        pole_gry[aktualny.x3][aktualny.y3] = znak;
        pole_gry[aktualny.x4][aktualny.y4] = znak;
}
 
void wypelnij_pole(char pole_gry[][SZEROKOSC_POLA])
{
        for (int i = 0; i < WYSOKOSC_POLA; i++)
        {
                for (int j = 0; j < SZEROKOSC_POLA; j++)
                {
                        if (j == 0 || j == SZEROKOSC_POLA-1 || i == WYSOKOSC_POLA-1)
                                pole_gry[i][j] = 'x';
                        else
                                pole_gry[i][j] = ' ';
                }
        }
}
 
void skret(klocki *aktualny, char znak)
{
        if (znak == 'p')
        {
                (*aktualny).y1++;
                (*aktualny).y2++;
                (*aktualny).y3++;
                (*aktualny).y4++;
        }
        else if (znak == 'l')
        {
                (*aktualny).y1--;
                (*aktualny).y2--;
                (*aktualny).y3--;
                (*aktualny).y4--;
        }
        else if (znak == 'x')
        {
                (*aktualny).x1++;
                (*aktualny).x2++;
                (*aktualny).x3++;
                (*aktualny).x4++;
        }
        
}
void usun_pelne_wiersze(char pole_gry[][SZEROKOSC_POLA],klocki aktualny, int *ile_skasowanych_wierszy)
{
        bool caly_pasek = true;
        for (int i = 0; i < WYSOKOSC_POLA-1; i++)
        {
                caly_pasek = true;
                for (int j = 0; j < SZEROKOSC_POLA; j++)
                {
                        if (pole_gry[i][j] == ' ' || pole_gry[i][j] == aktualny.znak)
                        {
                                caly_pasek = false;
                        }
                }
                if (caly_pasek == true)
                {
                        *ile_skasowanych_wierszy+=1;
                        for (int t = i; t > 0; t--)
                        {
                                for (int j = 1; j < SZEROKOSC_POLA-1; j++)
                                {
                                        pole_gry[t][j] = pole_gry[t - 1][j];
                                }
                        }
                        
                }
 
        }
}
 
void dodaj_punkty(int ile_skasowanych_wierszy, int etap, bool *mnoznik_pktow, double *punkty)
{
        if (ile_skasowanych_wierszy == 1)
        {
                *punkty += 100 * (etap + 1);
                *mnoznik_pktow = false;
        }
        else if (ile_skasowanych_wierszy == 2)
        {
                *punkty += 200 * (etap + 1);
                *mnoznik_pktow = false;
        }
        else if (ile_skasowanych_wierszy == 3)
        {
                *punkty += 400 * (etap + 1);
                *mnoznik_pktow = false;
        }
        else if (ile_skasowanych_wierszy == 4 && *mnoznik_pktow)
                *punkty += 1200 * (etap + 1);
        else if (ile_skasowanych_wierszy == 4)
        {
                *punkty += 800 * (etap + 1);
                *mnoznik_pktow = true;
        }
}
 
 
void rysuj_pole_gry(char pole_gry[][SZEROKOSC_POLA], SDL_Surface *screen, klocki dlugi, klocki bolec, klocki kwadrat, klocki l, klocki odwrotne_l, klocki s, klocki wonsz)
{
        for (int i = 0; i < WYSOKOSC_POLA; i++)
        {
                for (int j = 0; j < SZEROKOSC_POLA; j++)
                {
                        if (pole_gry[i][j] == 'x')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, 0x696969, 0x000000);
                        else if (pole_gry[i][j] == 'D' || pole_gry[i][j] == 'd')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, dlugi.kolor, dlugi.kolor_wypelnienia);
                        else if (pole_gry[i][j] == 'B' || pole_gry[i][j] == 'b')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, bolec.kolor, bolec.kolor_wypelnienia);
                        else if (pole_gry[i][j] == 'K' || pole_gry[i][j] == 'k')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, kwadrat.kolor, kwadrat.kolor_wypelnienia);
                        else if (pole_gry[i][j] == 'L' || pole_gry[i][j] == 'l')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, l.kolor, l.kolor_wypelnienia);
                        else if (pole_gry[i][j] == 'O' || pole_gry[i][j] == 'o')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, odwrotne_l.kolor, odwrotne_l.kolor_wypelnienia);
                        else if (pole_gry[i][j] == 'S' || pole_gry[i][j] == 's')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, s.kolor, s.kolor_wypelnienia);
                        else if (pole_gry[i][j] == 'W' || pole_gry[i][j] == 'w')
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, wonsz.kolor, wonsz.kolor_wypelnienia);
                        else
                                DrawRectangle(screen, j * WIELKOSC_KLOCKA, i * WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, WIELKOSC_KLOCKA, 0x000000, 0x000000);
                }
 
        }
}
 
 
 
 
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
        int t1, t2, quit, frames, rc;
        double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
        SDL_Event event;
        //SDL_Event koniec;
        SDL_Surface *screen, *charset;
        SDL_Surface *eti;
        SDL_Texture *scrtex;
        SDL_Window *window;
        SDL_Renderer *renderer;
 
        if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
                printf("SDL_Init error: %s\n", SDL_GetError());
                return 1;
                }
 
        // tryb pe?noekranowy
//      rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//                                       &window, &renderer);
        rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                         &window, &renderer);
        if(rc != 0) {
                SDL_Quit();
                printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
                return 1;
                };
        
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
 
        SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2014");
 
 
        screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                      0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
 
        scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   SCREEN_WIDTH, SCREEN_HEIGHT);
 
 
        // wy??czenie widoczno?ci kursora myszy
        SDL_ShowCursor(SDL_DISABLE);
 
        // wczytanie obrazka cs8x8.bmp
        charset = SDL_LoadBMP("./cs8x8.bmp");
        if(charset == NULL) {
                printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
                SDL_FreeSurface(screen);
                SDL_DestroyTexture(scrtex);
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                SDL_Quit();
                return 1;
                };
        SDL_SetColorKey(charset, true, 0x000000);
 
        eti = SDL_LoadBMP("./eti.bmp");
        if(eti == NULL) {
                printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
                SDL_FreeSurface(charset);
                SDL_FreeSurface(screen);
                SDL_DestroyTexture(scrtex);
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                SDL_Quit();
                return 1;
                };
 
        char text[128];
        int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
        int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
        int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
        int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
 
        t1 = SDL_GetTicks();
 
        frames = 0;
        fpsTimer = 0;
        fps = 0;
        quit = 0;
        worldTime = 0;
        distance = 0;
        etiSpeed = 1;
 
        // moje definicje
        int szerokosc = SZEROKOSC_POLA;
        int wysokosc = WYSOKOSC_POLA;
 
        bool nowa_gra=true;
        double worldTime_pauza;
        double czas_pauza;
        bool pause=false;       
 
        //obrot klocka
        int spadek_w_dol = 0;
        int przesuniecie_w_bok = 0;
        int liczni_obrotow = 0;
 
        srand(time(NULL));
        int losowa_liczba;
 
        //spadanie
        const double predkosc = 0.5; //przerwa pomiedzy skokami klockow, podane w sekundach
        double czas = predkosc;
        double przeskok = czas;
 
        double przeskok_przed_dolem = predkosc;
        
        //etapowosc
        //CO_ILE_NOWY_ETAP PODAJE CZAS W SEKUNDACH DO NASTEPNEGO ETAPU OD STARUT LUB WSKOCZENIA NOWEGO ETAPU
        const double co_ile_nowy_etap_T = 30;
        int przeskok_w_etapie = co_ile_nowy_etap_T;
        
        //MAX ILOSC ETAPOW ZALEZNE OD ZMNIEJSZENIE_PREDKOSCI_CO_ETAP (PRZY 0.04 MAX 9(10) ETAPOW)
        //MAX ZMNIEJSZENIE_PREDKOSCI_CO_ETAP ZALEZNE OD ILOSCI ETAPOW
        const int ile_etapow = 9;
        int etap = 0;
        const double zmniejszesznie_predkosci_co_etap = 0.04; //o ile zmniejszana jest przerwa pomiedzy skokami klockow, podane w sekundach
        double o_ile_przeskok_etap = zmniejszesznie_predkosci_co_etap;
 
        double etapTime=0;
 
        //pkty
        double punkty = 0;
        int ile_skasowanych_wierszy = 0;
        bool mnoznik_pktow = false;
 
        //zapisywanie stanu gry
        bool czy_zapisano = false;
        int ile_razy_zapisano = 0;
 
        char czy_koniec='f';
        char czy_klawisz_dol = false;
 
        //schematy klockow
        //                           x1     y1                          x2         y2                  x3       y3                            x4     y4                               kolor     kolor2
        struct klocki dlugi =      { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     3, SZEROKOSC_POLA / 2,     0xFF0000, 0x8B0000, 'D' };
        struct klocki bolec =      { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
        struct klocki kwadrat =    { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2,     0x00FFFF, 0x5F9EA0, 'K' };
        struct klocki l =          { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     2, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
        struct klocki odwrotne_l = { 0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 2, (SZEROKOSC_POLA / 2)+1, 2, SZEROKOSC_POLA / 2,     0xFF00FF, 0x8B008B, 'O' };
        struct klocki s =          { 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2,     0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0x00008B, 0x191970, 'S' };
        struct klocki wonsz =      { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     1, (SZEROKOSC_POLA / 2)+1, 0x00FF00, 0x006400, 'W' };
        struct klocki aktualny =   { 0, 0, 0, 0, 0, 0, 0, 0, 0x000000, 0x000000, ' ' };
        struct klocki zastepczy =  { 0, 0, 0, 0, 0, 0, 0, 0, 0x000000, 0x000000, ' ' };
        
 
        //rozne obroty
                /*
                //dlugi
                //                           x1     y1                          x2         y2                  x3       y3                            x4     y4                               kolor     kolor2
                rodzaje.dlugi_poziom =     { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0, (SZEROKOSC_POLA / 2)+2, 0xFF0000, 0x8B0000, 'D' };
                rodzaje.dlugi_pion =       { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     3, SZEROKOSC_POLA / 2,     0xFF0000, 0x8B0000, 'D' };
                //bolec
                rodzaje.bolec_dol =                { 0, (SZEROKOSC_POLA / 2)-1,   0, SZEROKOSC_POLA / 2, 1, SZEROKOSC_POLA / 2, 0, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_lewo =       { 0, SZEROKOSC_POLA / 2,       1, SZEROKOSC_POLA / 2, 2, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)-1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_gora =       { 1, (SZEROKOSC_POLA / 2)-1,   1, SZEROKOSC_POLA / 2, 0, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_praw =       { 0, SZEROKOSC_POLA / 2,       1, SZEROKOSC_POLA / 2, 2, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
                //l
                rodzaje.l_prawo =                  { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     2, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_dol =                    { 1, (SZEROKOSC_POLA / 2)-1, 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_lewo =                   { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_gora =                   { 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2,     1, (SZEROKOSC_POLA / 2)+1, 0, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
                //odwrotne l
                rodzaje.odwrotne_l_lewo =  { 0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 2, (SZEROKOSC_POLA / 2)+1, 2, SZEROKOSC_POLA / 2,     0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_gora =  { 0, (SZEROKOSC_POLA / 2)-1, 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2,     1, (SZEROKOSC_POLA / 2)+1, 0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_prawo = { 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_dol =   { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 0xFF00FF, 0x8B008B, 'O' };
                //s
                rodzaje.s_poziom =                 { 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0x00008B, 0x191970, 'S' };
                rodzaje.s_pion =                   { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 2, (SZEROKOSC_POLA / 2)+1, 0x00008B, 0x191970, 'S' };
                //wonsz
                rodzaje.wonsz_poziom =     { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 0x00FF00, 0x006400, 'W' };
                rodzaje.wonsz_pion =       { 0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 1, SZEROKOSC_POLA / 2, 2, SZEROKOSC_POLA / 2,     0x00FF00, 0x006400, 'W' };
                */
                //dlugi poziom
                rodzaje.dlugi_poziom.x1 = 0;
                rodzaje.dlugi_poziom.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.dlugi_poziom.x2 = 0;
                rodzaje.dlugi_poziom.y2 = SZEROKOSC_POLA / 2;
                rodzaje.dlugi_poziom.x3 = 0;
                rodzaje.dlugi_poziom.y3 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.dlugi_poziom.x4 = 0;
                rodzaje.dlugi_poziom.y4 = (SZEROKOSC_POLA / 2) + 2;
                rodzaje.dlugi_poziom.kolor = 0xFF0000;
                rodzaje.dlugi_poziom.kolor_wypelnienia = 0x8B0000;
                rodzaje.dlugi_poziom.znak = 'D';
                //dlugi pion
                rodzaje.dlugi_pion.x1 = 0;
                rodzaje.dlugi_pion.y1 = SZEROKOSC_POLA / 2;
                rodzaje.dlugi_pion.x2 = 1;
                rodzaje.dlugi_pion.y2 = SZEROKOSC_POLA / 2;
                rodzaje.dlugi_pion.x3 = 2;
                rodzaje.dlugi_pion.y3 = SZEROKOSC_POLA / 2;
                rodzaje.dlugi_pion.x4 = 3;
                rodzaje.dlugi_pion.y4 = SZEROKOSC_POLA / 2;
                rodzaje.dlugi_pion.kolor = 0xFF0000;
                rodzaje.dlugi_pion.kolor_wypelnienia = 0x8B0000;
                rodzaje.dlugi_pion.znak = 'D';
                //bolec dol { 0, (SZEROKOSC_POLA / 2)-1,   0, SZEROKOSC_POLA / 2, 1, SZEROKOSC_POLA / 2, 0, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_dol.x1 = 0;
                rodzaje.bolec_dol.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.bolec_dol.x2 = 0;
                rodzaje.bolec_dol.y2 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_dol.x3 = 1;
                rodzaje.bolec_dol.y3 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_dol.x4 = 0;
                rodzaje.bolec_dol.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.bolec_dol.kolor = 0x4B0082;
                rodzaje.bolec_dol.kolor_wypelnienia = 0x483D8B;
                rodzaje.bolec_dol.znak = 'B';
                //bolec lewo { 0, SZEROKOSC_POLA / 2,       1, SZEROKOSC_POLA / 2, 2, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)-1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_lewo.x1 = 0;
                rodzaje.bolec_lewo.y1 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_lewo.x2 = 1;
                rodzaje.bolec_lewo.y2 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_lewo.x3 = 2;
                rodzaje.bolec_lewo.y3 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_lewo.x4 = 1;
                rodzaje.bolec_lewo.y4 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.bolec_lewo.kolor = 0x4B0082;
                rodzaje.bolec_lewo.kolor_wypelnienia = 0x483D8B;
                rodzaje.bolec_lewo.znak = 'B';
                //bolec gora  { 1, (SZEROKOSC_POLA / 2)-1,   1, SZEROKOSC_POLA / 2, 0, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_gora.x1 = 1;
                rodzaje.bolec_gora.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.bolec_gora.x2 = 1;
                rodzaje.bolec_gora.y2 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_gora.x3 = 0;
                rodzaje.bolec_gora.y3 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_gora.x4 = 1;
                rodzaje.bolec_gora.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.bolec_gora.kolor = 0x4B0082;
                rodzaje.bolec_gora.kolor_wypelnienia = 0x483D8B;
                rodzaje.bolec_gora.znak = 'B';
                //bole prawo  { 0, SZEROKOSC_POLA / 2,       1, SZEROKOSC_POLA / 2, 2, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 0x4B0082, 0x483D8B, 'B' };
                rodzaje.bolec_praw.x1 = 0;
                rodzaje.bolec_praw.y1 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_praw.x2 = 1;
                rodzaje.bolec_praw.y2 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_praw.x3 = 2;
                rodzaje.bolec_praw.y3 = SZEROKOSC_POLA / 2;
                rodzaje.bolec_praw.x4 = 1;
                rodzaje.bolec_praw.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.bolec_praw.kolor = 0x4B0082;
                rodzaje.bolec_praw.kolor_wypelnienia = 0x483D8B;
                rodzaje.bolec_praw.znak = 'B';
                //l prawo  { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     2, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_prawo.x1 = 0;
                rodzaje.l_prawo.y1 = SZEROKOSC_POLA / 2;
                rodzaje.l_prawo.x2 = 1;
                rodzaje.l_prawo.y2 = SZEROKOSC_POLA / 2;
                rodzaje.l_prawo.x3 = 2;
                rodzaje.l_prawo.y3 = SZEROKOSC_POLA / 2;
                rodzaje.l_prawo.x4 = 2;
                rodzaje.l_prawo.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.l_prawo.kolor = 0xFFFF00;
                rodzaje.l_prawo.kolor_wypelnienia = 0xFFD700;
                rodzaje.l_prawo.znak = 'L';
                //l dol  { 1, (SZEROKOSC_POLA / 2)-1, 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_dol.x1 = 1;
                rodzaje.l_dol.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.l_dol.x2 = 0;
                rodzaje.l_dol.y2 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.l_dol.x3 = 0;
                rodzaje.l_dol.y3 = SZEROKOSC_POLA / 2;
                rodzaje.l_dol.x4 = 0;
                rodzaje.l_dol.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.l_dol.kolor = 0xFFFF00;
                rodzaje.l_dol.kolor_wypelnienia = 0xFFD700;
                rodzaje.l_dol.znak = 'L';
                //l lewo  { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_lewo.x1 = 0;
                rodzaje.l_lewo.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.l_lewo.x2 = 0;
                rodzaje.l_lewo.y2 = SZEROKOSC_POLA / 2;
                rodzaje.l_lewo.x3 = 1;
                rodzaje.l_lewo.y3 = SZEROKOSC_POLA / 2;
                rodzaje.l_lewo.x4 = 2;
                rodzaje.l_lewo.y4 = SZEROKOSC_POLA / 2;
                rodzaje.l_lewo.kolor = 0xFFFF00;
                rodzaje.l_lewo.kolor_wypelnienia = 0xFFD700;
                rodzaje.l_lewo.znak = 'L';
                //l gora { 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2,     1, (SZEROKOSC_POLA / 2)+1, 0, (SZEROKOSC_POLA / 2)+1, 0xFFFF00, 0xFFD700, 'L' };
                rodzaje.l_gora.x1 = 1;
                rodzaje.l_gora.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.l_gora.x2 = 1;
                rodzaje.l_gora.y2 = SZEROKOSC_POLA / 2;
                rodzaje.l_gora.x3 = 1;
                rodzaje.l_gora.y3 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.l_gora.x4 = 0;
                rodzaje.l_gora.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.l_gora.kolor = 0xFFFF00;
                rodzaje.l_gora.kolor_wypelnienia = 0xFFD700;
                rodzaje.l_gora.znak = 'L';
                //odwrotne l lewo { 0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 2, (SZEROKOSC_POLA / 2)+1, 2, SZEROKOSC_POLA / 2,     0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_lewo.x1 = 0;
                rodzaje.odwrotne_l_lewo.y1 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_lewo.x2 = 1;
                rodzaje.odwrotne_l_lewo.y2 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_lewo.x3 = 2;
                rodzaje.odwrotne_l_lewo.y3 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_lewo.x4 = 2;
                rodzaje.odwrotne_l_lewo.y4 = SZEROKOSC_POLA / 2;
                rodzaje.odwrotne_l_lewo.kolor = 0xFF00FF;
                rodzaje.odwrotne_l_lewo.kolor_wypelnienia = 0x8B008B;
                rodzaje.odwrotne_l_lewo.znak = 'O';
                //odwrotne l gora  { 0, (SZEROKOSC_POLA / 2)-1, 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2,     1, (SZEROKOSC_POLA / 2)+1, 0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_gora.x1 = 0;
                rodzaje.odwrotne_l_gora.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.odwrotne_l_gora.x2 = 1;
                rodzaje.odwrotne_l_gora.y2 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.odwrotne_l_gora.x3 = 1;
                rodzaje.odwrotne_l_gora.y3 = SZEROKOSC_POLA / 2;
                rodzaje.odwrotne_l_gora.x4 = 1;
                rodzaje.odwrotne_l_gora.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_gora.kolor = 0xFF00FF;
                rodzaje.odwrotne_l_gora.kolor_wypelnienia = 0x8B008B;
                rodzaje.odwrotne_l_gora.znak = 'O';
                //odwrotne l prawo { 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 1, SZEROKOSC_POLA / 2,     2, SZEROKOSC_POLA / 2,     0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_prawo.x1 = 0;
                rodzaje.odwrotne_l_prawo.y1 = SZEROKOSC_POLA / 2;
                rodzaje.odwrotne_l_prawo.x2 = 0;
                rodzaje.odwrotne_l_prawo.y2 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_prawo.x3 = 1;
                rodzaje.odwrotne_l_prawo.y3 = SZEROKOSC_POLA / 2;
                rodzaje.odwrotne_l_prawo.x4 = 2;
                rodzaje.odwrotne_l_prawo.y4 = SZEROKOSC_POLA / 2;
                rodzaje.odwrotne_l_prawo.kolor = 0xFF00FF;
                rodzaje.odwrotne_l_prawo.kolor_wypelnienia = 0x8B008B;
                rodzaje.odwrotne_l_prawo.znak = 'O';
                //odwrotne l dol { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 0xFF00FF, 0x8B008B, 'O' };
                rodzaje.odwrotne_l_dol.x1 = 0;
                rodzaje.odwrotne_l_dol.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.odwrotne_l_dol.x2 = 0;
                rodzaje.odwrotne_l_dol.y2 = SZEROKOSC_POLA / 2;
                rodzaje.odwrotne_l_dol.x3 = 0;
                rodzaje.odwrotne_l_dol.y3 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_dol.x4 = 1;
                rodzaje.odwrotne_l_dol.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.odwrotne_l_dol.kolor = 0xFF00FF;
                rodzaje.odwrotne_l_dol.kolor_wypelnienia = 0x8B008B;
                rodzaje.odwrotne_l_dol.znak = 'O';
                //s poziom { 1, (SZEROKOSC_POLA / 2)-1, 1, SZEROKOSC_POLA / 2, 0, SZEROKOSC_POLA / 2,     0, (SZEROKOSC_POLA / 2)+1, 0x00008B, 0x191970, 'S' };
                rodzaje.s_poziom.x1 = 1;
                rodzaje.s_poziom.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.s_poziom.x2 = 1;
                rodzaje.s_poziom.y2 = SZEROKOSC_POLA / 2;
                rodzaje.s_poziom.x3 = 0;
                rodzaje.s_poziom.y3 = SZEROKOSC_POLA / 2;
                rodzaje.s_poziom.x4 = 0;
                rodzaje.s_poziom.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.s_poziom.kolor = 0x00008B;
                rodzaje.s_poziom.kolor_wypelnienia = 0x191970;
                rodzaje.s_poziom.znak = 'S';
                //s pion  { 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 2, (SZEROKOSC_POLA / 2)+1, 0x00008B, 0x191970, 'S' };
                rodzaje.s_pion.x1 = 0;
                rodzaje.s_pion.y1 = SZEROKOSC_POLA / 2;
                rodzaje.s_pion.x2 = 1;
                rodzaje.s_pion.y2 = SZEROKOSC_POLA / 2;
                rodzaje.s_pion.x3 = 1;
                rodzaje.s_pion.y3 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.s_pion.x4 = 2;
                rodzaje.s_pion.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.s_pion.kolor = 0x00008B;
                rodzaje.s_pion.kolor_wypelnienia = 0x191970;
                rodzaje.s_pion.znak = 'S';
                //wonsz poziom { 0, (SZEROKOSC_POLA / 2)-1, 0, SZEROKOSC_POLA / 2,     1, SZEROKOSC_POLA / 2, 1, (SZEROKOSC_POLA / 2)+1, 0x00FF00, 0x006400, 'W' };
                rodzaje.wonsz_poziom.x1 = 0;
                rodzaje.wonsz_poziom.y1 = (SZEROKOSC_POLA / 2) - 1;
                rodzaje.wonsz_poziom.x2 = 0;
                rodzaje.wonsz_poziom.y2 = SZEROKOSC_POLA / 2;
                rodzaje.wonsz_poziom.x3 = 1;
                rodzaje.wonsz_poziom.y3 = SZEROKOSC_POLA / 2;
                rodzaje.wonsz_poziom.x4 = 1;
                rodzaje.wonsz_poziom.y4 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.wonsz_poziom.kolor = 0x00FF00;
                rodzaje.wonsz_poziom.kolor_wypelnienia = 0x006400;
                rodzaje.wonsz_poziom.znak = 'W';
                //wonsz pion { 0, (SZEROKOSC_POLA / 2)+1, 1, (SZEROKOSC_POLA / 2)+1, 1, SZEROKOSC_POLA / 2, 2, SZEROKOSC_POLA / 2,     0x00FF00, 0x006400, 'W' };
                rodzaje.wonsz_pion.x1 = 0;
                rodzaje.wonsz_pion.y1 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.wonsz_pion.x2 = 1;
                rodzaje.wonsz_pion.y2 = (SZEROKOSC_POLA / 2) + 1;
                rodzaje.wonsz_pion.x3 = 1;
                rodzaje.wonsz_pion.y3 = SZEROKOSC_POLA / 2;
                rodzaje.wonsz_pion.x4 = 2;
                rodzaje.wonsz_pion.y4 = SZEROKOSC_POLA / 2;
                rodzaje.wonsz_pion.kolor = 0x00FF00;
                rodzaje.wonsz_pion.kolor_wypelnienia = 0x006400;
                rodzaje.wonsz_pion.znak = 'W';
                // koniec schematow
        
        bool czy_jest_klocek = false;
 
        char pole_gry[WYSOKOSC_POLA][SZEROKOSC_POLA];
        wypelnij_pole(pole_gry);
 
        
        do
        {
 
                
 
        do {
                if (nowa_gra)
                {
                        t1 = SDL_GetTicks();
                        worldTime = 0;
                        czas = predkosc;
 
                        //etapowosc
                        etap = 0;
                        przeskok = predkosc;
                        etapTime = 0;
 
                        //zapisywanie
                        czy_zapisano = false;
                        ile_razy_zapisano = 0;
 
                        nowa_gra = false;
                }
                else if (pause)
                {
                        t1 = SDL_GetTicks();
                        worldTime = worldTime_pauza;
                        czas = czas_pauza;
                        pause = false;
                }
                
                
                t2 = SDL_GetTicks();
 
                // w tym momencie t2-t1 to czas w milisekundach,
                // jaki uplyna? od ostatniego narysowania ekranu
                // delta to ten sam czas w sekundach
                delta = (t2 - t1) * 0.001;
                t1 = t2;
 
                worldTime += delta;
                etapTime += delta;
 
                distance += etiSpeed * delta;
 
                SDL_FillRect(screen, NULL, czarny);
                
                                
 
                if (czy_jest_klocek == false)
                {
                        spadek_w_dol = 0;
                        przesuniecie_w_bok = 0;
                        liczni_obrotow = 0;
 
                        zmiana_wg_obrotow(liczni_obrotow, &dlugi, &bolec, &l, &odwrotne_l, &s, &wonsz, rodzaje);
                                                
                        losowa_liczba = (rand() % 7) + 1;
                        
                        podmiana_klocka_losowym(&aktualny, losowa_liczba, dlugi, kwadrat, bolec, l, odwrotne_l, s, wonsz);
 
                        czy_jest_klocek = true;
                }
                
                zmien_pole_pod_aktualnym(pole_gry, aktualny, aktualny.znak);
                
                rysuj_pole_gry(pole_gry, screen, dlugi, bolec, kwadrat, l, odwrotne_l, s, wonsz);
 
                //etapowosc
                
                if (etapTime > co_ile_nowy_etap_T && etap < ile_etapow)
                {
                        przeskok -= o_ile_przeskok_etap;
                        etap++;
                        etapTime = 0;
                }
                
                // opadanie
 
                if (worldTime > czas && (pole_gry[aktualny.x1 + 1][aktualny.y1] == ' ' || pole_gry[aktualny.x1 + 1][aktualny.y1] == aktualny.znak) && (pole_gry[aktualny.x2 + 1][aktualny.y2] == ' ' || pole_gry[aktualny.x2 + 1][aktualny.y2] == aktualny.znak) && (pole_gry[aktualny.x3 + 1][aktualny.y3] == ' ' || pole_gry[aktualny.x3 + 1][aktualny.y3] == aktualny.znak) && (pole_gry[aktualny.x4 + 1][aktualny.y4] == ' ' || pole_gry[aktualny.x4 + 1][aktualny.y4] == aktualny.znak))
                {
                        zmien_pole_pod_aktualnym(pole_gry, aktualny, ' ');
 
                        skret(&aktualny, 'x');
 
                        czas += przeskok;
                        spadek_w_dol++;
                }
                                
 
 
                //              DrawScreen(screen, plane, ship, charset, worldTime, delta, vertSpeed);
 
                                // naniesienie wyniku rysowania na rzeczywisty ekran
                //              SDL_Flip(screen);
 
                fpsTimer += delta;
                if (fpsTimer > 0.5) {
                        fps = frames * 2;
                        frames = 0;
                        fpsTimer -= 0.5;
                };
 
                sprintf(text, "Czas trwania = %.1lf s  %d etap  %.0f punkty %f przeskok", worldTime, etap, punkty, przeskok);
                DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
                
                SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
                //              SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, scrtex, NULL, NULL);
                SDL_RenderPresent(renderer);
                
                if (czy_klawisz_dol == false)
                {
                        // obs?uga zdarze? (o ile jakie? zasz?y)
                        while (SDL_PollEvent(&event)) {
                                switch (event.type) {
                                case SDL_KEYDOWN:
                                        if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
                                        else if (event.key.keysym.sym == SDLK_DOWN)
                                        {
                                                przeskok_przed_dolem = przeskok;
                                                przeskok = 0.01;
                                                czy_klawisz_dol = true;
                                        }
                                        else if (event.key.keysym.sym == SDLK_LEFT)
                                        {
                                                if ((pole_gry[aktualny.x1][aktualny.y1 - 1] == ' ' || pole_gry[aktualny.x1][aktualny.y1 - 1] == aktualny.znak) && (pole_gry[aktualny.x2][aktualny.y2 - 1] == ' ' || pole_gry[aktualny.x2][aktualny.y2 - 1] == aktualny.znak) && (pole_gry[aktualny.x3][aktualny.y3 - 1] == ' ' || pole_gry[aktualny.x3][aktualny.y3 - 1] == aktualny.znak) && (pole_gry[aktualny.x4][aktualny.y4 - 1] == ' ' || pole_gry[aktualny.x4][aktualny.y4 - 1] == aktualny.znak))
                                                {
                                                        zmien_pole_pod_aktualnym(pole_gry, aktualny, ' ');
 
                                                        skret(&aktualny, 'l');
                                                        przesuniecie_w_bok--;
                                                }
                                        }
                                        else if (event.key.keysym.sym == SDLK_RIGHT)
                                        {
                                                if ((pole_gry[aktualny.x1][aktualny.y1 + 1] == ' ' || pole_gry[aktualny.x1][aktualny.y1 + 1] == aktualny.znak) && (pole_gry[aktualny.x2][aktualny.y2 + 1] == ' ' || pole_gry[aktualny.x2][aktualny.y2 + 1] == aktualny.znak) && (pole_gry[aktualny.x3][aktualny.y3 + 1] == ' ' || pole_gry[aktualny.x3][aktualny.y3 + 1] == aktualny.znak) && (pole_gry[aktualny.x4][aktualny.y4 + 1] == ' ' || pole_gry[aktualny.x4][aktualny.y4 + 1] == aktualny.znak))
                                                {
                                                        zmien_pole_pod_aktualnym(pole_gry, aktualny, ' ');
 
                                                        skret(&aktualny, 'p');
 
                                                        przesuniecie_w_bok++;
                                                }
                                        }
                                        else if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_SPACE)
                                        {
                                                liczni_obrotow++;
                                                zmien_pole_pod_aktualnym(pole_gry, aktualny, ' ');
 
                                                if (liczni_obrotow == 4)
                                                        liczni_obrotow = 0;
 
                                                zmiana_wg_obrotow(liczni_obrotow, &dlugi, &bolec, &l, &odwrotne_l, &s, &wonsz, rodzaje);
 
                                                podmiana_klocka_zas(aktualny, dlugi, kwadrat, bolec, l, odwrotne_l, s, wonsz, &zastepczy);
 
                                                przesuniecie_klocka(&zastepczy, spadek_w_dol, przesuniecie_w_bok);
 
                                                if ((pole_gry[zastepczy.x1][zastepczy.y1] == ' ' || pole_gry[zastepczy.x1][zastepczy.y1] == aktualny.znak) && (pole_gry[zastepczy.x2][zastepczy.y2] == ' ' || pole_gry[zastepczy.x2][zastepczy.y2] == aktualny.znak) && (pole_gry[zastepczy.x3][zastepczy.y3] == ' ' || pole_gry[zastepczy.x3][zastepczy.y3] == aktualny.znak) && (pole_gry[zastepczy.x4][zastepczy.y4] == ' ' || pole_gry[zastepczy.x4][zastepczy.y4] == aktualny.znak))
                                                        aktualny = zastepczy;
                                                else
                                                        liczni_obrotow--;
                                        }
                                        else if (event.key.keysym.sym == SDLK_n)
                                        {
                                                //reset
                                                wypelnij_pole(pole_gry);
                                                czy_jest_klocek = false;
                                                nowa_gra = true;
                                        }
                                        else if (event.key.keysym.sym == SDLK_p)
                                        {
                                                czy_koniec = 'p';
                                        }
                                        else if (event.key.keysym.sym == SDLK_s)
                                        {
                                                if (etap < ile_etapow)
                                                {
                                                        przeskok -= o_ile_przeskok_etap;
                                                        etap++;
                                                        etapTime = 0;
                                                        
                                                }
                                        }
                                        else if (event.key.keysym.sym == SDLK_z && ile_razy_zapisano<3)
                                        {
                                                zapisz_do_pliku(pole_gry,przeskok,etap, punkty);
                                                czy_zapisano = true;
                                                ile_razy_zapisano++;
                                        }
                                        else if (event.key.keysym.sym == SDLK_l && czy_zapisano==true)
                                        {
                                                wczytaj(pole_gry, &etap, &przeskok, &punkty, &aktualny);
                                                etapTime = 0;
                                        }
                                        break;
                                case SDL_KEYUP:
                                        break;
                                case SDL_QUIT:
                                        quit = 1;
                                        break;
                                };
                        };
                }
                        if ((pole_gry[aktualny.x1 + 1][aktualny.y1] != ' ' && pole_gry[aktualny.x1 + 1][aktualny.y1] != aktualny.znak) || (pole_gry[aktualny.x2 + 1][aktualny.y2] != ' ' && pole_gry[aktualny.x2 + 1][aktualny.y2] != aktualny.znak) || (pole_gry[aktualny.x3 + 1][aktualny.y3] != ' ' && pole_gry[aktualny.x3 + 1][aktualny.y3] != aktualny.znak) || (pole_gry[aktualny.x4 + 1][aktualny.y4] != ' ' && pole_gry[aktualny.x4 + 1][aktualny.y4] != aktualny.znak))
                        {
                                zmien_pole_pod_aktualnym(pole_gry, aktualny, tolower(aktualny.znak));
                                
                                if (czy_klawisz_dol)
                                {
                                        przeskok = przeskok_przed_dolem;
                                        czy_klawisz_dol = false;
                                }
 
                                czy_jest_klocek = false;
                                                                                                
                                usun_pelne_wiersze(pole_gry, aktualny, &ile_skasowanych_wierszy);
 
                                dodaj_punkty(ile_skasowanych_wierszy, etap, &mnoznik_pktow, &punkty);
                                
                                ile_skasowanych_wierszy = 0;
 
                                if (aktualny.x1 == 0 || aktualny.x2 == 0 || aktualny.x3 == 0 || aktualny.x4 == 0)
                                {
                                        czy_jest_klocek = true;
                                        czy_koniec = 'k';
                                        
                                }
                        }
 
 
                        frames++;
                }while (!quit && czy_koniec != 'k' && czy_koniec != 'p');
                
                
                
                if (czy_koniec=='k' || czy_koniec == 'p')
                {       
                        if (czy_koniec == 'p')
                        {
                                worldTime_pauza = worldTime;
                                czas_pauza = czas;                              
                                pause = true;
                        }
                        //break;
                        bool koniec_ekranu_pauzy= false;
                        do
                        {
                                DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, 0x8B0000);
                                if (czy_koniec == 'k')
                                {
                                        sprintf(text, "Esc - wyjscie, n - nowa gra");
                                        DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
                                        sprintf(text, "Koniec gry");
                                }
                                else
                                {
                                        sprintf(text, "Pauza");
                                }
                                DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
                                
 
                                SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
                                //              SDL_RenderClear(renderer);
                                SDL_RenderCopy(renderer, scrtex, NULL, NULL);
                                SDL_RenderPresent(renderer);
                                if (czy_koniec == 'k')
                                {
                                        while (SDL_PollEvent(&event)) {
                                                switch (event.type) {
                                                case SDL_KEYDOWN:
                                                        if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
                                                        else if (event.key.keysym.sym == SDLK_n)
                                                        {
                                                                wypelnij_pole(pole_gry);
                                                                czy_jest_klocek = false;                                                                
                                                                nowa_gra = true;
                                                                czy_koniec = 'f';
                                                                koniec_ekranu_pauzy = true;
                                                        }
                                                        else if (event.key.keysym.sym == SDLK_l && czy_zapisano == true)
                                                        {
                                                                wczytaj(pole_gry, &etap, &przeskok, &punkty, &aktualny);
                                                                etapTime = 0;
                                                        }
                                                        break;
                                                }
                                        }
                                }
                                else
                                {
                                        while (SDL_PollEvent(&event)) {
                                                switch (event.type) {
                                                case SDL_KEYDOWN:
                                                        if (event.key.keysym.sym == SDLK_p)
                                                        {
                                                                koniec_ekranu_pauzy = true;
                                                                czy_koniec = 'f';
                                                        }
                                                }
                                        }
                                }
                                
                                        
                        } while (!quit && koniec_ekranu_pauzy != true);
                        
                        koniec_ekranu_pauzy = false;
 
                }
        }while ( !quit);
        
 
        // zwolnienie powierzchni
        SDL_FreeSurface(charset);
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
 
        SDL_Quit();
        return 0;
        };