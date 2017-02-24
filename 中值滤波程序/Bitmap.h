//
//  Bitmap.h
//  hpalab5
//
//  Created by Harshdeep Singh Chawla on 10/11/16.
//  Copyright © 2016 Harshdeep Singh Chawla. All rights reserved.
//
#ifndef BITMAP_H
#define BITMAP_H
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;

class Bitmap {
public:
	Bitmap();
	Bitmap(int w, int h);
	Bitmap(Bitmap&temp);
	bool Save(const char* filename);
	bool Load(const char* filename);
	~Bitmap();
	int Width();
	int Height();
	char GetPixel(int x, int y);
	void SetPixel(int x, int y, char color);
	char *image;
	Bitmap& operator=(Bitmap &temp);
private:
	struct  bmpHeader {
		uint32_t filesz;
		uint16_t creator1;
		uint16_t creator2;
		uint32_t bmp_offset;
	};
	
	struct bmpInfo {
		uint32_t header_sz;
		int32_t width;
		int32_t height;
		uint16_t nplanes;
		uint16_t bitspp;
		uint32_t compress_type;
		uint32_t bmp_bytesz;
		int32_t hres;
		int32_t vres;
		uint32_t ncolors;
		uint32_t nimpcolors;
	};
	bmpHeader header;
	bmpInfo info;
};

#endif