CPPFLAGS = -lGLU -lglut -lGL -lGLEW -std=c++11 -Ofast
LIBS = -I/home/ikea/workplace/p3d2016/raytracing/raytracing/lib 
TARGET = source

all: $(TARGET)

$(TARGET):
	g++ $(CPPFLAGS) $(LIBS) $(TARGET).cpp -o $(TARGET)
clean:
	$(RM) $(TARGET)