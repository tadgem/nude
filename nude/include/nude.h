#pragma once
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imfilebrowser.h"
#include "zep/filesystem.h"
#include "zep/imgui/display_imgui.h"
#include "zep/imgui/editor_imgui.h"
#include "zep/mcommon/animation/timer.h"
#include "zep/mode_repl.h"
#include "zep/mode_standard.h"
#include "zep/mode_vim.h"
#include "zep/regress.h"
#include "zep/tab_window.h"
#include "zep/theme.h"
#include "zep/window.h"
#include "clip.h"

namespace nude
{
  static Zep::NVec2f GetPixelScale()
  {
    float ddpi = 0.0f;
    float hdpi = 0.0f;
    float vdpi = 0.0f;

    auto window = SDL_GL_GetCurrentWindow();
    auto index = window ? SDL_GetDisplayForWindow(window) : 0;

    auto res = SDL_GetDisplayContentScale(index);

    if (res == 0 && hdpi != 0)
    {
      return Zep::NVec2f(hdpi, vdpi) / 96.0f;
    }
    return Zep::NVec2f(1.0f);
  }

  static float dpi_pixel_height_from_point_size(float pointSize, float pixelScaleY)
  {
    const auto fontDotsPerInch = 72.0f;
    auto inches = pointSize / fontDotsPerInch;
    return inches * (pixelScaleY * 96.0f);
  }

  class ZepContainer : public Zep::IZepComponent, public Zep::IZepReplProvider
  {
  public:
    const std::string shader = R"R(
      #version 330 core

      uniform mat4 Projection;

      // Coordinates  of the geometry
      layout(location = 0) in vec3 in_position;
      layout(location = 1) in vec2 in_tex_coord;
      layout(location = 2) in vec4 in_color;

      // Outputs to the pixel shader
      out vec2 frag_tex_coord;
      out vec4 frag_color;

      void main()
      {
          gl_Position = Projection * vec4(in_position.xyz, 1.0);
          frag_tex_coord = in_tex_coord;
          frag_color = in_color;
      }
      )R";

    ZepContainer(const std::string& startupFilePath, const std::string& configPath)
        : spEditor(std::make_unique<Zep::ZepEditor_ImGui>(configPath, GetPixelScale()))
    //, fileWatcher(spEditor->GetFileSystem().GetConfigPath(), std::chrono::seconds(2))
    {
      // janet_init("");
      // ZepEditor_ImGui will have created the fonts for us; but we need to build
      // the font atlas
      auto fontPath = std::string(SDL_GetBasePath()) + "Cousine-Regular.ttf";
      auto& display = static_cast<Zep::ZepDisplay_ImGui&>(spEditor->GetDisplay());
      const float DemoFontPtSize = 14.0f;
      int fontPixelHeight = (int)dpi_pixel_height_from_point_size(DemoFontPtSize, GetPixelScale().y);

      auto& io = ImGui::GetIO();
      ImVector<ImWchar> ranges;
      ImFontGlyphRangesBuilder builder;
      builder.AddRanges(io.Fonts->GetGlyphRangesDefault()); // Add one of the default ranges
      builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic()); // Add one of the default ranges
      builder.BuildRanges(&ranges); // Build the final result (ordered ranges with all the unique characters submitted)

      ImFontConfig cfg;
      cfg.OversampleH = 4;
      cfg.OversampleV = 4;

      // display.SetFont(Zep::ZepTextType::UI, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, fontPixelHeight));
      // display.SetFont(Zep::ZepTextType::Text, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, fontPixelHeight));
      // display.SetFont(Zep::ZepTextType::Heading1, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(fontPixelHeight * 1.75)));
      // display.SetFont(Zep::ZepTextType::Heading2, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(fontPixelHeight * 1.5)));
      // display.SetFont(Zep::ZepTextType::Heading3, std::make_shared<Zep::ZepFont_ImGui>(display, pImFont, int(fontPixelHeight * 1.25)));

      unsigned int flags = 0; // ImGuiFreeType::NoHinting;
      // ImGuiFreeType::BuildFontAtlas(ImGui::GetIO().Fonts, flags);

      spEditor->RegisterCallback(this);

      Zep::ZepRegressExCommand::Register(*spEditor);

      // Repl
      Zep::ZepReplExCommand::Register(*spEditor, this);
      Zep::ZepReplEvaluateOuterCommand::Register(*spEditor, this);
      Zep::ZepReplEvaluateInnerCommand::Register(*spEditor, this);
      Zep::ZepReplEvaluateCommand::Register(*spEditor, this);

      if (!startupFilePath.empty())
      {
        spEditor->InitWithFileOrDir(startupFilePath);
      }
      else
      {
        spEditor->InitWithText("Shader.vert", shader);
      }

