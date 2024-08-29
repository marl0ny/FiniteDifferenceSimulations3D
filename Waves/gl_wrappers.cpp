#include <OpenGL/OpenGL.h>
#include <sys/_types/_u_int32_t.h>
#define GL_SILENCE_DEPRECATION
// #define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_ES3

#include "gl_wrappers.hpp"

#include <stdio.h>
#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <iostream>
#include <fstream>

size_t s_frames_count = 0;


static size_t acquire_new_frame() {
    size_t frame_id = s_frames_count++;
    return frame_id;
}

static GLuint to_base(int sized) {
    switch(sized) {
    case GL_RGBA32F: case GL_RGBA32I: case GL_RGBA32UI: case GL_RGBA16F:
    case GL_RGBA16I: case GL_RGBA16UI:
    case GL_RGBA8I: case GL_RGBA8UI: case GL_RGBA8:
        return GL_RGBA;
    case GL_RGB32F: case GL_RGB32I: case GL_RGB32UI: case GL_RGB16F:
    case GL_RGB16I: case GL_RGB16UI: case GL_RGB8I: case GL_RGB8UI:
    case GL_RGB8:
        return GL_RGB;
    case GL_RG32F: case GL_RG32I: case GL_RG32UI: case GL_RG16F:
    case GL_RG16I: case GL_RG16UI: case GL_RG8I: case GL_RG8UI:
        return GL_RG;
    case GL_R32F: case GL_R32I: case GL_R32UI: case GL_R16F:
    case GL_R16I: case GL_R16UI: case GL_R8: case GL_R8UI:
        return GL_RED;
    }
    return -1;
}

static GLuint to_type(int sized) {
    switch(sized) {
    case GL_RGBA32F: case GL_RGB32F: case GL_RG32F: case GL_R32F:
        return GL_FLOAT;
    case GL_RGBA32I: case GL_RGB32I: case GL_RG32I: case GL_R32I:
        return GL_INT;
    case GL_RGBA32UI: case GL_RGB32UI: case GL_RG32UI: case GL_R32UI:
        return GL_UNSIGNED_INT;
    case GL_RGBA16F: case GL_RGB16F: case GL_RG16F: case GL_R16F:
        return GL_HALF_FLOAT;
    case GL_RGBA16I: case GL_RGB16I: case GL_RG16I: case GL_R16I:
        return GL_SHORT;
    case GL_RGBA16UI: case GL_RGB16UI: case GL_RG16UI: case GL_R16UI:
        return GL_UNSIGNED_SHORT;
    case GL_RGBA8: case GL_RGB8: case GL_RG8: case GL_R8:
        return GL_UNSIGNED_BYTE;
    case GL_RGBA8UI: case GL_RGB8UI: case GL_RG8UI: case GL_R8UI:
        return GL_UNSIGNED_BYTE;
    }
    return -1;
}

float &Vec2::operator[](size_t index) {
    return this->ind[index];
}

float Vec2::operator[](size_t index) const {
    return this->ind[index];
}

Vec2 Vec2::operator*(const Vec2 &other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] * other[i];
    return res;
}

Vec2 Vec2::operator+(const Vec2 &other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] + other[i];
    return res;
}

Vec2 Vec2::operator-(const Vec2 &other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] - other[i];
    return res;
}

Vec2 Vec2::operator/(const Vec2 &other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] / other[i];
    return res;
}

Vec2 Vec2::operator*(float other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] * other;
    return res;
}


Vec2 Vec2::operator+(float other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] + other;
    return res;
}

Vec2 Vec2::operator-(float other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] - other;
    return res;
}

Vec2 Vec2::operator/(float other) const {
    Vec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] / other;
    return res;
}

float Vec2::length_squared() const {
    float sum = 0.0;
    for (size_t i = 0; i < 2; i++) {
        sum += this->ind[i]*this->ind[i];
    }
    return sum;
}

float Vec2::length() const {
    return sqrt(this->length_squared());
}

Vec2 Vec2::normalized() const {
    return *this / this->length_squared();
}

Vec2 operator+(float r, const Vec2 &v) {
    return v + r;
}

Vec2 operator*(float r, const Vec2 &v) {
    return v * r;
}

Vec2 operator-(float r, const Vec2 &v) {
    Vec2 res;
    for (int i = 0; i < 2; i++)
        res[i] = r - v[i];
    return res;
}

Vec2 operator/(float r, const Vec2 &v) {
    Vec2 res;
    for (int i = 0; i < 2; i++)
        res[i] = r / v[i];
    return res;
}

float &Vec3::operator[](size_t index) {
    return this->ind[index];
}

float Vec3::operator[](size_t index) const {
    return this->ind[index];
}

Vec3 Vec3::operator*(const Vec3 &other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] * other[i];
    return res;
}

