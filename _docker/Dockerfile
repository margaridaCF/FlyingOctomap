# Assuming this is run with the command 
# docker run -it --privileged -v <absolute_path_to_dev_source>:/home/user/ros_workspace:rw -v /tmp/.X11-unix:/tmp/.X11-unix:ro -e DISPLAY=:0 -p 14556:14556/udp --name=px4_gazebo garuda bash

# Base image is "the toolchain including simulation and ROS (incl. MAVROS)" 
# from DroneCode https://dev.px4.io/en/test_and_ci/docker.html
FROM px4io/px4-dev-ros:2017-12-08

MAINTAINER Magarida Faria

# Update the repository sources list
# Download target release of Px4
RUN apt-get update \
	&& sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu `lsb_release -sc` main" > /etc/apt/sources.list.d/ros-latest.list' \
	&& wget http://packages.ros.org/ros.key -O - | sudo apt-key add - \
	&& apt-get update \
	&& apt-get install -y python-catkin-tools gdb ros-kinetic-octomap ros-kinetic-octomap-msgs ros-kinetic-octomap-ros ros-kinetic-pcl-conversions ros-kinetic-pcl-ros geographiclib-tools \
	&& mkdir /Firmware \
	&& cd /Firmware \
	&& wget https://raw.githubusercontent.com/mavlink/mavros/master/mavros/scripts/install_geographiclib_datasets.sh \
	&& chmod +x install_geographiclib_datasets.sh \
	&& sudo  ./install_geographiclib_datasets.sh 