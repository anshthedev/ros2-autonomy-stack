// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/wheel_speeds.hpp"
#include "std_msgs/msg/float64.hpp"


class SpeedAverager : public rclcpp::Node
{
public:
  SpeedAverager()
  : Node("speed_averager")
  {
    publisher_ = this->create_publisher<std_msgs::msg::Float64>("average_speed", 10);
    auto topic_callback =
      [this](robot_interfaces::msg::WheelSpeeds::UniquePtr msg) -> void {
        auto average = (msg->left + msg->right) / 2.0;
        RCLCPP_INFO(this->get_logger(), "Speed Average: %.2f", average);
        auto avg_msg = std_msgs::msg::Float64();
        avg_msg.data = average;
        this->publisher_->publish(avg_msg);
      };
    subscription_ =
      this->create_subscription<robot_interfaces::msg::WheelSpeeds>("wheel_speeds", 10, topic_callback);
  }

private:
  rclcpp::Subscription<robot_interfaces::msg::WheelSpeeds>::SharedPtr subscription_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SpeedAverager>());
  rclcpp::shutdown();
  return 0;
}
