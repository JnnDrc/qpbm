// -------------------------------------
// Quick PBM library
// *
// by jayy ;3
// -------------------------------------

#ifndef QPBM_H
#define QPBM_H

#include <stdio.h>
#include <stdint.h>

typedef enum qpbm_err{
    QPBM_OK               =  0,  // no error
    QPBM_ERR_NULLPTR      = -1,  // null pointer passed to function
    QPBM_ERR_INVALID_TYPE = -2,  // qpbm_type passed is not a valid type
    QPBM_ERR_NOT_PBM      = -3,  // file is not a netpbm file
    QPBM_ERR_WRONG_TYPE   = -4,  // netpbm file is not of the desired type
    QPBM_ERR_MEM          = -5,  // failed to allocate memory
    QPBM_ERR_PARSE_ERROR  = -6,  // failed at parsing file
    QPBM_ERR_UNKNOWN      = -7,  // unknown error
}qpbm_err_n;

typedef enum qpbm_filetype{
    QPBM_FT_NOTY =  0,      // No file, error/internal use only
    QPBM_FT_PBMA = '1',     // P1, PBM ASCII
    QPBM_FT_PBMB = '4',     // P4, PBM BINARY
    QPBM_FT_PGMA = '2',     // P2, PGM ASCII
    QPBM_FT_PGMB = '5',     // P5, PGM BINARY
    QPBM_FT_PPMA = '3',     // P3, PPM ASCII
    QPBM_FT_PPMB = '6',     // P6, PPM BINARY
}qpbm_filetype_n;

typedef enum qpbm_type{
    QPBM_PNM,               // Any file type (used only for read/load)
    QPBM_PBM,               // PBM file type
    QPBM_PGM,               // PGM file type
    QPBM_PPM,               // PPM file type
}qpbm_type_n;

typedef enum qpbm_fmt{
    QPBM_ASCII  =  0,   // ASCII text format
    QPBM_BINARY =  3,   // Binary format
}qpbm_fmt_n;

typedef struct qpbm{
    qpbm_type_n type;           // image type (PBM,PGM,PPM)
    qpbm_fmt_n  format;         // image format (ASCII/BINARY)
    uint32_t    width, height;  // image dimensions
    uint16_t    max_value;      // pixel maximum value
    uint8_t*    pixels;         // pixels buffer, depends on type and max_value
}qpbm_t;                        // PBM: 1 byte per pixel
                                // PGM: 1/2 bytes per pixel
                                // PPM: 4/8 bytes per pixel (1/2 bytes per component)
                                //      - Alpha is added and set to max value for conveniency,
                                //      even not existing on ppm format

// @desc  write data as specified netpbm file
// @param fp        FILE*       file handle to save
// @param pixels    uint8_t*    pixel data
// @param width     uint32_t    image width
// @param height    uint32_t    image height
// @param max_color uint16_t    maximum value for pixels (ignored if pbm)
// @param type      qpbm_type_n netpbm type (pbm,pgm,ppm)
// @param format    qpbm_fmt_n  file format (ascii/binary)
// @error will error if #fp is a null pointer 
// @error will error if #pixels is a null pointer
// @error will error if file type is not some supported netpbm
// @return errc int
int qpbm_write(FILE* fp, uint8_t* pixels, uint32_t  width, uint32_t  height, uint16_t max_value,  qpbm_type_n type,qpbm_fmt_n format);
// @desc  read pixel data and image metadata from file
// @param fp        FILE*           file handle to load
// @param pixels    uint8_t**       pixel data
// @param width     uint32_t*       image width
// @param height    uint32_t*       image height
// @param max_color uint16_t*       maximum value of pixels (set to NULL if pbm)
// @param type      qpbm_type_n*    netpbm type (pbm,pgm,ppm) (can use QPBM_PNM to read any type)
// @param format    qpbm_fmt_n*     file format (ascii/binary)
// @error will error if #fp is a null pointer 
// @error will error if file type is not some supported netpbm
// @error will error if file type is not the specified, (except if type is QPBM_PNM)
// @error will error if can't allocate required resources
// @error will error if file have bad formating
// @return errc int
int qpbm_read(FILE* fp, uint8_t** pixels, uint32_t* width, uint32_t* height, uint16_t* max_value, qpbm_type_n* type, qpbm_fmt_n* format);

