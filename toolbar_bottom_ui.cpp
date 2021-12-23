#include "stdafx.h"
#include "toolbar_bottom_ui.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <vector>

#define IDM_CODE_SAMPLES 101

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

std::vector <int> waitings;
CToolbarBottom::CToolbarBottom(CToolbarMgr* pToolbarMgr)
{
	m_hParentWnd = NULL;
	m_pToolbarMgr = pToolbarMgr;
	m_bInChat = false;
	m_bInParticipant = false;
	m_bInGalleryView = false;
}

CToolbarBottom::~CToolbarBottom()
{
	m_hParentWnd = NULL;
	m_pToolbarMgr = NULL;
	m_bInChat = false;
	m_bInParticipant = false;
	m_bInGalleryView = false;
}
void timer_start(void func() , unsigned int interval)
{
	std::thread([func, interval]()
		{
			while (true)
			{
				auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
				func();
				std::this_thread::sleep_until(x);
			}
		}).detach();
}
void WaitingRoom()
{
	const wchar_t* abc = L"I am running ...";
	OutputDebugStringW(abc);


	ZOOM_SDK_NAMESPACE::IList<unsigned int>* lstUser = SDKInterfaceWrap::GetInst().GetMeetingWaitingRoomController()->GetWaitingRoomLst();
	ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController* pUserCtrl = SDKInterfaceWrap::GetInst().GetMeetingWaitingRoomController();
	if (lstUser)
	{
		int count = lstUser->GetCount();
		bool already = false;
		for (int i = 0; i < count; i++)
		{
			int userId = lstUser->GetItem(i);
			
			for (int i = 0; i < waitings.size(); i++)
			{
				if (waitings[i] == userId) {
					already = true;
					break;
				}
				// add new value from 0 to 9 to next slot
				//arr.push_back(i);
			}
			if (!already) {
				ZOOM_SDK_NAMESPACE::IUserInfo* pUserInfo = pUserCtrl->GetWaitingRoomUserInfoByID(userId);
				const wchar_t* username = pUserInfo->GetUserNameW();
				const wchar_t* xyz = L"New user in Waiting room ";
				std::wstring s(xyz);
				s += std::wstring(username) + L"!";
				s = L"" + s;
				OutputDebugStringW(xyz);
				waitings.push_back(userId);
				const int result = ::MessageBox(NULL, s.c_str(), _T("Pure Sonic"), MB_OK);
				
				/*if (result == IDYES) {
					OutputDebugStringW(xyz);
					SDKInterfaceWrap::GetInst().GetMeetingWaitingRoomController()->AdmitToMeeting(userId);
				}
				else {
					OutputDebugStringW(L"you are in else");
					
				}*/
			}
		}
	}


}

void CToolbarBottom::InitWindow()
{
	RECT rc = { 0 };
	if( !::GetClientRect(m_hWnd, &rc) ) 
		return;
	rc.top = rc.bottom - DEFAULT_TOOLBAR_HEIGHT;
	rc.bottom = rc.top + DEFAULT_TOOLBAR_HEIGHT;
	if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), false, GetWindowExStyle(m_hWnd)) ) return;
	::SetWindowPos(m_hWnd, HWND_BOTTOM,0,0,0,0, SWP_NOSIZE | SWP_NOMOVE);

	m_containerBottomButtons = static_cast<CHorizontalLayoutUI* >(m_PaintManager.FindControl(_T("container_buttons_bottom_ui")));
	m_btnParticipant = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_participant")));
	if(m_btnParticipant)
	{								
		m_btnParticipant->SetText(L"Show Participant"); 
	}

	m_btnParticipantWaiting = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_participant_waiting")));
	if(m_btnParticipantWaiting)
	{								
		m_btnParticipantWaiting->SetText(L"Allow all Waiting");
	}

	m_btnThumbnailVideo = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_thumbnailvideo")));
	if(m_btnThumbnailVideo)            
		m_btnThumbnailVideo->SetText(L"Show Gallery");
	m_btnAudio = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_audio")));
	if(m_btnAudio)
		m_btnAudio->SetText(L"Mute Audio");
	m_btnVideo = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_video")));
	if(m_btnVideo)
		m_btnVideo->SetText(L"Stop video");
	//m_btnInvite = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_invite")));
	
	m_btnShare = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_share")));
	if(m_btnShare)
		m_btnShare->SetText(L"Share");
	m_btnChat = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_chat")));
	if(m_btnChat)
		m_btnChat->SetText(L"Show Chat");
	m_btnLeave = static_cast<CButtonUI* >(m_PaintManager.FindControl(_T("btn_leavemeeting")));
	if(m_btnLeave)
	{
		if(m_pToolbarMgr && m_pToolbarMgr->IsHost())
			m_btnLeave->SetText(L"End Meeting");
		else
			m_btnLeave->SetText(L"Leave Meeting");
	}
	m_btnMore = static_cast<CButtonUI* >(m_PaintManager.FindControl(L"btn_more"));
	if (m_btnMore)
	{
		m_btnMore->SetText(L"More");
	}

	m_ToolbarBottomWorkFlow.SetUIEvent(this);
	timer_start(WaitingRoom, 5000);
}

