/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "modules/drivers/smartereye/camera_device.h"

#include "modules/common/log.h"

namespace apollo {
namespace drivers {
namespace smartereye {

FrameHandlerFunc SmartereyeDevice::image_handler_s = nullptr;

int FrameHandler(const char* camId, struct ImageFrame* frame, FrameReleaser releaser) {
  ADEBUG << "Receive image from camid: " << camId << ", timestamp: "<< frame->timestamp;
  if (SmartereyeDevice::image_handler_s != nullptr) {
    return SmartereyeDevice::image_handler_s(camId, frame, releaser);
  }
  return -1;
}

SmartereyeDevice::SmartereyeDevice() :
      inited_(false),
      is_capturing_(false) {}

SmartereyeDevice::~SmartereyeDevice() {
  AINFO << "Uninit smartereye device.";
  UninitDevice();
  inited_ = false;
}

int SmartereyeDevice::InitDevice(const CameraConf& camera_config) {
  config_ = camera_config;
  int ret =  NoError; // NoError: 0

  if (config_.camera_dev().size() == 0) {
    AERROR << "Camera_dev must be ip address for smartereye camera";
    return ErrorCode::DRIVER_ERROR_SMARTEREYE;
  }
  ret = initSmarterEyeSDK(config_.camera_dev().c_str());
  if (ret != NoError) {
    AERROR << "InitSmarterEyeSDK error. ret: " << ret;
    return ErrorCode::DRIVER_ERROR_SMARTEREYE;
  }
  ret = startSmarterEyeSDK();
  if (ret != NoError) {
    AERROR << "StartSmarterEyeSDK error. ret: " << ret;
    return ErrorCode::DRIVER_ERROR_SMARTEREYE;
  }

  auto sdk_info = getSmarterEyeSDKInfo();
  if (sdk_info != NULL) {
    AINFO << "SmarterEye SDK Info: " << sdk_info;
    free((void*)sdk_info);
  }
  inited_ = true;
  AINFO << "Init smartereye finished.";
  return ErrorCode::OK;
}

int SmartereyeDevice::StartCapture() {
  if (!inited_) {
    AERROR << "Please init device first.";
    return ErrorCode::DRIVER_ERROR_SMARTEREYE;
  }
  const char* cam_list = getCameraList();
  if (cam_list == NULL) {
    AERROR << "getCameraList null value error.";
    return ErrorCode::DRIVER_ERROR_SMARTEREYE;
  }
  char* strdup_cam_list = strdup(cam_list);
  ParseCameraList(strdup_cam_list);
  free((void*)strdup_cam_list);
  free((void*)cam_list);
  if (camera_list_.empty()) {
    AERROR << "ParseCameraList error.";
    return ErrorCode::DRIVER_ERROR_SMARTEREYE;
  }
  for (auto& cam : camera_list_) {
    setFrameHandler(cam.id.c_str(), FrameHandler);
    int ret = startCapture(cam.id.c_str());
    if (ret != NoError) {
      AERROR << "StartCapture error. cam.id: " << cam.id << ", ret: " << ret;
      return ErrorCode::DRIVER_ERROR_SMARTEREYE;
    }
    AINFO << "Start caputre successfully. cam.id: " << cam.id;
  }
  is_capturing_ = true;
  return ErrorCode::OK;
}

void SmartereyeDevice::ParseCameraList(const char* cam_list) {
  AINFO << "Cam_list: " << cam_list;
  Json cams = Json::parse(cam_list);
  struct CameraInfo cam;
  cam.id = cams[config_.camera_dev().c_str()]["ID"].get<std::string>();
  cam.name = cams[config_.camera_dev().c_str()]["name"].get<std::string>();
  cam.type = cams[config_.camera_dev().c_str()]["type"].get<int>();
  camera_list_.push_back(cam);
}

int SmartereyeDevice::StopCapture() {
  for (auto& cam : camera_list_) {
    stopCapture(cam.id.c_str());
  }
  is_capturing_ = false;
  AINFO << "Stop capture.";
  return ErrorCode::OK;
}

int SmartereyeDevice::UninitDevice() {
  if (!inited_) {
    AERROR << "Device not inited.";
    return ErrorCode::OK;
  }
  StopCapture();
  stopSmarterEyeSDK();
  deinitSmarterEyeSDK();

  inited_ = false;
  AINFO << "Uninit device finished.";
  return ErrorCode::OK;
}

}  // namespace smartereye 
}  // namespace drivers 
}  // namespace apollo 
