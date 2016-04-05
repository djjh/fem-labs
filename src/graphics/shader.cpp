#include "shader.h"

#define STRINGIFY(A) #A

typedef VertexColorShaderProgram::Vertex Vertex;
typedef VertexColorShaderProgram::GpuAsset GpuAsset;

void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error 0x%08x, at %s:%i - for %s\n", err, fname, line, stmt);
        exit(1);
    }
}

#define GL_CHECK(stmt) \
    { \
        stmt; \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    }

static char const * vertex_shader_text =
    "#version 330\n" STRINGIFY(
    uniform mat4 MVP;
    in vec4 vertex_position;
    in vec4 vertex_color;
    out vec4 color;
    void main()
    {
        gl_Position = MVP * vertex_position;
        color = vertex_color;
    });

static char const * fragment_shader_text = 
    "#version 330\n" STRINGIFY(
    in vec4 color;
    out vec4 fragment_color;
    void main()
    {
        fragment_color = color;
    });

void VertexColorShaderProgram::initialize()
{
    
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LESS));
    // GL_CHECK(glDepthFunc(GL_LESS));
    // GL_CHECK(glDisable(GL_CULL_FACE));
    // GL_CHECK(glEnable(GL_BLEND));
    // GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    compile();
    link();
    initialize_buffers();
    initialize_mvp();
}

void VertexColorShaderProgram::set_m(mat4x4 m)
{
    mat4x4_dup(_m, m);
    update_mvp();
}

void VertexColorShaderProgram::set_v(mat4x4 v)
{
    mat4x4_dup(_v, v);
    update_mvp();
}

void VertexColorShaderProgram::set_p(mat4x4 p)
{
    mat4x4_dup(_p, p);
    update_mvp();
}

void VertexColorShaderProgram::draw_vertices(std::vector<Vertex> const & vv, GLint mode)
{
    GL_CHECK(glUseProgram(program));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CHECK(glBindVertexArray(vao));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vv.size(), vv.data(), GL_DYNAMIC_DRAW));
    GL_CHECK(glDrawArrays(mode, 0, vv.size()));
    GL_CHECK(glBindVertexArray(0));
}

void VertexColorShaderProgram::draw_triangles(std::vector<Vertex> const & vv)
{
    draw_vertices(vv, GL_TRIANGLES);
}

void VertexColorShaderProgram::draw_lines(std::vector<Vertex> const & vv)
{
    draw_vertices(vv, GL_LINES);
}

void VertexColorShaderProgram::draw_line_strip(std::vector<Vertex> const & vv)
{
    draw_vertices(vv, GL_LINE_STRIP);
}

GpuAsset * VertexColorShaderProgram::gpu_create_asset(std::vector<Vertex> const & vv)
{

        //
    // GLint rb = 0;
    // GLint gb = 0;
    // GLint bb = 0;
    // GL_CHECK(glGetIntegerv(GL_RED_BITS, &rb));
    // GL_CHECK(glGetIntegerv(GL_GREEN_BITS, &gb));
    // GL_CHECK(glGetIntegerv(GL_BLUE_BITS, &bb));
    // fprintf(stderr, "Possible colors: 2^%d\n", (rb+gb+bb));


    GpuAsset * res = new GpuAsset;
    assert(res);

    GL_CHECK(glUseProgram(program));
    res->program = program;

    GL_CHECK(glGenVertexArrays(1, &res->vao));
    GL_CHECK(glBindVertexArray(res->vao));

    GL_CHECK(glGenBuffers(1, &res->vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, res->vbo));
    res->vbi = 0;

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vv.size(), vv.data(), GL_STATIC_DRAW));
    res->size = vv.size();

    GL_CHECK(glEnableVertexAttribArray(vpos_location));
    GL_CHECK(glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*) 0));
    GL_CHECK(glEnableVertexAttribArray(vcol_location));
    GL_CHECK(glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*) (sizeof(float) * 4)));

    GL_CHECK(glBindVertexArray(0));

    return res;
}

void VertexColorShaderProgram::gpu_update_asset(GpuAsset * ga, std::vector<Vertex> const & vv)
{
    assert(ga);

    GL_CHECK(glUseProgram(ga->program));

    GL_CHECK(glBindVertexArray(ga->vao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, ga->vbo));
    ga->vbi = 0;
    
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vv.size(), vv.data(), GL_DYNAMIC_DRAW));
    ga->size = vv.size();

    GL_CHECK(glBindVertexArray(0));
}

GpuAsset * VertexColorShaderProgram::gpu_create_asset(std::vector<Vertex> const & vv, std::vector<unsigned> const & vi)
{
    GpuAsset * res = new GpuAsset;
    assert(res);

    GL_CHECK(glUseProgram(program));
    res->program = program;

    GL_CHECK(glGenVertexArrays(1, &res->vao));
    GL_CHECK(glBindVertexArray(res->vao));

    GL_CHECK(glGenBuffers(1, &res->vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, res->vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vv.size(), vv.data(), GL_STATIC_DRAW));
    res->size = vv.size();
    GL_CHECK(glEnableVertexAttribArray(vpos_location));
    GL_CHECK(glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*) 0));
    GL_CHECK(glEnableVertexAttribArray(vcol_location));
    GL_CHECK(glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*) (sizeof(float) * 4)));

    GL_CHECK(glGenBuffers(1, &res->vbi));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res->vbi));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * vi.size(), vi.data(), GL_STATIC_DRAW));
    res->count = vi.size();

    GL_CHECK(glBindVertexArray(0));

    return res;
}