LRESULT CToolbarBottom::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	if( uMsg == WM_CREATE ) 
	{
		m_PaintManager.Init(m_hWnd);

		CDialogBuilder builder;
		STRINGorID xml(GetSkinRes());
		CControlUI* pRoot = builder.Create(xml, _T("xml"), 0, &m_PaintManager);
		ASSERT(pRoot && "Failed to parse XML");

		m_PaintManager.AttachDialog(pRoot);
		m_PaintManager.AddNotifier(this);
		InitWindow(); 
		return lRes;
	}

	if( m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes) ) 
	{
		return lRes;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}


void CToolbarBottom::Notify(TNotifyUI& msg)
{
	if(msg.sType == _T("click"))
	{
		if(msg.pSender == m_btnAudio)
		{
			DoAudioButtonClick();
		}
		else if(msg.pSender == m_btnVideo)
		{
			DoVideoButtonClick();
		}
		
		// else if(msg.pSender == m_btnInvite)
		// {
		// 	DoInviteButtonClick();
		// }
		
		else if (msg.pSender == m_btnThumbnailVideo)
		{
			DoThumbnailVideoClick();
		}
		else if(msg.pSender == m_btnParticipant)
		{
			DoParticipantButtonClick();
		}
		else if(msg.pSender == m_btnParticipantWaiting)
		{
			DoWaitingRoomButtonClick();
		}
		else if(msg.pSender == m_btnChat)
		{
			DoChatButtonClick();

			if (SDKInterfaceWrap::GetInst().GetMeetingService()->GetMeetingChatController()->IsMeetingChatLegalNoticeAvailable())
			{
				const wchar_t* sChartExplained = SDKInterfaceWrap::GetInst().GetMeetingService()->GetMeetingChatController()->getChatLegalNoticesExplained();
				::MessageBox(NULL, sChartExplained, _T("Pure Sonic"), MB_OK);
			}
		}
		else if(msg.pSender == m_btnShare)
		{
			DoShareButtonClick();
		}
		else if(msg.pSender == m_btnLeave)
		{
			DoLeaveButtonClick();
		}
		else if (msg.pSender == m_btnMore)
		{
			DoMoreMenuClick();
		}

	}
}

bool CToolbarBottom::IsWindow()
{
	return ::IsWindow(m_hWnd) ? true:false;
}

bool CToolbarBottom::DestroyWindow()
{
	if(IsWindow())
		return ::DestroyWindow(m_hWnd) ? true:false;
	return false;
}

