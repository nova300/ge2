#include "graphics.h"
#include <engine.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
    int x;
    int y;
    int z;
}vec3int;

int geo_obj_loadFromFile(const char* filename, GeoObject *obj)
{
    printf("GEO: %s : Loading obj... ", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        printf("FILE NOT FOUND\n");
        return 1;
    }

    vec3 *vBuffer = (vec3*)malloc(sizeof(vec3) * 128);
    int vBuffLen = 128;
    int vCount = 0;
    vec2 *vtBuffer = (vec2*)malloc(sizeof(vec2) * 128);
    int vtBuffLen = 128;
    int vtCount = 0;
    vec3 *vnBuffer = (vec3*)malloc(sizeof(vec3) * 128);
    int vnBuffLen = 128;
    int vnCount = 0;

    vec3int *vertexIndices = (vec3int*)malloc(sizeof(vec3int) * 128);
    vec3int *uvIndices = (vec3int*)malloc(sizeof(vec3int) * 128);
    vec3int *normalIndices = (vec3int*)malloc(sizeof(vec3int) * 128);
    int iAlloc = 128;
    int iCount = 0;




    int fileEnded = 0;
    while ( !fileEnded )
    {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) fileEnded = 1;

        if (strcmp(lineHeader, "v") == 0)
        {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            if (vCount == vBuffLen)
            {
                printf("v: %d\n", vCount);
                vBuffer = (vec3*)realloc(vBuffer, sizeof(vec3) * (vBuffLen + 128));
                vBuffLen = vBuffLen + 128;
            }
            vBuffer[vCount].x = vertex.x;
            vBuffer[vCount].y = vertex.y;
            vBuffer[vCount].z = vertex.z;
            vCount = vCount + 1;
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            if (vtCount == vtBuffLen)
            {
                printf("vt: %d\n", vtCount);
                vtBuffer = (vec2*)realloc(vtBuffer, sizeof(vec2) * (vtBuffLen + 128));
                vtBuffLen = vtBuffLen + 128;
            }
            vtBuffer[vtCount].x = uv.x;
            vtBuffer[vtCount].y = uv.y;
            vtCount = vtCount + 1;
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            if (vnCount == vnBuffLen)
            {
                printf("vn: %d\n", vnCount);
                vnBuffer = (vec3*)realloc(vnBuffer, sizeof(vec3) * (vnBuffLen + 128));
                vnBuffLen = vnBuffLen + 128;
            }
            vnBuffer[vnCount].x = normal.x;
            vnBuffer[vnCount].y = normal.y;
            vnBuffer[vnCount].z = normal.z;
            vnCount = vnCount + 1;
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            
            if ((iCount + 1) == iAlloc)
            {
                printf("i: %d\n", iCount);
                vertexIndices = (vec3int*)realloc(vertexIndices ,sizeof(vec3int) * (iAlloc + 128));
                uvIndices = (vec3int*)realloc(uvIndices , sizeof(vec3int) * (iAlloc + 128));
                normalIndices = (vec3int*)realloc(normalIndices ,sizeof(vec3int) * (iAlloc + 128));
                iAlloc = iAlloc + 128;
            }
            vertexIndices[iCount].x = vertexIndex[0];
            vertexIndices[iCount].y = vertexIndex[1];
            vertexIndices[iCount].z = vertexIndex[2];
            uvIndices[iCount].x = uvIndex[0];
            uvIndices[iCount].y = uvIndex[1];
            uvIndices[iCount].z = uvIndex[2];
            normalIndices[iCount].x = normalIndex[0];
            normalIndices[iCount].y = normalIndex[1];
            normalIndices[iCount].z = normalIndex[2];
            iCount = iCount + 1;
        }
    }

    

    int vertCount = (iCount - 1) * 3;

    int bufferLength = sizeof(vec3) * vertCount;
    int triCount = (iCount - 1);
    vec3 *vertexBuffer = (vec3*)malloc(sizeof(vec3) * vertCount);
    vec2 *uvBuffer = (vec2*)malloc(sizeof(vec2) * vertCount);
    vec3 *normalBuffer = (vec3*)malloc(sizeof(vec3) * vertCount);

    int *vi = (int*)vertexIndices;
    int *ui = (int*)uvIndices;
    int *ni = (int*)normalIndices;

    for (int i = 0; i < vertCount; i++)
    {
        int vertexIndex = vi[i];
        int uvIndex = ui[i];
        int normalIndex = ni[i];
        vec3 vertex = vBuffer[vertexIndex - 1];
        vec2 uv = vtBuffer[uvIndex - 1];
        vec3 normal = vnBuffer[normalIndex - 1];
        vertexBuffer[i] = vertex;
        uvBuffer[i] = uv;
        normalBuffer[i] = normal;
    }

    fclose(file);
    free(vBuffer);
    free(vtBuffer);
    free(vnBuffer);
    free(vertexIndices);
    free(uvIndices);
    free(normalIndices);

    printf("OK\n----------FILE STATS----------\n");
    printf("polygons to render: %d\n", iCount - 1);
    printf("polygons on disk: %d\n", vCount - 1);
    printf("verts to render: %d\n", vertCount);
    printf("verts on disk: %d\n------------------------------\n", (vCount - 1) * 3);

    geo_obj_createObjectData(obj, vertexBuffer, uvBuffer, normalBuffer, vertCount, 0.001f);

    free(vertexBuffer);
    free(uvBuffer);
    free(normalBuffer);

    return 0;
}

