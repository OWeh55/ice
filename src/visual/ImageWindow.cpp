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

#include <wx/image.h>       // for class wxImage
#include <wx/dcbuffer.h>    // needed for wxGTK >= 2.5.x

#include "visual/ImageWindow.h"
#include "visual/Interaction.h"
#include "visual/SelPntInteraction.h"
#include "visual/SelLineInteraction.h"
#include "visual/SelRectInteraction.h"

#include "visual/App.h"         // for wxGetApp
#include "visual/ConsoleWin.h"  // for passing through key events to the console window
#include "visual/ImageMgr.h"

#include <iostream>
using namespace std;

#include "macro.h"          // for min/max templates
#include "visual/mouse.h"

namespace ice
{
  DEFINE_EVENT_TYPE(ZOOM)
  DEFINE_EVENT_TYPE(INIT_INTERACTION)
  DEFINE_EVENT_TYPE(CLEANUP_INTERACTION)
  DEFINE_EVENT_TYPE(CURSOR_ENABLE)
  DEFINE_EVENT_TYPE(CURSOR_UPDATE)

  BEGIN_EVENT_TABLE(ImageWindow, wxScrolledWindow)
    EVT_CHAR(ImageWindow::OnChar)
    EVT_CUSTOM(INIT_INTERACTION,    wxID_ANY, ImageWindow::OnInitInteraction)
    EVT_CUSTOM(CLEANUP_INTERACTION, wxID_ANY, ImageWindow::OnCleanupInteraction)
    EVT_COMMAND(wxID_ANY, ZOOM,                ImageWindow::OnZoom)

    EVT_COMMAND(wxID_ANY, CURSOR_ENABLE,       ImageWindow::OnCursorEnable)
    EVT_COMMAND(wxID_ANY, CURSOR_UPDATE,       ImageWindow::OnCursorUpdate)
#if 0
    EVT_MOTION(ImageWindow::OnMove)
    EVT_LEFT_DOWN(ImageWindow::OnLeftDown)
    EVT_LEFT_UP(ImageWindow::OnLeftUp)
    EVT_MIDDLE_DOWN(ImageWindow::OnMiddleDown)
    EVT_MIDDLE_UP(ImageWindow::OnMiddleUp)
    EVT_RIGHT_DOWN(ImageWindow::OnRightDown)
    EVT_RIGHT_UP(ImageWindow::OnRightUp)
    EVT_MOTION(ImageWindow::OnMotion)
    EVT_ENTER_WINDOW(ImageWindow::OnEnterWindow)
    EVT_LEAVE_WINDOW(ImageWindow::OnLeaveWindow)
#endif
    EVT_MOUSE_EVENTS(ImageWindow::OnMouseEvents)
    EVT_ERASE_BACKGROUND(ImageWindow::OnEraseBackground)
  END_EVENT_TABLE()

  ;;

  ImageWindow::ImageWindow(ImageBase* img,
                           const std::string& windowname):
    wxScrolledWindow(),
    Caption(windowname),
    ParentFrame(NULL),
    SizeX(img->xsize),
    SizeY(img->ysize),
    ZoomFactor(1),
    PixelSize(1),
    ZoomStep(1),
    PaintIsRunning(false),
    Interaction(NULL),
    CursorPosition(0, 0),
    CursorIsEnabled(false),
    NeedsCreation(false),
    isFullScreen(false)
  {
    Create(SizeX, SizeY);
    addImage(img);
  }

  ImageWindow::ImageWindow(ImageD* img,
                           const std::string& windowname):
    wxScrolledWindow(),
    Caption(windowname),
    ParentFrame(NULL),
    SizeX(img->xsize),
    SizeY(img->ysize),
    ZoomFactor(1),
    PixelSize(1),
    ZoomStep(1),
    PaintIsRunning(false),
    Interaction(NULL),
    CursorPosition(0, 0),
    CursorIsEnabled(false),
    NeedsCreation(false),
    isFullScreen(false)
  {
    Create(SizeX, SizeY);
    addImage(img);
  }

