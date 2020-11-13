CXXSOURCE = \
	AllianceGroup.cpp\
	CapabilityExtractor.cpp\
	CapabilityExtractor2020.cpp\
	ClimbingStrategies.cpp\
	Const2020.cpp\
	CsvReader.cpp\
	MatchAlliance.cpp\
	MatchSimulator2020.cpp\
	picklist2020.cpp\
	PickListFormatter.cpp\
	Result.cpp\
	RobotCapabilities.cpp\
	TeamPicker.cpp\
	TeamPicker2020.cpp

TARGET=picklist2020

VALIDCONFIG=FALSE
ifeq ($(CONFIG),Debug)
VALIDCONFIG=TRUE
endif

ifeq ($(CONFIG),Release)
VALIDCONFIG=TRUE
endif

ifeq ($(VALIDCONFIG),FALSE)
$(error "No valid configuration given, must include CONFIG=Debug or CONFIG=Release")
endif

CXXFLAGS = -g -std=c++17


OBJS = $(addprefix $(CONFIG)/,$(CXXSOURCE:%.cpp=%.o))

all: directory $(CONFIG)/$(TARGET)

$(CONFIG)/$(TARGET): $(OBJS)
	$(CXX) -o $(CONFIG)/$(TARGET) $(OBJS) -lstdc++fs

directory:
	mkdir -p $(CONFIG)

.phony: directory
	echo $(OBJS)

$(CONFIG)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


