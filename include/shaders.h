#ifndef SHADERS_H
#define SHADERS_H

extern const char *vertex_shader_0;
extern const char *vertex_shader_0_animated;
extern const char *fragment_shader_0;
extern const char *vertex_shader_1;
extern const char *fragment_shader_1;
extern const char *vertex_shader_2;
extern const char *fragment_shader_2;

extern const char *vertex_shader_3;
extern const char *fragment_shader_3;


#ifdef SHADERS_H_IMPLEMENTATION

const char *vertex_shader_0 = 
    "#version 430 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "layout(location = 2) in vec3 vertexNormal;\n"
    "layout(location = 3) in uint texture;\n"
    "layout(location = 4) in mat4 MOD;\n"
    "out vec2 uv;\n"
    "out float light;\n"
    "flat out uint textureIndex;\n"
    "uniform mat4 PRO;\n"
    "uniform mat4 VIE;\n"
    "float lambert(vec3 N, vec3 L)\n"
    "{\n"
    "   vec3 nrmN = normalize(N);\n"
    "   vec3 nrmL = normalize(L);\n"
    "   float result = dot(nrmN, nrmL) / 1 * (1 - 0.6) + 0.6;\n"
    "   return max(result, 0.0);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    mat4 mvp = PRO * VIE * MOD;\n"
    "    vec2 grid = vec2(320,240).xy * 0.5;\n"
    "    vec4 vertexPosition_clipspace = mvp * vec4(vertexPosition_modelspace,1);\n"
    "    vertexPosition_clipspace.xyz = vertexPosition_clipspace.xyz / vertexPosition_clipspace.w;\n"
    "    vertexPosition_clipspace.xy = floor(grid * vertexPosition_clipspace.xy) / grid;\n"
    "    vertexPosition_clipspace.xyz *= vertexPosition_clipspace.w;\n"
    "    gl_Position = vertexPosition_clipspace;\n"
    "    uv = vertexUV;\n"
    "    vec3 vertexPosition_cameraspace = ( VIE * MOD * vec4(vertexPosition_modelspace,1)).xyz;\n"
    "    vec3 vertexNormal_cameraspace = ( VIE * MOD * vec4(vertexNormal,0)).xyz;\n"
    "    vec3 eyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"
    "    light = lambert(vertexNormal_cameraspace, eyeDirection_cameraspace);\n"
    "    textureIndex = texture;\n"
    "}\n";

const char *vertex_shader_0_animated = 
    "#version 430 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "layout(location = 2) in vec3 vertexNormal;\n"
    "layout(location = 3) in uint texture;\n"
    "layout(location = 4) in mat4 MOD;\n"
    "out vec2 uv;\n"
    "out float light;\n"
    "flat out uint textureIndex;\n"
    "uniform mat4 PRO;\n"
    "uniform mat4 VIE;\n"
    "float lambert(vec3 N, vec3 L)\n"
    "{\n"
    "   vec3 nrmN = normalize(N);\n"
    "   vec3 nrmL = normalize(L);\n"
    "   float result = dot(nrmN, nrmL) / 1 * (1 - 0.6) + 0.6;\n"
    "   return max(result, 0.0);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    mat4 mvp = PRO * VIE * MOD;\n"
    "    vec2 grid = vec2(320,240).xy * 0.5;\n"
    "    vec4 vertexPosition_clipspace = mvp * vec4(vertexPosition_modelspace,1);\n"
    "    vertexPosition_clipspace.xyz = vertexPosition_clipspace.xyz / vertexPosition_clipspace.w;\n"
    "    vertexPosition_clipspace.xy = floor(grid * vertexPosition_clipspace.xy) / grid;\n"
    "    vertexPosition_clipspace.xyz *= vertexPosition_clipspace.w;\n"
    "    gl_Position = vertexPosition_clipspace;\n"
    "    uv = vertexUV;\n"
    "    vec3 vertexPosition_cameraspace = ( VIE * MOD * vec4(vertexPosition_modelspace,1)).xyz;\n"
    "    vec3 vertexNormal_cameraspace = ( VIE * MOD * vec4(vertexNormal,0)).xyz;\n"
    "    vec3 eyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"
    "    light = lambert(vertexNormal_cameraspace, eyeDirection_cameraspace);\n"
    "    textureIndex = texture;\n"
    "}\n";

const char *fragment_shader_0 =
"#version 430 core\n"
"in vec2 uv;\n"
"in float light;\n"
"flat in uint textureIndex;\n"
"out vec4 color;\n"
"uniform sampler2DArray texSampler;\n"
"void main(){\n"
"   vec3 col = texture( texSampler, vec3(uv.x, uv.y, textureIndex)).rgb;\n"
"   col = col * light;\n"
"   color = vec4(col , 1);\n"
"}\n";