  bool ImageWindow::Create(unsigned int sizeX, unsigned int sizeY)
  {
    // create the parent frame
    ParentFrame = new iceFrame(wxString(Caption.c_str(), wxConvLibc), // window title
                               wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCAPTION |
                               wxSYSTEM_MENU | wxRESIZE_BORDER); // window style

    ParentFrame->Show(true);

    // create the GUI window object
    bool RetVal = wxScrolledWindow::Create(ParentFrame, // parent window
                                           wxID_ANY, // WindowsID
                                           wxDefaultPosition, // window position
                                           wxSize(getVirtualSizeX(),
                                               getVirtualSizeY())); // window size

    ParentFrame->SetChildWindow(*this);

    // set the appropiate height and width
    ParentFrame->SetClientSize(
      std::min(wxGetApp().DefXSize(), getVirtualSizeX()),
      std::min(wxGetApp().DefYSize(), getVirtualSizeY()));

    Show(true);
    SetFocus();

    SetVirtualSize(getVirtualSizeX(), getVirtualSizeY());
    // setting up the scrollbar
    SetScrollRate(SCROLLUNIT, SCROLLUNIT); // int pixelsPerUnit

    NeedsCreation = false;
    return RetVal;
  }

  bool ImageWindow::Destroy()
  {
    while (PaintIsRunning) usleep(1000);

    ParentFrame->Destroy();
    return wxScrolledWindow::Destroy();
  }

  bool ImageWindow::addImage(ImageBase* img)
  {
    if ((img->xsize != SizeX) || (img->ysize != SizeY))
      {
        return false;
      }

    imgs.push_back(new ImageStructInt(img));

    return true;
  }

  bool ImageWindow::addImage(ImageD* img)
  {
    if ((img->xsize != SizeX) || (img->ysize != SizeY))
      {
        return false;
      }
    imgs.push_back(new ImageStructDouble(img));

    return true;
  }

  // Does the Window display the given image?
  bool ImageWindow::showsImage(const ImageBase* img) const
  {
    bool isShown = false;
    for (unsigned int i = 0; i < imgs.size() && ! isShown; i++)
      {
        if (imgs[i]->ImageType() == 1)
          {
            isShown = ((ImageStructInt*)imgs[i])->Img() == img;
          }
      }
    return isShown;
  }

  bool ImageWindow::showsImage(const ImageD* img) const
  {
    bool isShown = false;
    for (unsigned int i = 0; i < imgs.size() && ! isShown; i++)
      {
        if (imgs[i]->ImageType() == 2)
          {
            isShown = ((ImageStructDouble*)imgs[i])->Img()->getMatrixPointer() == img->getMatrixPointer();
          }
      }
    return isShown;
  }

  void ImageWindow::SetZoomFactor(int NewZoomFactor)
  {
    ZoomFactor = NewZoomFactor;
    if (ZoomFactor > 0)
      {
        PixelSize = ZoomFactor;
        ZoomStep = 1;
      }
    else
      {
        PixelSize = 1;
        ZoomStep = -ZoomFactor;
      }
  }

  int ImageWindow::CalcOptimalZoom() const
  {
    wxSize AvailableSize = ParentFrame->GetClientSize();

    AvailableSize.SetWidth(max<int>(0, AvailableSize.GetWidth()));
    AvailableSize.SetHeight(max<int>(0, AvailableSize.GetHeight()));

    // if the window is too small to display anything, we don't change the zoom factor and do nothing
    if ((AvailableSize.GetWidth() == 0) || (AvailableSize.GetHeight() == 0))
      {
        return 1;
      }

    // determine the optimal zoom factor (zoom in)
    int NewZoomFactor = min<int>(AvailableSize.GetWidth() / SizeX,
                                 AvailableSize.GetHeight() / SizeY);
    if (NewZoomFactor == 0) // have to zoom out ?
      NewZoomFactor = -max<int>((SizeX + AvailableSize.GetWidth() - 1) / AvailableSize.GetWidth(),
                                (SizeY + AvailableSize.GetHeight() - 1) / AvailableSize.GetHeight());

    return NewZoomFactor;
  }

