CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

LDFLAGS = -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

SRC_DIR = src
OBJ_DIR = build
TARGET = derivator.out

SRCS = $(SRC_DIR)/calculator.cpp \
       $(SRC_DIR)/derivative.cpp \
       $(SRC_DIR)/derivative_main.cpp \
       $(SRC_DIR)/dump.cpp \
       $(SRC_DIR)/equation_simplification.cpp \
       $(SRC_DIR)/file_using.cpp \
       $(SRC_DIR)/infix_read.cpp \
       $(SRC_DIR)/Taras_Bulba.cpp

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
