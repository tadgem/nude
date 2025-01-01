#include "nude.h"
#include "font.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif
#include <stdio.h>
#ifdef _WIN32
#include "windows.h"
#endif

nude::API::State nude::API::Init() {

#ifdef WIN32
  SetProcessDPIAware();
#endif
  State s {};
  // Setup SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
  {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return {};
  }

  // Create window with SDL_Renderer graphics context
  Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
  s.m_window= SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example", 1280, 720, window_flags);
  if (s.m_window == nullptr)
  {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return {};
  }
  s.m_renderer = SDL_CreateRenderer(s.m_window, nullptr);
  SDL_SetRenderVSync(s.m_renderer, 1);
  if (s.m_renderer == nullptr)
  {
    SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
    return {};
  }
  SDL_SetWindowPosition(s.m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(s.m_window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(s.m_window, s.m_renderer);
  ImGui_ImplSDLRenderer3_Init(s.m_renderer);

  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  s.m_global_dockspace_id = UINT32_MAX - 1;

  s.m_zep_state = new ZepContainer("", "");

  font::InitBuiltInFonts(s);

  return s;
}
void nude::API::PreFrame(nude::API::State &s) {
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL3_ProcessEvent(&event);
    if (event.type == SDL_EVENT_QUIT)
      s.m_quit = true;
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(s.m_window))
      s.m_quit = true;
  }
  if (SDL_GetWindowFlags(s.m_window) & SDL_WINDOW_MINIMIZED)
  {
    SDL_Delay(10);
    return;
  }

  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;


  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.


  // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
  // all active windows docked into it will lose their parent and become undocked.
  // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
  // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 winSize = viewport->Size;
  ImGui::SetNextWindowPos({0,0});
  ImGui::SetNextWindowSize(winSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::Begin("DockSpace", nullptr, window_flags);
  ImGui::DockSpace(s.m_global_dockspace_id, winSize, ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();


}

void nude::API::PostFrame(nude::API::State &s) {
    // Rendering
    ImGui::Render();
    //SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColorFloat(s.m_renderer,
                                s.m_clear_color.x,
                                s.m_clear_color.y,
                                s.m_clear_color.z,
                                s.m_clear_color.w);
    SDL_RenderClear(s.m_renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), s.m_renderer);
    SDL_RenderPresent(s.m_renderer);
}
void nude::API::Quit(nude::API::State &s) {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(s.m_renderer);
    SDL_DestroyWindow(s.m_window);
    SDL_Quit();
    delete s.m_zep_state;
}