Vec3 Vec3::operator+(const Vec3 &other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] + other[i];
    return res;
}

Vec3 Vec3::operator-(const Vec3 &other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] - other[i];
    return res;
}

Vec3 Vec3::operator/(const Vec3 &other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] / other[i];
    return res;
}

Vec3 Vec3::operator*(float other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] * other;
    return res;
}


Vec3 Vec3::operator+(float other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] + other;
    return res;
}

Vec3 Vec3::operator-(float other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] - other;
    return res;
}

Vec3 Vec3::operator/(float other) const {
    Vec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] / other;
    return res;
}

float Vec3::length_squared() const {
    float sum = 0.0;
    for (size_t i = 0; i < 3; i++) {
        sum += this->ind[i]*this->ind[i];
    }
    return sum;
}

float Vec3::length() const {
    return sqrt(this->length_squared());
}

Vec3 Vec3::normalized() const {
    return *this / this->length_squared();
}

Vec3 operator+(float r, const Vec3 &v) {
    return v + r;
}

Vec3 operator*(float r, const Vec3 &v) {
    return v * r;
}

Vec3 operator-(float r, const Vec3 &v) {
    Vec3 res;
    for (int i = 0; i < 3; i++)
        res[i] = r - v[i];
    return res;
}

Vec3 operator/(float r, const Vec3 &v) {
    Vec3 res;
    for (int i = 0; i < 3; i++)
        res[i] = r / v[i];
    return res;
}

float &Vec4::operator[](size_t index) {
    return this->ind[index];
}

float Vec4::operator[](size_t index) const {
    return this->ind[index];
}

Vec4 Vec4::operator*(const Vec4 &other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] * other[i];
    return res;
}

Vec4 Vec4::operator+(const Vec4 &other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] + other[i];
    return res;
}

Vec4 Vec4::operator-(const Vec4 &other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] - other[i];
    return res;
}

Vec4 Vec4::operator/(const Vec4 &other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] / other[i];
    return res;
}

Vec4 Vec4::operator*(float other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] * other;
    return res;
}


Vec4 Vec4::operator+(float other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] + other;
    return res;
}

Vec4 Vec4::operator-(float other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] - other;
    return res;
}

Vec4 Vec4::operator/(float other) const {
    Vec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] / other;
    return res;
}

float Vec4::length_squared() const {
    float sum = 0.0;
    for (size_t i = 0; i < 4; i++) {
        sum += this->ind[i]*this->ind[i];
    }
    return sum;
}

float Vec4::length() const {
    return sqrt(this->length_squared());
}

Vec4 Vec4::normalized() const {
    return *this / this->length_squared();
}

Vec4 operator+(float r, const Vec4 &v) {
    return v + r;
}

Vec4 operator*(float r, const Vec4 &v) {
    return v * r;
}

Vec4 operator-(float r, const Vec4 &v) {
    Vec4 res;
    for (int i = 0; i < 4; i++)
        res[i] = r - v[i];
    return res;
}

Vec4 operator/(float r, const Vec4 &v) {
    Vec4 res;
    for (int i = 0; i < 4; i++)
        res[i] = r / v[i];
    return res;
}

int &IVec2::operator[](size_t index) {
    return this->ind[index];
}

int IVec2::operator[](size_t index) const {
    return this->ind[index];
}

IVec2 IVec2::operator*(const IVec2 &other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] * other[i];
    return res;
}

IVec2 IVec2::operator+(const IVec2 &other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] + other[i];
    return res;
}

IVec2 IVec2::operator-(const IVec2 &other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] - other[i];
    return res;
}

IVec2 IVec2::operator/(const IVec2 &other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] / other[i];
    return res;
}

IVec2 IVec2::operator*(int other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] * other;
    return res;
}


IVec2 IVec2::operator+(int other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] + other;
    return res;
}

IVec2 IVec2::operator-(int other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] - other;
    return res;
}

IVec2 IVec2::operator/(int other) const {
    IVec2 res;
    for (size_t i = 0; i < 2; i++)
        res[i] = this->ind[i] / other;
    return res;
}

int IVec2::length_squared() const {
    int sum = 0.0;
    for (size_t i = 0; i < 2; i++) {
        sum += this->ind[i]*this->ind[i];
    }
    return sum;
}

IVec2 operator+(int r, const IVec2 &v) {
    return v + r;
}

IVec2 operator*(int r, const IVec2 &v) {
    return v * r;
}

IVec2 operator-(int r, const IVec2 &v) {
    IVec2 res;
    for (int i = 0; i < 2; i++)
        res[i] = r - v[i];
    return res;
}

IVec2 operator/(int r, const IVec2 &v) {
    IVec2 res;
    for (int i = 0; i < 2; i++)
        res[i] = r / v[i];
    return res;
}

