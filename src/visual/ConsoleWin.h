/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ICECONSOLEWIN_H
#define __ICECONSOLEWIN_H

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

#include <wchar.h>
#include <string>
#include <deque>
#include <stack>

#include <wx/wx.h>
#include <wx/frame.h>   // iceConsoleWin inherits from wxFrame
#include <wx/gdicmn.h>  // GDI common stuff; we need only wxPoint
#include <wx/timer.h>   // for the RefreshTimer member

#include "Window.h"
#include "visual/App.h"
#include "visual/ThreadCommHelper.h"
#include "visual/ConsoleCursor.h"
#include "visual/attribute.h"
#include "visual/Frame.h"

// SCREEN_XS and SCREEN_YS denote the width and height (in characters) of the contents
// of a console window

#define SCREEN_XS 80
#define SCREEN_YS 25

namespace ice
{
  // A console like window for text input and output.
  class iceConsoleWin : public wxWindow, public ThreadCommHelper
  {

  protected:
    static const std::string StdCaption;

  public:
    bool isOpen;

    iceConsoleWin(iceApp& App);
    ~iceConsoleWin();

    iceFrame* GetParentFrame();

    // A nice gadget for the cursor inside a console window.
    /* We can use such a cursor for loops that start at a
       certain cursor position and end at a later cursor position. Some operators
       are provided to move the cursor forward and backward, according to the width
       and height of the console window.
       <BR>Cursor positions can also be compared, that means a cursor position is
       considered greater than another if the user can move from the first to the second
       position by typing some keys. For example the cursor position (20, 1) is less than
       (21, 1), as well as less than (1, 2). (20, 1) denotes the 20th position in the first
       row.*/

    // Returns the current position of the text cursor in the screen.
    iceCursor GetCursorPos() const;
    void SetCursorPos(const iceCursor& CursorPos);

    // Returns the current screen size in characters.
    IPoint GetScreenSize() const;

    /* Prints the character >c< at the current cursor position and does all the
       line-breaking and scrolling stuff if nessecary.
       \return Returns the number of lines that had to be scrolled in order to print
       this character(well, at most one).*/
    int PutChar(int c);

    // get next char from input (blocking)
    int GetChar();
    int GetCharW();

    // get next char from input (nonblocking)
    int GetKey();
    int GetKeyW();

  private:
    void WaitWhileKeybufferEmpty();

  public:
    /* This method works just like the stdio.h printf function, except for the return value.
       <BR>CAUTION: Since we use sprintf inside there is a limit to the number of characters that
       can be printed in one invokation, e.g. 400. Make sure that the output string
       won't exceed this number, else it will be clipped!
       \return Returns how many lines the contents of the console window had to be scrolled
       in order to execute this command.*/

    int Print(const std::string& s);
    int Print(const std::wstring& s);

    // Input of string
    int InputString(std::string& str, bool control,
                    const std::string& def, int& lastkey);

    // Saves the current content and status of the ConsoleWin
    // on an internal stack.
    void PushContents();

    // Restores the last saved content of the ConsoleWin, including cursor position,
    // from the internal stack. This method will do nothing if the stack is empty.
    void PopContents();

    // Minimized or maximizes the console window depending on mode.
    /* Minimized or maximizes the console window depending on mode.
       \param mode ON maximizes the window
       OFF minimizes the window*/
    void Alpha(int mode = ON);

    // OpenAlpha shows the window and sets it's caption to >windowname<
    // (if it is already open OpenAlpha just changes it's caption).
    void Open(const std::string& windowname = StdCaption);

    // override refresh from wxWindow
    // organizes timer controlled update
    virtual void Refresh();

    // Sets the text attributes.
    void SetAttribute(int FgColor, int BgColor, int Inv, int High);
    // Gets the text attributes.
    void GetAttribute(int& FgColor, int& BgColor, int& Inv, int& High) const;

    // Sets the text attributes, but with more comfortable parameter names.
    void SetTextAttributes(TextAttribs TextAttribs);
    TextAttribs GetTextAttributes() const;

    // Clears the entire screen with the current background color.
    void Clear();

    virtual bool Destroy();

  protected:
    // list of event handlers (and helpers)
    void OnGetChar(wxEvent& event);

    void OnOpenAlpha(wxCommandEvent& event);
    void OnCloseAlpha(wxEvent& event);


    void OnShowAlpha(wxEvent& event);
    void OnKeyDown(wxKeyEvent& Event);
    void OnChar(wxKeyEvent& Event);
    //      void OnFocus(wxFocusEvent& Event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& Event);

    void DrawCursor();
    void OnForceCursor(wxEvent& event);
    void OnUpdateTimer(wxTimerEvent& Event);

    // The parent frame, that hosts this window as a child.
    iceFrame* ParentFrame;

    // We hold a pointer to the main app.
    iceApp* App;

    // ConsoleWinContent is a collection of all the data that is needed for
    // saving and restoring of the contents of the console window;
    struct ConsoleWinContent
    {
      // Default constructor.
      ConsoleWinContent();

