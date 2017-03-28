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
#ifndef __ICEIMAGEWINDOW_H
#define __ICEIMAGEWINDOW_H

#include <unistd.h>
#include <string>

#include <wx/frame.h>
#include <wx/scrolwin.h>

#include "Window.h"

#include <unistd.h>
#include "visual/Frame.h"
#include "ImageTemplate.h"
#include "visual/ThreadCommHelper.h"
#include "defs.h"  // for error code in inline implementations
#include "base.h"
#include "visual/ImageStruct.h"
#include "visual/pointsel.h"

#define SCROLLUNIT 1

namespace ice
{
  BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(CLEANUP_INTERACTION, 1)
  DECLARE_EVENT_TYPE(REGULAR_UPDATE, 2)
  END_DECLARE_EVENT_TYPES()
  ;;
  // --------------------------------------------------------

  class ImageWindow : public wxScrolledWindow, private ThreadCommHelper
  {
  protected:
    static const int border = 10; // grey border around Image (in pixels)

    //! The Caption for image windows.
    std::string Caption;

    //! The parent frame, that hosts this window as a child.
    iceFrame* ParentFrame;

    //! The horizontal size of the image(s), that is displayed in this window.
    int SizeX;

    //! The vertical size of the image(s), that is displayed in this window.
    int SizeY;

    //! The zoom factor
    int ZoomFactor;
    //! Zoom is splitted to 2 values:
    //! ZoomFactor>0 PixelSize=Zoom Step=1
    //! ZoomFactor<0 PixelSize=1 Step=-Zoom
    int PixelSize;
    int ZoomStep;

    bool PaintIsRunning;

    // "globale" Instanzvariablen für Paint <-> PutPixel
    int PaintImageX;
    int PaintImageY;
    int PaintWindowX;
    int PaintWindowY;

    unsigned char* PixelArrayPtr;

    // The interaction that is currently running, if there is one.
    class InteractionHandler* Interaction;

    // The position of the image cursor.
    IPoint CursorPosition;

    // This flag tells us, whether the cursor should be displayed.
    bool CursorIsEnabled;

    /* This integer holds the flags concerning the mouse events
       that have happened since the last call of GetMousePosition.
    */
    int MouseFlags;

    // The position of the mouse. We'll need this member for GetMousePosition.
    wxPoint MousePosition;

    // Images belonging to this window
    std::vector<ImageStruct*> imgs;

  private:
    //! This flag tells us whether the window still has to be initialized.
    bool NeedsCreation;
    //! This flag tells us whether the window is displayed in full screen mode
    bool isFullScreen;

  protected:
    virtual ~ImageWindow()
    {
      for (unsigned int i = 0; i < imgs.size(); ++i)
        delete imgs[i];
    };
    virtual bool Create(unsigned int SizeX, unsigned int SizeY);

  public:
    ImageWindow(ImageBase* img,
                const std::string& windowname = "ICE Image");

    ImageWindow(ImageD* img,
                const std::string& windowname = "ICE Image");

    virtual bool addImage(ImageBase* img);
    virtual bool addImage(ImageD* img);

    virtual bool Destroy();

    //! Zooms the window.
    virtual int Zoom(int ZoomFactor);
    // Get the zoom factor. Setting the zoom factor can be done with Zoom().
    virtual int GetZoomFactor() const;

    //! Get the image size.
    virtual IPoint GetImageSize() const;

    /* Get image size as rectangle. The image rectangle will have
       corners (0, 0) and (GetImageSize().GetWidth()-1,
       GetImageSize().GetHeight () - 1). This method is quite useful
       for checking, whether a given point lies inside the image.*/
    virtual Window GetImageRect() const;

    // Does the Window display the given image?
    virtual bool showsImage(ImageBase* img) const;
    virtual bool showsImage(ImageD* img) const;

    // returns the windowtype (
    //     0=GrayImage without Colormap,
    //     1=GrayImgWin,
    //     2=OverlayImgWin,
    //     3=RGBImageWin,
    //     4=StereoIHImageWin
    //     5=Overlay2ImgWin)
    virtual char ShowType() const = 0;

    /* Translates image pixel position PixelPos to window pixel
       position WindowPos, that means the pixel at PixelPos in
       the image will be displayed at the position WindowPos
       in the window.
       PixelPos can range from (0, 0) to (SizeX - 1, SizeY - 1).
       The resulting WindowPos position can be outside the window,
       meaning that the pixel at PixelPos will not be displayed at
       the current zoom factor and current scrolling position.
       If the zoom factor is greater than one, this function
       returns the upper left corner of the displayed rectangle that
       represents the image pixel. */

    virtual void translateWin2ImagePos(int xw, int yw, int& xi, int& yi) const;
    //    virtual IPoint TranslateWin2ImagePos(const IPoint& WindowPosition) const;
    virtual IPoint translateWin2ImagePos(const wxPoint& WindowPosition) const;

    /* .. and back */
    virtual void translateImage2WinPos(int xi, int yi, int& xw, int& yw) const;
    //    virtual IPoint TranslateImage2WinPos(const IPoint& ImagePosition) const;
    virtual wxPoint translateImage2WinPos(IPoint ImagePosition) const;