int &IVec3::operator[](size_t index) {
    return this->ind[index];
}

int IVec3::operator[](size_t index) const {
    return this->ind[index];
}

IVec3 IVec3::operator*(const IVec3 &other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] * other[i];
    return res;
}

IVec3 IVec3::operator+(const IVec3 &other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] + other[i];
    return res;
}

IVec3 IVec3::operator-(const IVec3 &other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] - other[i];
    return res;
}

IVec3 IVec3::operator/(const IVec3 &other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] / other[i];
    return res;
}

IVec3 IVec3::operator*(int other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] * other;
    return res;
}


IVec3 IVec3::operator+(int other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] + other;
    return res;
}

IVec3 IVec3::operator-(int other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] - other;
    return res;
}

IVec3 IVec3::operator/(int other) const {
    IVec3 res;
    for (size_t i = 0; i < 3; i++)
        res[i] = this->ind[i] / other;
    return res;
}

int IVec3::length_squared() const {
    int sum = 0.0;
    for (size_t i = 0; i < 3; i++) {
        sum += this->ind[i]*this->ind[i];
    }
    return sum;
}

IVec3 operator+(int r, const IVec3 &v) {
    return v + r;
}

IVec3 operator*(int r, const IVec3 &v) {
    return v * r;
}

IVec3 operator-(int r, const IVec3 &v) {
    IVec3 res;
    for (int i = 0; i < 3; i++)
        res[i] = r - v[i];
    return res;
}

IVec3 operator/(int r, const IVec3 &v) {
    IVec3 res;
    for (int i = 0; i < 3; i++)
        res[i] = r / v[i];
    return res;
}

int &IVec4::operator[](size_t index) {
    return this->ind[index];
}

int IVec4::operator[](size_t index) const {
    return this->ind[index];
}

IVec4 IVec4::operator*(const IVec4 &other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] * other[i];
    return res;
}

IVec4 IVec4::operator+(const IVec4 &other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] + other[i];
    return res;
}

IVec4 IVec4::operator-(const IVec4 &other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] - other[i];
    return res;
}

IVec4 IVec4::operator/(const IVec4 &other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] / other[i];
    return res;
}

IVec4 IVec4::operator*(int other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] * other;
    return res;
}


IVec4 IVec4::operator+(int other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] + other;
    return res;
}

IVec4 IVec4::operator-(int other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] - other;
    return res;
}

IVec4 IVec4::operator/(int other) const {
    IVec4 res;
    for (size_t i = 0; i < 4; i++)
        res[i] = this->ind[i] / other;
    return res;
}

int IVec4::length_squared() const {
    int sum = 0.0;
    for (size_t i = 0; i < 4; i++) {
        sum += this->ind[i]*this->ind[i];
    }
    return sum;
}

IVec4 operator+(int r, const IVec4 &v) {
    return v + r;
}

IVec4 operator*(int r, const IVec4 &v) {
    return v * r;
}

IVec4 operator-(int r, const IVec4 &v) {
    IVec4 res;
    for (int i = 0; i < 4; i++)
        res[i] = r - v[i];
    return res;
}

IVec4 operator/(int r, const IVec4 &v) {
    IVec4 res;
    for (int i = 0; i < 4; i++)
        res[i] = r / v[i];
    return res;
}

u_int32_t shader_from_source(std::string shader_source, u_int32_t shader_type) {
    GLuint shader_ref = glCreateShader(shader_type);
    // int minor_version, major_version;
    // glGetIntegerv(GL_MINOR_VERSION, &minor_version);
    // glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    if (shader_ref == 0) {
        fprintf(stderr, "unable to "
                "create vertex shader (error code %d).\n",
                glGetError());
        return 0;
    }
    #ifndef __EMSCRIPTEN__
    shader_source = "#version 330\n" + shader_source;   
    #else
    shader_source = "#version 300 es\n" + shader_source;
    #endif
    const char *shader_source_c_str = (const char *)shader_source.c_str();
    glShaderSource(shader_ref, 1, &shader_source_c_str, NULL);
    glCompileShader(shader_ref);
    GLint status;
    char buf[1024] = {'\0',};
    glGetShaderiv(shader_ref, GL_COMPILE_STATUS, &status);
    glGetShaderInfoLog(shader_ref, 1024, NULL, buf);
    if (status == GL_TRUE) {
        if (buf[0] != '\0') {
            fprintf(stdout, "%s", buf);
        }
        return shader_ref;
    } else {
        fprintf(stderr, "%s\n%s\n", "Shader compilation failed:", buf);
        return 0;
    }
}

