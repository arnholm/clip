/***************************************************************
 * Name:      clipMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Carsten Arnholm ()
 * Created:   2013-08-31
 * Copyright: Carsten Arnholm ()
 * License:
 **************************************************************/

#include "clipMain.h"
#include <wx/msgdlg.h>

#include <string>
#include <math.h>
#include <sstream>
#include <wx/settings.h>


#include "images/portaclip16X16.xpm"
#include "images/portaclip32X32.xpm"
#include "images/portaclip48X48.xpm"
#include "images/portaclip64X64.xpm"


//(*InternalHeaders(clipDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

static wxColor wxDarkGreen(0,164,0);
const long clipDialog::UDP_SOCKET_ID = wxNewId();

//(*IdInit(clipDialog)
const long clipDialog::ID_STATICTEXT2 = wxNewId();
const long clipDialog::ID_TEXTCTRL1 = wxNewId();
const long clipDialog::ID_BUTTON3 = wxNewId();
const long clipDialog::ID_PANEL1 = wxNewId();
const long clipDialog::ID_CHECKBOX1 = wxNewId();
const long clipDialog::ID_STATICTEXT1 = wxNewId();
const long clipDialog::ID_TIMER1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(clipDialog,wxDialog)
    //(*EventTable(clipDialog)
    //*)
    EVT_SOCKET(UDP_SOCKET_ID, clipDialog::OnUDPSocket)
END_EVENT_TABLE()

