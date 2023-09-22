#ifndef WRITE_H
#define WRITE_H
	void write_pgm_image( void *image, unsigned int maxval, unsigned int xsize, unsigned int ysize, const char *image_name);
#endif

#ifndef READ_H
#define READ_H
	void read_pgm_image( void **image, unsigned int *maxval, unsigned int *xsize, unsigned int *ysize, const char *image_name);
#endif