  void ImageWindow::DoZoom(int NewZoomFactor)
  {
    // if the ZoomFactor is zero, we shall compute the optimal
    // zoom factor such that the image can be wholly displayed
    // in the window
    if (NewZoomFactor == 0)
      {
        // compute the size that we have to display the image
        // (remember that we want to be able to display the border as well)
        NewZoomFactor = CalcOptimalZoom();
      }

    // is there something to change
    if (NewZoomFactor == ZoomFactor)
      {
        return;
      }

    SetZoomFactor(NewZoomFactor);

    ParentFrame->SetClientSize(
      std::min(wxGetApp().DefXSize(), getVirtualSizeX()),
      std::min(wxGetApp().DefYSize(), getVirtualSizeY())
    );

    SetVirtualSize(getVirtualSizeX(), getVirtualSizeY());

    AdjustScrollbars();
    SetScrollRate(SCROLLUNIT, SCROLLUNIT); // int pixelsPerUnit

    Refresh();
  }

  void ImageWindow::OnZoom(wxCommandEvent& Event)
  {
    int NewZoomFactor = Event.GetInt();
    DoZoom(NewZoomFactor);
    WakeUpUserThread();
  }

  int ImageWindow::Zoom(int zoomfactor)
  {
    // return ERROR, if we haven't created the actual GUI window
    if (NeedsCreation)
      {
        return ERROR;
      }

    wxCommandEvent Event(ZOOM);
    Event.SetInt(zoomfactor);
    AddPendingEvent(Event);
    WaitForMainThread();
    return OK;
  }

  void ImageWindow::OnChar(wxKeyEvent& KeyEvent)
  {
    // processed key events are: plus and minus key for zooming
    // (the rest will do the scroll helper class for us)
    if (KeyEvent.GetKeyCode() == '>')
      {
        // ZoomFactors 0 and -1 must be skipped
        if (ZoomFactor == -2)
          DoZoom(1);
        else
          DoZoom(ZoomFactor + 1);
        return;
      }

    if (KeyEvent.GetKeyCode() == '<')
      {
        // ZoomFactors 0 and -1 must be skipped
        if (ZoomFactor == 1)
          DoZoom(-2);
        else
          DoZoom(ZoomFactor - 1);
        return;
      }

    if (KeyEvent.GetKeyCode() == '*')
      {
        SwitchFullScreen();
        return;
      }

    if (KeyEvent.GetKeyCode() == '=')
      {
        DoZoom(1);
        return;
      }

    // any other key is passed through to the console
    // window
#if wxMAJOR_VERSION == 2
#if wxMINOR_VERSION <= 8
    wxGetApp().GetConsoleWin()->ProcessEvent(KeyEvent);
#else
    wxGetApp().GetConsoleWin()->GetEventHandler()->ProcessEvent(KeyEvent);
#endif
#else
    wxGetApp().GetConsoleWin()->GetEventHandler()->ProcessEvent(KeyEvent);
#endif
  }

  bool ImageWindow::SwitchFullScreen()
  {
    isFullScreen = ! isFullScreen;
    ParentFrame->ShowFullScreen(isFullScreen, wxFULLSCREEN_ALL);
    return isFullScreen;
  }

  /*
   * Coordinates transformation
   */

  void ImageWindow::translateWin2ImagePos(int xw, int yw,
                                          int& xi, int& yi) const
  {
    // Calculate the position (xi,yi) of the pixel that would be displayed at
    // position (xw,yw).
    // This position depends on the current scroller offsets and the
    // current zoom factor.
    xi = (xw - border + GetScrollPos(wxHORIZONTAL)) * ZoomStep / PixelSize;
    yi = (yw - border + GetScrollPos(wxVERTICAL)) * ZoomStep / PixelSize;
  }

  IPoint ImageWindow::translateWin2ImagePos(const wxPoint& WindowPosition) const
  {
    IPoint result;
    translateWin2ImagePos(WindowPosition.x, WindowPosition.y, result.x, result.y);
    return result;
  }

  void ImageWindow::translateImage2WinPos(int xi, int yi, int& xw, int& yw) const
  {
    xw = xi * PixelSize / ZoomStep + border - GetScrollPos(wxHORIZONTAL);
    yw = yi * PixelSize / ZoomStep + border - GetScrollPos(wxVERTICAL);
  }

