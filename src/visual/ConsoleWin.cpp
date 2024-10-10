/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <wx/wx.h>
#include <wx/dcclient.h> // needed for wxGTK >=2.5.x 

#include "visual/ConsoleWin.h"

#ifdef EVENTDEBUG
#include <iostream>
using namespace std;
#endif

#include "visual/UserThread.h"
#include "errno.h"

#include "defs.h"
#include "IceException.h"
#include "macro.h"    // for min/max templates
#include "strtool.h"
#include "visual/screen.h"

// font family (wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN)
//#define CONSOLEFONT wxMODERN

namespace ice
{
  //  wxPaintEvent p(wxID_ANY); !!!

  const std::string iceConsoleWin::StdCaption = "ICE Console Window";

  //  DEFINE_EVENT_TYPE (GET_CHAR)
  DEFINE_EVENT_TYPE(CURSOR_TIMER)
  DEFINE_EVENT_TYPE(OPEN_ALPHA)
  DEFINE_EVENT_TYPE(PUSH_ALPHA)
  DEFINE_EVENT_TYPE(POP_ALPHA)
  DEFINE_EVENT_TYPE(SHOW_ALPHA)
  DEFINE_EVENT_TYPE(FORCE_CURSOR)
  ;

  const int border = 2;

  const unsigned char MaxC = 255; // bright
  const unsigned char LowC = MaxC * 3 / 4; // not so bright
  const unsigned char BgC  = MaxC * 3 / 5; // Background-Intensity

  wxColour GlobalColorTable[]
  //           red  green   blue
    = {wxColor(0,     0,     0),     // 0 black
       wxColor(LowC,     0,     0),  // 1 dark red
       wxColor(0,  LowC,     0),     // 2 dark green
       wxColor(0,     0,  LowC),     // 3 dark blue
       wxColor(LowC,  LowC,     0),  // 4 dark yellow
       wxColor(0,  LowC,  LowC),     // 5 dark cyan
       wxColor(LowC,     0,  LowC),  // 6 dark magenta
       wxColor(LowC,  LowC,  LowC),  // 7 grey
       wxColor(0,     0,     0),     // 8 black (but a lighter shade of black ;-)
       wxColor(MaxC,    0,    0), // 9  red
       wxColor(0, MaxC,    0),    // 10 green
       wxColor(0,    0, MaxC),    // 11 blue
       wxColor(MaxC, MaxC,    0), // 12 yellow
       wxColor(0, MaxC, MaxC),    // 13 cyan
       wxColor(MaxC,    0, MaxC), // 14 magenta
       wxColor(MaxC, MaxC, MaxC), // 15 white
      };

  wxColour BgColorTable[]
  //          red green  blue
    = {wxColor(0,    0,    0),    // 0 black
       wxColor(BgC,    0,    0),  // 1 red
       wxColor(0,  BgC,    0),    // 2 green
       wxColor(0,    0,  BgC),    // 3 blue
       wxColor(BgC,  BgC,    0),  // 4 yellow
       wxColor(0,  BgC,  BgC),    // 5 cyan
       wxColor(BgC,    0,  BgC),  // 6 magenta
       wxColor(BgC,  BgC,  BgC),  // 7 grey
      };

  // these macros define ConsoleWin's event table
  BEGIN_EVENT_TABLE(iceConsoleWin, wxWindow)
    //    EVT_CUSTOM           (GET_CHAR,     wxID_ANY, iceConsoleWin::OnGetChar)
    EVT_COMMAND(wxID_ANY,   OPEN_ALPHA, iceConsoleWin::OnOpenAlpha)
    //    EVT_SET_FOCUS(iceConsoleWin::OnFocus)
    EVT_PAINT(iceConsoleWin::OnPaint)
    EVT_ERASE_BACKGROUND(iceConsoleWin::OnEraseBackground)
    EVT_KEY_DOWN(iceConsoleWin::OnKeyDown)
    EVT_CHAR(iceConsoleWin::OnChar)
    EVT_TIMER(CURSOR_TIMER,           iceConsoleWin::OnUpdateTimer)
    EVT_CUSTOM(SHOW_ALPHA,   wxID_ANY, iceConsoleWin::OnShowAlpha)
    EVT_CUSTOM(FORCE_CURSOR, wxID_ANY, iceConsoleWin::OnForceCursor)
  END_EVENT_TABLE()

