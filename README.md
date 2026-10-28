# ros2-autonomy-stack

C++ / ROS 2 motion planning and state estimation, built in simulation in three stages: a ground robot, a PX4 quadrotor, and a batch failure-analysis harness.

## Roadmap

- [ ] **Stage 1: Ground robot.** A* path planning, pure pursuit path following, and a custom EKF in Gazebo.
- [ ] **Stage 2: Quadrotor.** 3D A*, RRT*, minimum-snap trajectories, and a drone state estimator with PX4 SITL.
- [ ] **Stage 3: Evaluation.** Randomized scenario batches, planner comparison, and long-tail failure analysis.

## Tech stack

- C++17, Eigen, GoogleTest
- ROS 2 Jazzy, Gazebo, PX4 SITL
- Python (pandas, matplotlib) for analysis
- Docker

## Development setup (macOS, Docker)

ROS 2 runs inside a Docker container, so no native install is needed.

1. Install [Docker Desktop](https://www.docker.com/products/docker-desktop/) and clone this repo.
2. From the repo root, start the container:

```bash
docker run -d --name ros-jazzy \
  -p 6080:80 \
  --security-opt seccomp=unconfined \
  --shm-size=512m \
  -v "$(pwd)/src:/home/ubuntu/ros2_ws/src" \
  tiryoh/ros2-desktop-vnc:jazzy
```

3. Open http://localhost:6080 for the Ubuntu desktop (GUI tools like RViz2 appear here).
4. Verify ROS 2 works in a desktop terminal:

```bash
source /opt/ros/jazzy/setup.bash
ros2 run demo_nodes_cpp talker
```

5. Optional: use VS Code with the Dev Containers extension. Run "Dev Containers: Attach to Running Container", choose `ros-jazzy`, open `/home/ubuntu/ros2_ws`, then run `su - ubuntu` in its terminal.

Build the workspace inside the container:

```bash
cd ~/ros2_ws
source /opt/ros/jazzy/setup.bash
colcon build
source install/setup.bash
```

Start and stop the container with `docker start ros-jazzy` and `docker stop ros-jazzy`.

## Repository layout

- `src/`: ROS 2 packages (mounted into the container at `~/ros2_ws/src`)
- Build output (`build/`, `install/`, `log/`) stays inside the container and is git-ignored.

## Status

Environment setup complete. Stage 1 in progress.