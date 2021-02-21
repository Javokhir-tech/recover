#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check usage
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }
    // Open memory card
    FILE *file = fopen(argv[1], "r");
    
    // check for null
    if (!file)
    {
        printf("Error: can't open a file");
        return 1;
    }
    
    // to store data in a buffer
    BYTE buffer[512];
    
    // pic to store img name
    char *pic = (char *)malloc(8 * sizeof(char));   // 8 bytes allocated
    
    // count jpeg
    int c = 0;
    
    //to store recovered image
    FILE *img = NULL;
    
    // read by 512 chunks, returns number
    while (fread(buffer, sizeof(BYTE), 512, file) == 512)
    {
        // Looks for beginning of JPEG, bitwise only compare 1st 4 bites
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // if it's 1st
            if (c == 0)
            {
                // creates to string 000.jpg
                sprintf(pic, "%03i.jpg", c);
                
                // opens img with that name
                img = fopen(pic, "w");
                
                // writes to image from buffer
                fwrite(buffer, sizeof(BYTE), 512, img);
                
                // counter is used to count how many times new jpeg started
                c++;
            }
            // if not 1st
            else
            {
                // close previous
                fclose(img);
                
                sprintf(pic, "%03i.jpg", c);
                
                // open new one
                img = fopen(pic, "w");
                
                fwrite(buffer, sizeof(BYTE), 512, img);
                
                c++;
            }
        }
        // only if we've got one block of data with jpeg bytes, we add next block of date into it
        // no need to open img as it's already opened 
        else if (c > 0)
        {
            fwrite(buffer, sizeof(BYTE), 512, img);
        }
    }
    // free space
    free(pic);
    
    fclose(file);
    
    fclose(img);
    
    return 0;
}