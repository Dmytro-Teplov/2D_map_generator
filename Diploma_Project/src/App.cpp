
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include "class_helpers.h"
#include <stb_image.h>

StateHandler state;
glm::vec3 sun = glm::vec3(1.0, 1.0, 1.0);
int w_width, w_height;
glm::mat4 relative_cursor = glm::mat4(1.0f);


Canvas canvas(100, 100);
float prev_zoom = 1.0;
glm::mat4 default_view = state.view;
glm::vec3 global_transform = glm::vec3(0.0, 0.0, 0.0);

void windowResizeHandler(int window_width, int window_height) {
    glViewport(0, 0, window_width, window_height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    state.zoom += (float)yoffset * state.zoom / 10.0f;
    if (state.zoom < 0.1f)
        state.zoom = 0.1f;
    if (state.zoom > 25.0f)
        state.zoom = 25.0f;
    state.view = glm::scale(default_view, glm::vec3(state.zoom, state.zoom, state.zoom));
    state.view = glm::translate(state.view, global_transform / glm::vec3(state.zoom));
    canvas.calculateSSBB(state);
}
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (state.sel_tool == 0) 
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            //int w_width, w_height;
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            glfwGetWindowSize(window, &w_width, &w_height);

            if (state.last_x > state.batman_panel_width && state.last_x < state.robin_panel_width)
            {
                state.mouse_pressed = true;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            /*double x, y;
            glfwGetCursorPos(window, &x, &y);*/
            glfwGetWindowSize(window, &w_width, &w_height);
            if (state.curs_x > state.batman_panel_width && state.curs_x < state.robin_panel_width) {
                state.mouse_pressed = false;
                state.view = glm::translate(state.view, state.transform / glm::vec3(state.zoom));
                global_transform += state.transform;
                canvas.calculateSSBB(state);
            }
        }
    }
    else if (state.sel_tool == 1|| state.sel_tool == 2)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            if (canvas.isInside(state.last_x, state.last_y))
            {
                state.brush_pressed = true;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            state.brush_pressed = false;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        {
            //int w_width, w_height;
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            glfwGetWindowSize(window, &w_width, &w_height);

            if (state.last_x > state.batman_panel_width && state.last_x < state.robin_panel_width)
            {
                state.mouse_pressed = true;

            }
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        {
            /*double x, y;
            glfwGetCursorPos(window, &x, &y);*/
            glfwGetWindowSize(window, &w_width, &w_height);
            if (state.curs_x > state.batman_panel_width && state.curs_x < state.robin_panel_width) {
                state.mouse_pressed = false;
                state.view = glm::translate(state.view, state.transform / glm::vec3(state.zoom));
                global_transform += state.transform;
                canvas.calculateSSBB(state);
               
            }
        }
    }

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    
    float canvas_aspect_ratio = 1.0;
    int canvas_width = 1000, canvas_height = 500;
    window = glfwCreateWindow(1280, 720, "Magic Maps", NULL, NULL);
    glfwGetWindowSize(window, &w_width, &w_height);
    
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "Error";
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    ShaderProgramSource heightmap_sources, terrain_sources, cursor_sources, asset_sources;

    heightmap_sources = ParseShader("res/shaders/mg_heightmap.shader");
    unsigned int heightmap_shader = CreateShader(heightmap_sources.vertexShader, heightmap_sources.fragmentShader);
    
    terrain_sources = ParseShader("res/shaders/mg_terrain.shader");
    unsigned int terrain_shader = CreateShader(terrain_sources.vertexShader, terrain_sources.fragmentShader);

    cursor_sources = ParseShader("res/shaders/mg_cursor.shader");
    unsigned int cursor_shader = CreateShader(cursor_sources.vertexShader, cursor_sources.fragmentShader);
    
    asset_sources = ParseShader("res/shaders/mg_mpds.shader");
    unsigned int asset_shader = CreateShader(asset_sources.vertexShader, asset_sources.fragmentShader);

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current path is " << currentPath << std::endl;

    
    canvas.initialize(false);
    canvas.createTexture(canvas_width,canvas_height);
    canvas.debug();
    canvas.setShader(terrain_shader);

    AssetHandler mountains;

    Quad frm_buffr(w_width, w_height);
    frm_buffr.initialize(false);
    frm_buffr.texture = canvas.fb_texture;
    frm_buffr.debug();
    frm_buffr.setShader(heightmap_shader);

    Quad brush(100,100);
    brush.initialize(false);
    brush.debug();
    brush.setShader(cursor_shader);

    Quad ass(100, 100);
    ass.initialize(false);
    ass.debug();
    ass.setShader(asset_shader);

    state.window = window;

    //SETTING UP TERRAIN SHADER

    state.attachShader(terrain_shader);
    canvas.setSize(state, canvas_width, canvas_height);
    //canvas.model = glm::scale(glm::mat4(1.0f), glm::vec3(canvas_width, canvas_height, 1.0f));
    state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
    //state.updMat(canvas.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    state.updVec(sun, "u_sun_pos");
    float res = (float)canvas_width / canvas_height;
    state.updFloat(res, "u_BgRes");

    //SETTING UP CURSOR SHADER

    state.attachShader(cursor_shader);
    state.updMat(brush.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    state.updVec(glm::vec2(w_width, w_height), "u_resolution");
    state.updVec(glm::vec2(0, 0), "u_pos");
    state.updFloat(state.brush_size, "u_circle_size");

    //SETTING UP ASSET SHADER

    state.attachShader(asset_shader);
    state.updMat(ass.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    //res = (float)canvas_width / canvas_height;
    state.updFloat(res, "u_BgRes");

    //SETTING UP HEIGHTMAP GENERATION SHADER

    state.attachShader(heightmap_shader);
    state.updFloat(res, "u_BgRes");

    //CREATING FRAME BUFFER OBJECT FOR TERRAIN GEN

    FrameBuffer heightmap_FB(w_width, w_height);
    canvas.texture = heightmap_FB.getResultTexture();


    FrameBuffer resulting_FB(w_width, w_height);
    mountains.bgTexture_ID = resulting_FB.getResultTexture();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float c = 1.0f;
    float sz = 100.0f;

    double x = 0;
    double y = 0;

    UiHandler ui;
    ui.setCustomFont("res/fonts/AtkinsonHyperlegible-Regular.ttf", "res/fonts/AtkinsonHyperlegible-Bold.ttf");
    ui.setCustomStyle();

    Painter painter;

    //HATE THIS - needed to enable correct painting at the start.
    glfwGetWindowSize(window, &w_width, &w_height);
    state.w_width = w_width;
    state.w_height = w_height;
    glViewport(0, 0, w_width, w_height);
    glfwGetCursorPos(window, &state.curs_x, &state.curs_y);
    canvas.calculateSSBB(state);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwGetWindowSize(window, &w_width, &w_height);
        state.w_width = w_width;
        state.w_height = w_height;
        glViewport(0, 0, w_width, w_height);
        glfwGetCursorPos(window, &state.curs_x, &state.curs_y);

        //FRAME BUFFER GENERATION
        heightmap_FB.updateSize(w_width, w_height);
        heightmap_FB.bind();
        state.saveFbID(heightmap_FB.getFbID());


        //updating terrain generation settings
        state.updFloat(canvas.noise_compl, "u_complexity");
        state.updFloat(canvas.noise_1_scale, "u_scale1");
        state.updFloat(canvas.noise_2_scale, "u_scale2");
        frm_buffr.draw();
        
        heightmap_FB.unBind();
        state.saveFbID(0);

        //CANVAS DRAWING 
        state.attachShader(terrain_shader);
        state.updInt(0, "u_isFb");
        state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
        state.updMat(state.projection, "projection");
        state.updVec(canvas.terrain_c,"u_terrain_color");
        state.updVec(canvas.water_c,"u_water_color");
        state.updVec(canvas.outline_c,"u_outline_color");
        state.updVec(canvas.terrain_secondary_c, "u_terrain_secondary_c");
        state.updVec(canvas.water_secondary_c, "u_water_secondary_c");
        state.updInt((int)canvas.use_secondary_tc, "u_use_secondary_tc");
        state.updInt((int)canvas.use_secondary_wc, "u_use_secondary_wc");
        state.updFloat(canvas.outline_thickness,"u_outline_thickness");
        state.updFloat(canvas.outline_hardness,"u_outline_hardness");
        state.updFloat(canvas.use_outline,"u_use_outline");
        

        if (state.save)
        {
            state.save = false;
            //DO SAVE
        }
        
        state.view_relative = state.view;
        if (state.mouse_pressed)
        {
            glfwGetCursorPos(window, &x, &y);

            if (x > state.batman_panel_width)
            {
                state.transform = glm::vec3(2)*glm::vec3((x - state.last_x), -(y - state.last_y), 0.0f);
                state.view_relative = glm::translate(state.view_relative, state.transform / glm::vec3(state.zoom));
            }

        }
        
        state.updMat(state.view_relative, "view");
        canvas.draw();

        state.updInt(1, "u_isFb");
        state.updMat(glm::translate(default_view, glm::vec3(-(float)(w_width-canvas_width), (float)(w_height - canvas_height),0)), "transform_");
        

        resulting_FB.updateSize(w_width, w_height);
        resulting_FB.bind();
        state.saveFbID(resulting_FB.getFbID());
        canvas.draw();
        resulting_FB.unBind();
        state.saveFbID(0);
        //if (mountains.number_of_assets>0)
        //{
        //    for (int i = 0; i < mountains.number_of_assets; i++)
        //    {
        //        //mountains.assets[i].draw(state, cursor_shader);
        //        std::cout << "\n" << mountains.asset_positions[i][0] << "\t" << mountains.asset_positions[i][1] << std::endl;
        //    }
        //    mountains.number_of_assets = 0;
        //}


        

        mountains.draw(state,canvas, asset_shader, glm::translate(default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)));
        
        //CURSOR DRAWING
        if (state.sel_tool != 0)
        {

            state.attachShader(cursor_shader);
            relative_cursor = glm::translate(brush.model, glm::vec3(state.curs_x - w_width / 2.0, -state.curs_y + w_height / 2.0, 0) * glm::vec3(2));
            relative_cursor = glm::scale(relative_cursor, glm::vec3(state.brush_size));
            state.updMat(state.projection, "projection");
            state.updMat(relative_cursor, "model");
            state.updFloat(state.brush_size, "u_circle_size");
            state.updVec(glm::vec2(state.curs_x, w_height - state.curs_y), "u_pos");
            brush.draw();
            if (state.brush_pressed)
            {
                glfwGetCursorPos(window, &x, &y);

                painter.brush_size = state.brush_size;
                painter.brush_hardness = state.brush_hardness;
                painter.paint(x, y, canvas, state);
            }
        }

        state.attachShader(heightmap_shader);
        //UI
        ui.renderUI(state, canvas, mountains ,w_width, w_height, canvas_width, canvas_height, res);

        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(heightmap_shader);
    glDeleteProgram(terrain_shader);

    glfwTerminate();
    return 0;
}