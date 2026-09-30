# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2 -I../../Extern/include -I/usr/local/include

# Target library
TARGET = libxml.a

# Source and object files
SOURCES = Document.cpp Util.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
all: $(TARGET)

# Build static library
$(TARGET): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
