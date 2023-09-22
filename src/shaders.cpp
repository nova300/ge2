#include "graphics.h"
#include "engine.h"
#include "stdlib.h"

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