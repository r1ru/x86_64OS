#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    char buf0[100] = {};
    char buf1[100] = "const uint8_t font_bitmap[][FONT_HEIGHT] = {\n";
    unsigned int c;

    if(argc != 3) {
        puts("Usage: ./a.out [INPUT] [OUTPUT]");
        return 1;
    }

    FILE *fp0 = fopen(argv[1], "r");
    FILE *fp1 = fopen(argv[2], "w+");
    
    fwrite(buf1, 1, strlen(buf1), fp1);
    for(int i = 0; i < 256; i++) {
        while(buf0[0] != '0' || buf0[1] != 'x') {
            fgets(buf0, 100, fp0);
        }

        c = strtol(buf0, NULL, 16);

        sprintf(buf1, "    [%#x] = {\n", c);
        fwrite(buf1, 1, strlen(buf1), fp1);

            for(int j = 0; j < 16; j++) {
                fgets(buf0, 100, fp0);
                assert(buf0[0] == '0' && buf0[1] == 'b');
                if(j != 15)  sprintf(buf1, "        %.10s,\n", buf0);
                else  sprintf(buf1, "        %.10s\n", buf0);
                fwrite(buf1, 1, strlen(buf1), fp1);
            }
            if(i != 256 - 1) fwrite("    },\n", 1, strlen("    },\n"), fp1);
            else fwrite("    }\n", 1, strlen("    }\n"), fp1);
    }
    fwrite("};\n", 1, strlen("};\n"), fp1);

    fclose(fp0);
    fclose(fp1);
    return 0;

    
}