int FloatEquals(float a, float b, float floatEqualityThreshold) 
{
    return fabsf(a - b) <= floatEqualityThreshold;
}

int geo_obj_createObjectData(GeoObject *obj, vec3* vertices, vec2* uvs, vec3* normals, int vertexCount, float floatEqualityThreshold) {
    
    obj->indexCount = vertexCount;

    obj->type = GOBJ_TYPE_STANDARD;

    obj->data = (vertex *)malloc(vertexCount * sizeof(vertex));
    
    obj->indicies = (int*)malloc(vertexCount * sizeof(int));

    int index = 0;
    for (int i = 0; i < vertexCount; i++) 
    {
        int j;
        for (j = 0; j < i; j++) 
        {
            if (FloatEquals(vertices[i].x, obj->data[j].vertex.x, floatEqualityThreshold) &&
                FloatEquals(vertices[i].y, obj->data[j].vertex.y, floatEqualityThreshold) &&
                FloatEquals(vertices[i].z, obj->data[j].vertex.z, floatEqualityThreshold) &&
                FloatEquals(uvs[i].x, obj->data[j].uv.x, floatEqualityThreshold) &&
                FloatEquals(uvs[i].y, obj->data[j].uv.y, floatEqualityThreshold) &&
                FloatEquals(normals[i].x, obj->data[j].normal.x, floatEqualityThreshold) &&
                FloatEquals(normals[i].y, obj->data[j].normal.y, floatEqualityThreshold) &&
                FloatEquals(normals[i].z, obj->data[j].normal.z, floatEqualityThreshold)) 
            {
                obj->indicies[i] = j;
                break;
            }
        }

        if (j == i) 
        {
            obj->data[index].vertex = vertices[i];
            obj->data[index].uv = uvs[i];
            obj->data[index].normal = normals[i];
            obj->indicies[i] = index;
            index++;
        }
    }

    obj->dataCount = index;

    return 0;
}

GeoObject *geo_obj_createFromParShape(par_shapes_mesh* mesh)
{
    GeoObject *g = (GeoObject*)malloc(sizeof(GeoObject));
    g->data = NULL;
    g->indicies = NULL;
    g->type = GOBJ_TYPE_STANDARD;
    transform_set_identity(&g->baseTransform);
    g->instanceCount = 1;
    g->instanceCapacity = 1;
    g->transform = &(g->baseTransform.matrix);
    g->texture = &(g->baseTexture);

    g->dataCount = mesh->npoints;
    g->data = (vertex*)malloc(g->dataCount * sizeof(vertex));
    g->indexCount = mesh->ntriangles * 3;
    g->indicies = (int*)malloc(g->indexCount * sizeof(int));

    for (int i = 0; i < mesh->npoints; i++) 
    {
        g->data[i].vertex.x = mesh->points[i * 3];
        g->data[i].vertex.y = mesh->points[i * 3 + 1];
        g->data[i].vertex.z = mesh->points[i * 3 + 2];

        if (mesh->normals != NULL)
        {
            g->data[i].normal.x = mesh->normals[i * 3];
            g->data[i].normal.y = mesh->normals[i * 3 + 1];
            g->data[i].normal.z = mesh->normals[i * 3 + 2];
        }
        else
        {
            g->data[i].normal.x = 0;
            g->data[i].normal.y = 0;
            g->data[i].normal.z = 0;
        }

        if (mesh->tcoords != NULL)
        {
            g->data[i].uv.x = mesh->tcoords[i * 2];
            g->data[i].uv.y = mesh->tcoords[i * 2 + 1];
        }
        else
        {
            g->data[i].uv.x = 0;
            g->data[i].uv.y = 0;
        }
    }

    for (int i = 0; i < mesh->ntriangles; i++) 
    {
        g->indicies[i * 3] = mesh->triangles[i * 3];
        g->indicies[i * 3 + 1] = mesh->triangles[i * 3 + 1];
        g->indicies[i * 3 + 2] = mesh->triangles[i * 3 + 2];
    }

    return g;
}

void geo_obj_free(GeoObject *gobj)
{
    free(gobj->data);
    free(gobj->indicies);
    free(gobj);
}

void geo_instanceop_init(GeoObject *obj, int capacity) 
{
    obj->transform = (mat4*)malloc(capacity * sizeof(mat4));
    obj->texture = (int*)malloc(capacity * sizeof(int));
    obj->instanceCount = 0;
    obj->instanceCapacity = capacity;
}

void geo_instanceop_free(GeoObject *obj)
{
    free(obj->transform);
    free(obj->texture);
    obj->transform = (mat4*)&obj->baseTransform;
    obj->texture = &obj->baseTexture;
    obj->instanceCapacity = 0;
    obj->instanceCount = 0;
    obj->instanceDirty = 1;
}

