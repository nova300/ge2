#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define PAR_SHAPES_IMPLEMENTATION
#include "par_shapes.h"

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"

#include "graphics.h"

using namespace hlslpp;

/* 3D math */

//mat4 IDENTITY_MATRIX = 

float radians(float dgr)
{
    float rad = dgr * 3.14 / 180;
    return rad;
}

void vector_normalize(float4* v) 
{
    #ifndef DO_MMX
	float sqr = v->x * v->x + v->y * v->y + v->z * v->z;
	if(sqr == 1 || sqr == 0)
		return;
	float invrt = 1.f/sqrt(sqr);
	v->x *= invrt;
	v->y *= invrt;
	v->z *= invrt;

    return;
    #endif

    #ifdef DO_MMX
    const float *p = (float*)v;

    __m128 acc;
    __m128 result;

    const __m128 a = _mm_loadu_ps(p);
    acc = _mm_mul_ps(a , a);

    __m128 shuf = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums = _mm_add_ps(acc, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    result = _mm_add_ss(sums, shuf);

    result = _mm_rsqrt_ss(result);

    acc = _mm_loadu_ps(p);
    result = _mm_mul_ss(acc, result);

    _mm_store_ps((float*)v, result);

    v->w = 0;
    #endif
}

float vector_dot(float4 *v1, float4 *v2) 
{
    #ifndef DO_MMX
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
    #endif
    
    
    #ifdef DO_MMX
    const float *p1 = (float*)v1;
    const float *p2 = (float*)v2;

    const float* const p1End = p1 + 4;

    __m128 acc;

    {
        const __m128 a = _mm_loadu_ps(p1);
        const __m128 b = _mm_loadu_ps(p2);
        acc = _mm_mul_ps(a , b);
        p1 += 4;
		p2 += 4;
    }

    for (; p1 < p1End; v1 += 4, p2 += 4)
    {
        const __m128 a = _mm_loadu_ps(p1);
        const __m128 b = _mm_loadu_ps(p2);
        acc = _mm_add_ps( _mm_mul_ps( a, b ), acc );
    }

    //const __m128 r2 = _mm_add_ps( dot0, _mm_movehl_ps( dot0, dot0 ) );
    //const __m128 r1 = _mm_add_ss( r2, _mm_shuffle_ps(r2, r2, 0x55) );

    //r = _mm_shuffle_ps(dot0, dot0, 0x55);

    __m128 shuf = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums = _mm_add_ps(acc, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    //float result = _mm_cvtss_f32(sums);

    return _mm_cvtss_f32(sums);
    #endif
}

/*
float *vector_dot_multi(vec4 *v1, vec4 *v2, unsigned int count) 
{

    //scalar failsafe
    #ifndef DO_MMX
    float ret = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
	return &ret;
    #endif

    #ifdef DO_MMX
    float *results = malloc(sizeof(float) * count);

    const float *p1 = (float*)v1;
    const float *p2 = (float*)v2;
    unsigned int accumulators = 0;

    if (count > 3) accumulators = 4;
    else if(count > 1) accumulators = 2;
    else accumulators = 1;

    unsigned int valuesPerLoop = 0;
    valuesPerLoop = accumulators * 4;

    if( !(0 == count % valuesPerLoop ))
    {
        printf("[VECTOR DOT MULTI] alignment error\n");
    }

    const float* const p1End = p1 + count;

    __m128 acc0, acc1, acc2, acc3;

    for (; p1 < p1End; v1 += valuesPerLoop, p2 += valuesPerLoop)
    {
        __m128 a = _mm_loadu_ps(p1);
        __m128 b = _mm_loadu_ps(p2);
        acc0 = _mm_mul_ps(a , b);
        __m128 shuf = _mm_shuffle_ps(acc0, acc0, _MM_SHUFFLE(2, 3, 0, 1));
        __m128 sums = _mm_add_ps(acc0, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        //float result = _mm_cvtss_f32(sums);

        if (accumulators > 1)
        {
            a = _mm_loadu_ps(p1 + 4);
            b = _mm_loadu_ps(p2 + 4);
            acc1 = _mm_mul_ps(a , b);
            shuf = _mm_shuffle_ps(acc1, acc1, _MM_SHUFFLE(2, 3, 0, 1));
            sums = _mm_add_ps(acc1, shuf);
            shuf = _mm_movehl_ps(shuf, sums);
            sums = _mm_add_ss(sums, shuf);
            //float result = _mm_cvtss_f32(sums);
        }
        if (accumulators > 2)
        {
            a = _mm_loadu_ps(p1 + 8);
            b = _mm_loadu_ps(p2 + 8);
            acc2 = _mm_mul_ps(a , b);
            shuf = _mm_shuffle_ps(acc2, acc2, _MM_SHUFFLE(2, 3, 0, 1));
            sums = _mm_add_ps(acc2, shuf);
            shuf = _mm_movehl_ps(shuf, sums);
            sums = _mm_add_ss(sums, shuf);
            //float result = _mm_cvtss_f32(sums);
        }
        if (accumulators > 3)
        {
            a = _mm_loadu_ps(p1 + 12);
            b = _mm_loadu_ps(p2 + 12);
            acc3 = _mm_mul_ps(a , b);
            shuf = _mm_shuffle_ps(acc3, acc3, _MM_SHUFFLE(2, 3, 0, 1));
            sums = _mm_add_ps(acc3, shuf);
            shuf = _mm_movehl_ps(shuf, sums);
            sums = _mm_add_ss(sums, shuf);
            //float result = _mm_cvtss_f32(sums);
        }
    }

    return results;

    for (; p1 < p1End; v1 += 4, p2 += 4)
    {
        const __m128 a = _mm_loadu_ps(p1);
        const __m128 b = _mm_loadu_ps(p2);
        acc = _mm_add_ps( _mm_mul_ps( a, b ), acc );
    }

    //const __m128 r2 = _mm_add_ps( dot0, _mm_movehl_ps( dot0, dot0 ) );
    //const __m128 r1 = _mm_add_ss( r2, _mm_shuffle_ps(r2, r2, 0x55) );

    //r = _mm_shuffle_ps(dot0, dot0, 0x55);

    __m128 shuf = _mm_shuffle_ps(acc, acc, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sums = _mm_add_ps(acc, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    //float result = _mm_cvtss_f32(sums);

    //return _mm_cvtss_f32(sums);

    #endif
}
*/


float4 vector_cross(float4 v1, float4 v2) 
{
	float4 out;
	out.x = v1.y*v2.z - v1.z*v2.y;
	out.y = v1.z*v2.x - v1.x*v2.z;
	out.z = v1.x*v2.y - v1.y*v2.x;
	return out;
}

float4 vector_subtract(float4 v1, float4 v2)
{
    float4 out;
    out.x = v1.x - v2.x;
    out.y = v1.y - v2.y;
    out.z = v1.z - v2.z;
    return out;
}

float4 vector_scale(float4 v1, float s)
{
    float4 out;
    out.x = v1.x * s;
    out.y = v1.y * s;
    out.z = v1.z * s;
    return out;
}

float4 vector_add(float4 v1, float4 v2)
{
    float4 out;
    out.x = v1.x + v2.x;
    out.y = v1.y + v2.y;
    out.z = v1.z + v2.z;
    return out;
}

float vector_distance(float4 v1, float4 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    
    return sqrt(dx * dx + dy * dy + dz * dz);
}

float vector_distance_sqr(float4 v1, float4 v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float dz = v2.z - v1.z;
    
    return dx * dx + dy * dy + dz * dz;
}

float4x4 matrix_perspective(float fovy, float aspect_ratio, float near_plane, float far_plane) 
{
	float4x4 out;

    float tan_half_angle = tan(fovy / 2);
	float x_scale = 1 / (aspect_ratio * tan_half_angle);
	float y_scale = 1 / (tan_half_angle);
	float frustum_length = far_plane - near_plane;

	//out.x.x = x_scale;
	//out.y.y = y_scale;
	//out.z.z = -((far_plane + near_plane) / frustum_length);
	//out.z.w = -1;
	//out.w.z = -((2 * near_plane * far_plane) / frustum_length);

    out = float4x4(
        x_scale, 0, 0, 0,
        0, y_scale, 0, 0,
        0, 0, -((far_plane + near_plane) / frustum_length), -1,
        0, 0, -((2 * near_plane * far_plane) / frustum_length), 0
    );
	
	return out;
}

float4x4 matrix_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane) 
{
	float4x4 out = IDENTITY_MATRIX;

	out._m00 = 2.0f / (right - left);
	out._m11 = 2.0f / (top - bottom);
	out._m22 = -2.0f / (far_plane - near_plane);
	out._m30 = -(right + left) / (right - left);
	out._m31 = -(top + bottom) / (top - bottom);
    out._m32 = -(far_plane + near_plane) / (far_plane - near_plane);
    out._m33 = 1.0f;
	
	return out;
}

float4x4 matrix_lookAt(float4 eye, float4 center, float4 up) 
{
    float4 f = vector_subtract(center, eye);
	vector_normalize(&f);
	float4 u = up;
    vector_normalize(&u);
	float4 s = vector_cross(f, u);
	vector_normalize(&s);
	u = vector_cross(s, f);

	float4x4 out = IDENTITY_MATRIX;
	out._m00 = s.x;
	out._m10 = s.y;
	out._m20 = s.z;

	out._m01 = u.x;
	out._m11 = u.y;
	out._m21 = u.z;

	out._m02 = -f.x;
	out._m12 = -f.y;
	out._m22 = -f.z;

	out._m30 = -vector_dot(&s, &eye);
	out._m31 = -vector_dot(&u, &eye);
	out._m32 =  vector_dot(&f, &eye);
	return out;
}

void matrix_rotateY(float4x4* m, float angle) 
{
	/*mat4 rotation = IDENTITY_MATRIX;
	float sine = (float)sin(angle);
	float cosine = (float)cos(angle);
	
	rotation._m00 = cosine;
	rotation._m20 = sine;
	rotation._m02 = -sine;
	rotation._m22 = cosine;*/
    float4x4 matrix_rotated;
	matrix_rotated = m->rotation_y(angle);
    *m = matrix_rotated;
}

float4x4 matrix_multiply(float4x4* m1, float4x4* m2) 
{
	float4x4 out = *m1 * *m2;
	return out;
}

/* Object Transforms */

void transform_position(float x, float y, float z, Transform *t)
{
    t->position.x = x;
    t->position.y = y;
    t->position.z = z;
    //transform_make_matrix(t);
}

void transform_move(float x, float y, float z, Transform *t)
{
    t->position.x += x;
    t->position.y += y;
    t->position.z += z;
    //transform_make_matrix(t);
}

void transform_rotate(float x, float y, float z, Transform *t)
{
    // Convert Euler angles to quaternion
    float cy = cosf(z * 0.5f);
    float sy = sinf(z * 0.5f);
    float cr = cosf(x * 0.5f);
    float sr = sinf(x * 0.5f);
    float cp = cosf(x * 0.5f);
    float sp = sinf(x * 0.5f);

    float4 rotation;
    rotation.x = sr * cp * cy - cr * sp * sy;
    rotation.y = cr * sp * cy + sr * cp * sy;
    rotation.z = cr * cp * sy - sr * sp * cy;
    rotation.w = cr * cp * cy + sr * sp * sy;

    float4 rotated_q;
    rotated_q.w = t->rotation.w * rotation.w - t->rotation.x * rotation.x - t->rotation.y * rotation.y - t->rotation.z * rotation.z;
    rotated_q.x = t->rotation.w * rotation.x + t->rotation.x * rotation.w + t->rotation.y * rotation.z - t->rotation.z * rotation.y;
    rotated_q.y = t->rotation.w * rotation.y - t->rotation.x * rotation.z + t->rotation.y * rotation.w + t->rotation.z * rotation.x;
    rotated_q.z = t->rotation.w * rotation.z + t->rotation.x * rotation.y - t->rotation.y * rotation.x + t->rotation.z * rotation.w;

    t->rotation = rotated_q;

    //transform_make_matrix(t);
}

void transform_set_rotation(float x, float y, float z, Transform *t)
{
    float cy = cosf(z * 0.5f);
    float sy = sinf(z * 0.5f);
    float cr = cosf(x * 0.5f);
    float sr = sinf(x * 0.5f);
    float cp = cosf(y * 0.5f);
    float sp = sinf(y * 0.5f);

    t->rotation.x = sr * cp * cy - cr * sp * sy;
    t->rotation.y = cr * sp * cy + sr * cp * sy;
    t->rotation.z = cr * cp * sy - sr * sp * cy;
    t->rotation.w = cr * cp * cy + sr * sp * sy;
    //transform_make_matrix(t);
}

void transform_make_matrix(Transform *t) 
{

    float4x4 model_matrix = IDENTITY_MATRIX;

    // Translation matrix
    model_matrix._m30 = t->position.x;
    model_matrix._m31 = t->position.y;
    model_matrix._m32 = t->position.z;
    model_matrix._m33 = 1.0f;

    // Rotation matrix
    float x = t->rotation.x;
    float y = t->rotation.y;
    float z = t->rotation.z;
    float w = t->rotation.w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float ww = w * w;

    model_matrix._m00 = xx - yy - zz + ww;
    model_matrix._m10 = 2.0f * (x * y + z * w);
    model_matrix._m20 = 2.0f * (x * z - y * w);

    model_matrix._m01 = 2.0f * (x * y - z * w);
    model_matrix._m11 = -xx + yy - zz + ww;
    model_matrix._m21 = 2.0f * (y * z + x * w);

    model_matrix._m02 = 2.0f * (x * z + y * w);
    model_matrix._m12 = 2.0f * (y * z - x * w);
    model_matrix._m22 = -xx - yy + zz + ww;

    // Scale matrix
    float4x4 scaled_matrix;
    scaled_matrix = model_matrix.scale(t->scale.x, t->scale.y, t->scale.z);

    //t->matrix = scaled_matrix;
    store(model_matrix ,(float*)&t->matrix);
}

void transform_set_identity(Transform* t) 
{
    t->position = {0.0f, 0.0f, 0.0f, 0.0f};
    t->rotation = {0.0f, 0.0f, 0.0f, 1.0f};
    t->scale = {1.0f, 1.0f, 1.0f, 0.0f};
    store(float4x4::identity(), (float*)&t->matrix);
}

void geo_render(GeoObject_gpu_handle *rq)
{
    if (rq->type == GOBJ_TYPE_UNDEFINED) return;
    glBindVertexArray(rq->vertexArray);
    glUseProgram(rq->shader->ShaderID);
    glUniformMatrix4fv(rq->shader->ViewID, 1, GL_FALSE, &(viewMatrix.m[0]));
    glUniformMatrix4fv(rq->shader->ProjectionID, 1, GL_FALSE, &(projectionMatrix.m[0]));

    glBindTexture(GL_TEXTURE_2D_ARRAY, rq->textureAtlas);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rq->elementBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, rq->vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, rq->textureBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, rq->transformBuffer);

    switch (rq->type)
    {
    case GOBJ_TYPE_MULTI:
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rq->commandBuffer);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, rq->count, 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        break;
    
    default:
        glDrawElementsInstanced(GL_TRIANGLES, rq->indexCount, GL_UNSIGNED_INT, (void*)0, rq->indexCount);
        break;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


GLuint generateColorTexture(float r, float g, float b, float a)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLfloat colors[4];

    colors[0] = r;
    colors[1] = g;
    colors[2] = b;
    colors[3] = a;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, colors);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
}

