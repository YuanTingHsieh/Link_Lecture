all	:	LinkCourse
LinkCourse	:	src/main.cpp src/linkmap.cpp
	g++ src/main.cpp src/linkmap.cpp -o LinkCourse
