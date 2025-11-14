// RoomListDialog.cpp
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
#include "RoomListDialog.h"
#include "MeetingRoomDialog.h"
#include "resource.h"
#include <windowsx.h>

RoomListDialog::RoomListDialog()
	: mInternetRoom(NULL), mSession(NULL), mVideoBuffer(NULL),
	  mDialogHandle(NULL), mSelectedRoomIndex(-1), mJoinedSuccessfully(FALSE)
{
}

RoomListDialog::~RoomListDialog()
{
}

BOOL RoomListDialog::DisplayRoomList(
	HWND pParentWindow,
	MR_InternetRoom* pInternetRoom,
	MR_NetworkSession* pSession,
	MR_VideoBuffer* pVideoBuffer,
	int& /*pSelectedRoomIndex*/)
{
	mInternetRoom = pInternetRoom;
	mSession = pSession;
	mVideoBuffer = pVideoBuffer;
	mSelectedRoomIndex = -1;
	mJoinedSuccessfully = FALSE;

	// InternetRoom already has a chat room display dialog
	// Use that instead of creating a new one
	if (mInternetRoom && mInternetRoom->DisplayChatRoom(pParentWindow, mSession, mVideoBuffer))
	{
		mJoinedSuccessfully = TRUE;
		
		// Now show the meeting room dialog
		MeetingRoomDialog lMeetingRoom;
		if (lMeetingRoom.DisplayMeetingRoom(pParentWindow, mInternetRoom, mSession, mVideoBuffer, 0))
		{
			// Race started successfully
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CALLBACK RoomListDialog::RoomListCallBack(
	HWND pWindow,
	UINT pMsgId,
	WPARAM pWParam,
	LPARAM pLParam)
{
	RoomListDialog* lThis = NULL;

	if (pMsgId == WM_INITDIALOG)
	{
		lThis = (RoomListDialog*)pLParam;
		SetWindowLong(pWindow, GWL_USERDATA, (LONG)lThis);
	}
	else
	{
		lThis = (RoomListDialog*)GetWindowLong(pWindow, GWL_USERDATA);
	}

	if (lThis)
		return lThis->HandleRoomListMessage(pWindow, pMsgId, pWParam, pLParam);

	return FALSE;
}

BOOL RoomListDialog::HandleRoomListMessage(
	HWND pWindow,
	UINT pMsgId,
	WPARAM pWParam,
	LPARAM pLParam)
{
	switch (pMsgId)
	{
	case WM_INITDIALOG:
		mDialogHandle = pWindow;
		PopulateRoomList(pWindow);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(pWParam))
		{
		case IDC_REFRESH_ROOMS:
			OnRefreshRooms(pWindow);
			return TRUE;

		case IDC_CREATE_ROOM:
			OnCreateRoom(pWindow);
			return TRUE;

		case IDC_JOIN_ROOM:
			OnJoinRoom(pWindow);
			return TRUE;

		case IDCANCEL:
			OnCancel(pWindow);
			return TRUE;
		}
		break;

	case WM_DESTROY:
		EndDialog(pWindow, 0);
		return TRUE;
	}

	return FALSE;
}

void RoomListDialog::OnRefreshRooms(HWND /*pWindow*/)
{
	if (!mInternetRoom)
		return;

	// The InternetRoom class handles its own refresh
	// This would be called automatically by the chat room dialog
}

void RoomListDialog::OnCreateRoom(HWND pWindow)
{
	if (!mInternetRoom || !mSession || !mVideoBuffer)
		return;

	// Room creation is handled by DisplayChatRoom dialog
	// Display the room dialog which allows creating/joining rooms
	MessageBox(pWindow, "Room creation is handled in the Internet Room dialog.", "Create Room", MB_OK | MB_ICONINFORMATION);
}

void RoomListDialog::OnJoinRoom(HWND pWindow)
{
	// Room joining is handled by DisplayChatRoom dialog
	MessageBox(pWindow, "Room joining is handled in the Internet Room dialog.", "Join Room", MB_OK | MB_ICONINFORMATION);
}

void RoomListDialog::OnCancel(HWND pWindow)
{
	mJoinedSuccessfully = FALSE;
	mSelectedRoomIndex = -1;
	DestroyWindow(pWindow);
}

void RoomListDialog::PopulateRoomList(HWND pWindow)
{
	if (!mInternetRoom)
		return;

	HWND lListBox = GetDlgItem(pWindow, IDC_ROOM_LIST);
	if (!lListBox)
		return;

	// The room list is managed by InternetRoom's internal game list
	// This will be populated by the chat room dialog which already
	// handles room list display and updates
}

void RoomListDialog::SelectRoom(HWND pWindow, int pRoomIndex)
{
	if (!mInternetRoom)
		return;

	// Room selection is handled by DisplayChatRoom dialog
	// This just closes this wrapper dialog
	mJoinedSuccessfully = TRUE;
	mSelectedRoomIndex = pRoomIndex;
	DestroyWindow(pWindow);
}

