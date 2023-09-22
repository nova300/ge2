#include "graphics.h"
#include "engine.h"
#include "stdlib.h"

#include "stb_image.h"
#include "stb_image_resize.h"

#define FBWIDTH 320
#define FBHEIGHT 200

#define HIWIDTH 720
#define HIHEIGHT 400

GLuint program;
GLuint program_bg;
GLuint vao;
GLuint vbo;
GLuint texture;
GLuint texture_bg;
GLuint texture_hi;

GLuint program1;
GLuint program2;

char bg_dirty;
char hi_dirty;

vec4 colors[FBWIDTH * FBHEIGHT];
vec4 colors_bg[FBWIDTH * FBHEIGHT];
vec4 colors_hi[HIWIDTH * HIHEIGHT];

const char *vertex_shader_2d =
    "#version 430 core\n"
    "layout(location = 0) in vec2 position;\n"
    "out vec2 texCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(position * 2.0 - 1.0, -1.0, 1.0);\n"
    "    texCoord.x = position.x;\n"
    "    texCoord.y = 1.0 - position.y;\n"
    "}\n";

const char *fragment_shader_2d =
"#version 430 core\n"
"uniform sampler2D colorTexture;\n"
"uniform float time;\n"
"in vec2 texCoord;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = texture(colorTexture, texCoord);\n"
"}\n";

const char *fragment_shader_2d_water =
"#version 430 core\n"
"uniform sampler2D colorTexture;\n"
"uniform float time;\n"
"in vec2 texCoord;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    vec2 offset;\n"
"    offset.x = cos((time * 0.5) + texCoord.x + texCoord.y * 20.0 * 2.0) * 2.0 * 0.01;\n"
"    fragColor = texture(colorTexture, texCoord + offset);\n"
"}\n";


void fb_init()
{
    program1 = loadShaders(vertex_shader_2d, fragment_shader_2d);
    program2 = loadShaders(vertex_shader_2d, fragment_shader_2d_water);

    program = program1;
    program_bg = program1;

    bg_dirty = true;
    hi_dirty = true;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLfloat vertices[] = 
    {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FBWIDTH, FBHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &texture_bg);
    glBindTexture(GL_TEXTURE_2D, texture_bg);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FBWIDTH, FBHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenTextures(1, &texture_hi);
    glBindTexture(GL_TEXTURE_2D, texture_hi);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, HIWIDTH, HIHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindVertexArray(0);
}

void fb_update()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FBWIDTH, FBHEIGHT, GL_RGBA, GL_FLOAT, colors);
}

void fb_update_bg()
{
    glBindTexture(GL_TEXTURE_2D, texture_bg);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FBWIDTH, FBHEIGHT, GL_RGBA, GL_FLOAT, colors_bg);
    bg_dirty = false;
}

void fb_update_hi()
{
    glBindTexture(GL_TEXTURE_2D, texture_hi);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, HIWIDTH, HIHEIGHT, GL_RGBA, GL_FLOAT, colors_hi);
    hi_dirty = false;
}

void fb_render()
{
    glBindVertexArray(vao);
    fb_update();
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "colorTexture"), 0);
    glUniform1f(glGetUniformLocation(program, "time"), (float)appTime);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void fb_copy_to_bg()
{
    memcpy(colors_bg, colors, sizeof(colors_bg));
    bg_dirty = true;
}

void fb_render_bg()
{
    if (bg_dirty)
    {
        fb_update_bg();
    }
    glBindVertexArray(vao);
    glUseProgram(program_bg);
    glBindTexture(GL_TEXTURE_2D, texture_bg);
    glUniform1i(glGetUniformLocation(program_bg, "colorTexture"), 0);
    glUniform1f(glGetUniformLocation(program_bg, "time"), (float)appTime);
    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
}

void fb_render_hi()
{
    if (hi_dirty)
    {
        fb_update_hi();
    }
    glBindVertexArray(vao);
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, texture_hi);
    glUniform1i(glGetUniformLocation(program, "colorTexture"), 0);
    glUniform1f(glGetUniformLocation(program, "time"), (float)appTime);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void fb_clear()
{
    for (int i = 0; i < FBWIDTH * FBHEIGHT; i++)
    {
        colors[i].x = 0.0f;
        colors[i].y = 0.0f;
        colors[i].z = 0.0f;
        colors[i].w = 0.0f;
    }
}

