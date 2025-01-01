#include "nude.h"

// Main code
int main(int, char**)
{
  nude::API::State s = nude::API::Init();

  ImGui::FileBrowser file_browser (
      ImGuiFileBrowserFlags_NoModal
  );

  file_browser.SetTitle("Workspace");
  file_browser.Open();

  while (!s.m_quit)
  {
    nude::API::PreFrame(s);
    ImGui::PushFont(s.m_ui_font);

    file_browser.Display();

    if(file_browser.HasSelected())
    {
      std::cout << "Selected filename" << file_browser.GetSelected().string() << std::endl;
      s.m_zep_state->spEditor->InitWithFile(file_browser.GetSelected().string());
      file_browser.ClearSelected();
    }

    static bool show = true;
    ImGui::PushFont(s.m_source_code_font);
    // ImGui::SetNextWindowDockID(s.m_global_dockspace_id, ImGuiCond_Always);
    if (!ImGui::Begin("Zep", &show,
                          ImGuiWindowFlags_NoScrollbar |
                          ImGuiWindowFlags_NoTitleBar))
    {
      ImGui::End();
    }
    else {

      auto min = ImGui::GetCursorScreenPos();
      auto max = ImGui::GetContentRegionAvail();
      max.x = std::max(1.0f, max.x);
      max.y = std::max(1.0f, max.y);

      // Fill the window
      max.x = min.x + max.x;
      max.y = min.y + max.y;
      s.m_zep_state->spEditor->SetDisplayRegion(Zep::NVec2f(min.x, min.y),
                                                Zep::NVec2f(max.x, max.y));

      // Display the editor inside this window
      s.m_zep_state->spEditor->Display();
      s.m_zep_state->spEditor->HandleInput();

      bool zep_focused = ImGui::IsWindowFocused();
      if (zep_focused)
      {
        s.m_zep_state->spEditor->HandleInput();
      }

      ImGui::End();
    }
    ImGui::PopFont();
    ImGui::PopFont();
    nude::API::PostFrame(s);

  }

  // Cleanup
  nude::API::Quit(s);

  return 0;
}
