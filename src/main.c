#include "../include/vector.h"
#include "../include/texture-atlas.h"
#include "../include/texture-font.h"
#include <stdio.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

int
cmp(const void *a, const void *b)
{
        const char *a_ = (const char *) a;
        const char *b_ = (const char *) b;
        return strcmp(a_, b_);
}

static SDL_Window *window;
static SDL_GLContext *context;
static int running;
static SDL_Event event;

static void
sdlopengl_init(const char *name, int x, int y, int w, int h,
               int flags)
{
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow(name, x, y, w, h, flags);
        context = SDL_GL_CreateContext(window);
        glewInit();

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(1.0, 1.0, 1.0, 1.0);

        glViewport(0, 0, w, h);
        ll_matrix_mode(LL_MATRIX_MODEL);
        ll_matrix_identity();
        ll_matrix_mode(LL_MATRIX_VIEW);
        ll_matrix_identity();
        ll_matrix_mode(LL_MATRIX_PROJECTION);
        ll_matrix_orthographic(0, w, h, 0, 10.0, -10.0);

        running = 1;
}

static void
sdlopengl_free(void)
{
        SDL_DestroyWindow(window);
        SDL_GL_DeleteContext(context);
        SDL_Quit();
}

#define BUFFER_CAPACITY 50

char buffer[BUFFER_CAPACITY];
size_t buffer_size;

#define WINDOW_W 1600
#define WINDOW_H 1000

