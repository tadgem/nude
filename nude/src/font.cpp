#include "font.h"
#include "opensans.h"
#include "sourcecodepro.h"

void nude::font::InitBuiltInFonts(nude::API::State& s) {
  auto io = ImGui::GetIO();

  ImFontConfig font_cfg;
  font_cfg.FontDataOwnedByAtlas = false;

  s.m_ui_font = io.Fonts->AddFontFromMemoryTTF((void*)(&open_sans_ttf[0]),
                                               static_cast<int>(OPEN_SANS_TTF_SIZE),
                                               16.0f, &font_cfg);

  s.m_source_code_font = io.Fonts->AddFontFromMemoryTTF((void*)(&source_code_pro_ttf[0]),
                                                        static_cast<int>(source_code_pro_SIZE),
                                                        18.0f, &font_cfg);

}