  iceConsoleWin::iceConsoleWin(iceApp& App) : wxWindow(),
    ThreadCommHelper(),
    isOpen(false),
    ParentFrame(NULL),
    App(&App),
    ContentStack(),   // content stack empty at the beginning
    CurrContent(),
    UpdateTimer(this, CURSOR_TIMER),
    windowneedsrefresh(false),
    CursorState(0),
    LastCursorPos(0, 0),
    TextHeight(12)
  {
    // create the parent frame
    ParentFrame = new iceFrame(wxString(StdCaption.c_str(), wxConvLibc), // window title
                               wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
                               wxCAPTION | wxSYSTEM_MENU);  // window style

    // create the GUI window object
    wxWindow::Create(ParentFrame,   // pointer to parent window
                     wxID_ANY,      // WindowsID
                     wxDefaultPosition, // we don't care about window position
                     wxDefaultSize, // we don't know yet, it will be set later
                     wxWANTS_CHARS); // window style: we want to process the tab key

    // We need to know the text metrics (height and width of a single char)
    // for putting the current character at the right position and calculating
    // the window size

    int ExternalLeading, Descent;

    theFont = wxFont(wxFontInfo(TextHeight).Style(wxFONTSTYLE_NORMAL).Family(wxFONTFAMILY_TELETYPE));

    wxWindowDC WindowDC(this);

    // we have to select the font we will use
    WindowDC.SetFont(theFont);
    // WindowDC.SetFont(wxFont(wxFontInfo(TextHeight).Family(wxFONTFAMILY_MODERN)));

    WindowDC.GetTextExtent(wxString(wxT("M")),
                           &CharWidth, &CharHeight, &Descent, &ExternalLeading);

    CharHeight += ExternalLeading;

    // set the appropiate height and width of the Screen
    int xs = GetScreenSize().x * CharWidth + 2 * border;
    int ys = GetScreenSize().y * CharHeight + 2 * border;

    ParentFrame->SetClientSize(xs, ys);
    SetClientSize(xs, ys);

    ParentFrame->SetChildWindow(*this);
    ParentFrame->Show(false);
    Show(false);

    // initialize the text color values
    SetTextAttributes(TextAttribs::FgColorWhite | TextAttribs::BgColorBlue | TextAttribs::NotInverted | TextAttribs::NotHighlighted);

    // starting the update timer
    // 100 ms should be fast enough for interaction on Console
    UpdateTimer.Start(100, true);

    // get keyboard input ??
    //    SetFocus();
  }

  iceConsoleWin::~iceConsoleWin()
  {
    UpdateTimer.Stop();
  }

  void iceConsoleWin::Refresh()
  {
    windowneedsrefresh = true;
    //    cout << "R" ; cout.flush();
    //    wxWindow::Refresh();
  }

  iceFrame* iceConsoleWin::GetParentFrame()
  {
    return ParentFrame;
  }

#if 0
  void iceConsoleWin::OnGetChar(wxEvent& event)
  {
    if (!KeyBuffer.isEmpty())
      {
        WakeUpUserThread();
      }
    else
      {
        ShallProcessKeyEvents = true;
      }
  }
#endif

