FROM nvidia/cuda:11.0-cudnn8-devel-ubuntu18.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y locales \
    && sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales \
    && update-locale LANG=en_US.UTF-8

ENV LANG en_US.UTF-8 
ENV LC_ALL en_US.UTF-8

RUN apt-get update -y && apt-get install -y \
    software-properties-common build-essential \
    libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev \
    curl unzip g++ cmake wget bash-completion \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

# ~~ Build and install OpenCV
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/master.zip \
    && unzip opencv.zip \
    && mkdir -p build \
    && cd build \
    && cmake -D CMAKE_BUILD_TYPE=Release \
    -D OPENCV_GENERATE_PKGCONFIG=YES \
    -D CMAKE_INSTALL_PREFIX=/usr/local ../opencv-master \
    && make install