int main(int argc, char **argv)
{
        sdlopengl_init(argv[0],
                       SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED,
                       WINDOW_W,
                       WINDOW_H,
                       SDL_WINDOW_OPENGL);

        const char *src = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 !@#$%^&*()_+-=./;':\",{}[]";

        ftgl_texture_atlas_t *atlas = ftgl_texture_atlas_create(512, 512, 1);
        ftgl_texture_font_t *font = ftgl_texture_font_new_from_file(atlas, 48, "NotoSansKR-Regular.otf");
        if (!font) {
                fprintf(stderr, "%s\n", ftgl_log_pop_message());
                exit(EXIT_FAILURE);
        }

        /* load all characters from the src */
        for (int i = 0; i < strlen(src); i++ ) {
                if (ftgl_texture_font_load_glyph(font, src+i) != FTGL_NO_ERROR) {
                        fprintf(stderr, "Failed to load '%c'\n", src[i]);
                }
        }

        const char *vsource = {
                "#version 330 core\n"
                "uniform mat4 model;"
                "uniform mat4 view;"
                "uniform mat4 projection;"
                "layout (location = 0) in vec2 vertex;"
                "layout (location = 1) in vec2 tex_vertex;"
                "out vec2 t_vertex;"
                "void main()"
                "{"
                "t_vertex = tex_vertex;"
                "gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);"
                "}"
        };
        
        const char *fsource = {
                "#version 330 core\n"
                "uniform sampler2D texture_unit;"
                "uniform vec4 colour;"
                "in vec2 t_vertex;"
                "void main()"
                "{"
                "float a = texture2D(texture_unit, t_vertex).r;"
                "gl_FragColor = vec4(colour.rgb, colour.a * a);"
                "}"
        };
        
        GLfloat vertices[] = {
                0.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
        };
        
        const GLuint indices[] = {
                0, 1, 2,
                2, 3, 0
        };
        
        GLuint program, vshader, fshader, vao, vbo, ebo;
        program = glCreateProgram();
        
        vshader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vshader, 1, (const GLchar **) &vsource, NULL);
        glCompileShader(vshader);
        
        fshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fshader, 1, (const GLchar **) &fsource, NULL);
        glCompileShader(fshader);
        
        glAttachShader(program, vshader);
        glAttachShader(program, fshader);
        glLinkProgram(program);
        glDeleteShader(vshader);
        glDeleteShader(fshader);
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                     vertices, GL_DYNAMIC_DRAW);
        
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
                     indices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(*vertices),
                              (const GLvoid *) 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(*vertices),
                              (const GLvoid *) (2 * sizeof(*vertices)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        
        unsigned char data[25*25];
        for (int i = 0; i < 25*25; i++)
                data[i] = i & 2;
        
        glGenTextures(1, &atlas->id);
        glBindTexture(GL_TEXTURE_2D, atlas->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas->width, atlas->height,
                     0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);

        int shift = 0;
        
        while (running) {
                while (SDL_PollEvent(&event)) {
                        switch (event.type) {
                        case SDL_QUIT:
                                running = 0;
                                break;
                        case SDL_KEYUP:
                        case SDLK_LSHIFT:
                                shift = 0;
                                break;
                        case SDL_KEYDOWN:
                                switch (event.key.keysym.sym) {
                                case SDLK_LSHIFT:
                                        shift = 1;
                                        break;
                                case SDLK_a:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'A' : 'a';
                                        break;
                                case SDLK_b:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'B' : 'b';
                                        break;
                                case SDLK_c:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'C' : 'c';
                                        break;
                                case SDLK_d:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'D' : 'd';
                                        break;
                                case SDLK_e:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'E' : 'e';
                                        break;
                                case SDLK_f:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'F' : 'f';
                                        break;
                                case SDLK_g:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'G' : 'g';
                                        break;
                                case SDLK_h:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'H' : 'h';
                                        break;
                                case SDLK_i:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'I' : 'i';
                                        break;
                                case SDLK_j:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'J' : 'j';
                                        break;
                                case SDLK_k:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'K' : 'k';
                                        break;
                                case SDLK_l:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'L' : 'l';
                                        break;
                                case SDLK_m:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'M' : 'm';
                                        break;
                                case SDLK_n:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'N' : 'n';
                                        break;
                                case SDLK_o:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'O' : 'o';
                                        break;
                                case SDLK_p:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'P' : 'p';
                                        break;
                                case SDLK_q:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'Q' : 'q';
                                        break;
                                case SDLK_r:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'R' : 'r';
                                        break;
                                case SDLK_s:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'S' : 's';
                                        break;
                                case SDLK_t:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'T' : 't';
                                        break;
                                case SDLK_u:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'U' : 'u';
                                        break;
                                case SDLK_v:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'V' : 'v';
                                        break;
                                case SDLK_w:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'W' : 'w';
                                        break;
                                case SDLK_x:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'X' : 'x';
                                        break;
                                case SDLK_y:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'Y' : 'y';
                                        break;
                                case SDLK_z:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = shift ? 'Z' : 'z';
                                        break;
                                case SDLK_COMMA:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = ',';
                                        break;
                                case SDLK_PERIOD:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = '.';
                                        break;
                                case SDLK_SPACE:
                                        if (buffer_size > BUFFER_CAPACITY)
                                                break;
                                        buffer[buffer_size++] = ' ';
                                        break;
                                case SDLK_BACKSPACE:
                                        if (buffer_size > 0)
                                                buffer_size--;
                                        break;
                                }
                                break;
                        }
                }
                glClear(GL_COLOR_BUFFER_BIT);
                
                glUseProgram(program);
                glBindTexture(GL_TEXTURE_2D, atlas->id);
                glBindVertexArray(vao);

                vertices[0]  = 0.0f;
                vertices[1]  = 0.0f;
                vertices[2]  = 0.0f;
                vertices[3]  = 0.0f;

                vertices[4]  = 1.0f;
                vertices[5]  = 0.0f;
                vertices[6]  = 1.0f;
                vertices[7]  = 0.0f;

                vertices[8]  = 1.0f;
                vertices[9]  = 1.0f;
                vertices[10] = 1.0f;
                vertices[11] = 1.0f;
                
                vertices[12] = 0.0f;
                vertices[13] = 1.0f;
                vertices[14] = 0.0f;
                vertices[15] = 1.0f;

                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

                ll_matrix_mode(LL_MATRIX_MODEL);
                ll_matrix_identity();
                ll_matrix_scale3f(512.0, 512.0, 1.0);
                ll_matrix_translate3f((WINDOW_W - 512.0) / 2.0, (WINDOW_H - 512.0) / 2.0, 0.0);
                glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE,
                                   ll_matrix_get_copy().data);
                ll_matrix_mode(LL_MATRIX_VIEW);
                glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE,
                                   ll_matrix_get_copy().data);
                ll_matrix_mode(LL_MATRIX_PROJECTION);
                glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE,
                                   ll_matrix_get_copy().data);
                glUniform4f(glGetUniformLocation(program, "colour"), 0.0, 0.0, 0.0, 1.0);
        
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

                vec2_t pen = ll_vec2_create(200.0, 600.0);
                for (int i = 0; i < buffer_size; i++) {
                        ftgl_texture_glyph_t *glyph;

                        glyph = ftgl_texture_font_get_glyph(font, buffer+i);
                        if (glyph) {
                                float kerning = 0.0f;
                                if (i > 0) {
                                        kerning = ftgl_texture_glyph_get_kerning(glyph, buffer + i - 1);
                                }
                                pen.x += kerning;

                                int x0 = (int) (pen.x + glyph->offset_x);
                                int y0 = (int) (pen.y + glyph->offset_y);
                                int x1 = (int) (x0 + glyph->width);
                                int y1 = (int) (y0 - glyph->height);

                                vertices[0]  = 0.0f;
                                vertices[1]  = 0.0f;
                                vertices[2]  = glyph->s0;
                                vertices[3]  = glyph->t0;

                                vertices[4]  = 1.0f;
                                vertices[5]  = 0.0f;
                                vertices[6]  = glyph->s1;
                                vertices[7]  = glyph->t0;

                                vertices[8]  = 1.0f;
                                vertices[9]  = 1.0f;
                                vertices[10] = glyph->s1;
                                vertices[11] = glyph->t1;
                
                                vertices[12] = 0.0f;
                                vertices[13] = 1.0f;
                                vertices[14] = glyph->s0;
                                vertices[15] = glyph->t1;

                                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                                
                                ll_matrix_mode(LL_MATRIX_MODEL);
                                ll_matrix_identity();
                                ll_matrix_scale3f(glyph->width, glyph->height, 0.0);
                                ll_matrix_translate3f((int) (pen.x + glyph->offset_x), (int) (pen.y - glyph->offset_y), 0.0);
                                glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE,
                                                   ll_matrix_get_copy().data);
                                ll_matrix_mode(LL_MATRIX_VIEW);
                                glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE,
                                                   ll_matrix_get_copy().data);
                                ll_matrix_mode(LL_MATRIX_PROJECTION);
                                glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE,
                                                   ll_matrix_get_copy().data);
                                glUniform4f(glGetUniformLocation(program, "colour"), 0.0, 0.0, 0.0, 1.0);

                                pen.x += glyph->advance_x;
                                
                                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
                        }
                }
                
                glBindVertexArray(0);
                glUseProgram(0);
                SDL_GL_SwapWindow(window);
        }

        glDeleteProgram(program);
        glDeleteTextures(1, &atlas->id);
        atlas->id = 0;
        ftgl_texture_atlas_free(atlas);
        ftgl_texture_font_free(font);
        sdlopengl_free();
        return 0;
}