static std::string get_file_contents(const std::string &fname) {
    std::string s = std::string ();
    std::filebuf fb;
    if (!fb.open(fname, std::ios::in)) {
        std::cout << "Opening " << fname << " failed." << std::endl;
        fb.close();
    }
    for (; fb.sgetc() > 0; fb.snextc())
        s.push_back(fb.sgetc());
    s.push_back('\0');
    fb.close();
    return s;
}

u_int32_t make_program_from_sources(
    std::string vertex_src, std::string fragment_src) {
    u_int32_t vs_ref = shader_from_source(vertex_src, GL_VERTEX_SHADER);
    u_int32_t fs_ref = shader_from_source(fragment_src, GL_FRAGMENT_SHADER);
    u_int32_t program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Unable to create program.\n");
    }
    glAttachShader(program, vs_ref);
    glAttachShader(program, fs_ref);
    glLinkProgram(program);
    GLint status;
    char buf[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    glGetProgramInfoLog(program, 1024, NULL, buf);
    if (status != GL_TRUE) {
        fprintf(stderr, "%s\n%s\n", "Failed to link program:", buf);
    }
    glUseProgram(program);
    return program;
}

u_int32_t make_program_from_paths(
    std::string vertex_path, std::string fragment_path) {
    std::string vertex_src = get_file_contents(vertex_path);
    std::string fragment_src = get_file_contents(fragment_path);
    return make_program_from_sources(vertex_src, fragment_src);
}

class RecycledRender {
    enum { RENDER_TARGET, QUAD };
    int render_type;
    size_t id;
    u_int32_t fbo;
    u_int32_t rbo;
    u_int32_t texture;
    TextureParams params;
};

static std::vector<RecycledRender> s_recycled_renders 
    = std::vector<RecycledRender>(10);

WireFrame::WireFrame(
    const Attributes &attributes,
    const std::vector<float> &vertices,
    const std::vector<int> &elements, 
    int draw_type) {
    this->attributes = attributes;
    this->vertices = vertices;
    this->elements = elements;
    this->draw_type = draw_type;
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float),
                 &vertices[0], GL_STATIC_DRAW);
    if (elements.size()) {
        glGenBuffers(1, &this->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(int),
                    &elements[0], GL_STATIC_DRAW);
    }
}

void WireFrame::draw(u_int32_t program) {
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    for (auto &name_attribute: this->attributes) {
        std::string name = name_attribute.first;
        Attribute attribute = name_attribute.second;
        GLint id = glGetAttribLocation(program, name.c_str());
        glEnableVertexAttribArray(id);
        glVertexAttribPointer(
            id, attribute.size, attribute.type,
            attribute.normalized, attribute.stride,
            (attribute.offset == 0)?
                NULL: (&this->vertices[0] + attribute.offset)
            );
    }
    switch(this->draw_type) {
        case WireFrame::LINES:
        if (this->elements.size() == 0) {
            glDrawArrays(GL_LINES, 0, this->vertices.size());
        } else {
            glDrawElements(
                GL_LINES, this->elements.size(), GL_UNSIGNED_INT, NULL);
        }
        break;
        case WireFrame::TRIANGLES:
        if (this->elements.size() == 0) {
            glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
        } else {
            glDrawElements(
                GL_TRIANGLES, this->elements.size(), GL_UNSIGNED_INT, NULL
            );
        }
    }
}

/* std::vector<float> get_vertices() {
    std::vector<float> vertices = std::vector<float> ();

}*/

static void unbind() {
    glBindVertexArray((GLuint)0);
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (GLuint)0);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLint)0);
    glBindRenderbuffer(GL_RENDERBUFFER, (GLuint)0);
}

void RenderTarget::init_texture() {
    if (this->id == 0)
        return;
    glActiveTexture(GL_TEXTURE0 + this->id);
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    TextureParams params = this->params;
    glTexImage2D(GL_TEXTURE_2D, 0, params.format,
                 params.width, params.height, 0,
                 to_base(params.format), to_type(params.format),
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    if (params.generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);
}

void RenderTarget::init_buffer() {
    if (this->id == 0)
        return;
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_2D, this->texture, 0);
    glGenRenderbuffers(1, &this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH_STENCIL, 
        this->params.width, this->params.height
    );
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, this->rbo);
}

void RenderTarget::adjust_viewport_before_drawing(const Config config) {
    if (config.usage == Config::VIEWPORT) {
        glViewport(config.x0, config.y0, config.width, config.height);
    } else {
        glViewport(0, 0, this->params.width, this->params.height);
    }
}

RenderTarget::RenderTarget(TextureParams const &params) {
    this->id = acquire_new_frame();
    this->params = params;
    this->init_texture();
    this->init_buffer();
    unbind();
}

int RenderTarget::get_id() const {
    return this->id;
}

IVec2 RenderTarget::texture_dimensions() const {
    return {.x=(int)this->params.width,
            .y=(int)this->params.height};
}