  void iceConsoleWin::DrawCursor()
  {
    if (IsShown())
      {
        wxClientDC DC(this);

        // set the font
        /*
          DC.SetFont(wxFont(TextHeight, // pointsize
          CONSOLEFONT, // font family(wxDEFAULT, wxDECORATIVE, wxROMAN, wxSCRIPT, wxSWISS, wxMODERN)
          wxNORMAL, // font style(wxNORMAL, wxSLANT or wxITALIC)
          wxNORMAL)); // font weight(wxNORMAL, wxLIGHT or wxBOLD)
        */
        // we set the background mode to transparent since the character
        // at the current position should not get overpainted
        DC.SetBackgroundMode(wxTRANSPARENT);

        // if the cursor should be visible at this time slice, it's color is set to the current foreground color of
        // the current cursor position(else it will be painted in the current background color and thus
        // be invisible). We also remember the this cursor position, because we need to delete
        // the cursor from there.
        if (CursorState > 6)
          {
            DC.SetTextForeground(GlobalColorTable[CurrContent[CurrContent.CursorPos].Attribs.FgColor]);
            LastCursorPos = CurrContent.CursorPos;
          }
        else
          {
            DC.SetTextForeground(GlobalColorTable[CurrContent[CurrContent.CursorPos].Attribs.BgColor]);
          }

        DC.DrawText(wxString(wxT('_')), // the text to draw
                    LastCursorPos.x * CharWidth + border, // the position
                    LastCursorPos.y * CharHeight + border);

      }
  }

  void iceConsoleWin::OnForceCursor(wxEvent& event)
  {
    CursorState = 6;
    // in any case the cursor will be painted
    DrawCursor();
  }

  void iceConsoleWin::OnUpdateTimer(wxTimerEvent& Event)
  {
#ifdef EVENTDEBUG
    cout << "CT ";
    cout.flush();
#endif
    CursorState = (CursorState + 1) % 10;
    DrawCursor();

    if (windowneedsrefresh)
      {
        windowneedsrefresh = false;
        wxWindow::Refresh();
      }
    // restart update timer
    UpdateTimer.Start(-1, true);
  }

  void iceConsoleWin::OnEraseBackground(wxEraseEvent& Event)
  {
    // do nothing(and thus prevent the background from being repainted,
    // at the moment we repaint the whole client area anyway)
  }

  void iceConsoleWin::OnPaint(wxPaintEvent& event)
  {
    windowneedsrefresh = false;
    wxPaintDC dc(this);
#ifdef EVENTDEBUG
    cout << "CP ";
    cout.flush();
#endif
    // set the font to the selected monospaced font
    dc.SetFont(theFont);
    dc.SetBackgroundMode(wxSOLID);

    // we go through the character field and paint each one
    // Since every character may have it's own back- and foreground
    // color we would have to change these values quite often. Instead
    // we use OldBgColor and OldFgColor to keep these changes at a minimum.

    int OldBgColor = -1, OldFgColor = -1;

    for (int y = 0; y < SCREEN_YS; y++)
      for (int x = 0; x < SCREEN_XS; x++)
        {
          // extract current back- and foreground color
          int CurrFgColor = CurrContent.characters[y][x].Attribs.FgColor;
          int CurrBgColor = CurrContent.characters[y][x].Attribs.BgColor;

          // update colors only if necessary
          if (OldBgColor != CurrBgColor)
            {
              OldBgColor = CurrBgColor;
              dc.SetTextBackground(BgColorTable[CurrBgColor]);
            }
          if (OldFgColor != CurrFgColor)
            {
              OldFgColor = CurrFgColor;
              dc.SetTextForeground(GlobalColorTable[CurrFgColor]);
            }

          // tricky: Make sure, that the background is drawn
          dc.DrawText(wxString(wxT("  ")), x * CharWidth + border, y * CharHeight + border);
          // Now the really wanted Char
#if wxUSE_UNICODE
          wxChar c = CurrContent.characters[y][x].Char;
#else
          wxChar c[7];// up to 6 characters if UTF8 is used
          int bytes = wctomb((char*)c, CurrContent.characters[y][x].Char);
          if (bytes > 0)
            {
              c[bytes] = 0;
            }
#endif
          dc.DrawText(wxString(c),                    // the text to draw
                      x * CharWidth + border, y * CharHeight + border); // the position
        }
  }

