#ifndef WRITE_HEADER_H
#define WRITE_HEADER_H
	void write_pgm_image_header(unsigned int maxval, unsigned int xsize, unsigned int ysize, MPI_Offset* current_position,  const char *image_name);
#endif

#ifndef WRITE_BODY_H
#define WRITE_BODY_H
	void write_pgm_image_body( void *image, unsigned int maxval, unsigned int buffer_size, MPI_Offset* current_position, const char *image_name);
#endif

#ifndef READ_HEADER_H
#define READ_HEADER_H
	void read_pgm_image_header(unsigned int *maxval, unsigned int *xsize, unsigned int *ysize, const char *image_name,unsigned int* offset);
#endif

#ifndef READ_BODY_H
#define READ_BODY_H
	void read_pgm_image_body(void* grid,long int buffer_size, long int offset, const char *image_name);
#endif