clipDialog::clipDialog(wxWindow* parent,wxWindowID id)
{
   //(*Initialize(clipDialog)
   wxBoxSizer* BoxSizer3;
   wxBoxSizer* BoxSizer5;
   wxButton* OkButton;
   wxStaticBoxSizer* StaticBoxSizer1;

   Create(parent, wxID_ANY, _("Clip"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX, _T("wxID_ANY"));
   BoxSizer1 = new wxBoxSizer(wxVERTICAL);
   BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
   StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Send to IP"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
   BoxSizer5->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   m_ip_address = new wxTextCtrl(this, ID_TEXTCTRL1, _("10.0.0.111"), wxDefaultPosition, wxSize(100,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
   BoxSizer5->Add(m_ip_address, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   OkButton = new wxButton(this, ID_BUTTON3, _("OK"), wxDefaultPosition, wxSize(50,-1), 0, wxDefaultValidator, _T("ID_BUTTON3"));
   BoxSizer5->Add(OkButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   BoxSizer1->Add(BoxSizer5, 1, wxALL|wxEXPAND, 5);
   BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
   m_led = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxSize(16,16), wxBORDER_RAISED|wxFULL_REPAINT_ON_RESIZE, _T("ID_PANEL1"));
   BoxSizer3->Add(m_led, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   MonitorMouse = new wxCheckBox(this, ID_CHECKBOX1, _("Monitor Mouse"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
   MonitorMouse->SetValue(false);
   BoxSizer3->Add(MonitorMouse, 1, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   BoxSizer1->Add(BoxSizer3, 0, wxEXPAND, 5);
   StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Info"));
   m_message = new wxStaticText(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
   StaticBoxSizer1->Add(m_message, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
   BoxSizer1->Add(StaticBoxSizer1, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
   SetSizer(BoxSizer1);
   Timer1.SetOwner(this, ID_TIMER1);
   BoxSizer1->Fit(this);
   BoxSizer1->SetSizeHints(this);

   Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&clipDialog::OnOkButtonClick);
   Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&clipDialog::OnMonitorMouseClick);
   Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&clipDialog::OnTimer1Trigger);
   //*)

   m_config = new wxConfig(wxT("portaclip"),wxT("Arnholm"));

   wxIconBundle icons;
   icons.AddIcon(wxIcon(portaclip16X16_xpm));
   icons.AddIcon(wxIcon(portaclip32X32_xpm));
   icons.AddIcon(wxIcon(portaclip48X48_xpm));
   icons.AddIcon(wxIcon(portaclip64X64_xpm));
   SetIcons(icons);

   m_inside = true;
   m_mouse_prev = wxGetMousePosition();
   m_screen = wxSize(wxSystemSettings::GetMetric(wxSYS_SCREEN_X),wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
   m_screen_center = wxPoint(m_screen.x/2,m_screen.y/2);

   // address for this computer (receiving end)
   m_addr.AnyAddress();
   m_addr.Service(7552);

   // set up UDP socket for communication
   // Also define an eventhandler for incoming data
   m_udp = new wxDatagramSocket(m_addr, wxSOCKET_REUSEADDR);
   m_udp->SetEventHandler(*this, UDP_SOCKET_ID);
   m_udp->SetNotify(wxSOCKET_INPUT_FLAG);
   m_udp->Notify(true);

   // get IP address to send to from configuration
   // if not configured, the user types the address and pushes the "Save" button
   wxString ip_address="255.255.255.0";
   m_config->Read("SendTo",&ip_address);
   if(ip_address.length() > 0) {
      m_sendto_addr.Hostname(ip_address);
   }

   // display the IP address in the text control
   m_sendto_addr.Service(7552);
   m_ip_address->SetValue(m_sendto_addr.IPAddress());

   m_led->SetBackgroundColour(wxDarkGreen);

   backup_clipboard(true);

   bool monitor_mouse=false;
   m_config->Read("MonitorMouse",&monitor_mouse,monitor_mouse);
   do_monitor_mouse(monitor_mouse);
   MonitorMouse->SetValue(m_monitor_mouse);

   /*
   int millisec = 25;
   Timer1.Start(millisec);
*/

}

clipDialog::~clipDialog()
{
   // save configuration
   m_config->Write("SendTo",m_sendto_addr.IPAddress());
   m_config->Write("MonitorMouse",m_monitor_mouse);
   delete m_config;
   delete m_udp;
}

void clipDialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void clipDialog::OnMonitorMouseClick(wxCommandEvent& event)
{
   do_monitor_mouse(event.IsChecked());
}

void clipDialog::do_monitor_mouse(bool monitor)
{
   if(m_monitor_mouse) {
      Timer1.Stop();
   }

   m_monitor_mouse = monitor;

   if(m_monitor_mouse) {
      int millisec = 15;
      Timer1.Start(millisec);

      backup_clipboard();
      send_mouse_state(false);
   }
}

void clipDialog::OnOkButtonClick(wxCommandEvent& event)
{
   m_sendto_addr.Hostname(m_ip_address->GetValue());
   m_sendto_addr.Service(7552);
}

void clipDialog::OnTimer1Trigger(wxTimerEvent& event)
{
   if(m_monitor_mouse) {

      wxPoint mouse = wxGetMousePosition();

      int dxcen = mouse.x - m_screen_center.x;
      int dycen = mouse.y - m_screen_center.y;
      int dx    = mouse.x - m_mouse_prev.x;
      int dy    = mouse.y - m_mouse_prev.y;

      int dcen = sqrt(dxcen*dxcen + dycen*dycen);

      // Here, we try to detect the "event" that the mouse moves
      // from one screen to the other. We change state only when
      // such an event is detected

      if((abs(dx)> m_screen.x/3)) {
         // A sudden jump in X implies moving from one screen to another

         bool inside_before = m_inside;

         // if at the same time we seem to be near the screen center
         // (which kind of impossible), we take it as a sign we just jumped
         // to the remote screen. Otherwise we must be on the local screen
         if(dcen<300) {
            backup_clipboard();
            send_clipboard();

            m_inside = false;  // moving from local to remote screen
            send_mouse_state(!m_inside);
         }
         else  {
            m_inside = true;   // moving from remote to local screen

            request_clipboard();
            send_mouse_state(!m_inside);
         }

         #if defined(__WXMSW__)
            m_inside = (dcen!=0);
         #endif

         if(m_inside != inside_before) {
            // change "led" color to indicate the new state
            m_led->SetBackgroundColour((m_inside)?  wxDarkGreen : *wxRED );
            m_led->Refresh();
         }
      }
      m_mouse_prev = mouse;
   }

/*
   // take a backup copy of the clipboard, at somewhat more relaxed intervals
   wxTimeSpan time_since_backup = wxDateTime::Now().Subtract(m_clipboard_time);
   if(time_since_backup.GetMilliseconds() > 330) {
      backup_clipboard();
   }
*/
}

void clipDialog::OnUDPSocket(wxSocketEvent& event)
{
   if(event.GetSocketEvent() == wxSOCKET_INPUT)  {

      // UDP socked event received. The UDP message will contain a copy of
      // the other computer clipboard text in UTF8 format

      // buffer to receive the data
      std::vector<char> buf(1024*1024);

      // we don't care which IP address the data came from
      wxIPV4address addr;
      addr.AnyAddress();
      m_udp->RecvFrom(addr, &buf[0], buf.size());
      size_t nbytes = m_udp->LastCount();

      /*
      if(m_udp->Error()) {
         m_message->SetLabel("OnUDPSocket- error " + std::to_string(nbytes));
         return;
      }
      */

      if(nbytes > 0) {

         buf[nbytes] = 0;
         byte cb_hdr = buf[0];
         byte cb_cmd = buf[1];

         switch(cb_cmd) {
            case CB_DATA: { receive_clipboard(buf,nbytes); break; }
            case CB_SEND: { backup_clipboard(); send_clipboard();       break; }
            case CB_MOUS: { receive_mouse_state(buf);   break; }
            default:      { m_message->SetLabel("cb_cmd unknown, defaulted!");}
         }
      }
   }
}

void  clipDialog::send_clipboard()
{
   wxString msg = m_clipboard_backup.GetText();
   if(msg.Length() > 0) {
      // send the clipboard text as UTF8
      wxScopedCharBuffer buffer = msg.ToUTF8();

      std::vector<char> message(CB_HDR+buffer.length());
      message[0]=CB_HDR;
      message[1]=CB_DATA;
      memcpy(&message[CB_HDR],buffer.data(),buffer.length());

      m_udp->SendTo(m_sendto_addr,&message[0],message.size());
      m_message->SetLabel("sent CB_DATA " + std::to_string(message.size()));
   }
}

void  clipDialog::request_clipboard()
{
   std::vector<char> message(CB_HDR);
   message[0]=CB_HDR;
   message[1]=CB_SEND;
   m_udp->SendTo(m_sendto_addr,&message[0],message.size());
   m_message->SetLabel("sent CB_SEND request ");
}

void clipDialog::backup_clipboard(bool force)
{
   if(force) {
      m_clipboard_time = wxDateTime::Now();
   }

   if(m_inside) {

      try {

        // Read from local system clipboard and store in our clipboard backup object
         if(wxTheClipboard->Open()) {
            if(wxTheClipboard->IsSupported( wxDF_TEXT )) {
               if(!wxTheClipboard->GetData( m_clipboard_backup )) {
                  m_message->SetLabel(wxString::Format("wxTheClipboard->GetData() failed."));
               }
           //    wxString txt = m_clipboard_backup.GetText();
            }
            wxTheClipboard->Close();
            m_clipboard_time = wxDateTime::Now();
         }
      }
      catch(...) {
         m_message->SetLabel(wxString::Format("backup_clipboard() failed"));
      }
   }
}

void clipDialog::receive_clipboard(std::vector<char>& buf, size_t nbytes)
{
   byte cb_hdr   = buf[0];
   if(buf[1] == CB_DATA) {
      size_t nb = nbytes-cb_hdr;

      // Interpret the buffer data as UTF8
      wxString text  = wxString::FromUTF8(&buf[cb_hdr],nb);

      // Write the received text to the local clipboard
      if (text.length()>0) {
         if(wxTheClipboard->Open()) {
             m_clipboard_backup.SetText(text);
             wxTheClipboard->SetData( new wxTextDataObject(text) );
             wxTheClipboard->Close();
             m_message->SetLabel("Received CB_DATA " + std::to_string(nbytes));
             m_clipboard_time = wxDateTime::Now();
         }
      }
   }
}

void clipDialog::send_mouse_state(bool inside)
{
   std::vector<char> message(CB_HDR);
   message[0]=CB_HDR;
   message[1]=CB_MOUS;
   message[2]=(inside)? 1 : 0;
   m_udp->SendTo(m_sendto_addr,&message[0],message.size());
}

void clipDialog::receive_mouse_state(std::vector<char>& buf)
{
   m_inside =  buf[2]==1;
   m_led->SetBackgroundColour((m_inside)?  wxDarkGreen : *wxRED );
   m_led->Refresh();
}