void VertexColorShaderProgram::gpu_update_asset(GpuAsset * ga, std::vector<Vertex> const & vv, std::vector<unsigned> const & vi)
{
    assert(ga);

    GL_CHECK(glUseProgram(ga->program));

    GL_CHECK(glBindVertexArray(ga->vao));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, ga->vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vv.size(), vv.data(), GL_DYNAMIC_DRAW));
    ga->size = vv.size();

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ga->vbi));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * vi.size(), vi.data(), GL_STATIC_DRAW));
    ga->count = vi.size();

    GL_CHECK(glBindVertexArray(0));
}

void VertexColorShaderProgram::gpu_draw_vertices(GpuAsset const * ga, GLint mode)
{
    assert(ga);
    assert(ga->program == program);

    if (!ga->vbi)
    {
        GL_CHECK(glUseProgram(program));
        GL_CHECK(glBindVertexArray(ga->vao));
        GL_CHECK(glDrawArrays(mode, 0, ga->size));
        GL_CHECK(glBindVertexArray(0));
    }
    else
    {
        GL_CHECK(glUseProgram(program));
        GL_CHECK(glBindVertexArray(ga->vao));
        GL_CHECK(glDrawElements(mode, ga->count, GL_UNSIGNED_INT, (void *) 0));
        GL_CHECK(glBindVertexArray(0));
    }
}

void VertexColorShaderProgram::gpu_draw_triangles(GpuAsset const * ga)
{
    gpu_draw_vertices(ga, GL_TRIANGLES);
}

void VertexColorShaderProgram::gpu_draw_lines(GpuAsset const * ga)
{
    gpu_draw_vertices(ga, GL_LINES);
}

void VertexColorShaderProgram::gpu_draw_line_strip(GpuAsset const * ga)
{
    gpu_draw_vertices(ga, GL_LINE_STRIP);
}

void VertexColorShaderProgram::gpu_draw_points(GpuAsset const * ga)
{
    gpu_draw_vertices(ga, GL_POINTS);
}

void VertexColorShaderProgram::compile()
{
    GL_CHECK(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
    GL_CHECK(glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL));
    GL_CHECK(glCompileShader(vertex_shader));
    GLint isCompiled = 0;
    GL_CHECK(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled));
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        GL_CHECK(glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength));

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        GL_CHECK(glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &errorLog[0]));
        std::string str(errorLog.begin(),errorLog.end());
        std::cout << str << std::endl;

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        GL_CHECK(glDeleteShader(vertex_shader)); // Don't leak the shader.
        exit(1);
    }

    GL_CHECK(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
    GL_CHECK(glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL));
    GL_CHECK(glCompileShader(fragment_shader));
    isCompiled = 0;
    GL_CHECK(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled));
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        GL_CHECK(glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength));

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        GL_CHECK(glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &errorLog[0]));
        std::string str(errorLog.begin(),errorLog.end());
        std::cout << str << std::endl;

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        GL_CHECK(glDeleteShader(fragment_shader)); // Don't leak the shader.
        exit(1);
    }
}

void VertexColorShaderProgram::link()
{
    GL_CHECK(program = glCreateProgram());
    GL_CHECK(glAttachShader(program, vertex_shader));
    GL_CHECK(glAttachShader(program, fragment_shader));
    GL_CHECK(glLinkProgram(program));
    GLint isLinked = 0;
    GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &isLinked));
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        GL_CHECK(glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]));
        std::string str(errorLog.begin(),errorLog.end());
        std::cout << str << std::endl;

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        GL_CHECK(glDeleteProgram(program)); // Don't leak the program.
        exit(1);
    }
}

void VertexColorShaderProgram::initialize_buffers()
{
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(mvp_location = glGetUniformLocation(program, "MVP"));
    GL_CHECK(vpos_location = glGetAttribLocation(program, "vertex_position"));
    GL_CHECK(vcol_location = glGetAttribLocation(program, "vertex_color"));

    GL_CHECK(glEnableVertexAttribArray(vpos_location));
    GL_CHECK(glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 8, (void*) 0));
    GL_CHECK(glEnableVertexAttribArray(vcol_location));
    GL_CHECK(glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 8, (void*) (sizeof(float) * 4)));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindVertexArray(0));
}

void VertexColorShaderProgram::initialize_mvp()
{
    mat4x4_identity(_m);
    mat4x4_identity(_v);
    mat4x4_identity(_p);
}

void VertexColorShaderProgram::update_mvp()
{
    mat4x4 mvp;
    mat4x4_identity(mvp);
    mat4x4_mul(mvp, _v, _m);
    mat4x4_mul(mvp, _p, mvp);
    GL_CHECK(glUseProgram(program));
    GL_CHECK(glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (GLfloat const *) mvp));
}

