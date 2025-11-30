CXX := g++
CXXFLAGS_VERSION := -std=c++23

DEBUG_FLAGS := -O0 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wall -Wextra
PROD_FLAGS := -O2 -march=native -ffast-math

BUILD ?= RELEASE

ifeq ($(BUILD),DEBUG)
	CXXFLAGS = $(CXXFLAGS_VERSION) $(DEBUG_FLAGS)
else ifeq ($(BUILD),RELEASE)
	CXXFLAGS = $(CXXFLAGS_VERSION) $(PROD_FLAGS)
endif

APP_SRCS := main.cpp vec.cpp ray.cpp 
APP_OBJS := $(APP_SRCS:.cpp=.o)

LIB_NAME := libcfg.a
LIB_SRCS := lib/cfg/config.cpp
LIB_OBJS := $(LIB_SRCS:.cpp=.o)



all: main

main: $(APP_OBJS) $(LIB_NAME)
	$(CXX) $(CXXFLAGS) -o $@ $(APP_OBJS) $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJS)
	ar rcs $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB_OBJS): $(LIB_SRCS) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

TEST_BIN := tests/test_cfg
TEST_FILES := tests/test_cfg.cpp tests/vec3_tests.cpp
CATCH_OBJ := third_party/catch2/catch_amalgamated.o

third_party/catch2/catch_amalgamated.o: third_party/catch2/catch_amalgamated.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_FILES) $(LIB_NAME) $(CATCH_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(APP_OBJS) $(LIB_OBJS) $(LIB_NAME) perf.data main $(TEST_BIN) example.ppm

.PHOHY: all test clean