// @desc  save qpbm image
// @param fp    FILE*   file handle to save
// @param pnm   qpbm_t  qpbm image
// @return errc int
int qpbm_save(FILE* fp, qpbm_t  img);
// @desc  load data and image metadata from file to qpbm image
// @param fp    FILE*       file handle to load
// @param pnm   qpbm_t*     qpbm image
// @param type  qpbm_type_n file format (pbm,pgm,ppm, ascci/binary) (use QPBM_ANY for no type checking)
// @return errc int
int qpbm_load(FILE* fp, qpbm_t* img, qpbm_type_n type);
// @desc create a qpbm image from raw pixels and meta data
// @param pixels    uint8_t*    pixel data
// @param width     uint32_t    image width
// @param height    uint32_t    image height
// @param max_color uint16_t    maximum value for pixels
// @param type      qpbm_type_n netpbm type (pbm,pgm,ppm)
// @param format    qpbm_fmt_n  file format (ascii/binary)
qpbm_t qpbm_from(uint8_t* pixels, uint32_t width, uint32_t height, uint16_t max_value, qpbm_type_n type, qpbm_fmt_n format);

// @desc  free qpbm image
// @param img qpbm_t* image to free
void qpbm_free(qpbm_t* img);

// @desc get amount of bytes per pixel on image buffer
// @param img qpbm_t qpbm image
// @return stride int
int qpbm_stride(qpbm_t* img);

#endif /* QPBM_H */

// ----------------------------------------------------------------------------|

#if defined QPBM_IMPL || defined QPBM_IMPLEMENTATION

#include <stdlib.h>
#include <ctype.h>

static int _qpbm_read_token(FILE* fp, char* buf, size_t sz){
    int c;
    while((c = fgetc(fp)) != EOF){
        if(isspace(c)) continue;
        if(c == '#') while((c = fgetc(fp)) != '\n' && c != EOF) continue;
        break;
    }
    if(c == EOF) return 0;
    size_t i = 0;
    do {
        if(i + 1 < sz) buf[i++] = (char)c;
        c = fgetc(fp);
    }while(c != EOF && !isspace(c));
    buf[i] = '\0';
    return 1;
}