    //! Starts >Interaktion< and waits for it's termination.
    virtual void RunInteraction(class InteractionHandler& interaktion);

    //! Returns the current mouse position
    virtual int GetMousePosition(int& x, int& y);

    // Interactively select Point
    virtual int SelPoint(int Mode, IPoint& p);

    // Interactively select a window
    virtual int SelectWindow(Window& w, wselmode mode = select);

    // Interactively select line
    virtual int SelectLine(IPoint& p1, IPoint& p2);
    virtual int SelectLineFromStart(const IPoint& p1, IPoint& p2);

    //! Enables the drawing of the cursor.
    virtual void EnableCursor(bool Enable);

    //! Sets the cursor to position (>x<, >y<).
    /*! Sets the cursor to position (>x<, >y<).
      \returns OK, if the request could be fulfilled.
      <BR>ERROR, if the new position was outside the image.*/
    virtual int SetCursor(int x, int y);

    //! Draws a cursor at the position >Position<. Drawing the cursor
    //! a second time at the same position will erase the cursor.
    virtual void DrawCursor(const IPoint& Position);

    // Handling of colortables
    // in this class these are dummy-functions and do nothing
    // functional implementations in GreyImageWindow and OverlayImageWindow
    virtual int GetGreyColor(unsigned int Entry,
                             unsigned char& RedVal,
                             unsigned char& GreenVal,
                             unsigned char& BlueVal);

    virtual int SetGreyColor(unsigned int Entry,
                             unsigned char RedVal,
                             unsigned char GreenVal,
                             unsigned char BlueVal);

    virtual int SetGreyLUT(unsigned int First, unsigned int Last);

    virtual int GetOverlayColor(unsigned int Entry,
                                unsigned char& RedVal,
                                unsigned char& GreenVal,
                                unsigned char& BlueVal);

    virtual int SetOverlayColor(unsigned int Entry,
                                unsigned char RedVal,
                                unsigned char GreenVal,
                                unsigned char BlueVal);

    virtual bool AddMenuItem(const std::string& menu,
                             const std::string& item, long userid)
    {
      return ParentFrame->AddMenuItem(menu, item, userid);
    }

    void RegularUpdate();

    virtual bool SwitchFullScreen();

    virtual iceFrame* Frame() const
    {
      return ParentFrame;
    }

  protected:
    // event handlers
    void OnPaint(wxPaintEvent& PaintEvent);

    void OnEraseBackground(wxEraseEvent& Event);
    void OnChar(wxKeyEvent& KeyEvent);
    void OnZoom(wxCommandEvent& Event);
    void OnCursorEnable(wxCommandEvent& Event);
    void OnCursorUpdate(wxCommandEvent& Event);

    void OnMove(wxMouseEvent& Event);
    void OnMouseEvents(wxMouseEvent& Event);

    /*! Zooms the window. This method can only be called from inside the
      main thread. It will be used by the OnZoom event handler and
      the OnChar event handler.*/
    virtual void DoZoom(int NewZoomFactor);

    virtual void SetZoomFactor(int NewZoomFactor);
    int CalcOptimalZoom() const;

    // This method tells if the window should be redrawn on the next
    // automatic update event.
    // this implementation decides on the base of changes in displayed
    // images
#ifdef CONTROLLED_REFRESH
    virtual bool NeedsUpdate()
    {
      bool rc = false;
      for (unsigned int i = 0; i < imgs.size(); i++)
        {
          if (imgs[i]->CheckTimeStamp())
            rc = true;
        }
      return rc;
    }
#endif

    // Writes the RGB value for image position (x, y) into the pixel
    // array at position Pos.
    virtual void PutPixel() = 0;

  protected:
    /* These functions tell us the virtual image size, that means how
       much space we will need to paint the image according to the
       zoom factor and the border. These functions come in handy
       when we want to readjust the scrollbars.*/
    virtual int getVirtualSizeX() const;
    virtual int getVirtualSizeY() const;

    /* Initializes the current Interaction. This method should only
       get invoked inside the main thread.*/
    void OnInitInteraction(wxEvent&);
    void OnCleanupInteraction(wxEvent&);

  private:

    DECLARE_EVENT_TABLE();
  };

  typedef ImageWindow* Visual;
  ////////////////////////////////////////////////////////////////////////////
  //
  //            inline implementations
  //

  inline int ImageWindow::GetZoomFactor() const
  {
    return ZoomFactor;
  }

  inline IPoint ImageWindow::GetImageSize() const
  {
    return IPoint(SizeX, SizeY);
  }

  inline Window ImageWindow::GetImageRect() const
  {
    return Window(0, 0, SizeX - 1, SizeY - 1);
  }

  inline int ImageWindow::getVirtualSizeX() const
  {
    if (ZoomFactor > 0)
      return SizeX * ZoomFactor + 2 * border;
    if (ZoomFactor < 0)
      return SizeX / -ZoomFactor + 2 * border;
    return 0;
  }

  inline int ImageWindow::getVirtualSizeY() const
  {
    if (ZoomFactor > 0)
      return SizeY * ZoomFactor + 2 * border;
    if (ZoomFactor < 0)
      return SizeY / -ZoomFactor + 2 * border;
    return 0;
  }
}
#endif // #ifndef __ICEIMAGEWINDOW_H