void geo_instanceop_resize(GeoObject *obj, int newCapacity) 
{
    mat4 *newTransform = (mat4*)realloc(obj->transform, newCapacity * sizeof(mat4));
    if (newTransform == NULL) return;
    int *newTexture = (int*)realloc(obj->texture, newCapacity * sizeof(int));
    if (newTexture == NULL) return;

    obj->transform = newTransform;
    obj->texture = newTexture;
    obj->instanceCapacity = newCapacity;
    obj->instanceDirty = 1;
}

void geo_instanceop_add(GeoObject *obj, mat4 matrix, int textureIndex) 
{
    if (obj->instanceCount >= obj->instanceCapacity)
    {
        geo_instanceop_resize(obj, obj->instanceCapacity * 2);
    }

    obj->transform[obj->instanceCount] = matrix;
    obj->texture[obj->instanceCount] = textureIndex;
    obj->instanceCount++; 
    obj->instanceDirty = 1;
}

void geo_instanceop_remove(GeoObject *obj, int index) 
{
    if (index >= obj->instanceCount) return;

    for (int i = index + 1; i < obj->instanceCount; i++) 
    {
        obj->data[i - 1] = obj->data[i];
    }

    obj->instanceCount--;
    obj->instanceDirty = 1;
}

void geo_instanceop_clear(GeoObject *obj) 
{
    obj->instanceCount = 0;
    obj->instanceDirty = 1;
}

GeoObject *geo_new_object(void)
{
    GeoObject *g = (GeoObject*)malloc(sizeof(GeoObject));
    g->data = NULL;
    g->indicies = NULL;
    g->baseTexture = 1;
    g->type = GOBJ_TYPE_UNDEFINED;

    transform_set_identity(&g->baseTransform);
    g->instanceCount = 1;
    g->instanceCapacity = 1;
    g->transform = &(g->baseTransform.matrix);
    g->texture = &(g->baseTexture);

    g->dataDirty = 1;
    g->instanceDirty = 1;

    return g;
}

GeoObject geo_new_stack_object(void)
{
    GeoObject g;
    g.data = NULL;
    g.indicies = NULL;
    g.type = GOBJ_TYPE_UNDEFINED;

    transform_set_identity(&g.baseTransform);

    g.baseTexture = 1;
    g.instanceCount = 1;
    g.instanceCapacity = 0;
    g.transform = &g.baseTransform.matrix;
    g.texture = &g.baseTexture;

    g.dataDirty = 1;
    g.instanceDirty = 1;
    
    return g;
}

void particle_update(ParticleSystem *ps)
{
    if (ps == NULL) return;
    geo_instanceop_clear(ps->geo);
    for (int i = 0; i < ps->amount; i++)
    {
        if (ps->particles[i].lifeTime < 0)
        {
            
            ps->particles[i].lifeTime = (rand() % 50) + 25;
            ps->particles[i].transform.position = ps->transform->position;
            ps->particles[i].xdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
            ps->particles[i].ydir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
            ps->particles[i].zdir = ((rand() - rand()) % 3) + ((rand() - rand()) % 10);
            ps->particles[i].texture = rand() % 50;
        }
        else
        {
            ps->particles[i].lifeTime = ps->particles[i].lifeTime - (deltaTime * 100);
            transform_move(ps->particles[i].xdir * (deltaTime * 10), ps->particles[i].ydir * (deltaTime * 10), ps->particles[i].zdir * (deltaTime * 10), &ps->particles[i].transform);
            geo_instanceop_add(ps->geo, ps->particles[i].transform.matrix, ps->particles[i].texture);
        }
    }
}

ParticleSystem* particle_new(GeoObject *g, int amount)
{
    geo_instanceop_init(g, amount);
    ParticleSystem *ps = (ParticleSystem*)malloc(sizeof(ParticleSystem));
    ps->transform = &(g->baseTransform);
    Particle *p = (Particle*)malloc(sizeof(Particle) * amount);
    ps->geo = g;
    ps->amount = amount;

    
    for (int i = 0; i < amount; i++)
    {
        Transform *pt = (Transform*)&p[i];
        p[i].lifeTime = 0;
        p[i].xdir = 0;
        p[i].ydir = 0;
        p[i].zdir = 0;
        transform_set_identity(pt);
    }
    
    ps->particles = p;

    return ps;
}


GeoObject_gpu *geo_obj_bindToGpu(GeoObject obj)
{
    GeoObject_gpu *gobj = (GeoObject_gpu*)malloc(sizeof(GeoObject_gpu));

    gobj->geoObject = obj;

    geo_obj_gpu_handle_genBuffers(&gobj->gpuHandle, gobj->geoObject.type);

    return gobj;
}

GeoObject_gpu *geo_obj_bindToGpu_and_free(GeoObject *obj)
{
    GeoObject_gpu *gobj = geo_obj_bindToGpu(*obj);
    free(obj);
    return gobj;
}

void geo_obj_gpu_updateBuffers(GeoObject_gpu *obj)
{

}


