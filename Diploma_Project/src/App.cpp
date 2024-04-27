
#include <filesystem>
#include "class_helpers.h"
#include "serialization.h"

std::string version = "1.0";
std::string file_version = "1.0";

StateHandler state;
glm::vec3 sun = glm::vec3(1.0, 1.0, 1.0);
int w_width, w_height;
glm::mat4 relative_cursor = glm::mat4(1.0f);


Canvas canvas(100, 100);
float prev_zoom = 1.0;
glm::vec3 global_transform = glm::vec3(0.0, 0.0, 0.0);

void windowResizeHandler(int window_width, int window_height) {
    //std::cout << "resized";
    state.frustum.initialize(-(float)window_width, (float)window_width, -(float)window_height, (float)window_height, 0.1f, 100.0f);
    state.frustum.adjust(1/state.zoom, global_transform / glm::vec3(state.zoom));
    //state.regenerate_assets = true;
    canvas.calculateSSBB(state);
    glViewport(0, 0, window_width, window_height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    state.zoom += (float)yoffset * state.zoom / 10.0f;
    if (state.zoom < 0.1f)
        state.zoom = 0.1f;
    if (state.zoom > 25.0f)
        state.zoom = 25.0f;
    state.view = glm::scale(state.default_view, glm::vec3(state.zoom, state.zoom, state.zoom));
    state.view = glm::translate(state.view, global_transform / glm::vec3(state.zoom));
    state.frustum.adjust(1/state.zoom, global_transform / glm::vec3(state.zoom));
    state.regenerate_assets = true;
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
                state.regenerate_assets = true;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            /*double x, y;
            glfwGetCursorPos(window, &x, &y);*/
            glfwGetWindowSize(window, &w_width, &w_height);
            if (state.curs_x > state.batman_panel_width && state.curs_x < state.robin_panel_width) {
                state.mouse_pressed = false;
                state.regenerate_assets = false;
                state.view = glm::translate(state.view, state.transform / glm::vec3(state.zoom));
                state.frustum.adjust(state.transform / glm::vec3(state.zoom));
                global_transform += state.transform;
                canvas.calculateSSBB(state);
                //std::cout << state.frustum.Left[0]<< "," << state.frustum.Left[1] << "," << state.frustum.Left[2]<< std::endl;
                //std::cout << state.frustum.Right[0] << "," << state.frustum.Right[1] << "," << state.frustum.Right[2] << "\n\n";
            }
        }
    }
    else if (state.sel_tool != 0)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            if (canvas.isInside(state.last_x, state.last_y)&& state.last_x> state.batman_panel_width && state.last_x<state.robin_panel_width)
            {
                state.brush_pressed = true;
                if(state.sel_tool > 2)
                    state.regenerate_assets = true;
                
            }
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            state.brush_pressed = false;
            if (state.sel_tool > 2)
                state.regenerate_assets = false;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            //int w_width, w_height;
            glfwGetCursorPos(window, &state.last_x, &state.last_y);
            glfwGetWindowSize(window, &w_width, &w_height);

            if (state.last_x > state.batman_panel_width && state.last_x < state.robin_panel_width)
            {
                state.mouse_pressed = true;
                state.regenerate_assets = true;

            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            /*double x, y;
            glfwGetCursorPos(window, &x, &y);*/
            glfwGetWindowSize(window, &w_width, &w_height);
            if (state.curs_x > state.batman_panel_width && state.curs_x < state.robin_panel_width) {
                state.mouse_pressed = false;
                state.regenerate_assets = false;
                state.view = glm::translate(state.view, state.transform / glm::vec3(state.zoom));
                state.frustum.adjust(state.transform / glm::vec3(state.zoom));
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

    /* C reate a windowed mode window and its OpenGL context */
    
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
    //glDisablei(GL_BLEND, 0);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glBlendEquationSeparatei(0, GL_FUNC_ADD, GL_MAX); 
    //glEnable(GL_BLEND);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    ShaderProgramSource heightmap_sources, terrain_sources, cursor_sources, asset_sources, startup_bg_source;

    heightmap_sources = ParseShader("res/shaders/mg_heightmap.shader");
    unsigned int heightmap_shader = CreateShader(heightmap_sources.vertexShader, heightmap_sources.fragmentShader);
    
    terrain_sources = ParseShader("res/shaders/mg_terrain.shader");
    unsigned int terrain_shader = CreateShader(terrain_sources.vertexShader, terrain_sources.fragmentShader);

    cursor_sources = ParseShader("res/shaders/mg_cursor.shader");
    unsigned int cursor_shader = CreateShader(cursor_sources.vertexShader, cursor_sources.fragmentShader);
    
    asset_sources = ParseShader("res/shaders/mg_mpds.shader");
    unsigned int asset_shader = CreateShader(asset_sources.vertexShader, asset_sources.fragmentShader);

    startup_bg_source = ParseShader("res/shaders/mg_startup_bg.shader");
    unsigned int startup_bg_shader = CreateShader(startup_bg_source.vertexShader, startup_bg_source.fragmentShader);

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::cout << "Current path is " << currentPath << std::endl;
    
    AssetHandler buildings;
    AssetHandler flora;
    AssetHandler mountains;
    flora.asset_type = 2;
    mountains.asset_type = 1;

    Quad frm_buffr(w_width, w_height);

    Quad brush(100,100);

    Quad ass(100, 100);

    state.window = window;

    float res = (float)canvas_width / canvas_height;

    FrameBuffer heightmap_FB(w_width, w_height);

    FrameBuffer resulting_FB(w_width, w_height);

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
    Painter painter;

    UiHandler ui(painter);
    ui.setCustomFont("res/fonts/AtkinsonHyperlegible-Regular.ttf", "res/fonts/AtkinsonHyperlegible-Bold.ttf");
    ui.setCustomStyle();


    //HATE THIS - needed to enable correct painting at the start.
    glfwGetWindowSize(window, &w_width, &w_height);
    state.w_width = w_width;
    state.w_height = w_height;
    glViewport(0, 0, w_width, w_height);
    glfwGetCursorPos(window, &state.curs_x, &state.curs_y);
    canvas.calculateSSBB(state);

    Quad startup_bg(w_width, w_height);
    startup_bg.texture_path = "res/assets/background.png";
    startup_bg.initialize(true);
    startup_bg.setShader(startup_bg_shader);

    state.attachShader(startup_bg_shader);
    state.updMat(startup_bg.model, "model");
    state.updMat(state.view, "view");
    state.updMat(state.projection, "projection");
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;

    Archive archive;
    
    const char* prevstate = archive.checkLastState();


    while (!glfwWindowShouldClose(window))
    {

        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;
        if (timeDiff >= 1.0 / 30.0)
        {
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff / counter)* 1000);
            std::string newTit1e = "Magic Maps " + FPS + "FPS / +" + ms + "ms";
            glfwSetWindowTitle(window, newTit1e.c_str());
            prevTime = crntTime;
            counter = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwGetWindowSize(window, &w_width, &w_height);
        
        if (state.save)
        {
            w_width = canvas_width;
            w_height = canvas_height;
        }
        state.w_width = w_width;
        state.w_height = w_height;
        windowResizeHandler(w_width, w_height);
        glfwGetCursorPos(window, &state.curs_x, &state.curs_y);
        if(state.initial_start)
        { 
            startup_bg.draw();

            bool open_start = false;

            ImGui::Begin("Startup", &open_start, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 cursorPos = ImGui::GetCursorPos();

            ImGui::SetWindowSize(ImVec2(w_width, w_height));
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGuiStyle& style = ImGui::GetStyle();
            ImGui::SetCursorPos(ImVec2(0, windowSize.y*0.3));
            ui.middleLabel("Maps Generator");

            cursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(windowSize.x*0.3, cursorPos.y));
            ImGui::PushItemWidth(windowSize.x * 0.4);
            ImGui::InputInt("canvas width", &canvas_width,0);

            cursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(windowSize.x * 0.3, cursorPos.y));
            ImGui::PushItemWidth(windowSize.x * 0.4);
            ImGui::InputInt("canvas height", &canvas_height, 0);
           
            ImVec2 textSize = ImGui::CalcTextSize("Load saved state");
            cursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(windowSize.x * 0.4, cursorPos.y));
            ImGui::Checkbox("Load saved state", &state.load_from_bin);

            if (prevstate[0] == 'n')
            {
                ImGui::SetCursorPos(ImVec2(windowSize.x * 0.4 + textSize[0] + 40.f, cursorPos.y + 2.f));
                ImGui::TextColored({ 1.0,0,0,1 }, prevstate);
            }
            else
            {
                ImGui::SetCursorPos(ImVec2(windowSize.x * 0.4 + textSize[0] + 40.f, cursorPos.y + 2.f));
                ImGui::TextColored({ 0,1.0,0,1 }, prevstate);
            }
            

            ui.middleLabel("You can't change the size later!");

            cursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(windowSize.x * 0.4, cursorPos.y));
            
            if (ImGui::Button("Start", ImVec2(windowSize.x * 0.2, windowSize.y*0.1)))
            {
                state.initial_start = false;

                canvas.initialize(false);
                canvas.createTexture(canvas_width, canvas_height);
                
                canvas.debug();
                canvas.setShader(terrain_shader);

                frm_buffr.initialize(false);
                frm_buffr.texture = canvas.fb_texture;
                frm_buffr.debug();
                frm_buffr.setShader(heightmap_shader);

                brush.initialize(false);
                brush.debug();
                brush.setShader(cursor_shader);

                ass.initialize(false);
                ass.debug();
                ass.setShader(asset_shader);

                //SETTING UP TERRAIN SHADER

                state.attachShader(terrain_shader);
                canvas.setSize(state, canvas_width, canvas_height);
                state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
                state.frustum.initialize(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
                state.updMat(state.view, "view");
                state.updMat(state.projection, "projection");
                state.updVec(sun, "u_sun_pos");
                res = (float)canvas_width / canvas_height;
                state.updFloat(res, "u_BgRes");

                if (state.load_from_bin)
                {
                    archive.startDeserialization();
                    
                    archive.deserialize(file_version);
                   /* if (file_version != version)
                    {
                        std::cout << file_version <<" version of the file" << "\n";
                        std::cout << version <<" version of the program" <<"\n";
                        continue;
                    }*/
                    archive.deserialize(state.brush_size);
                    archive.deserialize(state.brush_hardness);
                    archive.deserialize(state.brush_opacity);

                    archive.deserialize(canvas.noise_compl);
                    archive.deserialize(canvas.noise_1_scale);
                    archive.deserialize(canvas.noise_2_scale);

                    archive.deserialize(canvas.outline_thickness);
                    archive.deserialize(canvas.outline_hardness);

                    archive.deserialize(canvas.steps_w);
                    archive.deserialize(canvas.steps_t);

                    archive.deserialize(canvas.use_outline);
                    archive.deserialize(canvas.use_secondary_tc);
                    archive.deserialize(canvas.use_secondary_wc);
                    archive.deserialize(canvas.use_step_gradient_w);
                    archive.deserialize(canvas.use_step_gradient_t);

                    archive.deserialize(canvas.terrain_c);
                    archive.deserialize(canvas.terrain_secondary_c);
                    archive.deserialize(canvas.outline_c);

                    archive.deserialize(canvas.water_c);
                    archive.deserialize(canvas.water_secondary_c);

                    archive.deserialize(buildings.asset_size);
                    archive.deserialize(buildings.amount);
                    archive.deserialize(buildings.erase_asset);

                    archive.deserialize(flora.asset_size);
                    archive.deserialize(flora.amount);
                    archive.deserialize(flora.erase_asset);

                    archive.deserialize(mountains.asset_size);
                    archive.deserialize(mountains.amount);
                    archive.deserialize(mountains.erase_asset);

                    archive.deserialize(state.sel_tool);
                    archive.deserialize(state.density_1);
                    archive.deserialize(state.density_2);
                    archive.deserialize(state.density_3);
                    archive.deserialize(canvas.canvas_rgba, canvas_width* canvas_height * 4);
                    archive.deserialize(canvas.buildings_rgba, canvas_width* canvas_height * 4);
                    archive.stopDeserialization();
                    canvas.uploadFbTexture();
                    state.regenerate_assets = true;
                    std::cout << "\n" << "AFTER\n";
                    //state.load_from_bin = false;
                }

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
                state.updFloat(res, "u_BgRes");

                //SETTING UP HEIGHTMAP GENERATION SHADER

                state.attachShader(heightmap_shader);
                state.updFloat(res, "u_BgRes");

                canvas.texture = heightmap_FB.getResultTexture();

                buildings.bgTexture_ID = resulting_FB.getResultTexture();
                flora.bgTexture_ID = resulting_FB.getResultTexture();
                mountains.bgTexture_ID = resulting_FB.getResultTexture();

                canvas.calculateSSBB(state);

                // SETTING UP ASSETS

                buildings.genDistribution(canvas, 1.0 / state.density_1);
                flora.genDistribution(canvas, 1.0 / state.density_2);
                mountains.genDistribution(canvas, 1.0 / state.density_3);
                buildings.regenerate_mpds = true;
                flora.regenerate_mpds = true;
                mountains.regenerate_mpds = true;

            }
            ImGui::End();
            ImGui::Render();
            
        }
        else
        {
            // FRAME BUFFER GENERATION
            heightmap_FB.updateSize(w_width, w_height);
            heightmap_FB.bind();
            state.saveFbID(heightmap_FB.getFbID());

            //Fail-safe
            if (canvas.noise_compl > 100)
                canvas.noise_compl = 6;

            // updating terrain generation settings
            state.updFloat(canvas.noise_compl, "u_complexity");
            state.updFloat(canvas.noise_1_scale, "u_scale1");
            state.updFloat(canvas.noise_2_scale, "u_scale2");
            frm_buffr.draw();
            heightmap_FB.unBind();
            state.saveFbID(0);
            //---END FRAME BUFFER GENERATION


            // DRAWING CANVAS
            state.attachShader(terrain_shader);
            state.updInt(0, "u_isFb");
            state.projection = glm::ortho(-(float)w_width, (float)w_width, -(float)w_height, (float)w_height, 0.1f, 100.0f);
            state.updMat(state.projection, "projection");
            state.updVec(canvas.terrain_c, "u_terrain_color");
            state.updVec(canvas.water_c, "u_water_color");
            state.updVec(canvas.outline_c, "u_outline_color");
            state.updVec(canvas.terrain_secondary_c, "u_terrain_secondary_c");
            state.updVec(canvas.water_secondary_c, "u_water_secondary_c");
            state.updInt((int)canvas.use_secondary_tc, "u_use_secondary_tc");
            state.updInt((int)canvas.use_secondary_wc, "u_use_secondary_wc");
            state.updInt((int)canvas.use_step_gradient_w, "u_use_step_gradient_w");
            state.updInt((int)canvas.use_step_gradient_t, "u_use_step_gradient_t");
            state.updInt((int)canvas.steps_w, "u_steps_w");
            state.updInt((int)canvas.steps_t, "u_steps_t");
            state.updInt((int)canvas.use_proc_texture_w, "u_use_texture_w");
            state.updInt((int)canvas.use_proc_texture_t, "u_use_texture_t");
            state.updInt((int)canvas.dot_aff_w, "u_dot_aff_w");
            state.updInt((int)canvas.dot_aff_t, "u_dot_aff_t");
            state.updFloat(res, "u_BgRes");
            state.updFloat(canvas.outline_thickness, "u_outline_thickness");
            state.updFloat(canvas.outline_hardness, "u_outline_hardness");
            state.updFloat(canvas.use_outline, "u_use_outline");
            state.updFloat(canvas.dot_size_w, "u_dot_size_w");
            state.updFloat(canvas.dot_size_t, "u_dot_size_t");
            state.updFloat(0, "u_debug");
            if (state.reset)
            {
                state.reset = false;
                global_transform = glm::vec3(0.0, 0.0, 0.0);
            }
            state.view_relative = state.view;
            if (state.mouse_pressed)
            {
                glfwGetCursorPos(window, &x, &y);
                if (x > state.batman_panel_width)
                {
                    state.transform = glm::vec3(2) * glm::vec3((x - state.last_x), -(y - state.last_y), 0.0f);
                    state.view_relative = glm::translate(state.view_relative, state.transform / glm::vec3(state.zoom));
                }
                state.frustum.adjust(state.transform / glm::vec3(state.zoom));
                canvas.calculateSSBB(state);
                //state.frustum.adjust()//REAL TIME FRUSTUM CALCULATION
            }
            state.updMat(state.view_relative, "view");
            canvas.draw();
            state.updInt(1, "u_isFb");
            state.updMat(glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)), "transform_");
            //---END DRAWING CANVAS


            // DRAWING CANVAS TO TEXTURE
            resulting_FB.updateSize(w_width, w_height);
            resulting_FB.bind();
            
            state.saveFbID(resulting_FB.getFbID());
            canvas.draw();
            if (state.save)
            {
                glEnable(GL_BLEND);
                buildings.draw(state, canvas, asset_shader, glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)),1);
                flora.draw(state, canvas, asset_shader, glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)),1);
                mountains.draw(state, canvas, asset_shader, glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)),1);
                glDisable(GL_BLEND);
            }
            resulting_FB.unBind();
            state.saveFbID(0);
            //---END DRAWING CANVAS TO TEXTURE 


            // DRAWING ASSETS
            if (state.regenerate_assets)
            {
                buildings.regenerate_mpds = state.regenerate_assets;
                flora.regenerate_mpds = state.regenerate_assets;
                mountains.regenerate_mpds = state.regenerate_assets;
                state.regenerate_assets = false;
            }
            glEnable(GL_BLEND);
            buildings.draw(state, canvas, asset_shader, glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)), 0);
            flora.draw(state, canvas, asset_shader, glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)), 0);
            mountains.draw(state, canvas, asset_shader, glm::translate(state.default_view, glm::vec3(-(float)(w_width - canvas_width), (float)(w_height - canvas_height), 0)), 0);
            glDisable(GL_BLEND);

            //---END DRAWING ASSETS
             

            // SAVING MAP
            if (state.save)
            {
                state.save = false;
                state.exportAsPNG(resulting_FB.getResultTexture(), w_width, w_height,canvas_width,canvas_height ,state.export_name_data);
            }
            //---END SAVING MAP


            // SAVING STATE
            if (state.save_to_bin)
            {
                state.save_to_bin = false;
                archive.startSerialization();
                archive.serialize(version);
                archive.serialize(state.brush_size);
                archive.serialize(state.brush_hardness);
                archive.serialize(state.brush_opacity);

                archive.serialize(canvas.noise_compl);
                archive.serialize(canvas.noise_1_scale);
                archive.serialize(canvas.noise_2_scale);

                archive.serialize(canvas.outline_thickness);
                archive.serialize(canvas.outline_hardness);

                archive.serialize(canvas.steps_w);
                archive.serialize(canvas.steps_t);

                archive.serialize(canvas.use_outline);
                archive.serialize(canvas.use_secondary_tc);
                archive.serialize(canvas.use_secondary_wc);
                archive.serialize(canvas.use_step_gradient_w);
                archive.serialize(canvas.use_step_gradient_t);

                archive.serialize(canvas.terrain_c);
                archive.serialize(canvas.terrain_secondary_c);
                archive.serialize(canvas.outline_c);

                archive.serialize(canvas.water_c);
                archive.serialize(canvas.water_secondary_c);

                archive.serialize(buildings.asset_size);
                archive.serialize(buildings.amount);
                archive.serialize(buildings.erase_asset);

                archive.serialize(flora.asset_size);
                archive.serialize(flora.amount);
                archive.serialize(flora.erase_asset);

                archive.serialize(mountains.asset_size);
                archive.serialize(mountains.amount);
                archive.serialize(mountains.erase_asset);

                archive.serialize(state.sel_tool);
                archive.serialize(state.density_1);
                archive.serialize(state.density_2);
                archive.serialize(state.density_3);
                archive.serialize(canvas.canvas_rgba,canvas_width*canvas_height*4);
                archive.serialize(canvas.buildings_rgba,canvas_width*canvas_height*4);
                archive.stopSerialization();
            }
            //---END SAVING STATE


            // CURSOR DRAWING
            if (state.sel_tool != 0)
            {
                state.attachShader(cursor_shader);
                relative_cursor = glm::translate(brush.model, glm::vec3(state.curs_x - w_width / 2.0, -state.curs_y + w_height / 2.0, 0) * glm::vec3(2));
                relative_cursor = glm::scale(relative_cursor, glm::vec3(state.brush_size)*state.zoom);
                state.updMat(state.projection, "projection");
                state.updMat(relative_cursor, "model");
                state.updFloat(state.brush_size * state.zoom, "u_circle_size");
                state.updVec(glm::vec2(state.curs_x, w_height - state.curs_y), "u_pos");
                brush.draw();
                if (state.brush_pressed)
                {
                    glfwGetCursorPos(window, &x, &y);
                    painter.brush_size = state.brush_size;// SHOULD NOT HAPPEN
                    painter.brush_hardness = state.brush_hardness;
                    painter.brush_opacity = state.brush_opacity;
                    switch (state.sel_tool)
                    {
                    case 1:
                        painter.paint(x, y, canvas, state);
                        break;
                    case 2:
                        painter.paint(x, y, canvas, state);
                        break;
                    /*case 5:
                        painter.paint(x, y, canvas, state);
                        break;*/
                    case 3:
                        painter.paint(x, y, canvas, state, buildings);
                        break;
                    case 4:
                        painter.paint(x, y, canvas, state, flora);
                        break;
                    case 5:
                        painter.paint(x, y, canvas, state, mountains);
                        break;
                    }
                }
            }
            //---END CURSOR DRAWING


            state.attachShader(heightmap_shader);


            // UI
            // BASED ON THE TOOL_ID CALL THIS FUNCTION WITH APPROPRIATE ASSET
            switch (state.sel_tool)
            {
            case 3:
                ui.renderUI(state, canvas, buildings, w_width, w_height, canvas_width, canvas_height, res);
                break;
            case 4:
                ui.renderUI(state, canvas, flora, w_width, w_height, canvas_width, canvas_height, res);
                break;
            case 5:
                ui.renderUI(state, canvas, mountains, w_width, w_height, canvas_width, canvas_height, res);
                break;
            default:
                ui.renderUI(state, canvas, buildings, w_width, w_height, canvas_width, canvas_height, res);
            }
            //---END UI
            state.load_from_bin = false;
            
        }
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