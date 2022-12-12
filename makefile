TARGET=haversines
LINKS=-lOpenCL
#CPPSTD=-std=c++11

$(TARGET): src/main.cpp src/rapidcsv.h src/argparse.h
	g++ -o $(TARGET) src/main.cpp $(LINKS)

clean:
	rm $(TARGET)