void CToolbarBottom::SetParent(HWND hParentWnd)
{
	m_hParentWnd = hParentWnd;
}
CButtonUI* CToolbarBottom::GetButton(ToolbarButtonType buttonType)
{
	switch(buttonType)
	{
	case Toolbar_Button_Audio:
		return m_btnAudio;
	case Toolbar_Button_Video:
		return m_btnVideo;
	/*
	case Toolbar_Button_Invite:
		return m_btnInvite;
	*/
	case Toolbar_Button_Chat:
		return m_btnChat;
	case Toolbar_Button_Participant:
		return m_btnParticipant;
	case Toolbar_Button_Share:
		return m_btnShare;
	case Toolbar_Button_Leave:
		return m_btnLeave;
	case Toolbar_Button_ThumbnailVideo:
		return m_btnThumbnailVideo;
	}
	return NULL;
}

void CToolbarBottom::DoAudioButtonClick()
{
	m_ToolbarBottomWorkFlow.AudioButtonClicked();
}
void CToolbarBottom::DoVideoButtonClick()
{
	m_ToolbarBottomWorkFlow.VideoButtonClicked();
}

void CToolbarBottom::DoParticipantButtonClick()
{
	if(m_pToolbarMgr)
		m_pToolbarMgr->ShowParticipantWindow();
	buttonStatusType btnStatusType = Button_Disabled;
	if(m_bInParticipant)
		btnStatusType = Button_Muted;
	else
		btnStatusType = Button_UnMuted;
	UpdateButton(Toolbar_Button_Participant, btnStatusType);
	
}

void CToolbarBottom::DoWaitingRoomButtonClick()
{
	// if(m_pToolbarMgr)
	// 	m_pToolbarMgr->ShowParticipantWindow();

	
	//::MessageBox(NULL, "", _T("Pure Sonic"), MB_OK);
	/*
	
	listBox1.Items.Add("Item1");
            listBox1.Items.Add("Item2");
            listBox1.Items.Add("Item3");
            List<string> lt = new List<string>();
            string str = null;
            foreach (var item in listBox1.Items)
            {
                lt.Add(item.ToString()); //store the items in the list
                str += item + "\r\n";    //store the items in the string
            }
            MessageBox.Show(str);

	*/
	//::MessageBox(NULL, _T("Hello world"), _T("Pure Sonic"), MB_OK);
	ZOOM_SDK_NAMESPACE::IList<unsigned int>* lstUser = SDKInterfaceWrap::GetInst().GetMeetingWaitingRoomController()->GetWaitingRoomLst();
	ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomController* pUserCtrl = SDKInterfaceWrap::GetInst().GetMeetingWaitingRoomController();
	if (lstUser)
			{
				int count = lstUser->GetCount();
				for (int i = 0; i < count; i++)
				{
					int userId = lstUser->GetItem(i);
					int abc = 132456;
					//OutputDebugStringW(L"Users Lists Ids.= "+abc);
					//OutputDebugStringW(L""+ count);
					//printf("My variable is %d\n", userId);
					ZOOM_SDK_NAMESPACE::IUserInfo* pUserInfo = pUserCtrl->GetWaitingRoomUserInfoByID(userId);
					const wchar_t* username = pUserInfo->GetUserNameW();
					const wchar_t* xyz = L"Do you want to allow ";
					std::wstring s(xyz);
					s += std::wstring(username)+ L"?";
					s = L"" + s;
					OutputDebugStringW(xyz);
					const int result =  ::MessageBox(NULL, s.c_str(), _T("Pure Sonic"), MB_YESNO);
					if (result == IDYES) {
						SDKInterfaceWrap::GetInst().GetMeetingWaitingRoomController()->AdmitToMeeting(userId);
					}
					
				}
				//string s = lstUser.ToString();
				// ::MessageBox(NULL, _T(s), _T("Pure Sonic"), MB_OK);

			}
			

}
void CToolbarBottom::DoChatButtonClick()
{
	if(m_pToolbarMgr)
		m_pToolbarMgr->ShowChatWindow();
	buttonStatusType btnStatusType = Button_Disabled;
	if(m_bInChat)
		btnStatusType = Button_Muted;
	else
		btnStatusType = Button_UnMuted;
	UpdateButton(Toolbar_Button_Chat, btnStatusType);
}

