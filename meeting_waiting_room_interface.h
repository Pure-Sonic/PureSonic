 #include "sdk_util.h"
 

 class IMeetingWaitingRoomEvent
 {
 public:
     virtual void onWatingRoomUserJoin(unsigned int userID) = 0;
 
     virtual void onWatingRoomUserLeft(unsigned int userID) = 0;
 
 };
 class IMeetingWaitingRoomController
 {
 public:
     virtual ZOOM_SDK_NAMESPACE::SDKError SetEvent(IMeetingWaitingRoomEvent* pEvent) = 0;
 
     virtual bool IsSupportWaitingRoom() = 0;
 
     virtual bool IsWaitingRoomOnEntryFlagOn() = 0;
 
     virtual ZOOM_SDK_NAMESPACE::SDKError EnableWaitingRoomOnEntry(bool bEnable) = 0;
 
     virtual ZOOM_SDK_NAMESPACE::IList<unsigned int >* GetWaitingRoomLst() = 0;
 
     virtual ZOOM_SDK_NAMESPACE::IUserInfo* GetWaitingRoomUserInfoByID(unsigned int userid) = 0;
 
     virtual ZOOM_SDK_NAMESPACE::SDKError AdmitToMeeting(unsigned int userid) = 0;
 
     virtual ZOOM_SDK_NAMESPACE::SDKError PutInWaitingRoom(unsigned int userid) = 0;
 };

