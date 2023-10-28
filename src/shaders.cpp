#include "graphics.h"
#include "engine.h"
#include "stdlib.h"
#include "stdio.h"

Shader* newShaderObject(const char *vertex_source, const char *fragment_source)
{
    Shader *s = (Shader*)malloc(sizeof(Shader));
    s->ShaderID = loadShaders(vertex_source, fragment_source);
    s->ViewID = glGetUniformLocation(s->ShaderID, "VIE");
    s->ProjectionID = glGetUniformLocation(s->ShaderID, "PRO");
    
    return s;
}

void freeShaderObject(Shader *shader)
{
    glDeleteProgram(shader->ShaderID);
    free(shader);
}

GLuint loadShaders(const char *vertex_source, const char *fragment_source)
{
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    printf("GRAPHICS: Compiling Vertex Shader... ");
    glShaderSource(VertexShaderID, 1, &vertex_source, NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *errmsg = (char*)malloc(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, errmsg);
        printf("%s\n", errmsg);
        free(errmsg);
    }
    else
    {
        printf("OK\n");
    }

    printf("GRAPHICS: Compiling Fragment Shader... ");
    glShaderSource(FragmentShaderID, 1, &fragment_source, NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *errmsg = (char*)malloc(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, errmsg);
        printf("%s\n", errmsg);
        free(errmsg);
    }
    else
    {
        printf("OK\n");
    }

    printf("GRAPHICS: linking program... ");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 )
    {
        char *errmsg = (char*)malloc(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, errmsg);
        printf("%s\n", errmsg);
        free(errmsg);
    }
    else
    {
        printf("OK\n");
    }

    glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

char *readfile(FILE *f)
{
    // f invalid? fseek() fail?
    if (f == NULL || fseek(f, 0, SEEK_END))
    {
        return NULL;
    }

    long length = ftell(f);
    rewind(f);
    // Did ftell() fail?  Is the length too long?
    if (length == -1 || (unsigned long)length >= SIZE_MAX)
    {
        return NULL;
    }

    // Convert from long to size_t
    size_t ulength = (size_t)length;
    char *buffer = (char*)malloc(ulength + 1);
    // Allocation failed? Read incomplete?
    if (buffer == NULL || fread(buffer, 1, ulength, f) != ulength)
    {
        free(buffer);
        return NULL;
    }
    buffer[ulength] = '\0'; // Now buffer points to a string

    return buffer;
}

GLuint loadShadersDisk(const char *path_vertex_source, const char *path_fragment_source)
{
    char *vertex_source = NULL;
    char *fragment_source = NULL;

    FILE *vtx = fopen(path_vertex_source, "r");
    vertex_source = readfile(vtx);
    fclose(vtx);

    FILE *frg = fopen(path_fragment_source, "r");
    fragment_source = readfile(frg);
    fclose(frg);


    GLuint result;

    result = loadShaders(vertex_source, fragment_source);

    free(vertex_source);
    free(fragment_source);

    return result;
}