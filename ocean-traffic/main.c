#include <stdio.h>
#include <stdlib.h>

#include "ocean_traffic.h"

int main(void) {
const char* filename = "skull.png"; 
unsigned int width, height;
int size;
int bw_size;

// Прочитали картинку
unsigned char* picture = load_png("skull.png", &width, &height); 
if (picture == NULL)
{ 
printf("Problem reading picture from the file %s. Error.\n", filename); 
return -1; 
} 

size = width * height * 4;
bw_size = width * height;


unsigned char* bw_pic = (unsigned char*)malloc(bw_size*sizeof(unsigned char)); 
unsigned char* blr_pic = (unsigned char*)malloc(bw_size*sizeof(unsigned char)); 
unsigned char* finish = (unsigned char*)malloc(size*sizeof(unsigned char)); 

// Например, поиграли с  контрастом
contrast(bw_pic, bw_size); 
// посмотрим на промежуточные картинки
write_png("contrast.png", finish, width, height);

// поиграли с Гауссом
Gauss_blur(bw_pic, blr_pic, width, height); 
// посмотрим на промежуточные картинки
write_png("gauss.png", finish, width, height);

// сделали еще что-нибудь
// .....
// ....
// ....
// ....
// ....
// ....
// ....
//

write_png("intermediate_result.png", finish, width, height);
color(blr_pic, finish, bw_size); 

// выписали результат
write_png("picture_out.png", finish, width, height); 

// не забыли почистить память!
free(bw_pic); 
free(blr_pic); 
free(finish); 
free(picture); 

return 0; 
}

