NVCC = nvcc --std c++11
CC = c++ --std=c++11

OPENCV_ARGS = `pkg-config --cflags --libs opencv4`

MODULE_NAME = texture_descriptors
MODULE_PATH = $(addprefix ./${MODULE_NAME}/, *.cpp)

TARGET = build/${MODULE_NAME}

all:
	$(CC) $(MODULE_PATH) -I ${CURDIR} -o ${TARGET}.out ${OPENCV_ARGS}