      // The current position of the text cursor; at this place in the ConsoleWin the
      // next character from PutChar () will appear
      iceCursor CursorPos;

      // The current text attributes.
      TextAttribs TextAttributes;

      // The format for storing TextAttributes in a compressed way. The text attributes
      // have to be stored for each console window entry, so it should consume as little memory
      // as possible.
      struct TextAttribsCompressed
      {
        unsigned char FgColor: 4, BgColor: 3;
      };

      // The current text attributes in a compressed format.
      TextAttribsCompressed AttribsCompressed;

      // The text that will be displayed in the ConsoleWin. Beside the actual character we
      // also store the text attribute for the character.
      struct Entry
      {
        wchar_t Char;
        TextAttribsCompressed Attribs;
      } characters [SCREEN_YS][SCREEN_XS];

      // Fills the screen with the current background color.
      void Clear();

      // A more convenient way of accessing a single point.
      Entry& operator[](const wxPoint& Position);
    };

    // The internal stack upon which the current contents of the console window can be saved
    // for later reuse.
    std::stack<ConsoleWinContent> ContentStack;

    // The current window contents.
    ConsoleWinContent CurrContent;

    // This flag indicates whether we should respond to keyboard events. If it is set to
    // true, we store the last typed key at LastTypedKey.
    //      bool ShallProcessKeyEvents;

    // We use this timer to display the keyboard cursor. The cursor should blink, so it
    // must repeatedly painted and erased. This timer gives us the corresponding impulses.
    wxTimer UpdateTimer;

    bool windowneedsrefresh;

    // CursorState tells us current stat of cursor.
    // This flag will be evaluated in the event handler method for displaying the cursor.
    int CursorState;

    // LastCursorPos tells us where we painted the cursor last time.
    // Since the cursor can be changed from inside the user thread,
    // we must remember the cursor position, where the cursor was
    // painted the last time.
    iceCursor LastCursorPos;

    // If the user changed the current cursor position it would be nice if we
    // displayed the cursor, even if it should be invisible at the moment. This
    // should provide better orientation for the user. This
    // method will ensure that the cursor will be visible for the rest of the cursor
    // time slice (that means, until the next cursor timer event occurs).
    void ForceCursorDraw();

    // Like PutChar, but it doesn't tell the console window to update.
    // Thus we can use it in Printf as well, without the console
    // window updating after each printed character.
    int InsertWChar(wchar_t c);
    int InsertChar(char c, bool& needRefresh);

    // Determines the next Position of the cursor; if necessary it
    // will scroll the contents of the window.
    // returns The number lines that the window had to be scrolled
    // in order to go to the next cursor position.
    int GotoNextPos();

    // The height of the space that will be needed to paint a single character in
    // the console window.
    int CharHeight;
    // The width of the space that will be needed to paint a single character in
    // the console window.
    int CharWidth;

    // The text point size. Maybe this value should be configurable.
    const unsigned char TextHeight;

    // This class implements a thread-safe key buffer.
    class iceKeyBuffer
    {
    private:
      // The buffer itself.
      // Buffer for incoming wide chars
      std::deque<wchar_t> widebuffer;
      // Buffer for remaining chars
      // after wchar to char translation
      std::deque<unsigned char> charbuffer;
    public:
      // Get the latest key press. If there was no key pressed, this method return zero.
      bool isEmpty()
      {
        return charbuffer.empty() && widebuffer.empty();
      }

      // get one wchar from fifo
      wchar_t pop();

      // get one char from fifos
      // "char" is part of multibyte sequence (returned as int)
      // special control character (value > 255)
      int popC();

      // Store key press.
      void Push(wchar_t KeyCode);
      void Push(unsigned char KeyCode);

      // Flush the key buffer.
      void Flush();

    private:
      // The mutex to ensure that Push and Pop (or Flush) can't be invoked at the same time.
      wxMutex Mutex;
    };

    // The buffer that stores the pending keypresses.
    iceKeyBuffer KeyBuffer;

    // this macro declares that ConsoleWin has an event table that maps events to
    // event handler functions; the actual event table will be declared outside this
    // class declaration in the cpp file
    DECLARE_EVENT_TABLE()
  };


  //////////////////////////////////////////////////////////////////////////
  //
  //   inline implementations
  //

  inline iceCursor iceConsoleWin::GetCursorPos() const
  {
    return CurrContent.CursorPos;
  }

  inline void iceConsoleWin::SetCursorPos(const iceCursor& CursorPos)
  {
    CurrContent.CursorPos = CursorPos;
  }

  inline IPoint iceConsoleWin::GetScreenSize() const
  {
    return IPoint(SCREEN_XS, SCREEN_YS);
  }

  inline iceConsoleWin::ConsoleWinContent::Entry& iceConsoleWin::ConsoleWinContent::operator[](const wxPoint& Position)
  {
    return characters[Position.y][Position.x];
  }
}
#endif // define __ICECONSOLEWIN_H
