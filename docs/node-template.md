# ROS 2 C++ node template

My cheat sheet for adding a node. Replace `my_pkg`, `my_node`, and message types with real names.

## Wiring checklist (every new node touches all of these)

1. **`src/my_node.cpp`**: a class inheriting `rclcpp::Node`, plus `main()` with `init`, `spin`, `shutdown`.
2. **`package.xml`**: a `<depend>` for every package whose headers I `#include`.
3. **`CMakeLists.txt`**, four things:
   - `find_package(<pkg> REQUIRED)` for each dependency
   - `add_executable(my_node src/my_node.cpp)`
   - `ament_target_dependencies(my_node rclcpp <other pkgs>)`
   - add `my_node` to `install(TARGETS ...)`
4. **Build, source, run.**

If I `#include` something, it must be in all three places: `package.xml`, `find_package`, and `ament_target_dependencies`.

## package.xml

```xml
<depend>rclcpp</depend>
<depend>std_msgs</depend>
<depend>robot_interfaces</depend>
<exec_depend>ros2launch</exec_depend>
```

## CMakeLists.txt

```cmake
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(my_node src/my_node.cpp)
ament_target_dependencies(my_node rclcpp std_msgs)

install(TARGETS
  my_node
  DESTINATION lib/${PROJECT_NAME})

# only if the package has launch files
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

ament_package()
```

## Publisher node (timer + parameter)

```cpp
#include <chrono>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

class MyPublisher : public rclcpp::Node
{
public:
  MyPublisher()
  : Node("my_publisher"), count_(0)
  {
    declare_parameter("rate_hz", 2.0);
    double rate = get_parameter("rate_hz").as_double();

    publisher_ = create_publisher<std_msgs::msg::Float64>("my_topic", 10);
    auto period = std::chrono::duration<double>(1.0 / rate);
    timer_ = create_wall_timer(period, [this]() {
      auto msg = std_msgs::msg::Float64();
      msg.data = count_++;
      RCLCPP_INFO(get_logger(), "Publishing: %.2f", msg.data);
      publisher_->publish(msg);
    });
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr publisher_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MyPublisher>());
  rclcpp::shutdown();
  return 0;
}
```

## Subscriber node

```cpp
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

class MySubscriber : public rclcpp::Node
{
public:
  MySubscriber()
  : Node("my_subscriber")
  {
    subscription_ = create_subscription<std_msgs::msg::Float64>(
      "my_topic", 10,
      [this](const std_msgs::msg::Float64 & msg) {
        RCLCPP_INFO(get_logger(), "I heard: %.2f", msg.data);
      });
  }

private:
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MySubscriber>());
  rclcpp::shutdown();
  return 0;
}
```

## Node that subscribes AND publishes

- Keep both as members: `subscription_` and `publisher_`.
- Use `publisher_->publish(...)` inside the subscription's lambda (the lambda captures `this`).

## Custom message (separate interfaces package)

- Create the package, then `msg/Name.msg` (CamelCase file name).
- `CMakeLists.txt`: `find_package(rosidl_default_generators REQUIRED)` and `rosidl_generate_interfaces(${PROJECT_NAME} "msg/Name.msg")`.
- `package.xml`: `<buildtool_depend>rosidl_default_generators</buildtool_depend>`, `<exec_depend>rosidl_default_runtime</exec_depend>`, `<member_of_group>rosidl_interface_packages</member_of_group>`.
- In code: `#include "robot_interfaces/msg/name.hpp"` (snake_case) and type `robot_interfaces::msg::Name`.
- Build the interfaces package first, then source, then build the packages that use it.

## Launch file (`launch/demo.launch.py`)

```python
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='my_pkg',
            executable='my_publisher',
            parameters=[{'rate_hz': 5.0}],
        ),
        Node(
            package='my_pkg',
            executable='my_subscriber',
            output='screen',
        ),
    ])
```

## Commands

```bash
# in the container, as ubuntu
source /opt/ros/jazzy/setup.bash
cd ~/ros2_ws
colcon build --packages-select my_pkg
source install/setup.bash
ros2 run my_pkg my_node
ros2 launch my_pkg demo.launch.py
```

Useful checks: `ros2 node list`, `ros2 topic list -t`, `ros2 topic echo /my_topic`, `ros2 interface show <pkg>/msg/<Name>`, `ros2 param list`, `rqt_graph`.

## Common errors

- **Package not found:** forgot `source install/setup.bash`.
- **Header not found:** missing `find_package`, `ament_target_dependencies`, or `<depend>`; or the interfaces package wasn't built first.
- **Executable not found:** missing from `install(TARGETS ...)`, or the name differs from `add_executable`.
- **Launch file not found:** missing `install(DIRECTORY launch ...)`; rebuild and source.
- **Permission errors:** built as `root` once; delete `build/ install/ log/` and rebuild as `ubuntu`.
- **Container shell is root:** run `su - ubuntu`.