//generates array of 100 random color textures
GLuint generateRandomAtlas(void)
{
    GLuint texture;
    glGenTextures(1, &texture);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 1, 1, 50);

    for (unsigned int i = 0; i < 50; ++i)
    {
        GLubyte colors[4];
        colors[0] = ((GLubyte)(rand() % 255));
        colors[1] = ((GLubyte)(rand() % 255));
        colors[2] = ((GLubyte)(rand() % 255));
        colors[3] = 255;

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, colors);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return texture;
}

GLuint loadTexture(const char *name)
{
    GLuint texture;

    return texture;
}

void rq_update_buffers(RenderQueue *rq)
{
    if (rq->gpuHandle.type == GOBJ_TYPE_UNDEFINED) return;
    char dataNeedsUpdate = 0;
    char instanceNeedUpdate = 0;
    char commandsNeedUpdate = 0;
    int count = rq->count;
    int i;
    GeoObject **obj = rq->objectBuffer;
    for (i = 0; i < count; i++)
    {
        dataNeedsUpdate = dataNeedsUpdate | obj[i]->dataDirty;
        obj[i]->dataDirty = 0;
        instanceNeedUpdate = instanceNeedUpdate | obj[i]->instanceDirty;
        obj[i]->instanceDirty = 0;
    }

    if (dataNeedsUpdate)
    {
        int indexCount = 0;
        int vertexCount = 0;
        for (i = 0; i < count; i++)
        {
            indexCount += obj[i]->indexCount;
            vertexCount += obj[i]->dataCount;
        }

        int *indicies = (int*)malloc(indexCount * sizeof(int));
        vertex *vertices = (vertex*)malloc(vertexCount * sizeof(vertex));
        int vertex_offset = 0;
        int index_offset = 0;
        for (i = 0; i < count; i++) 
        {
            memcpy(vertices + vertex_offset, obj[i]->data, obj[i]->dataCount * sizeof(vertex));
            memcpy(indicies + index_offset, obj[i]->indicies, obj[i]->indexCount * sizeof(int));
            vertex_offset += obj[i]->dataCount;
            index_offset += obj[i]->indexCount;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rq->gpuHandle.elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indicies, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, rq->gpuHandle.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(vertex), vertices, GL_STATIC_DRAW);

        commandsNeedUpdate = 1;

        free(indicies);
        free(vertices);
    }

    if (instanceNeedUpdate)
    {
        int instanceCount = 0;
        for (i = 0; i < count; i++)
        {
            instanceCount += obj[i]->instanceCount;
        }
        int *textures = (int*)malloc(sizeof(int) * instanceCount);
        mat4 *transforms = (mat4*)malloc(sizeof(mat4) * instanceCount);
        int instance_offset = 0;
        for (i = 0; i < count; i++)
        {
            memcpy(textures + instance_offset, obj[i]->texture, obj[i]->instanceCount * sizeof(int));
            memcpy(transforms + instance_offset, obj[i]->transform, obj[i]->instanceCount * sizeof(mat4));
            instance_offset += obj[i]->instanceCount;
        }

        glBindBuffer(GL_ARRAY_BUFFER, rq->gpuHandle.textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(int), textures, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, rq->gpuHandle.transformBuffer);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(mat4), transforms, GL_DYNAMIC_DRAW);

        commandsNeedUpdate = 1;

        free(textures);
        free(transforms);
    }

    if (commandsNeedUpdate)
    {
        drawCommand *commands = (drawCommand*)malloc(sizeof(drawCommand) * count);
        int baseVert = 0;
        int fIndex = 0;
        int baseInst = 0;
        for (i = 0; i < count; i++)
        {
            commands[i].count = obj[i]->indexCount;
            commands[i].instanceCount = obj[i]->instanceCount;
            commands[i].firstIndex = fIndex;
            commands[i].baseVertex = baseVert;
            commands[i].baseInstance = baseInst;
            baseVert += obj[i]->dataCount;
            fIndex += obj[i]->indexCount;
            baseInst += obj[i]->instanceCount;
        }

        if (DO_INTEL_WORKAROUND)
        {
            glDeleteBuffers(1, &rq->gpuHandle.commandBuffer);
            glGenBuffers(1, &rq->gpuHandle.commandBuffer);    //hacky workaround for misbehaving intel gpus
        }

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, rq->gpuHandle.commandBuffer);
        if (rq->gpuHandle.count == count && !DO_INTEL_WORKAROUND)
        {
            glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(drawCommand) * count, commands);
        }
        else
        {
            glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommand) * count, commands, GL_DYNAMIC_DRAW);
        }
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        

        free(commands);

        rq->gpuHandle.count = count;
    }
}