const char *vertex_shader_1 = 
    "#version 420 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "layout(location = 2) in vec3 vertexNormal;\n"
    "layout(location = 3) in uint texture;\n"
    "layout(location = 4) in vec3 COL;\n"
    "layout(location = 5) in mat4 MOD;\n"
    "out vec2 uv;\n"
    "out vec3 vertexColor;\n"
    "uniform mat4 PRO;\n"
    "uniform mat4 VIE;\n"
    "float lambert(vec3 N, vec3 L)\n"
    "{\n"
    "   vec3 nrmN = normalize(N);\n"
    "   vec3 nrmL = normalize(L);\n"
    "   float result = dot(nrmN, nrmL) / 1 * (1 - 0.4) + 0.4;\n"
    "   return max(result, 0.0);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    mat4 mvp = PRO * VIE * MOD;\n"
    "    vec2 grid = vec2(320,240).xy * 0.5;\n"
    "    vec4 vertexPosition_clipspace = mvp * vec4(vertexPosition_modelspace,1);\n"
    "    vertexPosition_clipspace.xyz = vertexPosition_clipspace.xyz / vertexPosition_clipspace.w;\n"
    "    vertexPosition_clipspace.xy = floor(grid * vertexPosition_clipspace.xy) / grid;\n"
    "    vertexPosition_clipspace.xyz *= vertexPosition_clipspace.w;\n"
    "    gl_Position = vertexPosition_clipspace;\n"
    "    vec3 vertexPosition_cameraspace = ( VIE * MOD * vec4(vertexPosition_modelspace,1)).xyz;\n"
    "    vec3 vertexNormal_cameraspace = ( VIE * MOD * vec4(vertexNormal,0)).xyz;\n"
    "    vec3 eyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"
    "    float l = lambert(vertexNormal_cameraspace, eyeDirection_cameraspace);\n"
    "    vertexColor = COL * l;\n"
    "    textureIndex = texture;\n"
    "}\n";

const char *fragment_shader_1 =
"#version 420 core\n"
"in vec2 uv;\n"
"in vec3 vertexColor;\n"
"flat in uint textureIndex;\n"
"out vec3 color;\n"
"uniform sampler2D texSampler;\n"
"void main(){\n"
"   vec3 col = texture( texSampler, vec3(uv.x, uv.y, textureIndex)).rgb;\n"
"   col = col * vertexColor;\n"
"   color = vec4(col , 1);\n"
"}\n";

const char *vertex_shader_2 = 
    "#version 430 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "layout(location = 2) in vec3 vertexNormal;\n"
    "layout(location = 3) in uint texture;\n"
    "layout(location = 4) in mat4 MOD;\n"
    "out vec2 uv;\n"
    "out float light;\n"
    "flat out uint textureIndex;\n"
    "uniform mat4 PRO;\n"
    "uniform mat4 VIE;\n"
    "float lambert(vec3 N, vec3 L)\n"
    "{\n"
    "   vec3 nrmN = normalize(N);\n"
    "   vec3 nrmL = normalize(L);\n"
    "   float result = dot(nrmN, nrmL) / 1 * (1 - 0.6) + 0.6;\n"
    "   return max(result, 0.0);\n"
    "}\n"
    "void main()\n"
    "{\n"
    "    mat4 mvp = PRO * VIE * MOD;\n"
    "    vec2 grid = vec2(320,240).xy * 0.5;\n"
    "    vec4 vertexPosition_clipspace = mvp * vec4(vertexPosition_modelspace,1);\n"
    "    vertexPosition_clipspace.xyz = vertexPosition_clipspace.xyz / vertexPosition_clipspace.w;\n"
    "    vertexPosition_clipspace.xy = floor(grid * vertexPosition_clipspace.xy) / grid;\n"
    "    vertexPosition_clipspace.xyz *= vertexPosition_clipspace.w;\n"
    "    gl_Position = vertexPosition_clipspace;\n"
    "    uv = vertexUV;\n"
    "    vec3 vertexPosition_cameraspace = ( VIE * MOD * vec4(vertexPosition_modelspace,1)).xyz;\n"
    "    vec3 vertexNormal_cameraspace = ( VIE * MOD * vec4(vertexNormal,0)).xyz;\n"
    "    vec3 eyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;\n"
    "    light = lambert(vertexNormal_cameraspace, eyeDirection_cameraspace);\n"
    "    vec4 vertexPosition_worldspace = MOD * vec4(vertexPosition_modelspace, 1);\n"
    "    light *= vertexPosition_worldspace.y;\n"
    "    textureIndex = texture;\n"
    "}\n";

const char *fragment_shader_2 =
"#version 430 core\n"
"in vec2 uv;\n"
"in float light;\n"
"flat in uint textureIndex;\n"
"out vec4 color;\n"
"uniform sampler2DArray texSampler;\n"
"void main(){\n"
"   vec3 col = texture( texSampler, vec3(uv.x, uv.y, textureIndex)).rgb;\n"
"   col = col * light;\n"
"   color = vec4(col , 1);\n"
"}\n";


const char *vertex_shader_3 = 
    "#version 430 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    "layout(location = 1) in vec2 vertexUV;\n"
    "layout(location = 2) in vec3 vertexNormal;\n"
    "layout(location = 3) in uint texture;\n"
    "layout(location = 4) in mat4 MOD;\n"
    "out vec2 uv;\n"
    "flat out uint textureIndex;\n"
    "uniform mat4 PRO;\n"
    "uniform mat4 VIE;\n"
    "void main()\n"
    "{\n"
    "    //mat4 mvp = PRO * VIE * MOD;\n"
    "    vec4 vertexPosition_clipspace = MOD * PRO * vec4(vertexPosition_modelspace,1);\n"
    "    gl_Position = vertexPosition_clipspace;\n"
    "    uv = vertexUV;\n"
    "    textureIndex = texture;\n"
    "}\n";

const char *fragment_shader_3 =
"#version 430 core\n"
"in vec2 uv;\n"
"flat in uint textureIndex;\n"
"out vec3 color;\n"
"uniform sampler2DArray texSampler;\n"
"void main(){\n"
"   color = texture( texSampler, vec3(uv.x, uv.y, textureIndex)).rgb;\n"
"}\n";

#endif

#endif