  wxPoint ImageWindow::translateImage2WinPos(IPoint ImagePosition) const
  {
    int xw, yw;
    translateImage2WinPos(ImagePosition.x, ImagePosition.y, xw, yw);
    return wxPoint(xw, yw);
  }

  /*
   * OnPaint
   */

  void ImageWindow::OnPaint(wxPaintEvent& Event)
  {
    PaintIsRunning = true;
#ifdef EVENTDEBUG
    cout << "IP ";
    cout.flush();
#endif
    // We have to create a wxPaintDC, regardless whether we will
    // use it or not. Otherwise we will receive new wxPaintEvents
    // (only under win32; paint messages will reoccur unless
    // the invalidated rect of the window has been validated.
    // wxPaintDC's c'tor validates the whole window

    wxPaintDC dc(this);
    PrepareDC(dc);

    // at first determine which part of the image should be displayed

    wxRegion reg = GetUpdateRegion();

    int wx0, wy0;
    int width, height;

    reg.GetBox(wx0, wy0, width, height); // these are coordinates in Window

    // If there is nothing to display, we can return without doing anything.
    if ((width == 0) || (height == 0))
      {
        PaintIsRunning = false;
        return;
      }

    int scrolloffset_x = GetScrollPos(wxHORIZONTAL);
    int scrolloffset_y = GetScrollPos(wxVERTICAL);

    int wx1 = wx0 + width - 1;
    int wy1 = wy0 + height - 1;

    // We compute the part of the Image, that has to be painted,
    // let's call it the visible area.
    // x0 and y0 denote the left upper corner of the visible area,
    // x1 and y1 the lower right corner.

    int ix0, iy0;
    translateWin2ImagePos(wx0, wy0, ix0, iy0);
    int ix1, iy1;
    translateWin2ImagePos(wx1, wy1, ix1, iy1);

    // limit to image size
    if (ix0 < 0)
      {
        ix0 = 0;
      }
    if (iy0 < 0)
      {
        iy0 = 0;
      }
    if (ix1 >= SizeX)
      {
        ix1 = SizeX - 1;
      }
    if (iy1 >= SizeY)
      {
        iy1 = SizeY - 1;
      }

    int bmx0, bmy0, bmx1, bmy1; // position of bitmap to draw
    translateImage2WinPos(ix0, iy0, bmx0, bmy0);

    translateImage2WinPos(ix1, iy1, bmx1, bmy1);

    // Korrektur: rechte untere Ecke des Pixels
    bmx1 += PixelSize - 1;
    bmy1 += PixelSize - 1;

    int bmwidth = bmx1 - bmx0 + 1;
    int bmheight = bmy1 - bmy0 + 1;

    if ((bmwidth > 0) && (bmheight > 0))
      {
        // Create an array that is compatible to wxImage.
        unsigned char* PixelArray = nullptr;
        try
          {
            PixelArray = new unsigned char[bmwidth * bmheight * 3];
            if (PixelArray == nullptr)
              {
                throw std::bad_alloc();
              }
          }
        catch (std::bad_alloc&)
          {
            dc.DrawText(wxString("Out of memory.", wxConvLibc), 1, 10);
            PaintIsRunning = false;
            return;
          }

        PixelArrayPtr = PixelArray;
        // We fill the pixel array.
        if (PixelSize > 1)
          {
            PaintImageY = iy0;
            int yc = 0;
            for (PaintWindowY = 0; PaintWindowY < bmheight; PaintWindowY++)
              {
                PaintImageX = ix0;
                int xc = 0;
                for (PaintWindowX = 0; PaintWindowX < bmwidth; PaintWindowX++)
                  {
                    PutPixel();
                    xc++;
                    if (xc >= PixelSize)
                      {
                        xc = 0;
                        PaintImageX++;
                      }
                  }
                yc++;
                if (yc >= PixelSize)
                  {
                    yc = 0;
                    PaintImageY++;
                  }
              }
          }
        else
          {
            PaintImageY = iy0;
            for (PaintWindowY = 0; PaintWindowY < bmheight; PaintWindowY++, PaintImageY += ZoomStep)
              {
                PaintImageX = ix0;
                for (PaintWindowX = 0; PaintWindowX < bmwidth; PaintWindowX++, PaintImageX += ZoomStep)
                  {
                    PutPixel();
                  }
              }
          }

        // We transform the image to a bitmap, that can be painted.
        wxImage PaintImage(bmwidth, bmheight, PixelArray, true);

        dc.DrawBitmap(wxBitmap(PaintImage),
                      bmx0 + scrolloffset_x, bmy0 + scrolloffset_y,
                      false); // bool transparent
        delete [] PixelArray;
      }

    // Rand grau färben
    dc.SetBrush(*wxGREY_BRUSH);
    dc.SetPen(*wxGREY_PEN);
    if (bmx0 > wx0)   // linker Rand sichtbar ?
      {
        dc.DrawRectangle(wx0 + scrolloffset_x, wy0 + scrolloffset_y, bmx0 - wx0, height);
      }

    if (bmy0 > wy0)   // oberer Rand sichtbar ?
      {
        dc.DrawRectangle(wx0 + scrolloffset_x, wy0 + scrolloffset_y, width, bmy0 - wy0);
      }

    if (bmy1 < wy1)   // unterer Rand sichtbar ?
      {
        dc.DrawRectangle(wx0 + scrolloffset_x, bmy1 + 1 + scrolloffset_y, width, wy1 - bmy1);
      }

    if (bmx1 < wx1)   // rechter Rand sichtbar ?
      {
        dc.DrawRectangle(bmx1 + 1 + scrolloffset_x, wy0 + scrolloffset_y, wx1 - bmx1, height);
      }

    // paint the image cursor, if necessary
    if (CursorIsEnabled)
      {
        DrawCursor(CursorPosition);
      }

    PaintIsRunning = false;
  }