void geo_obj_gpu_updateBuffers(GeoObject_gpu *gobj)
{
    if (gobj->gpuHandle.type == GOBJ_TYPE_UNDEFINED) return;

    if (gobj->geoObject.dataDirty)
    {
        gobj->geoObject.dataDirty = false;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gobj->gpuHandle.elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, gobj->geoObject.indexCount * sizeof(unsigned int), gobj->geoObject.indicies, GL_STATIC_DRAW);

        gobj->gpuHandle.indexCount = gobj->geoObject.indexCount;

        glBindBuffer(GL_ARRAY_BUFFER, gobj->gpuHandle.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, gobj->geoObject.dataCount * sizeof(vertex), gobj->geoObject.data, GL_STATIC_DRAW);
    }

    if (gobj->geoObject.instanceDirty)
    {
        gobj->geoObject.instanceDirty = false;
        glBindBuffer(GL_ARRAY_BUFFER, gobj->gpuHandle.textureBuffer);
        glBufferData(GL_ARRAY_BUFFER, gobj->geoObject.instanceCount * sizeof(int), gobj->geoObject.texture, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, gobj->gpuHandle.transformBuffer);
        glBufferData(GL_ARRAY_BUFFER, gobj->geoObject.instanceCount * sizeof(mat4), gobj->geoObject.transform, GL_STATIC_DRAW);
    }

}

