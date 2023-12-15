//
// Created by baidu on 2021/5/25.
//

#include "EglHelper.h"
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/un.h>

struct texture_storage_metadata_t {
    int fourcc;
    EGLuint64KHR modifiers;
    EGLint stride;
    EGLint offset;
};

int EglHelper::initEgl(EGLNativeWindowType surface) {
    // 1.得到默认的显示设备，即屏幕窗口
    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(mEglDisplay == EGL_NO_DISPLAY){
        LOGE("eglGetDisplay error");
        return -1;
    }

    LOGE("before initialize");
    // 2.初始化默认显示设备
//    EGLint *version = new EGLint[2];
    if(!eglInitialize(mEglDisplay, NULL,NULL)){
        LOGE("eglInitialize error");
        return -1;
    }

    // 3.设置显示设备的属性
    const EGLint attrib_config_list[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
//            EGL_ALPHA_SIZE, 8,
//            EGL_DEPTH_SIZE, 8,
//            EGL_STENCIL_SIZE, 8,
//            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };
    LOGE("before chose config");
    // 3.1根据所需的参数获取符合该参数的config_size,主要是解决有些手机eglChooseConfig失败的兼容性问题
    EGLint  num_config;
//    if(!eglChooseConfig(mEglDisplay, attrib_config_list, NULL, 1, &num_config)){
//        LOGE("eglChooseConfig error");
//        return -1;
//    }
    // 3.2根据获取的config_size得到eglConfig
    EGLConfig eglConfig;
    if(!eglChooseConfig(mEglDisplay, attrib_config_list, &eglConfig, 1, &num_config)){
        LOGE("eglChooseConfig error");
        return -1;
    }
    // 4.创建egl上下文 eglCreateContext
    const EGLint attrib_ctx_list[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };
    LOGE("before create context");
    mEglContext = eglCreateContext(mEglDisplay, eglConfig, NULL, attrib_ctx_list);
    if(mEglContext == EGL_NO_CONTEXT){
        LOGE("eglCreateContext error");
        return -1;
    }
    // 5.创建渲染的surface
    mEglSurface = eglCreateWindowSurface(mEglDisplay, eglConfig, surface, NULL);
    if(mEglSurface == EGL_NO_SURFACE){
        LOGE("eglCreateWindowSurface error");
        return -1;
    }
    //6.绑定eglContext和Surface到display
    if(!eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface,mEglContext)){
        LOGE("eglCreateWindowSurface error");
        return -1;
    }
    // 7.刷新数据，显示渲染场景 -- eglSwapBuffers
    LOGE("before setup scene");
    gl_setup_scene();

    texture_data = create_data(TEXTURE_DATA_SIZE);

    // GL: Create and populate the texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_DATA_WIDTH, TEXTURE_DATA_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_DATA_WIDTH, TEXTURE_DATA_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    LOGE("before swap");
    swapBuffers();

    // EGL: Create EGL image from the GL texture
//    EGLImage image = eglCreateImage(mEglDisplay,
//                                    mEglContext,
//                                    EGL_GL_TEXTURE_2D,
//                                    (EGLClientBuffer)(uint64_t)texture,
//                                    NULL);
//    if (image == EGL_NO_IMAGE) {
//        return -1;
//    }

    // The next line works around an issue in radeonsi driver (fixed in master at the time of writing). If you are
    // having problems with texture rendering until the first texture update you can uncomment this line
    // glFlush();

    // EGL (extension: EGL_MESA_image_dma_buf_export): Get file descriptor (texture_dmabuf_fd) for the EGL image and get its
    // storage data (texture_storage_metadata)
//    int texture_dmabuf_fd;
//
//    struct texture_storage_metadata_t texture_storage_metadata;
//
//    int num_planes;
//    PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC eglExportDMABUFImageQueryMESA =
//            (PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC)eglGetProcAddress("eglExportDMABUFImageQueryMESA");
//    EGLBoolean queried = eglExportDMABUFImageQueryMESA(mEglDisplay,
//                                                       image,
//                                                       &texture_storage_metadata.fourcc,
//                                                       &num_planes,
//                                                       &texture_storage_metadata.modifiers);
//    if (!queried) {
//        return -1;
//    }
//    if (num_planes != 1) {
//        return -1;
//    }
//    PFNEGLEXPORTDMABUFIMAGEMESAPROC eglExportDMABUFImageMESA =
//            (PFNEGLEXPORTDMABUFIMAGEMESAPROC)eglGetProcAddress("eglExportDMABUFImageMESA");
//    EGLBoolean exported = eglExportDMABUFImageMESA(mEglDisplay,
//                                                   image,
//                                                   &texture_dmabuf_fd,
//                                                   &texture_storage_metadata.stride,
//                                                   &texture_storage_metadata.offset);
//    if (!exported) {
//        return -1;
//    }
//
//
//    int sock = create_socket(SERVER_FILE);
//    while(connect_socket(sock,CLIENT_FILE) != 0 )
//        ;
//    write_fd(sock, texture_dmabuf_fd, &texture_storage_metadata, sizeof(texture_storage_metadata));
//    close(sock);
//    close(texture_dmabuf_fd);
    return 0;

}

