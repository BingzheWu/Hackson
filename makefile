CC  ?= gcc
CXX ?= g++

INCLUDES = -I/home/wsx/opencv-3.1.0/build1/install/include
LIBPATH  = -L/home/wsx/opencv-3.1.0/build1/install/lib

LIBS =$(LIBPATH) -lopencv_objdetect -lopencv_imgproc -lopencv_imgcodecs -lopencv_core -lpthread \
                 -lopencv_datasets -lopencv_highgui -lopencv_videoio -lopencv_tracking -lopencv_ml \
				 -lopencv_features2d -lopencv_xfeatures2d

CXXFLAGS += -fPIC -pthread -O3
CXXFLAGS += -std=c++11

SRCS = main.cpp HorizonLineDetector.cpp logics.cpp
OBJS = $(SRCS:.cc=.o)
MAIN = run

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o $@ $^  $(LIBS) $(LDFLAGS)
	
.cc.o:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@  
	
clean:
	rm -f *.o *~ $(MAIN)