RenderQueue *rq_new_queue(int capacity)
{
    RenderQueue *rq = (RenderQueue*)malloc(sizeof(RenderQueue));
    rq_init(rq, capacity);
    return rq;
}

void rq_init(RenderQueue *rq, int capacity)
{
    rq->capacity = capacity;
    GeoObject **buf;
    buf = (GeoObject**)malloc(sizeof(*buf) * capacity);
    rq->objectBuffer = buf;
    rq->count = 0;

    geo_obj_gpu_handle_genBuffers(&rq->gpuHandle, GOBJ_TYPE_MULTI);
}

void rq_init_c(RenderQueue *rq, int capacity)
{
    rq->capacity = capacity;
    GeoObject **buf;
    buf = (GeoObject**)malloc(sizeof(*buf) * capacity);
    rq->objectBuffer = buf;
    rq->count = 0;

    geo_obj_gpu_handle_genBuffers(&rq->gpuHandle, GOBJ_TYPE_COLOR_MULTI);
}

static void setup_standard_vertexattribs(void)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture coordinates
}

static void setup_color_vertexattribs(void)
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);                           // Position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));         // Normal
    glVertexAttribPointer(1, 2, GL_UNSIGNED_SHORT, GL_TRUE, 8 * sizeof(float), (void *)(6 * sizeof(float))); // Texture coordinates
    glVertexAttribPointer(4, 3, GL_UNSIGNED_BYTE, GL_TRUE, 8 * sizeof(float), (void *)(7 * sizeof(float)));  // Color
}

