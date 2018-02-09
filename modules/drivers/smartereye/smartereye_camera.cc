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

/**
 * @file
 */

#include "modules/drivers/smartereye/smartereye_camera.h"
#include "sensor_msgs/Image.h"

/**
 * @namespace apollo::drivers::smartereye
 * @brief apollo::drivers
 */
namespace apollo {
namespace drivers {
namespace smartereye {

std::string SmartereyeCamera::Name() const {
  return FLAGS_sensor_node_name;
}

apollo::common::Status SmartereyeCamera::Init() {
  AdapterManager::Init(FLAGS_adapter_config_filename);
  AINFO << "The adapter manager is successfully initialized.";

  // load camera config
  if (!::apollo::common::util::GetProtoFromFile(FLAGS_sensor_conf_file,
                                                &camera_config_)) {
    return OnError("Unable to load smartereye camera conf file: " +
                   FLAGS_sensor_conf_file);
  }

  AINFO << "The smartereye conf file is loaded: " << FLAGS_sensor_conf_file;
  ADEBUG << "Smartereye_conf:" << camera_config_.ShortDebugString();

  // Init smartereye device
  smartereye_device_.reset(new SmartereyeDevice());
  if (smartereye_device_->InitDevice(camera_config_) != ErrorCode::OK) {
    return OnError("Failed to init smartereye device.");
  }

  // set sensor image with default value
  {
    short_image_.reset(new sensor_msgs::Image());
    short_image_->header.frame_id = camera_config_.frame_id();
    short_image_->encoding = camera_config_.pixel_format();
    short_image_->is_bigendian = 0;

    long_image_.reset(new sensor_msgs::Image());
    long_image_->header.frame_id = camera_config_.frame_id();
    long_image_->encoding = camera_config_.pixel_format();
    long_image_->is_bigendian = 0;
  }

  AINFO << "The smartereye device is successfully initialized.";
  return Status::OK();
}

apollo::common::Status SmartereyeCamera::Start() {
  // 1. set frame callback handler
  SmartereyeDevice::image_handler_s = std::bind(&SmartereyeCamera::ProcessImage, this,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
  // 2. start capturing 
  if (smartereye_device_->StartCapture() != ErrorCode::OK) {
    return OnError("Failed to start capture");
  }
  AINFO << "Device start capturing.";

  // 3. last step: publish monitor messages
  apollo::common::monitor::MonitorLogBuffer buffer(&monitor_logger_);
  buffer.INFO("Smartereye is started.");

  return Status::OK();
}

void SmartereyeCamera::Stop() {
  smartereye_device_->StopCapture();
  AINFO << "Device stop capturing.";
}

int SmartereyeCamera::ProcessImage(const char* cam_id, struct ImageFrame* frame, 
		                   FrameReleaser releaser) {
  ADEBUG << "receive image id: " << frame->id << ", width: " << frame->width
    << ", height: " << frame->height << ", timestamp: " << frame->timestamp
    << ", size: " << frame->size<< ", format: " << frame->format;

  // left camera image frame received
  if (frame->id == ImageFrame_Id_Left) {
    short_image_->header.stamp.sec = frame->timestamp / 1000;
    short_image_->header.stamp.nsec = frame->timestamp % 1000;
    if (camera_config_.use_host_time()) {
      auto current_time = apollo::common::time::AsInt64<nanos>(Clock::Now());
      short_image_->header.stamp.sec = current_time / 1e9;
      short_image_->header.stamp.nsec = current_time % 1000000000;
    }

    short_image_->width = frame->width;
    short_image_->height = frame->height;
    short_image_->step = 2 * frame->width; 
    short_image_->data.resize(frame->size);
    memcpy((void*)(&short_image_->data[0]), (void*)frame->data, frame->size);

    AdapterManager::PublishImageShort(*short_image_);
    ADEBUG << "publish short image finished. timestamp: " << frame->timestamp;
    releaser(frame);
  } 
  // right camera image frame received
  if (frame->id == ImageFrame_Id_Right) {
    long_image_->header.stamp.sec = frame->timestamp / 1000;
    long_image_->header.stamp.nsec = frame->timestamp % 1000;
    if (camera_config_.use_host_time()) {
      auto current_time = apollo::common::time::AsInt64<nanos>(Clock::Now());
      long_image_->header.stamp.sec = current_time / 1e9;
      long_image_->header.stamp.nsec = current_time % 1000000000;
    }

    long_image_->width = frame->width;
    long_image_->height = frame->height;
    long_image_->step = 2 * frame->width; 
    long_image_->data.resize(frame->size);
    memcpy((void*)(&long_image_->data[0]), (void*)frame->data, frame->size);

    AdapterManager::PublishImageLong(*long_image_);
    ADEBUG << "publish long image finished. timestamp: " << frame->timestamp;
    releaser(frame);
  } 

  return ErrorCode::OK;
}

// Send the error to monitor and return it
Status SmartereyeCamera::OnError(const std::string &error_msg) {
  apollo::common::monitor::MonitorLogBuffer buffer(&monitor_logger_);
  buffer.ERROR(error_msg);
  return Status(ErrorCode::DRIVER_ERROR_SMARTEREYE, error_msg);
}

}  // namespace smartereye 
}  // namespace drivers
}  // namespace apollo
