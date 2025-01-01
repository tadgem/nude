#pragma once
#include <filesystem>
#include "nude_zep.h"
#include "imgui.h"
#include "imfilebrowser.h"
namespace nude
{
  class API
  {
  public:
    struct State
    {
      SDL_Window*   m_window;
      SDL_Renderer* m_renderer;
      ZepContainer* m_zep_state;

      ImFont*       m_ui_font;
      ImFont*       m_source_code_font;

      ImGuiID       m_global_dockspace_id;

      ImVec4        m_clear_color = ImVec4(0.1f, 0.1f, 0.14f, 1.00f);
      bool          m_quit = false;
    };
    static State  Init();
    static void   SetImGuiStyle();
    static void   PreFrame(State& s);
    static void   PostFrame(State& s);
    static void   Quit(State& s);
  };

}