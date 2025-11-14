// RoomListDialog.h
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

#ifndef ROOM_LIST_DIALOG_H
#define ROOM_LIST_DIALOG_H

#include "resource.h"
#include "InternetRoom.h"
#include "NetworkSession.h"
#include "../VideoServices/VideoBuffer.h"

class RoomListDialog
{
public:
	RoomListDialog();
	~RoomListDialog();

	// Display the room list dialog
	// Returns TRUE if user successfully joined or created a room
	// Returns FALSE if user cancelled
	BOOL DisplayRoomList(
		HWND pParentWindow,
		MR_InternetRoom* pInternetRoom,
		MR_NetworkSession* pSession,
		MR_VideoBuffer* pVideoBuffer,
		int& pSelectedRoomIndex
	);

private:
	static BOOL CALLBACK RoomListCallBack(
		HWND pWindow,
		UINT pMsgId,
		WPARAM pWParam,
		LPARAM pLParam
	);

	// Callback helper - stores 'this' pointer in window user data
	BOOL HandleRoomListMessage(
		HWND pWindow,
		UINT pMsgId,
		WPARAM pWParam,
		LPARAM pLParam
	);

	// Event handlers
	void OnRefreshRooms(HWND pWindow);
	void OnCreateRoom(HWND pWindow);
	void OnJoinRoom(HWND pWindow);
	void OnCancel(HWND pWindow);

	// Helpers
	void PopulateRoomList(HWND pWindow);
	void SelectRoom(HWND pWindow, int pRoomIndex);

	// Member variables
	MR_InternetRoom* mInternetRoom;
	MR_NetworkSession* mSession;
	MR_VideoBuffer* mVideoBuffer;
	HWND mDialogHandle;
	int mSelectedRoomIndex;
	BOOL mJoinedSuccessfully;
};

#endif
