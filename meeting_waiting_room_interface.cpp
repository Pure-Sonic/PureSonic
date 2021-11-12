#include "stdafx.h"
#include "meeting_waiting_room_interface.h";

ZOOM_SDK_NAMESPACE::SDKError IMeetingWaitingRoomController::SetEvent(IMeetingWaitingRoomEvent* pEvent) {
	ZOOM_SDK_NAMESPACE::SDKError err;
	return err;
}
bool IMeetingWaitingRoomController::IsSupportWaitingRoom() {
	return true;
}

bool IMeetingWaitingRoomController::IsWaitingRoomOnEntryFlagOn() {
	return true;
}
ZOOM_SDK_NAMESPACE::SDKError IMeetingWaitingRoomController::EnableWaitingRoomOnEntry(bool bEnable) {
	ZOOM_SDK_NAMESPACE::SDKError err;
	return err;
}
ZOOM_SDK_NAMESPACE::IList<unsigned int >* IMeetingWaitingRoomController::GetWaitingRoomLst() {
	ZOOM_SDK_NAMESPACE::IList<unsigned int >* second;
	return second;
}
ZOOM_SDK_NAMESPACE::IUserInfo* IMeetingWaitingRoomController::GetWaitingRoomUserInfoByID(unsigned int userid) {
	ZOOM_SDK_NAMESPACE::IUserInfo* thrd;
	return thrd;
}
ZOOM_SDK_NAMESPACE::SDKError IMeetingWaitingRoomController::AdmitToMeeting(unsigned int userid) {
	ZOOM_SDK_NAMESPACE::SDKError err;
	return err;
}
ZOOM_SDK_NAMESPACE::SDKError IMeetingWaitingRoomController::PutInWaitingRoom(unsigned int userid) {
	ZOOM_SDK_NAMESPACE::SDKError err;
	return err;
}