  int iceConsoleWin::GotoNextPos()
  {
    // the easiest case would be if we only had to move the cursor one position to the right
    // if that gets us out of the line then we must move to the next line, if we are already
    // in the last line then we have to scroll the whole contents one up
    CurrContent.CursorPos.x++;
    if (CurrContent.CursorPos.x == SCREEN_XS)
      {
        CurrContent.CursorPos.x = 0;
        CurrContent.CursorPos.y++;

        // now scroll if necessary
        if (CurrContent.CursorPos.y == SCREEN_YS)
          {
            for (int y = 0; y < SCREEN_YS - 1; y++)
              for (int x = 0; x < SCREEN_XS; x++)
                {
                  CurrContent.characters[y][x] = CurrContent.characters[y + 1][x];
                }

            // clear the last (newly appeared) line
            for (int x = 0; x < SCREEN_XS; x++)
              {
                CurrContent.characters[SCREEN_YS - 1][x].Char = ' ';
                CurrContent.characters[SCREEN_YS - 1][x].Attribs = CurrContent.AttribsCompressed;
              }

            // don't forget to move back to the last line!
            CurrContent.CursorPos.y--;
            return 1;
          }
      }
    return 0;
  }

  void iceConsoleWin::OnKeyDown(wxKeyEvent& Event)
  {
    // we don't want to process key-down or key-released events but char events;
    // to receive char events we have to call event.skip(),
    // so the system generates char events
    Event.Skip();
  }

  void iceConsoleWin::OnChar(wxKeyEvent& Event)
  {
    // cout << "OnChar()"<<endl;
#if wxUSE_UNICODE
    wchar_t wc = Event.GetKeyCode();
    KeyBuffer.Push(wc);
#else
    unsigned char kc = Event.GetKeyCode();
    KeyBuffer.Push(kc);
#endif
  }

  // formerly there was a function to create the console
  // window, even if it was already created. In that case the old
  // one got destroyed and a new one created, but the former code
  // wasn't thread-safe. Now the console window will be created just once(at
  // the start of the application), and it will be deleted only after
  // the user thread will have terminated. If the user "closes" the console window
  // it will disappear from the screen, but it won't be destroyed!
  void iceConsoleWin::OnOpenAlpha(wxCommandEvent& Event)
  {
    SetName(Event.GetString());
    ParentFrame->SetTitle(Event.GetString());
    ParentFrame->Show(true);
    Show(true);
  }

  // make the ConsoleWin invisible, but don't destroy it(see iceConsoleWin::Open)
  void iceConsoleWin::OnCloseAlpha(wxEvent& event)
  {
    ParentFrame->Show(false);
    Show(false);
  }

  iceConsoleWin::ConsoleWinContent::ConsoleWinContent()
    : CursorPos(0, 0)
  {
    Clear();
  }

  void iceConsoleWin::ConsoleWinContent::Clear()
  {
    // initialize the character array to blanks
    for (int y = 0; y < SCREEN_YS; y++)
      for (int x = 0; x < SCREEN_XS; x++)
        {
          characters[y][x].Char = wxT(' ');
          characters[y][x].Attribs = AttribsCompressed;
        }
  }

  void iceConsoleWin::OnShowAlpha(wxEvent& Event)
  {
    Show(true);
  }

  // caution: these function will be called from the user thread, so we have
  //          be careful! If these functions change any values of the console
  //          window ConsoleWin, this must be done such a fashion as not to
  //          cause any inconsistent states. Almost all the attributes of the
  //          ConsoleWin can't be changed by calling the corresponding
  //          SetAttribute function because this function may be preempted and
  //          leave the attribute in an inconsistent state(not to mention the
  //          fact, that the attribute may be read by the console window on
  //          event processing at the very same time).
  //          Thus most console window attributes must be
  //          changed by means of events, so that the changes can be processed
  //          by the main thread synchronously.

  void iceConsoleWin::Open(const std::string& windowname)
  {
    // if the ConsoleWin has already been in use, clear it's content
    Clear();

    // set the new caption in the main thread
    wxCommandEvent Event(OPEN_ALPHA);
    Event.SetString(wxString((const char*)windowname.c_str(), wxConvLibc));
    AddPendingEvent(Event);

    isOpen = true;
  }

  void iceConsoleWin::Alpha(int mode)
  {
    if (mode == ON)
      {
        wxMaximizeEvent MaximizeEvent;
        AddPendingEvent(MaximizeEvent);
      }
    else
      {
        wxIconizeEvent IconizeEvent;
        AddPendingEvent(IconizeEvent);
      }
  }

