ga:	pictureGA.o PictureGA_dr.o individual.o
	g++ -o ga PictureGA_dr.o pictureGA.o individual.o

pictureGA.o:	pictureGA.h pictureGA.cc
	g++ -c -g pictureGA.cc

individual.o:	individual.h individual.cc
	g++ -c -g individual.cc

PictureGA_dr.o: PictureGA_dr.cc
	g++ -c -g PictureGA_dr.cc

clean:
	rm -f *~ ga *.o
