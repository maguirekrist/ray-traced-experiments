CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra

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

CATCH_OBJ := third_party/catch2/catch_amalgamated.o

third_party/catch2/catch_amalgamated.o: third_party/catch2/catch_amalgamated.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): tests/test_cfg.cpp $(LIB_NAME) $(CATCH_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(APP_OBJS) $(LIB_OBJS) $(LIB_NAME) main $(TEST_BIN) example.ppm

.PHOHY: all test clean
