/***************************************************************
 * Name:      clipMain.h
 * Purpose:   Defines Application Frame
 * Author:    Carsten Arnholm ()
 * Created:   2013-08-31
 * Copyright: Carsten Arnholm ()
 * License:
 **************************************************************/

#ifndef clipMAIN_H
#define clipMAIN_H

#include <wx/socket.h>
#include <wx/clipbrd.h>
#include <wx/config.h>
#include <vector>

//(*Headers(clipDialog)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
//*)

class clipDialog: public wxDialog {
public:

   using byte = unsigned char;

   static constexpr byte CB_HDR  = 3;
   static constexpr byte CB_SEND = 1;  // Request to send clipboard
   static constexpr byte CB_DATA = 2;  // Message with clipboard data contained
   static constexpr byte CB_MOUS = 3;  // Message with mouse status

   clipDialog(wxWindow* parent,wxWindowID id = -1);
   virtual ~clipDialog();

private:

   //(*Handlers(clipDialog)
   void OnQuit(wxCommandEvent& event);
   void OnMonitorMouseClick(wxCommandEvent& event);
   void OnOkButtonClick(wxCommandEvent& event);
   void OnTimer1Trigger(wxTimerEvent& event);
   //*)

   //(*Identifiers(clipDialog)
   static const long ID_STATICTEXT2;
   static const long ID_TEXTCTRL1;
   static const long ID_BUTTON3;
   static const long ID_PANEL1;
   static const long ID_CHECKBOX1;
   static const long ID_STATICTEXT1;
   static const long ID_TIMER1;
   //*)

   //(*Declarations(clipDialog)
   wxBoxSizer* BoxSizer1;
   wxCheckBox* MonitorMouse;
   wxPanel* m_led;
   wxStaticText* StaticText2;
   wxStaticText* m_message;
   wxTextCtrl* m_ip_address;
   wxTimer Timer1;
   //*)

   void OnUDPSocket(wxSocketEvent& event);
   static const long UDP_SOCKET_ID;

   void send_clipboard();
   void request_clipboard();
   void receive_clipboard(std::vector<char>& buf, size_t nbytes);
   void backup_clipboard(bool force = false);
   void send_mouse_state(bool inside);
   void receive_mouse_state(std::vector<char>& buf);

   void do_monitor_mouse(bool monitor);

   DECLARE_EVENT_TABLE()

private:
   bool  m_monitor_mouse;

   wxSize  m_screen;          // local screen size
   wxPoint m_screen_center;   // local screen center
   wxPoint m_mouse_prev;      // previous mouse position
   bool    m_inside;          // true when mouse is within local screen

   wxTextDataObject  m_clipboard_backup; // this is our clipboard backup
   wxDateTime        m_clipboard_time;   // time of clipboard backup

   wxIPV4address     m_addr;
   wxIPV4address     m_sendto_addr;  // IP address to other computer
   wxDatagramSocket* m_udp;
   wxConfig*         m_config;
};

#endif // clipMAIN_H
