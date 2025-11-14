// MeetingRoomDialog.cpp
//
// Copyright (c) 1995-1998 - Richard Langlois and Grokksoft Inc.
//
// Licensed under GrokkSoft HoverRace SourceCode License v1.0(the "License");
// you may not use this file except in compliance with the License.
//
// A copy of the license should have been attached to the package from which 
// you have taken this file. If you can not find the license you can not use 
// this file.
//
// The author makes no representations about the suitability of
// this software for any purpose.  It is provided "as is" "AS IS",
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
//
// See the License for the specific language governing permissions 
// and limitations under the License.
//

#include "stdafx.h"
#include "MeetingRoomDialog.h"
#include "resource.h"
#include <windowsx.h>

#define MEETING_ROOM_TIMER_ID 1001
#define MEETING_ROOM_UPDATE_INTERVAL 500  // Update every 500ms

MeetingRoomDialog::MeetingRoomDialog()
	: mInternetRoom(NULL), mSession(NULL), mVideoBuffer(NULL),
	  mDialogHandle(NULL), mRoomIndex(-1), mPlayerReady(FALSE),
	  mRaceStarted(FALSE), mTimerID(0)
{
}

MeetingRoomDialog::~MeetingRoomDialog()
{
	if (mTimerID != 0)
	{
		KillTimer(mDialogHandle, mTimerID);
		mTimerID = 0;
	}
}

BOOL MeetingRoomDialog::DisplayMeetingRoom(
	HWND pParentWindow,
	MR_InternetRoom* pInternetRoom,
	MR_NetworkSession* pSession,
	MR_VideoBuffer* pVideoBuffer,
	int pRoomIndex)
{
	mInternetRoom = pInternetRoom;
	mSession = pSession;
	mVideoBuffer = pVideoBuffer;
	mRoomIndex = pRoomIndex;
	mPlayerReady = FALSE;
	mRaceStarted = FALSE;

	// Create modeless dialog
	mDialogHandle = CreateDialogParam(
		AfxGetResourceHandle(),
		MAKEINTRESOURCE(IDD_MEETING_ROOM),
		pParentWindow,
		MeetingRoomCallBack,
		(LPARAM)this
	);

	if (!mDialogHandle)
		return FALSE;

	// Show the dialog
	ShowWindow(mDialogHandle, SW_SHOW);

	// Message loop for dialog
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(mDialogHandle, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!IsWindow(mDialogHandle))
			break;
	}

	return mRaceStarted;
}

BOOL CALLBACK MeetingRoomDialog::MeetingRoomCallBack(
	HWND pWindow,
	UINT pMsgId,
	WPARAM pWParam,
	LPARAM pLParam)
{
	MeetingRoomDialog* lThis = NULL;

	if (pMsgId == WM_INITDIALOG)
	{
		lThis = (MeetingRoomDialog*)pLParam;
		SetWindowLong(pWindow, GWL_USERDATA, (LONG)lThis);
	}
	else
	{
		lThis = (MeetingRoomDialog*)GetWindowLong(pWindow, GWL_USERDATA);
	}

	if (lThis)
		return lThis->HandleMeetingRoomMessage(pWindow, pMsgId, pWParam, pLParam);

	return FALSE;
}

BOOL MeetingRoomDialog::HandleMeetingRoomMessage(
	HWND pWindow,
	UINT pMsgId,
	WPARAM pWParam,
	LPARAM /*pLParam*/)
{
	switch (pMsgId)
	{
	case WM_INITDIALOG:
		mDialogHandle = pWindow;
		UpdateRoomInfo(pWindow);
		UpdatePlayerList(pWindow);
		
		// Set up timer for periodic updates
		mTimerID = SetTimer(pWindow, MEETING_ROOM_TIMER_ID, MEETING_ROOM_UPDATE_INTERVAL, NULL);
		return TRUE;

	case WM_TIMER:
		if (pWParam == MEETING_ROOM_TIMER_ID)
		{
			OnTimer(pWindow);
			return TRUE;
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(pWParam))
		{
		case IDC_READY_BUTTON:
			OnReadyClicked(pWindow);
			return TRUE;

		case IDC_START_RACE:
			OnStartRace(pWindow);
			return TRUE;

		case IDC_LEAVE_ROOM:
			OnLeaveRoom(pWindow);
			return TRUE;

		case IDCANCEL:
			OnLeaveRoom(pWindow);
			return TRUE;
		}
		break;

	case WM_DESTROY:
		if (mTimerID != 0)
		{
			KillTimer(pWindow, mTimerID);
			mTimerID = 0;
		}
		EndDialog(pWindow, 0);
		return TRUE;
	}

	return FALSE;
}