int qpbm_write(FILE* fp, uint8_t* pixels, uint32_t width, uint32_t height,uint16_t max_value, qpbm_type_n type, qpbm_fmt_n format){
    if(!fp || !pixels)                             return QPBM_ERR_NULLPTR;
    if(type < QPBM_PBM || type > QPBM_PPM)         return QPBM_ERR_INVALID_TYPE;
    
    qpbm_filetype_n fity = QPBM_FT_NOTY;
    switch(type){
        case QPBM_PBM: fity = format == QPBM_ASCII ? QPBM_FT_PBMA : QPBM_FT_PBMB; break;
        case QPBM_PGM: fity = format == QPBM_ASCII ? QPBM_FT_PGMA : QPBM_FT_PGMB; break;
        case QPBM_PPM: fity = format == QPBM_ASCII ? QPBM_FT_PPMA : QPBM_FT_PPMB; break;
        default: return QPBM_ERR_UNKNOWN;
    }

    // header
    fprintf(fp,"P%c\n",fity);                                       // print magic number
    fprintf(fp,"%u %u\n",width,height);                             // print size (width x height)
    if(fity != QPBM_FT_PBMA && fity != QPBM_FT_PBMB) fprintf(fp,"%u\n",max_value);  // max color (for pgm/ppm)

    // data
    switch(fity){
        case QPBM_FT_PBMA:
            for(uint32_t y = 0; y < height; y++){
                for (uint32_t x = 0; x < width; x++) fprintf(fp,"%d ",pixels[(x + y*width)]);
                fprintf(fp,"\n");
            }
            break;
        case QPBM_FT_PBMB:
            {
                uint16_t row_bytes = (width + 7) / 8;
                for(uint32_t y = 0; y < height; y++){
                    for(uint16_t b = 0; b < row_bytes; b++){
                        uint8_t byte = 0;
                        for(int bit = 0; bit < 8; bit++){
                            int x = b*8 + bit;
                            if(x < width && pixels[x + y*width]) byte |= (1 << (7 - bit));
                        }
                        fputc(byte,fp);
                    }
                }
            }
            break;
        case QPBM_FT_PGMA:
            for(uint32_t y = 0; y < height; y++){
                for (uint32_t x = 0; x < width; x++){
                    if(max_value > 255) fprintf(fp,"%hu ", ((uint16_t*)pixels)[(x + y*width)]);
                    else                fprintf(fp,"%hhu ",pixels[(x + y*width)]);
                }
                fprintf(fp,"\n");
            }
            break;
        case QPBM_FT_PGMB:
            for(uint32_t i = 0; i < width * height; i++){
                if(max_value <= 255) fputc(pixels[i],fp);
                else{
                    uint16_t w = ((uint16_t*)pixels)[i];
                    uint8_t hi = (w >> 8) & 0xFF;
                    uint8_t lo = w & 0xFF;
                    fputc(hi,fp);
                    fputc(lo,fp);
                }
            }
            break;
        case QPBM_FT_PPMA:
            if(max_value > 255){
                for(size_t i = 0; i < width*height; i++) {
                    uint16_t* p16 = (uint16_t*)pixels;
                    uint16_t words[3] = {p16[(i*4)],p16[(i*4)+1],p16[(i*4)+2]};
                    fprintf(fp,"%hu %hu %hu\n",words[0],words[1],words[2]);
                }
            }else{
                for(size_t i = 0; i < width*height; i++){
                    uint8_t bytes[3] = {pixels[(i*4)],pixels[(i*4)+1],pixels[(i*4)+2]};
                    fprintf(fp,"%d %d %d\n", bytes[0],bytes[1],bytes[2]);
                }
            }
            break;
        case QPBM_FT_PPMB:
            if(max_value > 255){
               for(size_t i = 0; i < width*height; i++){
                    uint16_t* p16 = (uint16_t*)pixels;
                    uint16_t r = p16[i*4];
                    uint16_t g = p16[i*4 + 1];
                    uint16_t b = p16[i*4 + 2];
                    uint8_t bytes[6] = {
                        (uint8_t)(r >> 8), (uint8_t)(r & 0xFF),
                        (uint8_t)(g >> 8), (uint8_t)(g & 0xFF),
                        (uint8_t)(b >> 8), (uint8_t)(b & 0xFF)
                    };
                    fwrite(bytes,sizeof(bytes),1,fp);
               }
            }else{
                for(size_t i = 0; i < width*height; i++){
                    uint8_t bytes[3] = {pixels[(i*4)],pixels[(i*4)+1],pixels[(i*4)+2]};
                    fwrite(bytes,sizeof(bytes),1,fp);
                }
            }
            break;
        default: return QPBM_ERR_UNKNOWN;
    }
    return QPBM_OK;
}

