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

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "robot_interfaces/msg/wheel_speeds.hpp"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses a fancy C++11 lambda
 * function to shorten the callback syntax, at the expense of making the
 * code somewhat more difficult to understand at first glance. */

 // Inheritance for SpeedPublisher to be a Node
class SpeedPublisher : public rclcpp::Node
{
public:
  SpeedPublisher()
  : Node("speed_publisher"), count_(0) // initalization
  {
    publisher_ = this->create_publisher<robot_interfaces::msg::WheelSpeeds>("wheel_speeds", 10);
    auto timer_callback =
      [this]() -> void { // this happens every loop
        auto message = robot_interfaces::msg::WheelSpeeds();
        message.left = 0.5;
        message.right = 0.1 * (this->count_++ % 10);
        RCLCPP_INFO(this->get_logger(), "Publishing: left=%.2f right=%.2f", message.left, message.right);
        this->publisher_->publish(message);
      };
    timer_ = this->create_wall_timer(500ms, timer_callback);
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<robot_interfaces::msg::WheelSpeeds>::SharedPtr publisher_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SpeedPublisher>());
  rclcpp::shutdown();
  return 0;
}