      // File watcher not used on apple yet ; needs investigating as to why it doesn't compile/run
      // The watcher is being used currently to update the config path, but clients may want to do more interesting things
      // by setting up watches for the current dir, etc.
      /*fileWatcher.start([=](std::string path, FileStatus status) {
          if (spEditor)
          {
              ZLOG(DBG, "Config File Change: " << path);
              spEditor->OnFileChanged(spEditor->GetFileSystem().GetConfigPath() / path);
          }
      });*/
    }

    ~ZepContainer()
    {
    }

    void Destroy()
    {
      spEditor->UnRegisterCallback(this);
      spEditor.reset();
    }

    virtual std::string ReplParse(Zep::ZepBuffer& buffer, const Zep::GlyphIterator& cursorOffset, Zep::ReplParseType type) override
    {
      ZEP_UNUSED(cursorOffset);
      ZEP_UNUSED(type);

      Zep::GlyphRange range;
      if (type == Zep::ReplParseType::OuterExpression)
      {
        range = buffer.GetExpression(Zep::ExpressionType::Outer, cursorOffset, { '(' }, { ')' });
      }
      else if (type == Zep::ReplParseType::SubExpression)
      {
        range = buffer.GetExpression(Zep::ExpressionType::Inner, cursorOffset, { '(' }, { ')' });
      }
      else
      {
        range = Zep::GlyphRange(buffer.Begin(), buffer.End());
      }

      if (range.first >= range.second)
        return "<No Expression>";

      const auto& text = buffer.GetWorkingBuffer();
      auto eval = std::string(text.begin() + range.first.Index(), text.begin() + range.second.Index());

      // Flash the evaluated expression
      Zep::FlashType flashType = Zep::FlashType::Flash;
      float time = 1.0f;
      buffer.BeginFlash(time, flashType, range);

      auto ret = std::string();
      GetEditor().SetCommandText(ret);
      return ret;
    }

    virtual std::string ReplParse(const std::string& str) override
    {
//      Janet out;
//      auto ret = janet_run(str, "main", &out);
//      ret = RTrim(ret);
      std::string ret;
      return ret;
    }

    virtual bool ReplIsFormComplete(const std::string& str, int& indent) override
    {
      int count = 0;
      for (auto& ch : str)
      {
        if (ch == '(')
          count++;
        if (ch == ')')
          count--;
      }

      if (count < 0)
      {
        indent = -1;
        return false;
      }
      else if (count == 0)
      {
        return true;
      }

      int count2 = 0;
      indent = 1;
      for (auto& ch : str)
      {
        if (ch == '(')
          count2++;
        if (ch == ')')
          count2--;
        if (count2 == count)
        {
          break;
        }
        indent++;
      }
      return false;
    }

    std::string GetClipboardText()
    {
      // Try opening the clipboard
      if (! OpenClipboard(nullptr))
      {
        return std::string();
      }

      // Get handle of clipboard object for ANSI text
      HANDLE hData = GetClipboardData(CF_TEXT);
      if (hData == nullptr) {
        return std::string();
      }

      // Lock the handle to get the actual text pointer
      char * pszText = static_cast<char*>( GlobalLock(hData) );
      if (pszText == nullptr) {
      }

      std::string text( pszText );

      // Release the lock
      GlobalUnlock( hData );

      // Release the clipboard
      CloseClipboard();

      return text;
    }
    // Inherited via IZepComponent
    virtual void Notify(std::shared_ptr<Zep::ZepMessage> message) override
    {
      if (message->messageId == Zep::Msg::GetClipBoard)
      {
        clip::get_text(message->str);
        message->handled = true;
      }
      else if (message->messageId == Zep::Msg::SetClipBoard)
      {
        clip::set_text(message->str);
        message->handled = true;
      }
      else if (message->messageId == Zep::Msg::RequestQuit)
      {
        quit = true;
      }
      else if (message->messageId == Zep::Msg::ToolTip)
      {
         auto spTipMsg = std::static_pointer_cast<Zep::ToolTipMessage>(message);
        if (spTipMsg->location.Valid() && spTipMsg->pBuffer)
        {
          auto pSyntax = spTipMsg->pBuffer->GetSyntax();
          if (pSyntax)
          {
            if (pSyntax->GetSyntaxAt(spTipMsg->location).foreground == Zep::ThemeColor::Identifier)
            {
              auto spMarker = std::make_shared<Zep::RangeMarker>(*spTipMsg->pBuffer);
              spMarker->SetDescription("This is an identifier");
              spMarker->SetHighlightColor(Zep::ThemeColor::Identifier);
              spMarker->SetTextColor(Zep::ThemeColor::Text);
              spTipMsg->spMarker = spMarker;
              spTipMsg->handled = true;
            }
            else if (pSyntax->GetSyntaxAt(spTipMsg->location).foreground == Zep::ThemeColor::Keyword)
            {
              auto spMarker = std::make_shared<Zep::RangeMarker>(*spTipMsg->pBuffer);
              spMarker->SetDescription("This is a keyword");
              spMarker->SetHighlightColor(Zep::ThemeColor::Keyword);
              spMarker->SetTextColor(Zep::ThemeColor::Text);
              spTipMsg->spMarker = spMarker;
              spTipMsg->handled = true;
            }
          }
        }
      }
    }

    virtual Zep::ZepEditor& GetEditor() const override
    {
      return *spEditor;
    }

    virtual void HandleInput()
    {
      spEditor->HandleInput();
    }

    bool quit = false;
    std::unique_ptr<Zep::ZepEditor_ImGui> spEditor;


  };

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