static void setup_standard_modelmatrix(int model_matrix_location)
{
    glEnableVertexAttribArray(model_matrix_location);
    glEnableVertexAttribArray(model_matrix_location + 1);
    glEnableVertexAttribArray(model_matrix_location + 2);
    glEnableVertexAttribArray(model_matrix_location + 3);
    glVertexAttribPointer(model_matrix_location, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)0);
    glVertexAttribPointer(model_matrix_location + 1, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(vec4)));
    glVertexAttribPointer(model_matrix_location + 2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(2 * sizeof(vec4)));
    glVertexAttribPointer(model_matrix_location + 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(3 * sizeof(vec4)));
    glVertexAttribDivisor(model_matrix_location, 1);
    glVertexAttribDivisor(model_matrix_location + 1, 1);
    glVertexAttribDivisor(model_matrix_location + 2, 1);
    glVertexAttribDivisor(model_matrix_location + 3, 1);
}

void geo_obj_gpu_handle_genBuffers(GeoObject_gpu_handle *gpuHandle, unsigned char type)
{
    
    glGenVertexArrays(1, &gpuHandle->vertexArray);
    glBindVertexArray(gpuHandle->vertexArray);

    glGenBuffers(1, &gpuHandle->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gpuHandle->vertexBuffer);

    switch (type)
    {
    case GOBJ_TYPE_COLOR_MULTI:
    case GOBJ_TYPE_COLOR:
        setup_color_vertexattribs();
        break;
    
    default:
        setup_standard_vertexattribs();
        break;
    }


    glGenBuffers(1, &gpuHandle->transformBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gpuHandle->transformBuffer);
    switch (type)
    {
    case GOBJ_TYPE_COLOR_MULTI:
    case GOBJ_TYPE_COLOR:
        setup_standard_modelmatrix(5);
        break;
    
    default:
        setup_standard_modelmatrix(4);
        break;
    }


    glGenBuffers(1, &gpuHandle->textureBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gpuHandle->textureBuffer);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, (void*)0);
    glVertexAttribDivisor(3, 1);

    glGenBuffers(1, &gpuHandle->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuHandle->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    switch (type)
    {
    case GOBJ_TYPE_MULTI:
    case GOBJ_TYPE_COLOR_MULTI:
        glGenBuffers(1, &gpuHandle->commandBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gpuHandle->commandBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        gpuHandle->type = type;
        break;
    
    default:
        gpuHandle->type = type;
        break;
    }
}

void geo_obj_gpu_handle_deleteBuffers(GeoObject_gpu_handle *gpuHandle)
{
    if (gpuHandle->type == GOBJ_TYPE_UNDEFINED) return;

    glDeleteVertexArrays(1, &gpuHandle->vertexArray);

    glDeleteBuffers(1, &gpuHandle->vertexBuffer);

    glDeleteBuffers(1, &gpuHandle->transformBuffer);

    glDeleteBuffers(1, &gpuHandle->textureBuffer);

    glDeleteBuffers(1, &gpuHandle->elementBuffer);

    unsigned char type = gpuHandle->type;

    switch (type)
    {
    case GOBJ_TYPE_MULTI:
    case GOBJ_TYPE_COLOR_MULTI:
        glDeleteBuffers(1, &gpuHandle->commandBuffer);
        break;    
    default:
        break;
    }

    gpuHandle->type = GOBJ_TYPE_UNDEFINED;
}

void rq_free(RenderQueue *r)
{
    free(r->objectBuffer);
    geo_obj_gpu_handle_deleteBuffers(&r->gpuHandle);
}

void rq_free_with_objects(RenderQueue *r)
{
    for (int i = 0; i < r->count; i++)
    {
        geo_obj_free(r->objectBuffer[i]);
    }
    free(r->objectBuffer);
    geo_obj_gpu_handle_deleteBuffers(&r->gpuHandle);
}

void rq_add_object(RenderQueue *rq, GeoObject *obj)
{
    if (rq->capacity <= rq->count)
    {
        printf("could not add object to renderqueue, at capacity\n");
        return;
    }

    obj->dataDirty = 1;
    obj->instanceDirty = 1;

    GeoObject **buf = rq->objectBuffer;

    buf[rq->count] = obj;
    rq->count++;
}


vertex gfx_make_vertex(float x, float y, float z, float uvx, float uvy)
{
    vertex out;

    out.vertex.x = x;
    out.vertex.y = y;
    out.vertex.z = z;
    out.uv.x = uvx;
    out.uv.y = uvy;

    out.normal.x = 0;
    out.normal.y = 0;
    out.normal.z = 0;

    return out;
}

vertex_c gfx_make_color_vertex(float x, float y, float z, short uvx, short uvy, color vertexColor)
{
    vertex_c out;

    out.vertex.x = x;
    out.vertex.y = y;
    out.vertex.z = z;
    out.uvx = uvx;
    out.uvy = uvy;

    out.normal.x = 0;
    out.normal.y = 0;
    out.normal.z = 0;

    out.r = vertexColor.r;
    out.g = vertexColor.g;
    out.b = vertexColor.b;

    return out;
}

color gfx_make_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    color out;

    out.r = r;
    out.g = g;
    out.b = b;
    out.a = a;

    return out;
}




