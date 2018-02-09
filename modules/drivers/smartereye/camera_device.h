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

#ifndef MODULES_DRIVERS_SMARTEREYE_CAMERA_DEVICE_H_
#define MODULES_DRIVERS_SMARTEREYE_CAMERA_DEVICE_H_

#include <string>
#include <vector>

#include "modules/drivers/smartereye/proto/smartereye_conf.pb.h"
#include "modules/common/proto/error_code.pb.h"
#include "modules/common/macro.h"
#include "modules/common/time/time.h"
#include "modules/common/util/json_util.h"
#include "third_party/json/json.hpp"
#include "third_party/smartereye/include/toolkit.h"

namespace apollo {
namespace drivers {
namespace smartereye {

using apollo::common::ErrorCode;
using apollo::common::util::JsonUtil;
using Json = nlohmann::json;
using apollo::drivers::smartereye::CameraConf;

typedef std::function<int (const char* camId, struct ImageFrame* frame, 
		FrameReleaser releaser)> FrameHandlerFunc;
int FrameHandler(const char* camId, struct ImageFrame* frame, FrameReleaser releaser);

struct CameraInfo {
  std::string id;
  std::string name;
  int type; // 0: stereo camera   1: trafficlight camera
};

class SmartereyeDevice {
 public:
  SmartereyeDevice();
  virtual ~SmartereyeDevice();

  int InitDevice(const CameraConf& camera_config);
  int UninitDevice();
  int StartCapture();
  int StopCapture();
  bool IsCapturing() { return is_capturing_; }

  static FrameHandlerFunc image_handler_s;

 private:
  void ParseCameraList(const char* cam_list);
 
 private:
  bool inited_;
  bool is_capturing_;
  CameraConf config_;
  std::vector<CameraInfo> camera_list_;
};

}
}
}

#endif  // MODULES_DRIVERS_SMARTEREYE_CAMERA_DEVICE_H
