#ifndef __ICEFRAME_H
#define __ICEFRAME_H

#include <string>
#include <vector>
#include <map>

#include <wx/frame.h>

namespace ice
{
  class iceFrame : public wxFrame
  {
  public:
    iceFrame(const iceFrame&) = delete; // disable copy constructor

    iceFrame(const wxString& WindowTitle);
    iceFrame(const wxString& WindowTitle, long WindowStyle);

    bool SetChildWindow(wxWindow& ChildWindow);

    virtual bool Destroy();

    virtual bool AddMenuItem(const std::string& menu, const std::string& item, long userid);

  protected:
    void OnClose(wxCloseEvent& Event);
    void OnUserItem(wxCommandEvent& event);

    std::vector<std::string> menuname;
    std::vector<wxMenu*> Menu;

    std::map<long, long> userid;

    //! The child window
    wxWindow* ChildWindow;

    wxMenuBar* MenuBar;

    DECLARE_EVENT_TABLE();
  };
} // namespace ice
#endif // #ifndef __ICEFRAME_H