  void iceConsoleWin::ForceCursorDraw()
  {
    wxCommandEvent CommandEvent(FORCE_CURSOR);
    AddPendingEvent(CommandEvent);
  }

  void iceConsoleWin::PushContents()
  {
    ContentStack.push(CurrContent);
  }

  void iceConsoleWin::PopContents()
  {
    // if the content stack is empty then simply ignore this command
    if (!ContentStack.empty())
      {
        CurrContent = ContentStack.top();
        ContentStack.pop();

        // we have to tell the ConsoleWin to repaint
        Refresh();
      }
  }

  void iceConsoleWin::Clear()
  {
    CurrContent.Clear();
    CurrContent.CursorPos = iceCursor(0, 0);
  }

  void iceConsoleWin::SetTextAttributes(TextAttribs Attribs)
  {
    // change the values the user wanted change
    CurrContent.TextAttributes.Apply(Attribs);

    // compute the compact form, find out if the text should be displayed inverted
    if (CurrContent.TextAttributes.getInvert() != TextAttribs::Inverted)
      {
        CurrContent.AttribsCompressed.FgColor = (unsigned char)(CurrContent.TextAttributes.getForeground() & 0x00E) >> 1;
        CurrContent.AttribsCompressed.BgColor = (unsigned char)(CurrContent.TextAttributes.getBackground() & 0x0E0) >> 5;
      }
    else
      {
        CurrContent.AttribsCompressed.FgColor = (unsigned char)(CurrContent.TextAttributes.getBackground() & 0x0E0) >> 5;
        CurrContent.AttribsCompressed.BgColor = (unsigned char)(CurrContent.TextAttributes.getForeground() & 0x00E) >> 1;
      }

    // if the text should be displayed highlighted, choose brighter colors
    if (CurrContent.TextAttributes.getHighlight() == TextAttribs::Highlighted)
      {
        CurrContent.AttribsCompressed.FgColor += 8;
      }
  }

  void iceConsoleWin::SetAttribute(int FgColor, int BgColor, int Inv, int High)
  {
    // Change the parameters and forward them to SetTextAttribs()
    TextAttribs TA(0);
    switch (FgColor)
      {
      case C_BLACK:
        TA.setForeground(TextAttribs::FgColorBlack);
        break;
      case C_RED:
        TA.setForeground(TextAttribs::FgColorRed);
        break;
      case C_GREEN:
        TA.setForeground(TextAttribs::FgColorGreen);
        break;
      case C_BLUE:
        TA.setForeground(TextAttribs::FgColorBlue);
        break;
      case C_YELLOW:
        TA.setForeground(TextAttribs::FgColorYellow);
        break;
      case C_CYAN:
        TA.setForeground(TextAttribs::FgColorCyan);
        break;
      case C_MAGENTA:
        TA.setForeground(TextAttribs::FgColorMagenta);
        break;
      case C_WHITE:
        TA.setForeground(TextAttribs::FgColorWhite);
        break;
      }

    switch (BgColor)
      {
      case C_BLACK:
        TA.setBackground(TextAttribs::BgColorBlack);
        break;
      case C_RED:
        TA.setBackground(TextAttribs::BgColorRed);
        break;
      case C_GREEN:
        TA.setBackground(TextAttribs::BgColorGreen);
        break;
      case C_BLUE:
        TA.setBackground(TextAttribs::BgColorBlue);
        break;
      case C_YELLOW:
        TA.setBackground(TextAttribs::BgColorYellow);
        break;
      case C_CYAN:
        TA.setBackground(TextAttribs::BgColorCyan);
        break;
      case C_MAGENTA:
        TA.setBackground(TextAttribs::BgColorMagenta);
        break;
      case C_WHITE:
        TA.setBackground(TextAttribs::BgColorWhite);
        break;
      }

    if (Inv == 1)
      {
        TA.setInvert(TextAttribs::Inverted);
      }
    else if (Inv == 0)
      {
        TA.setInvert(TextAttribs::NotInverted);
      }

    if (High == 1)
      {
        TA.setHighlight(TextAttribs::Highlighted);
      }
    else if (High == 0)
      {
        TA.setHighlight(TextAttribs::NotHighlighted);
      }

    SetTextAttributes(TA);
  }

