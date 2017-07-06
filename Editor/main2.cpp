#include <GL/glew.h>
#include <Usagi/Engine/Runtime/DevicePlatform/Win32DevicePlatform.hpp>
#include <Usagi/Engine/Runtime/RenderWindow.hpp>
#include <Usagi/Engine/Runtime/GraphicsInterface/OpenGL/Shader/OpenGLShader.hpp>
#include <Usagi/Engine/Runtime/GraphicsInterface/OpenGL/Shader/OpenGLProgram.hpp>

int main(int argc, char *argv[])
{
    using namespace yuki;
//
//    Win32Window w("UsagiEditor", 1280, 720);
//    w.show();
//    w.loop();

    try
    {
        Win32DevicePlatform dev;
        auto rwnd = dev.createRenderWindow("UsagiEditor", 1280, 720);
        rwnd->show();
        rwnd->setCurrent();

        OpenGLShader vtxshader(GL_VERTEX_SHADER, R"(
            #version 330 core

            // Input vertex data, different for all executions of this shader.
            layout(location = 0) in vec3 vertexPosition_modelspace;

            void main()
            {
                gl_Position.xyz = vertexPosition_modelspace;
                gl_Position.w = 1.0;

            }
        )"), fragshader(GL_FRAGMENT_SHADER, R"(
            #version 330 core

            // Ouput data
            out vec3 color;

            void main()
            {
	            // Output color = red 
	            color = vec3(1,0,0);
            }
        )");

        OpenGLProgram prog;
        prog.attachShader(vtxshader);
        prog.attachShader(fragshader);
        prog.link();


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // An array of 3 vectors which represents 3 vertices
        static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            0.0f,  1.0f, 0.0f,
        };

        // This will identify our vertex buffer
        GLuint vertexbuffer;
        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);
        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        // Draw the triangle !
        glUseProgram(prog.getId());

        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);

        rwnd->swapBuffer();

        rwnd->processEvents();
    }
    catch(std::exception &e)
    {
        throw;
    }

    return 0;
}