void EglHelper::gl_setup_scene()
{
    // Shader source that draws a textures quad
    const char *vertex_shader_source = "#version 330 core\n"
                                       "layout (location = 0) in vec3 aPos;\n"
                                       "layout (location = 1) in vec2 aTexCoords;\n"

                                       "out vec2 TexCoords;\n"

                                       "void main()\n"
                                       "{\n"
                                       "   TexCoords = aTexCoords;\n"
                                       "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                       "}\0";
    const char *fragment_shader_source = "#version 330 core\n"
                                         "out vec4 FragColor;\n"

                                         "in vec2 TexCoords;\n"

                                         "uniform sampler2D Texture1;\n"

                                         "void main()\n"
                                         "{\n"
                                         "   FragColor = texture(Texture1, TexCoords);\n"
                                         "}\0";

    // vertex shader
    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    // fragment shader
    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    // link shaders
    int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    // delete shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // quad
    float vertices[] = {
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f,   // top right
            0.5f, -0.5f, 0.0f, 1.0f, 1.0f,  // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom left
            -0.5f, 0.5f, 0.0f, 0.0f, 0.0f   // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first Triangle
            1, 2, 3  // second Triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // Prebind needed stuff for drawing
    glUseProgram(shader_program);
    glBindVertexArray(VAO);
}

void EglHelper::gl_draw_scene()
{
    // clear
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw quad
    // VAO and shader program are already bound from the call to gl_setup_scene
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



int* EglHelper::create_data(size_t size)
{
    size_t edge = sqrt(size);
    size_t half_edge = edge / 2;
    int * data;
    data = (int *)malloc(size * sizeof(int));

    // Paint the texture like so:
    // RG
    // BW
    // where R - red, G - green, B - blue, W - white
    int red = 0x000000FF;
    int green = 0x0000FF00;
    int blue = 0X00FF0000;
    int white = 0x00FFFFFF;
    for (size_t i = 0; i < size; i++) {
        size_t x = i % edge;
        size_t y = i / edge;

        if (x < half_edge) {
            if (y < half_edge) {
                data[i] = red;
            } else {
                data[i] = blue;
            }
        } else {
            if (y < half_edge) {
                data[i] = green;
            } else {
                data[i] = white;
            }
        }
    }

    return data;
}

void EglHelper::rotate_data()
{
    int * data = this->texture_data;

    size_t edge = sqrt(this->TEXTURE_DATA_SIZE);
    size_t half_edge = edge / 2;

    for (size_t i = 0; i < half_edge * half_edge; i++) {
        size_t x = i % half_edge;
        size_t y = i / half_edge;

        int temp = data[x + y * edge];
        data[x + y * edge] = data[(x + half_edge) + y * edge];
        data[(x + half_edge) + y * edge] = data[(x + half_edge) + (y + half_edge) * edge];
        data[(x + half_edge) + (y + half_edge) * edge] = data[x + (y + half_edge) * edge];
        data[x + (y + half_edge) * edge] = temp;
    }
}




int EglHelper::create_socket(const char *path)
{
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    unlink(path);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        exit(-1);
    }

    return sock;
}

int EglHelper::connect_socket(int sock, const char *path)
{
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    return connect(sock, (struct sockaddr *)&addr, sizeof(addr));
}

void EglHelper::write_fd(int sock, int fd, void *data, size_t data_len)
{
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(fd))];
    memset(buf, '\0', sizeof(buf));

    struct iovec io = {.iov_base = data, .iov_len = data_len};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));

    memmove(CMSG_DATA(cmsg), &fd, sizeof(fd));

    msg.msg_controllen = CMSG_SPACE(sizeof(fd));

    if (sendmsg(sock, &msg, 0) < 0)
    {
        exit(-1);
    }
}

void EglHelper::read_fd(int sock, int *fd, void *data, size_t data_len)
{
    struct msghdr msg = {0};

    struct iovec io = {.iov_base = data, .iov_len = data_len};
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    char c_buffer[256];
    msg.msg_control = c_buffer;
    msg.msg_controllen = sizeof(c_buffer);

    if (recvmsg(sock, &msg, 0) < 0)
    {
        exit(-1);
    }

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);

    memmove(fd, CMSG_DATA(cmsg), sizeof(fd));
}




int EglHelper::swapBuffers() {
    if(mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE){
        gl_draw_scene();
        eglSwapBuffers(mEglDisplay, mEglSurface);
        return 0;
    }
    return -1;
}

void EglHelper::destroyEgl() {
    if(mEglDisplay != EGL_NO_DISPLAY){
        // 解绑display上的eglContext和Surface
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        // 销毁surface和eglContext
        if(mEglSurface != EGL_NO_SURFACE){
            eglDestroySurface(mEglDisplay, mEglSurface);
            mEglSurface = EGL_NO_SURFACE;
        }

        if(mEglContext != EGL_NO_CONTEXT){
            eglDestroyContext(mEglDisplay, mEglContext);
            mEglContext = EGL_NO_CONTEXT;
        }

        if(mEglDisplay != EGL_NO_DISPLAY){
            eglTerminate(mEglDisplay);
            mEglDisplay = EGL_NO_DISPLAY;
        }
    }
}

EglHelper::EglHelper() {
    mEglDisplay = EGL_NO_DISPLAY;
    mEglSurface = EGL_NO_SURFACE;
    mEglContext = EGL_NO_CONTEXT;
}

EglHelper::~EglHelper() {

}