  TextAttribs iceConsoleWin::GetTextAttributes() const
  {
    return CurrContent.TextAttributes;
  }

  void iceConsoleWin::GetAttribute(int& FgColor, int& BgColor, int& Inv, int& High) const
  {
    FgColor =  CurrContent.TextAttributes.getForeground() >> 1;
    BgColor =  CurrContent.TextAttributes.getBackground() >> 5;
    High =  CurrContent.TextAttributes.getHighlight() == TextAttribs::Highlight ? 1 : 0 ;
    Inv =  CurrContent.TextAttributes.getInvert() == TextAttribs::Inverted ? 1 : 0 ;
  }

  int iceConsoleWin::InsertWChar(wchar_t c)
  {
    if (!isOpen)
      {
        Open();
      }

    // Basically we only have to insert the character into our buffer,
    // but we also have to deal with an awefull lot of escape sequences.
    // Also we want to keep track of the number of lines that had to be scrolled
    //(here this could be at most one since we only display a single character)

    int ScrolledLines = 0;
    switch (c)
      {
      case '\f':   // a.k.a. form feed(FF)
        // clear the screen and go to the upper left corner
        Clear();
        CurrContent.CursorPos.x = 0;
        CurrContent.CursorPos.y = 0;
        break;

      case '\n':  // a.k.a. newline(NL) or line feed(LF)
      case '\r':  // a.k.a. carriage return(CR)
        // we simply set the cursor at the last position of the line and
        // let nextpos() figure out if we need to scroll
        CurrContent.CursorPos.x = SCREEN_XS - 1;
        ScrolledLines = GotoNextPos();
        break;

      case 0x10:
        // Clear End of Line CLREOL
        // fill the rest of the line with blanks
        // dont move cursor
      {
        int i;
        for (i = CurrContent.CursorPos.x; i < SCREEN_XS; i++)
          {
            CurrContent.characters[CurrContent.CursorPos.y][i].Char = ' ';
            CurrContent.characters[CurrContent.CursorPos.y][i].Attribs = CurrContent.AttribsCompressed;
          }
      }
      break;

      case '\t': // a.k.a. horizontal tab(HT)
        // we go to the next multiple of 8 in the current line but at least 1 character.
        ScrolledLines = PutChar(' ');
        while (CurrContent.CursorPos.x % 8 != 1)
          {
            ScrolledLines += PutChar(' ');
          }
        break;

      case '\b': // a.k.a. backspace(BS)
        // move the cursor one back if possible
        CurrContent.CursorPos.x = max(CurrContent.CursorPos.x - 1, 0);
        CurrContent[CurrContent.CursorPos].Char = ' ';
        break;

      default: // the rest should be only printable characters
        CurrContent[CurrContent.CursorPos].Char = c;
        CurrContent[CurrContent.CursorPos].Attribs = CurrContent.AttribsCompressed;
        ScrolledLines = GotoNextPos();
        break;
      }
    return ScrolledLines;
  }

  int iceConsoleWin::InsertChar(char c, bool& needRefresh)
  {
    // expects characters
    // converts to unicode wchar_t
    static mbstate_t ps;
    size_t rc;
    char cs[2];
    wchar_t wc;

    needRefresh = false;
    cs[0] = c;
    cs[1] = 0;
    rc = mbrtowc(&wc, cs, 1, &ps);

    if (rc == (size_t) - 2)   // incomplete multibytesequence
      {
        return 0;
      }

    if (rc == (size_t) - 1)   // illegal sequence
      {
        cs[0] = 0;
        mbrtowc(&wc, cs, 1, &ps); // reset machine (is this correct ?)
        return 0;
      }

    // sequence complete
    needRefresh = true;
    // send complete wide character to Screen
    return InsertWChar(wc);
  }

  int iceConsoleWin::PutChar(int c)
  {
    bool needRefresh;
    // All the work is done in InsertChar(that is used by Printf as well).
    int ScrolledLines = InsertChar(c, needRefresh);

    // Since we changed the contents of the console window we have to repaint.
    if (needRefresh)
      {
        Refresh();
      }

    return ScrolledLines;
  }

