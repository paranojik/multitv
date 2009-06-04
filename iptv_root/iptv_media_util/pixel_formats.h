#ifndef PIXEL_FORMATS_H
#define PIXEL_FORMATS_H

enum PixFmt
{
	NO_PIX_FMT = 0,
	UNKNOWN_PIX_FMT,
	RGB8,
	BGR8,
	RGB565,
	RGB555,
	RGB24,
	BGR24,
	RGBA32,
	BGRA32,
	YUV420P,
	YUV420,
	YUV422, 
	YUYV422, 
	UYVY422, 
	MJPEG,
	PWC2,
	GREY		// QuickCam 4000 Pro
}; 

#endif