int qpbm_read(FILE* fp, uint8_t** pixels, uint32_t* width, uint32_t* height, uint16_t* max_value, qpbm_type_n* type, qpbm_fmt_n* format){
    if(!fp) return QPBM_ERR_NULLPTR;
    qpbm_filetype_n fity = QPBM_FT_NOTY;
    qpbm_type_n ftype;
    qpbm_fmt_n   ffmt;
    uint32_t fwid = 0, fhei = 0;
    uint16_t fmax = 0;
    uint8_t* fdat = NULL;
    int e = 0;

    // header
    if(fgetc(fp) != 'P') return QPBM_ERR_NOT_PBM;

    int c = fgetc(fp);
    if(c < QPBM_FT_PBMA || c > QPBM_FT_PPMB) return QPBM_ERR_INVALID_TYPE;
    fity  = (qpbm_filetype_n)c;

    ffmt  = fity >= QPBM_FT_PBMB ? QPBM_BINARY : QPBM_ASCII;
    ftype = (qpbm_type_n)(((c - '1') % 3) + 1);

    if(type && (*type != QPBM_PNM) && *type != ftype) return QPBM_ERR_WRONG_TYPE;
    
    char tok[32];
    // parse width and height
    if(!_qpbm_read_token(fp,tok,sizeof(tok))) return QPBM_ERR_PARSE_ERROR;
    fwid = strtoul(tok,NULL,10);
    if(!_qpbm_read_token(fp,tok,sizeof(tok))) return QPBM_ERR_PARSE_ERROR;
    fhei = strtoul(tok,NULL,10);
    
    // parse max value
    if(fity != QPBM_FT_PBMA && fity != QPBM_FT_PBMB){
        if(!_qpbm_read_token(fp,tok,sizeof(tok))) return QPBM_ERR_PARSE_ERROR;
        fmax = (uint16_t)strtoul(tok,NULL,10);
    }

    // data
    switch(fity){
        case QPBM_FT_PBMA:
            {
                fdat = (uint8_t*)calloc(fwid*fhei,sizeof(uint8_t));
                if(!fdat) return QPBM_ERR_MEM;
                
                for(int i = 0; i < fwid*fhei;){
                    char p = fgetc(fp);
                    if(p == EOF){
                        e = QPBM_ERR_UNKNOWN;
                        goto err;
                    }
                    if (isspace(p)) continue;
                    if      (p == '0') fdat[i] = 0;
                    else if (p == '1') fdat[i] = 1;
                    else{
                        e = QPBM_ERR_PARSE_ERROR;
                        goto err;
                    }
                    i++;
                }
            }
            break;
        case QPBM_FT_PBMB:
            {
                fdat = (uint8_t*) calloc(fwid*fhei,sizeof(uint8_t));
                if(!fdat) return QPBM_ERR_MEM;

                uint16_t row_bytes = (fwid + 7) / 8;
                uint8_t* buf = (uint8_t*)malloc(row_bytes*fhei);
                if(!buf){
                    e = QPBM_ERR_MEM;
                    goto err;
                }
                if(fread(buf,1,row_bytes*fhei,fp) != row_bytes*fhei){
                    free(buf);
                    e = QPBM_ERR_UNKNOWN;
                    goto err;
                }

                for(uint32_t y = 0; y < fhei; y++){
                    for (uint32_t x = 0; x < fwid; x++){
                        uint8_t byte = buf[y*row_bytes + x / 8];
                        int bit = 7 - (x%8);
                        fdat[x + y*fwid] = (byte >> bit) & 1;
                    }
                }
                free(buf);
            }
            break;
        case QPBM_FT_PGMA:
            {
                if (fmax > 255) fdat = (uint8_t*)calloc(fwid * fhei,sizeof(uint16_t));
                else            fdat = (uint8_t*)calloc(fwid * fhei,sizeof(uint8_t));
                if(!fdat) return QPBM_ERR_MEM;
                uint16_t* dat16 = (uint16_t*)fdat;
                
                for(int i = 0; i < fwid*fhei; i++){
                    uint16_t pix = 0;
                    if(fscanf(fp,"%d", &pix) != 1){
                        e = QPBM_ERR_PARSE_ERROR;
                        goto err;
                    }
                    if(fmax > 255) dat16[i] = pix;
                    else fdat[i] = pix;
                }
            }
            break;
        case QPBM_FT_PGMB:
            {
                if (fmax > 255) fdat = (uint8_t*)calloc(fwid * fhei,sizeof(uint16_t));
                else            fdat = (uint8_t*)calloc(fwid * fhei,sizeof(uint8_t));
                if(!fdat) return QPBM_ERR_MEM;
                uint16_t* dat16 = (uint16_t*)fdat;

                for(uint32_t i = 0; i < fwid * fhei; i++){
                    if(fmax <= 255){
                        if(fread(&fdat[i],sizeof(uint8_t),1,fp) != 1) {
                            e = QPBM_ERR_UNKNOWN;
                            goto err;
                        }
                    }
                    else{
                        uint8_t w[2];
                        if(fread(&w,1,2,fp) != 2){
                            e = QPBM_ERR_UNKNOWN;
                            goto err;
                        }
                        dat16[i] = (uint16_t)((w[0] << 8) | w[1]);
                    }
                }
            }
            break;
        case QPBM_FT_PPMA:
            {
                uint8_t bpc   = fmax > 255 ? 2 : 1;
                fdat = (uint8_t*)calloc(fwid*fhei,bpc*4);
                if(!fdat) return QPBM_ERR_MEM;
                uint16_t* dat16 = (uint16_t*)fdat;

                for(uint32_t i = 0; i < fwid * fhei; i++){
                    if (fmax > 255){
                        uint16_t r,g,b;
                        if(fscanf(fp,"%hu %hu %hu", &r,&g,&b) != 3){
                            e = QPBM_ERR_PARSE_ERROR;
                            goto err;
                        }
                        dat16[i*4+0] = r;
                        dat16[i*4+1] = g;
                        dat16[i*4+2] = b;
                        dat16[i*4+3] = 0xFFFF;  // padded max alpha to facilitate use with graphical libraries
                    }else{
                        uint8_t r,g,b;
                        if(fscanf(fp,"%hhu %hhu %hhu", &r,&g,&b) != 3){
                            e = QPBM_ERR_PARSE_ERROR;
                            goto err;
                        }
                        fdat[i*4+0] = r;
                        fdat[i*4+1] = g;
                        fdat[i*4+2] = b;
                        fdat[i*4+3] = 0xFF;  // padded max alpha to facilitate use with graphical libraries
                    }
                }
            }
            break;
        case QPBM_FT_PPMB:
            {
                uint8_t bpc   = fmax > 255 ? 2 : 1;
                fdat = (uint8_t*)calloc(fwid*fhei,bpc*4);
                if(!fdat) return QPBM_ERR_MEM;
                uint16_t* dat16 = (uint16_t*)fdat;

                for(uint32_t i = 0; i < fwid * fhei; i++){
                    if (fmax > 255){
                        uint8_t raw[6] = {0,0,0,0,0,0};
                        if(fread(raw,1,6,fp) != 6){
                            e = QPBM_ERR_PARSE_ERROR; // NOTE: Handle this later
                            goto err;
                        }
                        dat16[i*4+0] = ((raw[0] << 8) | raw[1]);
                        dat16[i*4+1] = ((raw[2] << 8) | raw[3]);
                        dat16[i*4+2] = ((raw[4] << 8) | raw[5]);
                        dat16[i*4+3] = 0xFFFF;  // padded max alpha to facilitate use with graphical libraries
                    }else{
                        uint8_t rgb[3] = {0,0,0};
                        if(fread(rgb,1,3,fp) != 3){
                            e = QPBM_ERR_PARSE_ERROR; // NOTE: Handle this later
                            goto err;
                        }
                        fdat[i*4+0] = rgb[0];
                        fdat[i*4+1] = rgb[1];
                        fdat[i*4+2] = rgb[2];
                        fdat[i*4+3] = 0xFF;  // padded max alpha to facilitate use with graphical libraries
                    }
                }
            }
            break;
        default: return QPBM_ERR_NOT_PBM;
    }
    
    if(type)      *type       = ftype;
    if(format)    *format     = ffmt;
    if(width)     *width      = fwid;
    if(height)    *height     = fhei;
    if(max_value) *max_value  = fmax;
    *pixels     = fdat;

    return QPBM_OK;
err:
    free(fdat);
    return e;
}

int qpbm_save(FILE* fp, qpbm_t  img){
    return qpbm_write(fp,img.pixels,img.width,img.height,img.max_value,img.type,img.format);
}

int qpbm_load(FILE* fp, qpbm_t* img, qpbm_type_n type){
    img->type = type;
    return qpbm_read(fp,&img->pixels,&img->width,&img->height,&img->max_value,&img->type,&img->format);
}

qpbm_t qpbm_from(uint8_t* pixels, uint32_t width, uint32_t height, uint16_t max_value, qpbm_type_n type, qpbm_fmt_n format){
    return (qpbm_t){.type = type, .format = format, .width = width, .height = height, .max_value = max_value, .pixels = pixels};
}

void qpbm_free(qpbm_t* img){
    if(img && img->pixels){
        free(img->pixels);
        img->pixels = NULL;
    }
}
int qpbm_stride(qpbm_t* img){
    if(!img) return 0;
    switch(img->type){
        case QPBM_PBM: return 1;
        case QPBM_PGM: return img->max_value > 255 ? 2 : 1;
        case QPBM_PPM: return img->max_value > 255 ? 8 : 4;
        default:       return 0;
    }
}

#endif /* QPBM_IMPL */