  int iceConsoleWin::Print(const std::string& s)
  {
    // ScrolledLines hold the overall value of the lines that
    // the contents of the console  window had to be scrolled

    return Print(WString(s));
  }

  int iceConsoleWin::Print(const std::wstring& s)
  {
    // ScrolledLines hold the overall value of the lines that
    // the contents of the console  window had to be scrolled

    int ScrolledLines = 0;
    for (unsigned int i = 0; i < s.length(); i++)
      {
        ScrolledLines += InsertWChar(s[i]);
      }
    // tell the window to repaint
    Refresh();

    return ScrolledLines;
  }

  void iceConsoleWin::WaitWhileKeybufferEmpty()
  {
    // wait while keyboard buffer is empty
    //    cout << "?"; cout.flush();
    while (KeyBuffer.isEmpty())
      {
        App->GetUserThread()->Sleep(100); // we use sleep from wxWidgets for compatibility
        //      cout << "W"; cout.flush();
      }
  }

  int iceConsoleWin::GetChar()
  {
    // open Alphanumerical terminal if neccessary
    if (!isOpen)
      {
        Open();
      }

    //    SetFocus();
    // wait while keyboard buffer is empty
    WaitWhileKeybufferEmpty();

    return KeyBuffer.popC();
  }

  int iceConsoleWin::GetKey()
  {
    if (!isOpen)
      {
        Open();
      }
    if (KeyBuffer.isEmpty())
      {
#ifdef WIN32
        Sleep(1);
#else
        usleep(10000); // avoid high load if polling
#endif
        return 0;
      }
    return KeyBuffer.popC();
  };

  int iceConsoleWin::GetCharW()
  {
    // open Alphanumerical terminal if neccessary
    if (!isOpen)
      {
        Open();
      }

    //    SetFocus();
    // wait while key buffer is empty
    WaitWhileKeybufferEmpty();

    return KeyBuffer.pop();
  }

  int iceConsoleWin::GetKeyW()
  {
    if (!isOpen)
      {
        Open();
      }
    if (KeyBuffer.isEmpty())
      {
#ifdef WIN32
        Sleep(1);
#else
        usleep(10000); // avoid high load if polling
#endif
        return 0;
      }
    return KeyBuffer.pop();
  };

  int iceConsoleWin::InputString(string& str,
                                 bool control,
                                 const string& def,
                                 int& lastkey)
  {
    //    SetFocus();
    // ScrolledLines hold the overall value of the lines that the contents
    // of the console window had to be scrolled
    int ScrolledLines = 0;

    // Save the current cursor position as FirstCursorPos(the user should
    // not be able to go back beyond this position). We also store the
    // rightmost position the user has reached(and should not be able to
    // go over with the arrow keys). These two values denote the beginning
    // and the end of the string that will be returned as parameter
    // >DestBuffer<
    iceCursor FirstCursorPos = GetCursorPos();

    wstring ibuffer;
    wstring::size_type ipos = 0;

    // if there is a default value we will print this first
    if (!def.empty())
      {
        ibuffer = WString(def);
        ipos = ibuffer.length();
        ScrolledLines = Print(def);
        FirstCursorPos -= ScrolledLines * SCREEN_XS;
      }

    iceCursor LastCursorPos = GetCursorPos();

    // Read the keyboard until the user presses Enter
    // or another control key if enabled

    bool ready = false;
    while (!ready)
      {
        for (iceCursor Cursor = FirstCursorPos;
             Cursor < LastCursorPos;
             Cursor++)
          {
            CurrContent[Cursor].Char = ' ';
          }

        SetCursorPos(FirstCursorPos);
        Print(ibuffer.substr(0, ipos));
        iceCursor ac = GetCursorPos();
        Print(ibuffer.substr(ipos));
        LastCursorPos = GetCursorPos();
        SetCursorPos(ac);

        int c = GetCharW();
        lastkey = c;
        switch (c)
          {
          // control key that may lead to abort
          case WXK_ESCAPE:
          case WXK_DOWN:
          case WXK_UP:
            if (!control)
              {
                break;  // do nothing if control = false
              }
          // in case control is true -> abort

          case WXK_RETURN:
            ready = true;
            break;

          case WXK_DELETE:
          {
            // if we are at the end of the input nothing happens(except for a bell)
            if (ipos == ibuffer.length())
              {
                ::wxBell();
                break;
              }

            // deleting the current character
            ibuffer.erase(ibuffer.begin() + ipos);

            LastCursorPos--;
            Refresh();
          }
          break;

          case WXK_END:
            ipos = ibuffer.length();
            break;

          case WXK_HOME:
            ipos = 0;
            break;

          case WXK_LEFT:
          {
            if (ipos > 0)
              {
                ipos--;
              }
          }
          break;

          case WXK_RIGHT:
          {
            if (ipos < ibuffer.length())
              {
                ipos++;
              }
          }
          break;

          case WXK_BACK:   // the backspace key
          {
            if (ipos == 0)
              {
                break;
              }
            ipos--;
            ibuffer.erase(ibuffer.begin() + ipos);
          }
          break;

          default:
            if (ipos == ibuffer.length())
              {
                ibuffer += c;
                ipos++;
              }
            else
              {
                ibuffer.insert(ibuffer.begin() + ipos, c);
                ipos++;
              }
            break;
          } // switch(c)

        // Since the cursor position has very likely changed, it would be bad if
        // the cursor is currently invisible.
        ForceCursorDraw();

      } // while (!ready)

    str = String(ibuffer);

    return ScrolledLines;
  }

