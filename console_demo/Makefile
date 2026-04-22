CXX ?= c++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
TARGET = mindease_console_demo
SOURCES = main.cpp Resource.cpp JournalEntry.cpp JournalManager.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f main journal_index.txt journal_entry_*.txt
