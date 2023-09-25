#include "graphics.h"
#include "engine.h"
#include "stb_image.h"
#include "shaders.h"

#define DEGREES(x) ((x) * 0x10000 / 360)

#define CONVERT(x) ((x) * (180/3.14f))

#define GFX_DIMENSIONS_ASPECT_RATIO (4.0f / 3.0f)

float convert(float radian)
{
   return(radian * (180/3.14f));
}

typedef struct
{
    float yaw;
    float pitch;
    int scaledX;
    int scaledY;
    int upperLeftIndex;
}Skybox;

GLuint skyboxTexture;

Shader *skyboxShader;

static RenderQueue skyboxRq;
static char renderQueueInitialized = false;

#define SKYBOX_WIDTH (4 * SCREEN_WIDTH)
#define SKYBOX_HEIGHT (4 * SCREEN_HEIGHT)

#define SKYBOX_TILE_WIDTH (SCREEN_WIDTH / 2)
#define SKYBOX_TILE_HEIGHT (SCREEN_HEIGHT / 2)

#define SKYBOX_COLS (10)
#define SKYBOX_ROWS (8)

Skybox skyboxinfo;


mat4 matrix;
mat4 modelMatrix;
GeoObject tile_grid[9];

static char initialized = false;

void skybox_cleanup(void)
{
    freeShaderObject(skyboxShader);
    glDeleteTextures(1, &skyboxTexture);
    rq_free(&skyboxRq);
}

void skybox_initialize(void)
{  
    rq_init(&skyboxRq, 10);
    Shader *s = newShaderObject(vertex_shader_3, fragment_shader_3);

    skyboxRq.gpuHandle.shader = s;
    skyboxRq.gpuHandle.textureAtlas = skyboxTexture;
    skyboxShader = s;

    initialized = true;
}

void skybox_load_texture(const char* filename)
{
    if (!initialized) skybox_initialize();

    int tileWidth = 32;
    int tileHeight = 32;
    int channels = 4;
    int tilesX = 8;
    int tilesY = 8;
    int imageCount = tilesX * tilesY;
    int w;
    int h;
    int comp;
    unsigned char *image = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);

    tileWidth = w / tilesX;
    tileHeight = h / tilesY;

    unsigned char *tile = (unsigned char*)malloc(sizeof(unsigned char) * (tileWidth * tileHeight * channels));

    int tileSizeX = tileWidth * channels;
    int rowLenght = tilesX * tileSizeX;

    if(glIsTexture(skyboxTexture)) glDeleteTextures(1, &skyboxTexture);
    
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, skyboxTexture);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, imageCount);

    int n = 0;

    for (unsigned int i = 0; i < tilesY; ++i)
    {   
        for (unsigned int y = 0; y < tilesX; ++y)
        {
            unsigned char *ptr = image + (i * (tileHeight * rowLenght)) + (y * tileSizeX);
            for (int row = 0; row < tileHeight; ++row)
            {
                memcpy(tile + row * tileSizeX, ptr + row * rowLenght, sizeof(unsigned char) * tileSizeX);
            }
            
            //int n = (i * tilesX) + (y * tilesY);
            //n = 80 - n;
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, n, tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, tile);
            n++;
        }
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);
}

int calculate_skybox_scaled_x()
{
    float yawInDegrees = skyboxinfo.yaw + 3.14f;
    //float yawScaled = SCREEN_WIDTH * 360.0f * skyboxinfo.yaw / (fov / 3.14f);
    float yawScaled = (360 / 90.0f) * (yawInDegrees / 6.28f) * SCREEN_WIDTH;
    int scaledX = yawScaled + 0.5f;

    if (scaledX > SKYBOX_WIDTH) 
    {
        scaledX -= scaledX / SKYBOX_WIDTH * SKYBOX_WIDTH;
    }
    return SKYBOX_WIDTH - scaledX;
}

int calculate_skybox_scaled_y()
{
    float pitchInDegrees = skyboxinfo.pitch * 360.0f / 6.28f;

    float degreesToScale = 360.0f * pitchInDegrees / 90.0;
    int roundedY = (int)degreesToScale;

    int scaledY = roundedY + 6 * SKYBOX_TILE_HEIGHT;

    if (scaledY > SKYBOX_HEIGHT) {
        scaledY = SKYBOX_HEIGHT;
    }
    if (scaledY < SCREEN_HEIGHT) {
        scaledY = SCREEN_HEIGHT;
    }
    return scaledY;
}

int get_top_left_tile_idx()
{
    int tileCol = skyboxinfo.scaledX / SKYBOX_TILE_WIDTH;
    int tileRow = (SKYBOX_HEIGHT - skyboxinfo.scaledY) / SKYBOX_TILE_HEIGHT;

    return tileRow * 10 + tileCol;
}