void RenderTarget::clear() {
    if (this->id == 0)
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLint)NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, (GLint)NULL);
}

void RenderTarget::draw(
    u_int32_t program,
    const Uniforms &uniforms, WireFrame &wire_frame,
    const Config config) {
    int original_viewport[4] = {0,};
    glGetIntegerv(GL_VIEWPORT, original_viewport);
    this->adjust_viewport_before_drawing(config);
    glUseProgram(program);
    if (this->id != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
        glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    }
    for (auto &uniform: uniforms) {
        GLint location = glGetUniformLocation(program, uniform.first.c_str());
        Uniform value = uniform.second;
        switch(value.type) {
            case Uniform::BOOL:
            break;
            case Uniform::FLOAT:
            glUniform1f(location, value.f32);
            break;
            case Uniform::FLOAT2:
            glUniform2f(location, value.vec2[0], value.vec2[1]);
            break;
            case Uniform::FLOAT3:
            glUniform3f(
                location, value.vec3[0], value.vec3[1], value.vec3[2]);
            break;
            case Uniform::FLOAT4:
            glUniform4f(
                location, 
                value.vec4[0], value.vec4[1], value.vec4[2], value.vec4[3]
            );
            break;
            case Uniform::INT:
            glUniform1i(location, value.i32);
            break;
            case Uniform::INT2:
            glUniform2i(location, value.ivec2[0], value.ivec2[1]);
            break;
            case Uniform::INT3:
            glUniform3i(
                location, value.ivec3[0], value.ivec3[1], value.ivec3[2]);
            break;
            case Uniform::INT4:
            glUniform4i(
                location, 
                value.ivec4[0], value.ivec4[1], value.ivec4[2], value.ivec4[3]
            );
            break;
            case Uniform::QUATERNION:
            glUniform4f(
                location, 
                value.quaternion.i, value.quaternion.j, value.quaternion.k, 
                value.quaternion.real);
            break;
            case Uniform::QUAD:
            glUniform1i(location, value.quad->get_id());
            break;
            case Uniform::RENDER_TARGET:
            glUniform1i(location, value.render_target->id);
            break;
            case Uniform::MULTIDIMENSIONAL_DATA_QUAD:
            glUniform1i(location, value.multidimensional_data_quad->get_id());
        }
    }
    wire_frame.draw(program);
    glViewport(original_viewport[0], original_viewport[1],
               original_viewport[2], original_viewport[3]);
}

struct {
    bool is_initialized;
    u_int32_t vao, vbo, ebo, fbo;
} s_quad_objects = {.is_initialized=false};


static const float QUAD_VERTICES[12] = {
    -1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -1.0, 0.0
};
static const int QUAD_ELEMENTS[6] = {
    0, 1, 2, 0, 2, 3
};

static void init_s_quad_objects() {
    if (!s_quad_objects.is_initialized) {
        glGenVertexArrays(1, &s_quad_objects.vao);
        glBindVertexArray(s_quad_objects.vao);
        glGenBuffers(1, &s_quad_objects.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, s_quad_objects.vbo);
        glBufferData(GL_ARRAY_BUFFER, 
            sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);
        glGenBuffers(1, &s_quad_objects.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_quad_objects.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_ELEMENTS), 
                     QUAD_ELEMENTS, GL_STATIC_DRAW);
        s_quad_objects.is_initialized = true;
    }

}