void CToolbarBottom::DoShareButtonClick()
{
	if(m_pToolbarMgr)
		m_pToolbarMgr->ShowShareWindow();
	buttonStatusType btnStatusType = Button_Disabled;
	if(m_bInShare)
		btnStatusType = Button_Muted;
	else
		btnStatusType = Button_UnMuted;
	UpdateButton(Toolbar_Button_Share, btnStatusType);
}

void CToolbarBottom::DoLeaveButtonClick()
{
	m_ToolbarBottomWorkFlow.LeaveButtonClicked();
}

void CToolbarBottom::onBtnClickedResult(ToolbarButtonType btnType, ZOOM_SDK_NAMESPACE::SDKError ret,  bool bOn)
{
	CButtonUI* button = GetButton(btnType);
	if(NULL == button)
		return;

	buttonStatusType btnStatusType = Button_Enabled;
	if(ZOOM_SDK_NAMESPACE::SDKERR_SUCCESS == ret)
	{
		if(bOn)
			btnStatusType = Button_UnMuted;
		else
			btnStatusType = Button_Muted;
	}
	UpdateButton(btnType, btnStatusType);
}

void CToolbarBottom::DoThumbnailVideoClick()
{
	//to do
	//show the gallery view vidoes
	if(m_pToolbarMgr)
	{
		m_pToolbarMgr->ShowGalleryViewWindow();
		buttonStatusType btnStatusType = Button_Disabled;
		if(m_bInGalleryView)
			btnStatusType = Button_Muted;
		else
			btnStatusType = Button_UnMuted;
		UpdateButton(Toolbar_Button_ThumbnailVideo, btnStatusType);
	}
}

void CToolbarBottom::UpdateButtonImage(void* pbutton, wchar_t* strImage, int& nImageRight, int nImageWidth)
{
	CButtonUI* pButton = static_cast<CButtonUI*>(pbutton);
	if (NULL == pButton || NULL == strImage) return;

	int nWidth = max(pButton->GetFixedWidth(), pButton->GetMinWidth());
	wchar_t strDest[128] = {0};
	int nLeft = (nWidth - nImageWidth) / 2;
	nImageRight = nLeft + nImageWidth;
	swprintf_s(strDest,128,_T("dest='%d,4,%d,34'"), nLeft, nImageRight);

	wchar_t strImagePath[128] = {0};
	swprintf_s(strImagePath,128, L"res='%s' restype='ZPIMGRES' source='0,0,%d,30' %s", strImage, nImageWidth, strDest); 
	pButton->SetForeImage(strImagePath);
	swprintf_s(strImagePath,128, L"res='%s' restype='ZPIMGRES' source='%d,0,%d,30' %s", strImage, nImageWidth * 2, nImageWidth * 3, strDest);
	pButton->SetHotForeImage(strImagePath);
	swprintf_s(strImagePath,128, L"res='%s' restype='ZPIMGRES' source='%d,0,%d,30' %s", strImage, nImageWidth, nImageWidth * 2, strDest);
	pButton->SetPushedImage(strImagePath);
}