  bool iceConsoleWin::Destroy()
  {
    ParentFrame->Destroy();
    return wxWindow::Destroy();
  }

  /*
   *
   *   inner class definitions
   *
   */

  /*
   *   iceKeyBuffer
   */

  wchar_t iceConsoleWin::iceKeyBuffer::pop()
  {
    // lock the mutex with the help of a mutex locker(that releases the mutex
    // automatically when this method is left)
    wxMutexLocker Locker(Mutex);

    if (!charbuffer.empty())
      {
        wchar_t res = charbuffer.back();
        charbuffer.pop_back();
        return res;
      }

    if (!widebuffer.empty())
      {
        wchar_t res = widebuffer.back();
        widebuffer.pop_back();
        return res;
      }

    return 0;
  }

  int iceConsoleWin::iceKeyBuffer::popC()
  {
    // lock the mutex with the help of a mutex locker(that releases the mutex
    // automatically when this method is left)
    wxMutexLocker Locker(Mutex);

    unsigned char res = 0;
    if (!charbuffer.empty())
      {
        res = charbuffer.back();
        charbuffer.pop_back();
        return res;
      }

    if (!widebuffer.empty())
      {
        wchar_t wChar = widebuffer.back();
        widebuffer.pop_back();

        // we need a null terminated buffer for conversion
        wchar_t wBuffer[2];
        wBuffer[0] = wChar;
        wBuffer[1] = 0;

        // a pointer to the first char
        wchar_t* pt = wBuffer;

        char c[MB_CUR_MAX];
        wctomb(NULL, *pt);
        int length = wctomb(c, *pt);

        if (length > 0)
          {
            res = c[0]; // return first byte directly
            // store following bytes in fifo
            for (int i = 1; i < length; ++i)
              charbuffer.push_front(c[i]);

            return res;
          }
        else
          return wChar;
      }

    return 0;
  }

  void iceConsoleWin::iceKeyBuffer::Push(wchar_t KeyCode)
  {
    // lock the mutex with the help of a mutex locker(that releases the mutex
    // automatically when this method is left)
    wxMutexLocker Locker(Mutex);
    widebuffer.push_front(KeyCode);
  }

  void iceConsoleWin::iceKeyBuffer::Push(unsigned char KeyCode)
  {
    // lock the mutex with the help of a mutex locker(that releases the mutex
    // automatically when this method is left)
    wxMutexLocker Locker(Mutex);
    charbuffer.push_front(KeyCode);
  }

  void iceConsoleWin::iceKeyBuffer::Flush()
  {
    wxMutexLocker Locker(Mutex);
    widebuffer.clear();
    charbuffer.clear();
  };
} // namespace ice