void Quad::init_texture() {
    if (this->id == 0)
        return;
    glActiveTexture(GL_TEXTURE0 + this->id);
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    TextureParams params = this->params;
    glTexImage2D(GL_TEXTURE_2D, 0, 
                 params.format, params.width, params.height, 0, 
                 to_base(params.format), to_type(params.format), NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
    if (params.generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

}

void Quad::init_buffer() {
    init_s_quad_objects();
    if (this->id != 0) {
        glGenFramebuffers(1, &this->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_2D, this->texture, 0);
    }
}

// TODO!
void Quad::bind(u_int32_t program) {
    glUseProgram(program);
    glBindVertexArray(s_quad_objects.vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_quad_objects.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_quad_objects.ebo);
    // if (s_quad_objects.vao == 0) {
    //     glBufferData(GL_ARRAY_BUFFER, 
    //         sizeof(QUAD_VERTICES), QUAD_VERTICES, GL_STATIC_DRAW);
    //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QUAD_ELEMENTS), 
    //                  QUAD_ELEMENTS, GL_STATIC_DRAW);
    // }
    if (this->id != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
        // glClear(GL_COLOR_BUFFER_BIT);
    }
    u_int32_t attrib = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 12, NULL);

}

void Quad::adjust_viewport_before_drawing(const Config config) {
    if (config.usage == Config::VIEWPORT) {
        glViewport(config.x0, config.y0, config.width, config.height);
    } else {
        glViewport(0, 0, this->width(), this->height());
    }
}

void Quad::init(const TextureParams &params) {
    this->id = acquire_new_frame();
    this->params = params;
    this->init_texture();
    this->init_buffer();
    unbind();
}

Quad::Quad(const TextureParams &params) {
    this->init(params);
}

Quad::Quad(Quad &&r_val) {
    this->id = r_val.id;
    this->params = r_val.params;
    this->texture = r_val.texture;
    this->fbo = r_val.fbo;
}

Quad::Quad(const Quad &q) {
    this->init(q.params);
    // TODO: need to copy texture value...
}

Quad& Quad::operator=(const Quad &q) {
    this->init(q.params);
    // TODO: need to copy texture value...
    return *this;
}


Quad& Quad::operator=(const Quad && r_val) {
    // TODO: need to copy texture value...
    this->id = r_val.id;
    this->params = r_val.params;
    this->texture = r_val.texture;
    this->fbo = r_val.fbo;
    return *this;
}

Quad::~Quad() {
    glDeleteTextures(1, &this->texture);
    glDeleteFramebuffers(1, &this->fbo);
    // TODO
}



const char QUAD_VERTEX_SHADER[] = 
R"(#if __VERSION__ <= 120
attribute vec3 position;
varying vec2 UV;
#else
in vec3 position;
out highp vec2 UV;
#endif
void main() {
    gl_Position = vec4(position.xyz, 1.0);
    UV = position.xy/2.0 + vec2(0.5, 0.5);
}
)";

u_int32_t Quad::make_program_from_path(std::string fragment_path) {
    std::string fragment_source = get_file_contents(fragment_path);
    return make_program_from_source(fragment_source);
}

u_int32_t Quad::make_program_from_source(std::string fragment_source) {
    u_int32_t vs_ref = shader_from_source(
        std::string(QUAD_VERTEX_SHADER), GL_VERTEX_SHADER);
    u_int32_t fs_ref = shader_from_source(
        fragment_source, GL_FRAGMENT_SHADER);
    u_int32_t program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "Unable to create program.\n");
    }
    glAttachShader(program, vs_ref);
    glAttachShader(program, fs_ref);
    glLinkProgram(program);
    GLint status;
    char buf[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    glGetProgramInfoLog(program, 1024, NULL, buf);
    if (status != GL_TRUE) {
        fprintf(stderr, "%s\n%s\n", "Failed to link program:", buf);
    }
    glUseProgram(program);
    return program;
}





int Quad::get_id() const {
    return this->id;
}

void Quad::clear() {
    if (this->id != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
        // glBindFramebuffer(GL_RENDERBUFFER, this->rbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, (unsigned int)0);
        // glBindRenderbuffer(GL_RENDERBUFFER, (unsigned int)0);
    }
}

void Quad::reset(const TextureParams &new_tex_params) {
    if (this->id != 0) {
        glDeleteTextures(1, &this->texture);
        glDeleteFramebuffers(1, &this->fbo);
        this->params = new_tex_params;
        glActiveTexture(GL_TEXTURE0 + this->id);
        glGenTextures(1, &this->texture);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, 
                 params.format, params.width, params.height, 0, 
                 to_base(params.format), to_type(params.format), NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter);
        if (params.generate_mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
    }
}

u_int32_t Quad::width() {
    return this->params.width;
}

u_int32_t Quad::height() {
    return this->params.height;
}

u_int32_t Quad::format() {
    return this->params.format;
}

