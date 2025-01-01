#include "nude.h"
#include <stdio.h>

// Main code
int main(int, char**)
{
  nude::API::State s = nude::API::Init();
  // Our state

  while (!s.m_quit)
  {
    nude::API::PreFrame(s);

    auto min = ImGui::GetCursorScreenPos();
    auto max = ImGui::GetContentRegionAvail();
    max.x = std::max(1.0f, max.x);
    max.y = std::max(1.0f, max.y);

    // Fill the window
    max.x = min.x + max.x;
    max.y = min.y + max.y;
    s.m_zep_state->spEditor->SetDisplayRegion(Zep::NVec2f(min.x, min.y), Zep::NVec2f(max.x, max.y));

    // Display the editor inside this window
    s.m_zep_state->spEditor->Display();
    s.m_zep_state->spEditor->HandleInput();

    nude::API::PostFrame(s);

  }
#ifdef __EMSCRIPTEN__
  EMSCRIPTEN_MAINLOOP_END;
#endif

  // Cleanup
  nude::API::Quit(s);

  return 0;
}
