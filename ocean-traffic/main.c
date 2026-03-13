#include <stdio.h>
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

  return 0;
}

