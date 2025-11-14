// MeetingRoomDialog.h
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

#ifndef MEETING_ROOM_DIALOG_H
#define MEETING_ROOM_DIALOG_H

#include "resource.h"
#include "InternetRoom.h"
#include "NetworkSession.h"
#include "../VideoServices/VideoBuffer.h"

class MeetingRoomDialog
{
public:
	MeetingRoomDialog();
	~MeetingRoomDialog();

	// Display the meeting room dialog
	// Returns TRUE if race started successfully
	// Returns FALSE if user left the room
	BOOL DisplayMeetingRoom(
		HWND pParentWindow,
		MR_InternetRoom* pInternetRoom,
		MR_NetworkSession* pSession,
		MR_VideoBuffer* pVideoBuffer,
		int pRoomIndex
	);

private:
	static BOOL CALLBACK MeetingRoomCallBack(
		HWND pWindow,
		UINT pMsgId,
		WPARAM pWParam,
		LPARAM pLParam
	);

	// Callback helper - stores 'this' pointer in window user data
	BOOL HandleMeetingRoomMessage(
		HWND pWindow,
		UINT pMsgId,
		WPARAM pWParam,
		LPARAM pLParam
	);

	// Event handlers
	void OnReadyClicked(HWND pWindow);
	void OnStartRace(HWND pWindow);
	void OnLeaveRoom(HWND pWindow);
	void OnTimer(HWND pWindow);

	// Helpers
	void UpdatePlayerList(HWND pWindow);
	void UpdateRoomInfo(HWND pWindow);
	void SetReadyStatus(HWND pWindow, BOOL pReady);

	// Member variables
	MR_InternetRoom* mInternetRoom;
	MR_NetworkSession* mSession;
	MR_VideoBuffer* mVideoBuffer;
	HWND mDialogHandle;
	int mRoomIndex;
	BOOL mPlayerReady;
	BOOL mRaceStarted;
	UINT_PTR mTimerID;
};

#endif
