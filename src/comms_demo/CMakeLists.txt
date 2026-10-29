cmake_minimum_required(VERSION 3.8)
project(comms_demo)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_interfaces REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(speed_publisher src/speed_publisher.cpp)
ament_target_dependencies(speed_publisher rclcpp robot_interfaces)

add_executable(wheel_listener src/wheel_listener.cpp)
ament_target_dependencies(wheel_listener rclcpp robot_interfaces)

add_executable(speed_averager src/speed_averager.cpp)
ament_target_dependencies(speed_averager rclcpp robot_interfaces std_msgs)

install(TARGETS
  speed_publisher
  wheel_listener
  speed_averager
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
DESTINATION share/${PROJECT_NAME})

ament_package()
