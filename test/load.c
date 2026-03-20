// -----------------------------------------------------------------------------
// load file test
// WARN: uses raylib
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <raylib.h>

#define QPBM_IMPL
#include "../qpbm.h"

void ppm3(){
    FILE* fj = fopen("res/p3.ppm","r");

    uint32_t* japan = NULL;
    unsigned int w, h;
    unsigned int maxval;
    
    qpbm_type_n t = QPBM_PPM;
    qpbm_fmt_n  f;

    int e = 0;
    if((e = qpbm_read(fj,(uint8_t**)&japan,&w,&h,(unsigned short*)&maxval,&t,&f)) < 0) printf("error: %d",e);

    for(int i = 0; i < w*h; i++){
        printf("%X\n",((uint32_t*)japan)[i]);
    }

    InitWindow(w,h,"load");

    Texture win = LoadTextureFromImage(GenImageColor(w,h,BLANK));

    UpdateTexture(win,japan);

    while(!WindowShouldClose()){
        BeginDrawing();

        DrawTexture(win,0,0,WHITE);

        EndDrawing();
    }
    
    CloseWindow();
    free(japan);
    fclose(fj);
}

void ppm6(){
    FILE* fj = fopen("res/p6.ppm","r");

    uint32_t* japan = NULL;
    unsigned int w, h;
    unsigned int maxval;
    
    qpbm_type_n t = QPBM_PPM;
    qpbm_fmt_n  f;
    int e = 0;
    if((e = qpbm_read(fj,(uint8_t**)&japan,&w,&h,(unsigned short*)&maxval,&t,&f)) < 0) printf("error: %d",e);
    fprintf(stdout,"P%c: %d x %d",t, w, h);
    InitWindow(w,h,"load");


    while(!WindowShouldClose()){
        BeginDrawing();
        
        for(int x = 0; x < w; x++){
            for (int y = 0; y < h; y++) {
                DrawPixel(x,y,*(Color*)&japan[x + y*w]);
            }
        }

        EndDrawing();
    }
    
    CloseWindow();

    free(japan);
    fclose(fj);
}

void pbm1(){
    FILE* f = fopen("res/p1.pbm","r");
    qpbm_t img;
    qpbm_load(f,&img,QPBM_PNM);
    for(int i = 0; i < img.width * img.height; i++) printf("%d",img.pixels[i]);
    const int fac = 16;
    InitWindow(img.width*fac,img.height*fac,"load");


    while(!WindowShouldClose()){
        BeginDrawing();
        
        for(int y = 0; y < img.height; y++){
            for(int x = 0; x < img.width; x++){
                DrawRectangle(x*fac,y*fac,fac,fac,img.pixels[x + y*img.width] ? WHITE : BLANK);
            }
        }

        EndDrawing();
    }
    
    CloseWindow();

    qpbm_free(&img);
    fclose(f);
}

void pbm4(){
    FILE* f = fopen("res/p4.pbm","r");
    qpbm_t img;
    qpbm_load(f,&img,QPBM_PNM);
    for(int i = 0; i < img.width * img.height; i++) printf("%d",img.pixels[i]);
    const int fac = 16;
    InitWindow(img.width*fac,img.height*fac,"load");


    while(!WindowShouldClose()){
        BeginDrawing();
        
        for(int y = 0; y < img.height; y++){
            for(int x = 0; x < img.width; x++){
                DrawRectangle(x*fac,y*fac,fac,fac,img.pixels[x + y*img.width] ? WHITE : BLANK);
            }
        }

        EndDrawing();
    }
    
    CloseWindow();

    qpbm_free(&img);
    fclose(f);
}

void pgm2(){
    FILE* f = fopen("res/p2.pgm","r");
    qpbm_t img;
    qpbm_load(f,&img,QPBM_PNM);
    for(int i = 0; i < img.width * img.height; i++) printf("%d ",img.pixels[i]);
    const int fac = 16;
    InitWindow(img.width*fac,img.height*fac,"load");

    while(!WindowShouldClose()){
        BeginDrawing();
        
        for(int y = 0; y < img.height; y++){
            for(int x = 0; x < img.width; x++){

                uint8_t c =  0 + (img.pixels[x + y*img.width] - 0) * (255 - 0) / (15 - 0);
                DrawRectangle(x*fac,y*fac,fac,fac,(Color){c,c,c,0xFF});
            }
        }

        EndDrawing();
    }
    
    CloseWindow();

    qpbm_free(&img);
    fclose(f);
}

void pgm5(){
    FILE* f = fopen("res/p5.pgm","r");
    qpbm_t img;
    qpbm_load(f,&img,QPBM_PNM);
    for(int i = 0; i < img.width * img.height; i++) printf("%d ",img.pixels[i]);
    const int fac = 16;
    InitWindow(img.width*fac,img.height*fac,"load");

    while(!WindowShouldClose()){
        BeginDrawing();
        
        for(int y = 0; y < img.height; y++){
            for(int x = 0; x < img.width; x++){

                uint8_t c =  0 + (img.pixels[x + y*img.width] - 0) * (255 - 0) / (15 - 0);
                DrawRectangle(x*fac,y*fac,fac,fac,(Color){c,c,c,0xFF});
            }
        }

        EndDrawing();
    }
    
    CloseWindow();
    qpbm_free(&img);
    fclose(f);
}

typedef void (*f)(void);

int main(void){
    int i = 3;
    f fns[] = {pbm1,pgm2,ppm3,pbm4,pgm5,ppm6};
    fns[i-1]();
    return 0;
}
