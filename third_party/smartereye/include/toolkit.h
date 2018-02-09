#ifndef TOOLKIT_H
#define TOOLKIT_H

#ifdef SMARTEREYESDK_LIBRARY
#define SMARTEREYESDK_EXPORT
#else
#define SMARTEREYESDK_EXPORT extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct ImageFrame{
    unsigned short id;
    unsigned short width;
    unsigned short height;
    unsigned short format;
    unsigned int   size;
    long long      timestamp;
    char           data[0];
};

enum ImageFrame_Format {
    ImageFrame_Format_Gray = 0,
    ImageFrame_Format_TrueColor = 1,
    ImageFrame_Format_YUV422 = 2,
    ImageFrame_Format_Disparity7 = 512,
    ImageFrame_Format_Disparity8,
    ImageFrame_Format_Disparity10,
    ImageFrame_Format_Disparity12,
    ImageFrame_Format_Default  = ImageFrame_Format_Gray,
};
enum ImageFrame_Id {
    ImageFrame_Id_NotUsed        = 0,
    ImageFrame_Id_Left           = 1,
    ImageFrame_Id_Right          = 2,
    ImageFrame_Id_Disparity      = 4,
};
enum CameraType {
    CameraType_ADAS,
    CameraType_Traffic,
};

enum SDKErrorType {
    NoError = 0,
    UnkownError = -1,
    IPAddressError = -2,
    CameraNotConnected = -3,
    CameraInErrorState = -4,
    CameraNotInitialized = -5,
    CameraNotRunning = -6,
    NoCameraFound = -7,

};

typedef void (*FrameReleaser)(struct ImageFrame* frame);
typedef int (*FrameHandler)(const char* camId, struct ImageFrame* frame, FrameReleaser releaser);

SMARTEREYESDK_EXPORT int initSmarterEyeSDK (const char *IpList);
SMARTEREYESDK_EXPORT int deinitSmarterEyeSDK ();
SMARTEREYESDK_EXPORT int startSmarterEyeSDK ();
SMARTEREYESDK_EXPORT int stopSmarterEyeSDK ();
SMARTEREYESDK_EXPORT const char *getSmarterEyeSDKInfo ();
SMARTEREYESDK_EXPORT const char *getCameraList ();
SMARTEREYESDK_EXPORT int setIPAddr (const char* camId, unsigned int ip, unsigned int mask, unsigned int gateway);
SMARTEREYESDK_EXPORT int getIPAddr (const char* camId, unsigned int *ip, unsigned int *mask, unsigned int *gateway);
SMARTEREYESDK_EXPORT int setTransferFrameRate (const char* camId, float rate);
SMARTEREYESDK_EXPORT float getTransferFrameRate (const char* camId);
SMARTEREYESDK_EXPORT const char *getDeviceInfo (const char* camId);
SMARTEREYESDK_EXPORT int setFrameHandler (const char* camId, FrameHandler handler);
SMARTEREYESDK_EXPORT int startCapture (const char* camId);
SMARTEREYESDK_EXPORT int stopCapture (const char* camId);
SMARTEREYESDK_EXPORT float getDistanceFromDisparity(const char* camId, int disparity);

#ifdef __cplusplus
}
#endif

#endif // TOOLKIT_H