  void ImageWindow::OnEraseBackground(wxEraseEvent& Event)
  {
    // do nothing(and thus prevent the background from being repainted,
    // at the moment we repaint the whole client area anyway)
  }

  void ImageWindow::RunInteraction(InteractionHandler& interaction)
  {
    // only one interaction at a time can be running
    this->Interaction = &interaction;

    wxCommandEvent Event(INIT_INTERACTION);
    AddPendingEvent(Event);
    WaitForMainThread();
  }

  void ImageWindow::OnInitInteraction(wxEvent&)
  {
    // initialize the interaction
    Interaction->Init();
    PushEventHandler(Interaction);
  }

  void ImageWindow::OnCleanupInteraction(wxEvent&)
  {
    // To clean up the interaction we only need to set our member to the null
    // pointer(we are not owner of the interaction object, so we are not
    // allowed to delete it!). The interaction works like an event filter,
    // so have have to unhook it from our event handler list.
    PopEventHandler();
    Interaction = NULL;

    // Since the interaction has finished we can wake the user thread
    // now.
    WakeUpUserThread();
  }

  bool ImageWindow::RegularUpdate()
  {
    // check if images still exist

    if (!existImages())
      {
        return false;
      }
    else
      {
        // check if there is any need to refresh
        Refresh();
        Update(); // Update enforces repainting of windows
        return true;
      }
  }

