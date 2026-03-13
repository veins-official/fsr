#include <stdio.h>
#include <stdlib.h>

#include "ocean_traffic.h"

int main(int argc, char* argv[]) {
  unsigned char* picture;
  unsigned int width, height;
  ot_error_t error;
  // int size;
  // int bw_size;
  
  if (argc != 2 && argc != 3) {
    printf("Usage: %s input_file [output_file]\n", argv[0]);
    return 1;
  }

  
  error = load_png(&picture, argv[1], &width, &height);
  if (error != OT_SUCCESS) {
    printf("%s\n", ot_error_string(error)); 
    return 1;
  }

  // size = width * height * 4;
  // bw_size = width * height;


  // unsigned char* bw_pic = (unsigned char*)malloc(bw_size*sizeof(unsigned char)); 
  // unsigned char* blr_pic = (unsigned char*)malloc(bw_size*sizeof(unsigned char)); 
  // unsigned char* finish = (unsigned char*)malloc(size*sizeof(unsigned char)); 

  // Например, поиграли с  контрастом
  // contrast(bw_pic, bw_size); 
  // посмотрим на промежуточные картинки
  // write_png("contrast.png", finish, width, height);

  // поиграли с Гауссом
  // Gauss_blur(bw_pic, blr_pic, width, height); 
  // посмотрим на промежуточные картинки
  // write_png("gauss.png", finish, width, height);

  // сделали еще что-нибудь
  // .....
  // ....
  // ....
  // ....
  // ....
  // ....
  // ....
  //

  // write_png("intermediate_result.png", finish, width, height);
  // color(blr_pic, finish, bw_size);

  if (argc == 3) error = write_png(argv[2], picture, width, height);
  else error = write_default_png(picture, width, height);
  if (error != OT_SUCCESS) {
    printf("%s\n", ot_error_string(error)); 
    return 1;
  }

  // не забыли почистить память!
  // free(bw_pic); 
  // free(blr_pic); 
  // free(finish); 
  free(picture); 

  return 0; 
}