void fb_drawSineWave(int amplitude, float frequency, float of)
{
    float increment = 2.0f * 3.14f / FBWIDTH;
    int midPoint = FBHEIGHT / 2;
    int x, y;

    for (x = 0; x < FBWIDTH; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of));
        y += midPoint;

        if (y >= 0 && y < FBHEIGHT) {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = 1.0f;
            colors[pixelIndex].y = 0.0f;
            colors[pixelIndex].z = 0.0f;
            colors[pixelIndex].w = 1.0f;
        }
    }
    for (x = 0; x < FBWIDTH; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of + 0.5f));
        y += midPoint;

        if (y >= 0 && y < FBHEIGHT) {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = 0.0f;
            colors[pixelIndex].y = 1.0f;
            colors[pixelIndex].z = 0.0f;
            colors[pixelIndex].w = 1.0f;
        }
    }
    for (x = 0; x < FBWIDTH; x++) {
        y = (int)round(amplitude * sin(frequency * x * increment + of + 1.0f));
        y += midPoint;

        if (y >= 0 && y < FBHEIGHT) {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = 0.0f;
            colors[pixelIndex].y = 0.0f;
            colors[pixelIndex].z = 1.0f;
            colors[pixelIndex].w = 1.0f;
        }
    }
}

void fb_test_dot(void)
{
    int index = rand() % ((FBWIDTH * FBHEIGHT) - 1);

    colors[index].x = (rand() % 100) / 100.0f;
    colors[index].y = (rand() % 100) / 100.0f;
    colors[index].z = (rand() % 100) / 100.0f;
    colors[index].w = 1.0f;
}

void fb_test_pattern(void)
{
    for (int y = 0; y < FBHEIGHT; y++) 
    {
        for (int x = 0; x < FBWIDTH; x++) 
        {
            int pixelIndex = (y * FBWIDTH + x);
            colors[pixelIndex].x = (float)x / FBWIDTH;
            colors[pixelIndex].y = (float)y / FBHEIGHT;
            colors[pixelIndex].z = 1.0 - (float)x / FBWIDTH;
            colors[pixelIndex].w = 1.0;
        }
    }
}

void fb_blit_hi(vec4 *sourceArray, int sourceWidth, rect sourceRect, int destX, int destY)
{
    int x, y;

    for (y = 0; y < sourceRect.h; y++) 
    {
        for (x = 0; x < sourceRect.w; x++) 
        {
            int sourceIndex = (sourceRect.y + y) * sourceWidth + (sourceRect.x + x);
            int destIndex = (destY + y) * HIWIDTH + (destX + x);

            // Copy color values from source to destination
            colors_hi[destIndex] = sourceArray[sourceIndex];
        }
    }

    hi_dirty = true;
}

void fb_blit(vec4 *sourceArray, int sourceWidth, rect sourceRect, int destX, int destY)
{
    int x, y;
    for (y = 0; y < sourceRect.h; y++) 
    {
        for (x = 0; x < sourceRect.w; x++) 
        {
            int sourceIndex = (sourceRect.y + y) * sourceWidth + (sourceRect.x + x);
            int destIndex = (destY + y) * FBWIDTH + (destX + x);
            colors[destIndex] = sourceArray[sourceIndex];
        }
    }
}

void fb_blit_bg(vec4 *sourceArray, int sourceWidth, rect sourceRect, int destX, int destY)
{
    int x, y;
    for (y = 0; y < sourceRect.h; y++) 
    {
        for (x = 0; x < sourceRect.w; x++) 
        {
            int sourceIndex = (sourceRect.y + y) * sourceWidth + (sourceRect.x + x);
            int destIndex = (destY + y) * FBWIDTH + (destX + x);
            colors_bg[destIndex] = sourceArray[sourceIndex];
        }
    }
}

void fb_load_bg(const char *name, char water)
{
    int w;
    int h;
    int comp;
    float *image = stbi_loadf(name, &w, &h, &comp, STBI_rgb_alpha);

    //stbir_resize_float(image, w, h, 0, (float*)colors_bg, 320, 200, 0, STBI_rgb_alpha);

    memcpy(colors_bg, image, sizeof(colors_bg));
    free(image);

    bg_dirty = true;

    if (water)
    {
        program_bg = program2;
    }
    else
    {
        program_bg = program1;
    }
}

void fb_unload_bg()
{
    program_bg = program1;
    memset(colors_bg, 0, sizeof(colors_bg));
    bg_dirty = true;
}