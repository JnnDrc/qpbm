# Quick PBM

small single header Netpbm formats library

## Usage

```c
#include <stdio.h>
#define QPBM_IMPL
#include "qpbm.h"

int main(void){
    FILE* fim = fopen("image.ppm","rb");
    qpbm_t img;
    int err = qpbm_load(fim,&img,QPBM_PPM);
    fclose(fim);
    if(err != QPBM_OK){
        fprintf(stderr,"Failed to load image: %d\n",err);
    }

    ...

    qpbm_free(&img);
    return 0;
}

```
