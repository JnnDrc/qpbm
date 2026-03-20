// -----------------------------------------------------------------------------
// small netpbm file visualizer to exemplify what qpbm can do
// WARN: uses raylib
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <raylib.h>

#define QPBM_IMPL
#include "../qpbm.h"

int main(int argc, char* argv[]){
    if (argc < 2) fprintf(stderr,"USAGE: %s file",argv[0]);
    char* in = argv[1];
    FILE* fin = fopen(in,"r");
    if(!fin){
        perror("ERROR: failed to open file ");
        return 127;
    }

    qpbm_t img;
    qpbm_load(fin,&img,QPBM_PNM);
    
    uint8_t scalx = 1, scaly = 1;
    if (img.width < 100)  scalx = 10;
    if (img.height < 100) scaly = 10;

    int wind_w = img.width*scalx;
    int wind_h = img.height*scaly;

    InitWindow(wind_w,wind_h,"ppmv");
    const char* infotext = TextFormat("%s: P%c (%d x %d) (%d)",in,img.type,img.width,img.height,img.max_value);
    SetWindowTitle(infotext);

    SetTargetFPS(60);
    uint8_t info = 0;
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLANK);

        for(int x = 0; x < img.width; x++){
            for (int y = 0; y < img.height; y++) {
                Color p = BLANK;

                if      (img.type == QPBM_PBM) p = img.pixels[x + y*img.width] ? WHITE : BLACK;
                else if (img.type == QPBM_PGM){
                    uint8_t c =  0 + (img.pixels[x + y*img.width] - 0) * (255 - 0) / (15 - 0);
                    p = (Color){c,c,c,0xFF};
                }
                else if (img.type == QPBM_PPM) p = ((Color*)img.pixels)[x + y*img.width];
                

                DrawRectangle(x*scalx,y*scaly,scalx,scaly,p);

            }
        }

        EndDrawing();
    }

    qpbm_free(&img);
    CloseWindow();
    return 0;
}