void CToolbarBottom::UpdateButton(ToolbarButtonType btnNeedUpdate, buttonStatusType btnStatus)
{
	CButtonUI* button = GetButton(btnNeedUpdate);
	if(NULL == button)
		return;

	switch(btnNeedUpdate)
	{
	case Toolbar_Button_Audio:
		{
			if(Button_UnMuted==btnStatus)
			{
				wchar_t* strText = L"Mute Audio";
				button->SetText(strText);
				int nWidth = max(button->GetFixedWidth(), button->GetMinWidth());
				button->SetFixedWidth(nWidth);
				int nImageRight = 0;
				UpdateButtonImage((void*)button, _T("toolbar_btn_mute.png"), nImageRight);
			}
			else if(Button_Muted == btnStatus)
			{
				wchar_t* strText = L"Unmute Audio";
				button->SetText(strText);
				int nWidth = max(button->GetFixedWidth(), button->GetMinWidth());
				button->SetFixedWidth(nWidth);
				int nImageRight = 0;
				UpdateButtonImage((void*)button, _T("toolbar_btn_unmute.png"), nImageRight);				
			}
			else if(Button_Enabled == btnStatus)
			{
				button->SetEnabled(true);
			}
			else if(Button_Disabled == btnStatus)
			{
				button->SetEnabled(false);
			}
		}
		break;
	case Toolbar_Button_Video:
		{
			if(Button_UnMuted == btnStatus)
			{
				wchar_t* strText = L"Hide Video";
				button->SetText(strText);
				int nWidth = max(button->GetFixedWidth(), button->GetMinWidth());
				button->SetFixedWidth(nWidth);
				int nImageRight = 0;
				UpdateButtonImage((void*)button, _T("toolbar_btn_stopVideo.png"), nImageRight);
			}
			else if(Button_Muted == btnStatus)
			{
				wchar_t* strText = L"Start Video";
				button->SetText(strText);
				int nWidth = max(button->GetFixedWidth(), button->GetMinWidth());
				button->SetFixedWidth(nWidth);
				int nImageRight = 0;
				UpdateButtonImage((void*)button, _T("toolbar_btn_startVideo.png"), nImageRight);				
			}
			else if(Button_Enabled == btnStatus)
			{
				button->SetEnabled(true);
			}
			else if(Button_Disabled == btnStatus)
			{
				button->SetEnabled(false);
			}
		}
		break;
	case Toolbar_Button_Invite:
		break;
	case Toolbar_Button_Chat:
		{
			if(Button_UnMuted == btnStatus)
			{
				wchar_t* strText = L"Show Chat";
				button->SetText(strText);
			}
			else if(Button_Muted == btnStatus)
			{
				wchar_t* strText = L"Hide Chat";
				button->SetText(strText);
			}
		}
		break;
	case Toolbar_Button_Participant:
		{
			if(Button_UnMuted == btnStatus)
			{
				wchar_t* strText = L"pant";
				button->SetText(strText);
			}
			else if(Button_Muted == btnStatus)
			{
				wchar_t* strText = L"Hide Participant";
				button->SetText(strText);
			}
		}
		break;
	case Toolbar_Button_ThumbnailVideo:
		{
			if(Button_UnMuted == btnStatus)
			{
				wchar_t* strText = L"Show Gallery";
				button->SetText(strText);
			}
			else if(Button_Muted == btnStatus)
			{
				wchar_t* strText = L"Hide Gallery";
				button->SetText(strText);
			}
		}
		break;
	case Toolbar_Button_Share:
		{
			if(Button_UnMuted == btnStatus)
			{
				wchar_t* strText = L"Share";
				button->SetText(strText);
			}
			else if(Button_Muted == btnStatus)
			{
				wchar_t* strText = L"Stop Share";
				button->SetText(strText);
			}
		}
		break;
	case Toolbar_Button_Leave:
		break;
	default:
		break;
	}
}

void CToolbarBottom::SetGalleryViewStatus(bool bOn)
{
	m_bInGalleryView = bOn;
}

void CToolbarBottom::SetChatUIStatus(bool bOn)
{
	m_bInChat = bOn;
	buttonStatusType btnStatusType = Button_Disabled;
	if(m_bInChat)
		btnStatusType = Button_Muted;
	else
		btnStatusType = Button_UnMuted;
	UpdateButton(Toolbar_Button_Chat, btnStatusType);
}

void CToolbarBottom::SetParticipantUIStatus(bool bOn)
{
	m_bInParticipant = bOn;
}

void CToolbarBottom::SetShareUIStatus(bool bOn)
{
	m_bInShare = bOn;
}

void CToolbarBottom::DoMoreMenuClick()
{
	if (m_pToolbarMgr)
	{
		RECT WndRC = {0};
		GetWindowRect(m_hWnd,&WndRC);
		m_pToolbarMgr->ShowMoreMenuWindow(m_btnMore->GetX()+WndRC.left, WndRC.top);
	}
	
}