void MeetingRoomDialog::OnReadyClicked(HWND pWindow)
{
	mPlayerReady = !mPlayerReady;
	SetReadyStatus(pWindow, mPlayerReady);

	// Signal ready status to other players via InternetRoom
	// This would be integrated with the InternetRoom's chat/status update system
}

void MeetingRoomDialog::OnStartRace(HWND pWindow)
{
	if (!mInternetRoom || !mSession)
		return;

	// Only allow game master to start race
	// Check if current user is the room creator/host
	MessageBox(pWindow, "Starting race...", "Race Start", MB_OK | MB_ICONINFORMATION);
	
	mRaceStarted = TRUE;
	DestroyWindow(pWindow);
}

void MeetingRoomDialog::OnLeaveRoom(HWND pWindow)
{
	if (!mInternetRoom)
		return;

	// Note: LeaveGameOp is handled by the main DisplayChatRoom dialog
	// Just close this meeting room dialog
	mRaceStarted = FALSE;
	DestroyWindow(pWindow);
}

void MeetingRoomDialog::OnTimer(HWND pWindow)
{
	// Update player list periodically from InternetRoom data
	UpdatePlayerList(pWindow);
	UpdateRoomInfo(pWindow);
}

void MeetingRoomDialog::UpdatePlayerList(HWND pWindow)
{
	HWND lPlayerList = GetDlgItem(pWindow, IDC_PLAYER_LIST);
	if (!lPlayerList)
		return;

	// Get player data from mInternetRoom
	// The InternetRoom class maintains mClientList and mGameList
	// This would iterate through and display connected players
	
	// Placeholder implementation:
	// For each player in current room:
	//   - Get player name
	//   - Check if ready status
	//   - Add to list box or list view
}

void MeetingRoomDialog::UpdateRoomInfo(HWND pWindow)
{
	if (!mInternetRoom)
		return;

	// Update room name from InternetRoom's current game
	HWND lRoomNameStatic = GetDlgItem(pWindow, IDC_ROOM_NAME);
	if (lRoomNameStatic)
	{
		// Would get from mInternetRoom->GetCurrentGameName()
		SetWindowText(lRoomNameStatic, "Internet Meeting Room");
	}

	// Update player count
	HWND lPlayerCountStatic = GetDlgItem(pWindow, IDC_PLAYER_COUNT);
	if (lPlayerCountStatic)
	{
		char lCountText[128];
		// Would get actual count from mInternetRoom
		sprintf_s(lCountText, sizeof(lCountText), "Players: 2/4");
		SetWindowText(lPlayerCountStatic, lCountText);
	}

	// Update race list (games available in this room)
	HWND lTextEdit = GetDlgItem(pWindow, IDC_TEXT);
	if (lTextEdit)
	{
		// Display list of races/games in this room
		// Each race that hasn't started yet should be available to join
		SetWindowText(lTextEdit, "Available Races:\r\n- Practice Race 1\r\n- Championship Qualifier");
	}
}

void MeetingRoomDialog::SetReadyStatus(HWND pWindow, BOOL pReady)
{
	HWND lReadyButton = GetDlgItem(pWindow, IDC_READY_BUTTON);
	if (lReadyButton)
	{
		SetWindowText(lReadyButton, pReady ? "Ready!" : "Ready");
	}
}

