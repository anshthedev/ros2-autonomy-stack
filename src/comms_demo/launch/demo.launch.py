from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='comms_demo',
            executable='speed_publisher',
            parameters=[{'rate_hz': 5.0, 'max_speed': 0.8}],
        ),
        Node(
            package='comms_demo',
            executable='wheel_listener',
            output='screen',
        ),
        Node(
            package='comms_demo',
            executable='speed_averager',
            output='screen',
        ),
    ])