void Quad::draw(u_int32_t program, const Uniforms &uniforms, const Config config) {
    int old_viewport[4] = {0,};
    glGetIntegerv(GL_VIEWPORT, old_viewport);
    this->adjust_viewport_before_drawing(config);
    this->bind(program);
    for (auto &uniform: uniforms) {
        GLint location = glGetUniformLocation(program, uniform.first.c_str());
        Uniform value = uniform.second;
        switch(value.type) {
            case Uniform::BOOL:
            break;
            case Uniform::FLOAT:
            glUniform1f(location, value.f32);
            break;
            case Uniform::FLOAT2:
            glUniform2f(location, value.vec2[0], value.vec2[1]);
            break;
            case Uniform::FLOAT3:
            glUniform3f(
                location, value.vec3[0], value.vec3[1], value.vec3[2]);
            break;
            case Uniform::FLOAT4:
            glUniform4f(
                location, 
                value.vec4[0], value.vec4[1], value.vec4[2], value.vec4[3]
            );
            break;
            case Uniform::INT:
            glUniform1i(location, value.i32);
            break;
            case Uniform::INT2:
            glUniform2i(location, value.ivec2[0], value.ivec2[1]);
            break;
            case Uniform::INT3:
            glUniform3i(
                location, value.ivec3[0], value.ivec3[1], value.ivec3[2]);
            break;
            case Uniform::INT4:
            glUniform4i(
                location, 
                value.ivec4[0], value.ivec4[1], value.ivec4[2], value.ivec4[3]
            );
            break;
            case Uniform::QUAD:
            glUniform1i(location, value.quad->id);
            break;
            case Uniform::RENDER_TARGET:
            glUniform1i(location, value.render_target->get_id());
            break;
            case Uniform::MULTIDIMENSIONAL_DATA_QUAD:
            glUniform1i(
                location, value.multidimensional_data_quad->get_id());
            
        }
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    glViewport(old_viewport[0], old_viewport[1],
               old_viewport[2], old_viewport[3]);
    unbind();
}

static IVec2 compute_texture_dimensions(std::vector<int> data_dimensions) {
    IVec2 texture_dimensions;
    if (data_dimensions.size() == 1) {
        texture_dimensions[0] = data_dimensions[0];
        texture_dimensions[1] = 1;
    } else if (data_dimensions.size() == 2) {
        texture_dimensions[0] = data_dimensions[0];
        texture_dimensions[1] = data_dimensions[1];
    } else if (data_dimensions.size() == 3) {
        IVec3 dimensions3d = {
            .x=data_dimensions[0],
            .y=data_dimensions[1],
            .z=data_dimensions[2]};
        texture_dimensions = get_2d_from_3d_dimensions(dimensions3d);
    } else {
        // TODO
    }
    return texture_dimensions;
}

MultidimensionalDataQuad::MultidimensionalDataQuad(
    const std::vector<int> &data_dimensions, const TextureParams &params
    ) : quad(Quad()) 
    {
    this->data_dimensions = data_dimensions;
    this->texture_dimensions = compute_texture_dimensions(data_dimensions);
    TextureParams new_params = params;
    new_params.width = this->texture_dimensions[0];
    new_params.height = this->texture_dimensions[1];
    this->quad.init(new_params);
}

int MultidimensionalDataQuad::get_id() const {
    return this->quad.get_id();
}

void MultidimensionalDataQuad::clear() {
    this->quad.clear();
}

void MultidimensionalDataQuad::reset(
    const std::vector<int> &data_dimensions, const TextureParams &params
) {
    this->data_dimensions = data_dimensions;
    this->texture_dimensions = compute_texture_dimensions(data_dimensions);
    TextureParams new_params = params;
    new_params.width = this->texture_dimensions[0];
    new_params.height = this->texture_dimensions[1];
    this->quad.reset(new_params);
}

IVec2 MultidimensionalDataQuad::get_texture_dimensions() const {
    return this->texture_dimensions;
}

IVec3 MultidimensionalDataQuad::get_3d_dimensions() const {
    if (this->data_dimensions.size() != 3) 
        fprintf(
            stderr, "Warning: the number of dimensions do not equal 3.");
    return {
        .x=this->data_dimensions[0],
        .y=this->data_dimensions[1],
        .z=this->data_dimensions[2]
    };
}

u_int32_t MultidimensionalDataQuad::format() const {
    return this->quad.params.format;
}

void MultidimensionalDataQuad::draw(
    u_int32_t program, const Uniforms &uniforms) {
    this->quad.draw(program, uniforms);
}

u_int32_t MultidimensionalDataQuad::make_program_from_path(std::string path) {
    return Quad::make_program_from_path(path);
}

u_int32_t MultidimensionalDataQuad::make_program_from_source(
    std::string source) {
    return Quad::make_program_from_path(source);
}


/*
static double floor(double a) {
    return  (double)((int)(a));
}*/

static double mod(double a, double b) {
    return  b*(a/b - floor(a/b));
}

/* If n is a perfect square, return its square root,
else return those values closest to making it a square.
Although this is implemented using brute force iteration,
for the problem that this function is meant to solve
the value of n shouldn't be too large (n < 1000).
*/
static struct IVec2 decompose(unsigned int n) {
    struct IVec2 d = {.ind={(int)n, 1}};
    int i = 1;
    for (; i*i < n; i++) {}
    for (; n % i; i--) {}
    d.ind[0] = ((n / i) > i)? (n / i): i;
    d.ind[1] = ((n / i) < i)? (n / i): i;
    return d;
}

/* 3D arrays of data are represented as 2D textures.
 This function returns the dimensions of the 2D texture given the dimensions
 of its 3D array representation. If width_3d, height_3d, and length_3d
 are the dimensions of the 3D data array, then the dimensions of the 2D
 texture are (d1 * width_2d) x (d2 * height_3d), where d1 * d2 = length_3d.
 Since OpenGL places restrictions on the maximum texture side length,
 the length of each of the two side lengths are kept as small as possible.
 This is done by minimizing the value of d1 + d2
 (related to the size of the perimeter of the texture).*/
struct IVec2 get_2d_from_3d_dimensions(const struct IVec3 &dimensions_3d) {
    int width = dimensions_3d[0];
    int height = dimensions_3d[1];
    int length = dimensions_3d[2];
    int max_tex_size = 0xfffffff;
    // #ifndef __APPLE__
    // glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
    // #endif
    struct IVec2 tex_dimensions_2d = {};
    // if (length*width < max_tex_size) {
    //     tex_dimensions_2d.width = length*width;
    //     tex_dimensions_2d.height = height;
    // } else if (length*height < max_tex_size) {
    //     tex_dimensions_2d.width = width;
    //     tex_dimensions_2d.height = length*height;
    // } else {
        struct IVec2 d = decompose(length);
        if (d.ind[0]*width < max_tex_size &&
            d.ind[1]*height < max_tex_size) {
            tex_dimensions_2d[0] = width*d.ind[0];
            tex_dimensions_2d[1] = height*d.ind[1];
        } else if (d.ind[1]*width < max_tex_size &&
                    d.ind[0]*height < max_tex_size) {
            tex_dimensions_2d[0] = width*d.ind[1];
            tex_dimensions_2d[1] = height*d.ind[0];
        } else {
            fprintf(stderr,
                    "Warning: 3D texture dimensions %d, %d, %d "
                    "with possible 2D representations %d, %d "
                    " or %d, %d exceed maximum "
                    "texture size. The maximum 2D texture side length "
                    "is %d.",
                    width, height, length,
                    width*d.ind[0], height*d.ind[1],
                    width*d.ind[1], height*d.ind[0],
                    max_tex_size);
            tex_dimensions_2d[0] = width*d.ind[1];
            tex_dimensions_2d[1] = height*d.ind[0];
        }
    // }
    return tex_dimensions_2d;
}

struct IVec2 get_2d_from_width_height_length(
    int width, int height, int length) {
    struct IVec3 dimensions_3d
         = {.x=width, .y=height, .z=length};
    return get_2d_from_3d_dimensions(dimensions_3d);
}

// void use_3d_texture(int width, int height, int length) {
//     struct IVec3 dimensions_3d = {
//         .x=width, .y=height, .z=length
//     };
//     struct IVec2 tex_dimensions_2d
//         = get_2d_from_3d_dimensions(dimensions_3d);
//     glViewport(0, 0, tex_dimensions_2d.x, tex_dimensions_2d.y);
// }

struct Vec2 get_2d_from_3d_texture_coordinates(
    struct Vec3 &uvw,
    struct IVec2 &tex_dimensions_2d,
    struct IVec3 &dimensions_3d) {
    int width_2d = tex_dimensions_2d[0];
    int height_2d = tex_dimensions_2d[1];
    int width_3d = dimensions_3d[0];
    int height_3d = dimensions_3d[1];
    int length_3d = dimensions_3d[2];
    double w_stack = (double)width_2d/(double)width_3d;
    // double h_stack = (double)height_2d/(double)height_3d;
    double x_index = (double)width_3d*mod(uvw[0], 1.0);
    double y_index = (double)height_3d*mod(uvw[1], 1.0);
    double z_index = mod(floor((double)length_3d*uvw[2]),
                         (double)length_3d);
    // fprintf(stdout, "%g, %g\n", z_index, mod(z_index, w_stack)*(double)width_3d);
    double u_index = (mod(z_index, w_stack)*((double)width_3d)) + x_index;
    // fprintf(stdout, "x_index: %g, u_index: %g\n", x_index, u_index);
    double v_index = (floor(z_index / w_stack)*((double)height_3d)) + y_index;
    struct Vec2 uv = {.u=(float)(u_index/(double)width_2d),
                      .v=(float)(v_index/(double)height_2d)};
    return uv;

}

struct Vec3 get_3d_from_2d_texture_coordinates(
    struct Vec2 &uv,
    struct IVec2 &tex_dimensions_2d,
    struct IVec3 &dimensions_3d) {
    int width_2d = tex_dimensions_2d[0];
    int height_2d = tex_dimensions_2d[1];
    int width_3d = dimensions_3d[0];
    int height_3d = dimensions_3d[1];
    int length_3d = dimensions_3d[2];
    double w_stack = (double)width_2d/(double)width_3d;
    double h_stack = (double)height_2d/(double)height_3d;
    double u = mod(uv[0]*w_stack, 1.0);
    double v = mod(uv[1]*h_stack, 1.0);
    double w = (floor(uv[1]*h_stack)*w_stack
                + floor(uv[0]*w_stack) + 0.5)/(double)length_3d;
    struct Vec3 uvw = {.x=(float)u, .y=(float)v, .z=(float)w};
    return uvw;
}
