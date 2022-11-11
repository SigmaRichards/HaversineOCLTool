TARGET=haversines
LINKS=-lOpenCL
#CPPSTD=-std=c++11

$(TARGET): src/main.cpp
	g++ -o $(TARGET) src/main.cpp $(LINKS)

clean:
	rm $(TARGET)
