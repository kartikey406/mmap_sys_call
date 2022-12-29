
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>


int main(){
    printf("in mmap file handling \n");
    int file_handle;
    char *fn="/dev/ab";
    const int SIZE = 4096;
    file_handle=open(fn,O_RDWR);
    printf("file_handle %d \n",file_handle);
    int *ptr;
    ptr = (int *)mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, file_handle, 0);
    printf("address of mmap %p\n",ptr);
    if(ptr == MAP_FAILED){
        printf("Mapping Failed\n");
        return 1;
    }
    //printf("value of ptr %d \n",ptr);
    printf("value of ptr %d \n",*ptr);
    ptr[2]=201;
    for(;;);
    return 0;
}