  void ImageWindow::DrawCursor(const IPoint& Position)
  {
    // We draw the cursor at the position SelectedPoint. If the user changes the
    // position of the cursor, we
    // have to restore the original contents of the window, and paint the
    // cursor at a different position. Restoring the original contents of the window
    // could be done in two ways: either repaint the area of the old cursor position
    // (which would be quite time-consuming) or paint the cursor in such a
    // fashion that it could be erased easily. This can be accomplished by painting
    // the cursor with logical function wxINVERT, but this has one drawback: if we
    // paint the cursor over a large patch of gray it will be hardly recognizable.
    // Instead we use logical function wxXOR and paint with RGB color(128, 128, 128).
    // This just swapes the most significant bit of the destination color.
    wxClientDC ClientDC(this);
    ClientDC.SetLogicalFunction(wxXOR);
    unsigned int LineWidth = max<int>(1, getZoomFactor());
    ClientDC.SetPen(wxPen(wxColor(128, 128, 128), // const  wxColour& colour
                          LineWidth, // int width
                          wxSOLID)); // int style

    wxPoint WinPos = translateImage2WinPos(Position);
    // + wxPoint(LineWidth / 2, LineWidth / 2);
    ClientDC.DrawLine(WinPos.x - 20, //2 * LineWidth, // wxCoord x1
                      WinPos.y, // wxCoord y1
                      WinPos.x + 20, //2 * LineWidth, // wxCoord x2
                      WinPos.y); // wxCoord y2
    ClientDC.DrawLine(WinPos.x, // wxCoord x1
                      WinPos.y - 20, //2 * LineWidth, // wxCoord y1
                      WinPos.x, // wxCoord x2
                      WinPos.y + 20); //2 * LineWidth); // wxCoord y2
  }

  void ImageWindow::EnableCursor(bool enable)
  {
    // just tell the main thread that the status of the cursor may have changed
    wxCommandEvent Event(CURSOR_ENABLE);
    Event.SetInt(enable ? 1 : 0);
    AddPendingEvent(Event);
  }

  void ImageWindow::OnCursorEnable(wxCommandEvent& Event)
  {
    // extract the new enable flag from Event
    bool ShouldEnable = (Event.GetInt() == 1);

    // do only something if the enable flag has changed
    if (ShouldEnable != CursorIsEnabled)
      {
        DrawCursor(CursorPosition);
        CursorIsEnabled = ShouldEnable;
      }
  }

  void ImageWindow::OnCursorUpdate(wxCommandEvent& Event)
  {
    if (!CursorIsEnabled)
      {
        return;
      }

    // extract the new cursor position from Event
    IPoint OldCursorPos = CursorPosition;
    CursorPosition = IPoint(Event.GetInt(), Event.GetExtraLong());

    // we only have to repaint the cursor, if it has changed its position
    if (OldCursorPos != CursorPosition)
      {
        // delete the old cursor
        DrawCursor(OldCursorPos);

        // draw the new cursor
        DrawCursor(CursorPosition);
      }
  }

  int ImageWindow::SetCursor(int x, int y)
  {
    if (!getImageRect().inside(x, y))
      {
        return WRONG_PARAM;
      }

    // just tell the main thread that the position of the cursor may have changed
    EnableCursor(true);
    wxCommandEvent Event(CURSOR_UPDATE);
    Event.SetInt(x);
    Event.SetExtraLong(y);
    AddPendingEvent(Event);
    return OK;
  }

  int ImageWindow::getMousePosition(int& x, int& y)
  {
    // Translate the mouse position into image coordinates
    IPoint MousePosImg = translateWin2ImagePos(MousePosition);

    if (getImageRect().inside(MousePosImg))
      {
        MouseFlags |= M_IN_WINDOW;
      }
    else
      {
        MouseFlags &= ~M_IN_WINDOW;
      }

    //    cout << MousePosImg.x << ","<<MousePosImg.y << endl;
    x = MousePosImg.x;
    y = MousePosImg.y;

    int RetVal = MouseFlags;

    MouseFlags = MouseFlags & (M_IN_WINDOW || M_LEFT_DOWN || M_RIGHT_DOWN || M_MIDDLE_DOWN);
    return RetVal;
  }

