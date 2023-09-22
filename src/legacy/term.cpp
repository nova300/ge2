#include "term.h"

#include "stb_image.h"
#include "stb_image_resize.h"

vec4 *term_font;
int term_font_x;
int term_font_y;

float timer = 0;

char* buffer = NULL;
char* framebuffer = NULL;

int cursorpos = 0;
int buffpos = 0;
int fblen = 0;
int bufflen = 0;
int buffalloc = 0;

int doClear = 0;
int cleared = 0;

int rows = 25;
int columns = 80;

int bufsize = 2000;



int terminal_render()
{
    timer += deltaTime;

    cursorpos = cursorpos % (bufsize - 1);

    if (framebuffer == NULL)
    {
        framebuffer = (char*)malloc(bufsize);

        for (int i = 0; i < bufsize; i++)
        {
            framebuffer[i] = rand();
        }
        
        //doClear = bufsize;

        return 0;
    }

    int k = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            rect glyph = {(9 * j)  , (16 * i) , 9, 16};
            rect clip  = term_get_glyph(framebuffer[k]);
            fb_blit_hi(term_font, term_font_x, clip, glyph.x, glyph.y);
            k++;
        }
    }

    if (doClear)
    {
        if (timer < 0.01) return 0;
        
        for (int c = 0; c < columns; c++)
        {
            framebuffer[cleared] = 0;
            cleared++;
            if (cleared >= doClear) 
            {
                doClear = 0;
                timer = 0;
                return 0;
            } 
        }
        timer = 0;

        return 0;
    }

    if (cleared != bufsize) return 0;

    //if (timer < 0.0001) return 0;
    if (buffpos >= bufflen) return 0;
    timer = 0;

    if (buffer[buffpos] == '\n') 
    {
        cursorpos = cursorpos + ( columns - (cursorpos % columns));
        buffpos++;
    }
    framebuffer[cursorpos] = buffer[buffpos];
    buffpos++;
    cursorpos++;
}

rect term_get_glyph(unsigned char code)
{
    //code = code - 31;
    rect r = {8 * (code % 32), 16 * (((int) code / 32) % 8), 8, 16};
    return r;
}

int terminal_print(const char* str)
{
    if (bufflen > bufsize)
    {
        bufflen = bufflen % bufsize;
    }
    if ( buffpos >= bufflen)
    {
        bufflen = 0;
        buffpos = 0;
    }
    int len = strlen(str);
    if (buffer == NULL)
    {
        buffer = (char*)malloc(32);
        buffalloc = 32;
    }
    if (bufflen + len >= buffalloc)
    {
        buffer = (char*)realloc(buffer, buffalloc + 32 + len);
        buffalloc += 32 + len;
        //printf("allocated %i bytes\n", buffalloc);
    }
    strcpy(buffer + bufflen, str);
    bufflen += len;
}

void terminal_display(const char *str)
{
    int n = strlen(str);
    n++;
    strcpy(framebuffer + (bufsize - n), str);
}

void terminal_clear()
{
    /*for (int i = 0; i < bufsize; i++)
    {
        framebuffer[i] = rand();
    }*/
    doClear = bufsize;
    cleared = 0;
    cursorpos = 0;
    bufflen = 0;
    buffpos = 0;
}

#include "font.inc.c"

void terminal_init()
{
    int comp;
    term_font = (vec4*)stbi_loadf_from_memory((const unsigned char*)&font_data, font_len, &term_font_x, &term_font_y, &comp, STBI_rgb_alpha);

    framebuffer = (char*)malloc(bufsize);
    for (int i = 0; i < bufsize; i++)
    {
        framebuffer[i] = rand();
    }
}