vertex *make_skybox_rect(int tileIndex, int *indicies)
{
    vertex *verts = (vertex*)malloc(4 * sizeof(vertex));
    int x = tileIndex % SKYBOX_COLS * SKYBOX_TILE_WIDTH;
    int y = SKYBOX_HEIGHT - tileIndex / SKYBOX_COLS * SKYBOX_TILE_HEIGHT;

    verts[0] = gfx_make_vertex(x, y, -1, 0, 0);
    verts[1] = gfx_make_vertex(x, y - SKYBOX_TILE_HEIGHT, -1, 0, 1);
    verts[2] = gfx_make_vertex(x + SKYBOX_TILE_WIDTH, y - SKYBOX_TILE_HEIGHT, -1, 1, 1);
    verts[3] = gfx_make_vertex(x + SKYBOX_TILE_WIDTH, y, -1, 1, 0);

    return verts;
}

void render_skybox()
{
    if (!initialized) return;
    vec4 cameraFocus = vector_add(c_pos, c_front);
    vec4 cameraFace = vector_subtract(cameraFocus, c_pos);
    //vector_normalize(&cameraFace);

    skyboxinfo.yaw = ((atan2f(cameraFace.x, cameraFace.z)));
    skyboxinfo.pitch = -((atan2f(sqrtf(cameraFace.x * cameraFace.x + cameraFace.z * cameraFace.z), cameraFace.y)));
    skyboxinfo.scaledX = calculate_skybox_scaled_x();
    skyboxinfo.scaledY = calculate_skybox_scaled_y();
    skyboxinfo.upperLeftIndex = get_top_left_tile_idx();

    

    float left = skyboxinfo.scaledX;
    float right = skyboxinfo.scaledX + SCREEN_WIDTH;

    float half_width = (4.0f / 3.0f) / GFX_DIMENSIONS_ASPECT_RATIO * SCREEN_WIDTH / 2;
    float center = (skyboxinfo.scaledX + SCREEN_WIDTH / 2);
    if (half_width < SCREEN_WIDTH / 2)
    {
        left = center - half_width;
        right = center + half_width;
    }

    //printf("%d\n", skyboxinfo.scaledX);

    if (!renderQueueInitialized)
    {


        renderQueueInitialized = true;
    }

    matrix = matrix_ortho(left, right, skyboxinfo.scaledY - SCREEN_HEIGHT, skyboxinfo.scaledY, 0.0f, 3.0f);
    modelMatrix = IDENTITY_MATRIX;

    skyboxRq.count = 0;

    int row;
    int col;

    int object = 0;

    for (row = 0; row < 3; row++) 
    {
        for (col = 0; col < 3; col++) 
        {
            int tileIndex = skyboxinfo.upperLeftIndex + (row * SKYBOX_COLS) + col;

            int *indicies = (int*)malloc(sizeof(int) * 6);
            indicies[0] = 0;
            indicies[1] = 1;
            indicies[2] = 2;
            indicies[3] = 3;
            indicies[4] = 2;
            indicies[5] = 0;
            vertex *vertices = make_skybox_rect(tileIndex, indicies);
            int *texture = (int*)malloc(sizeof(int));


            int texIndex = tileIndex - (2 * (int)(tileIndex / 10));

            if (tileIndex % 10 == 8 || tileIndex % 10 == 9)
            {
                texIndex = tileIndex - 8 - (2 * (int)(tileIndex / 10));
            }

            *texture = texIndex;
            //printf("%d\n", texIndex);

            tile_grid[object].data = vertices;
            tile_grid[object].indicies = indicies;
            tile_grid[object].dataCount = 4;
            tile_grid[object].indexCount = 6;
            tile_grid[object].transform = &modelMatrix;
            tile_grid[object].instanceCapacity = 1;
            tile_grid[object].instanceCount = 1;
            tile_grid[object].texture = texture;
            tile_grid[object].dataDirty = true;
            tile_grid[object].instanceDirty = true;

            rq_add_object(&skyboxRq, &tile_grid[object]);

            object++;
        }
    }
    

    rq_update_buffers(&skyboxRq);
    skyboxRq.gpuHandle.textureAtlas = skyboxTexture;
    mat4 oldproj = projectionMatrix;
    projectionMatrix = matrix;
    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    geo_render(&skyboxRq.gpuHandle);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    projectionMatrix = oldproj;

    for (int i = 0; i < 9; i++)
    {
        free(tile_grid[i].texture);
        free(tile_grid[i].data);
        free(tile_grid[i].indicies);
    }
}