  void ImageWindow::OnMouseEvents(wxMouseEvent& Event)
  {
#ifdef EVENTDEBUG
    cout << "M ";
    cout.flush();
#endif
    if (Event.LeftIsDown())
      {
        MouseFlags |= M_LEFT_DOWN;
      }
    else
      {
        MouseFlags &= ~M_LEFT_DOWN;
      }

    if (Event.LeftDown())
      {
        MouseFlags |= M_LEFT_PRESSED;
      }
    if (Event.LeftUp())
      {
        MouseFlags |= M_LEFT_RELEASED;
      }

    if (Event.MiddleIsDown())
      {
        MouseFlags |= M_MIDDLE_DOWN;
      }
    else
      {
        MouseFlags &= ~M_MIDDLE_DOWN;
      }

    if (Event.MiddleDown())
      {
        MouseFlags |= M_MIDDLE_PRESSED;
      }
    if (Event.MiddleUp())
      {
        MouseFlags |= M_MIDDLE_RELEASED;
      }

    if (Event.RightIsDown())
      {
        MouseFlags |= M_RIGHT_DOWN;
      }
    else
      {
        MouseFlags &= ~M_RIGHT_DOWN;
      }

    if (Event.RightDown())
      {
        MouseFlags |= M_RIGHT_PRESSED;
      }
    if (Event.RightUp())
      {
        MouseFlags |= M_RIGHT_RELEASED;
      }

    if (Event.Moving() || Event.Dragging())
      {
        MousePosition = Event.GetPosition();
        MouseFlags |= M_MOVED;
      }

    if (Event.Entering() || Event.Leaving())
      {
        MousePosition = Event.GetPosition();
      }

    Event.Skip();
  }


  int ImageWindow::SelPoint(int Mode, IPoint& p)
  {
    // run the interaction and evaluate it's return values
    iceSelectPointInteraction interaction(this);
    RunInteraction(interaction);

    mouseButtonState tmp = interaction.Pressed();

    //     if (tmp == RIGHTMB)
    //     {
    //       delete Interaction;
    //       return RIGHTMB;
    //     }

    p = interaction.GetSelectedPoint();

    return tmp;
  }

  int ImageWindow::SelectWindow(Window& w, windowSelectionMode mode)
  {
    // run the interaction and evaluate it's return values
    iceSelectRectInteraction interaction(this, w, mode);
    RunInteraction(interaction);

    mouseButtonState tmp = interaction.Pressed();

    if (tmp == RIGHTMB)
      {
        return RIGHTMB;
      }

    w = interaction.GetSelectedRect();
    return tmp;
  }

  int ImageWindow::SelectLine(IPoint& p1, IPoint& p2)
  {
    iceSelectLineInteraction interaction(this);

    RunInteraction(interaction);

    mouseButtonState tmp = interaction.Pressed();

    if (tmp == RIGHTMB)
      {
        return RIGHTMB;
      }

    interaction.GetSelectedLine(p1, p2);

    return tmp;
  }

  int ImageWindow::SelectLineFromStart(const IPoint& p1, IPoint& p2)
  {
    iceSelectLineInteraction interaction(this, p1);

    RunInteraction(interaction);

    mouseButtonState tmp = interaction.Pressed();

    if (tmp == RIGHTMB)
      {
        return RIGHTMB;
      }

    IPoint dummy;
    interaction.GetSelectedLine(dummy, p2);

    return tmp;
  }

  // Handling of colortables: here: methods that do nothing
  // Overide this in classes, where colortables make sense
#define FNAME "GetGrayColor"
  int ImageWindow::GetGrayColor(unsigned int Entry,
                                unsigned char& RedVal,
                                unsigned char& GreenVal,
                                unsigned char& BlueVal)
  {
    return OK;
  }

#undef FNAME
#define FNAME "SetGrayColor"
  int ImageWindow::SetGrayColor(unsigned int Entry,
                                unsigned char RedVal,
                                unsigned char GreenVal,
                                unsigned char BlueVal)
  {
    return OK;
  }
#undef FNAME
#define FNAME "SetGrayLUT"
  int ImageWindow::SetGrayLUT(unsigned int First, unsigned int Last)
  {
    return OK;
  }
#undef FNAME
#define FNAME "GetOverlayColor"
  int ImageWindow::GetOverlayColor(unsigned int Entry,
                                   unsigned char& RedVal,
                                   unsigned char& GreenVal,
                                   unsigned char& BlueVal)
  {
    return OK;
  }
#undef FNAME
#define FNAME "SetOverlayColor"
  int ImageWindow::SetOverlayColor(unsigned int Entry,
                                   unsigned char RedVal,
                                   unsigned char GreenVal,
                                   unsigned char BlueVal)
  {
    return OK;
  }
#undef FNAME
}
