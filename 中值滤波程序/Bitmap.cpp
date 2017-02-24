#include "Bitmap.h"
#include<iostream>
#include<fstream>
#include<cstdlib>
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;

 Bitmap::Bitmap() : image(nullptr) {}

 Bitmap::Bitmap(int w, int h) {
		image = new char[w * h];
		header.filesz = sizeof(bmpHeader) + sizeof(bmpInfo) + (w * h) + 2 + 1024;
		header.bmp_offset = sizeof(bmpHeader) + sizeof(bmpInfo) + 2 + 1024;
		info.header_sz = sizeof(bmpInfo);
		info.width = w;
		info.height = h;
		info.nplanes = 1;
		info.bitspp = 8;
		info.compress_type = 0;
		info.bmp_bytesz = w * h;
		info.hres = 2835;
		info.vres = 2835;
		info.ncolors = 0;
		info.nimpcolors = 0;
	}

 bool Bitmap::Save(const char* filename) {
		if (image == NULL) {
			std::cerr << "Image unitialized" << std::endl;
			return false;
		}

		std::ofstream file(filename, std::ios::out | std::ios::binary);

		file.write("BM", 2);
		file.write((char*)(&header), sizeof(bmpHeader));
		file.write((char*)(&info), sizeof(bmpInfo));

		char rgba[4];
		for (int i = 0; i < 256; ++i) {
			rgba[0] = i;
			rgba[1] = i;
			rgba[2] = i;
			rgba[3] = 0;

			file.write(rgba, 4);
		}

		file.write(image, Width() * Height());

		file.close();

		return true;
	}

 bool Bitmap:: Load(const char* filename) {
		if (image != nullptr) {
			delete[] image;
		}

		std::ifstream file(filename, std::ios::in | std::ios::binary);

		if (!file.is_open()) {
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		// Bitmaps have two magic characters at the beginning
		char BM[2];
		file.read((char*)(BM), 2);

		// Followed by the header
		file.read((char*)(&header), sizeof(bmpHeader));

		// Followed by the info about this image.
		file.read((char*)(&info), sizeof(bmpInfo));

		// We only support BI_RGB
		if (info.compress_type != 0) {
			std::cerr << "Unsupported compression type " << info.compress_type << std::endl;
			return false;
		}

		// And 8bpp (grayscale)
		if (info.bitspp != 8) {
			std::cerr << "Unsupported bit depth " << info.bitspp << std::endl;
			return false;
		}

		// Don't care about the palette, seek to the data
		file.seekg(header.bmp_offset, std::ios::beg);

		// And read it all in.
		image = new char[info.width * info.height];

		//Read the number of bytes based on the size of the image.
		//info.bmp_bytesz may not always be the same is the size of the image
		//array, resulting in a VS debug error when executing.
		file.read(image, info.width * info.height);

		file.close();

		return true;
	}

 Bitmap::~Bitmap() {
		if (image != nullptr) {
			delete[] image;
		}
	}

 int Bitmap:: Width() {
		return info.width;
	}

 int Bitmap:: Height() {
		return info.height;
	}

 char Bitmap:: GetPixel(int x, int y) {
		return image[y * info.width + x];
	}

 void Bitmap:: SetPixel(int x, int y, char color) {
		image[y * info.width + x] = color;
	}
 Bitmap& Bitmap:: operator=(Bitmap& temp)
 {
	 if (this == &temp)
		 return *this;
	 if (this->image!=nullptr)
		 delete[] this->image;
	 this->image=new char[temp.Width()*temp.Height()];
	 for (int i = 0; i < temp.Width(); i++)
		 for (int j = 0; j < temp.Height(); j++)
			 this->SetPixel(i, j, temp.GetPixel(i, j));
	 this->header.filesz = sizeof(bmpHeader) + sizeof(bmpInfo) + (temp.Width()*temp.Height()) + 2 + 1024;
	 this->header.bmp_offset = sizeof(bmpHeader) + sizeof(bmpInfo) + 2 + 1024;
	 this->info.header_sz = sizeof(bmpInfo);
	 this->info.width = temp.Width();
	 this->info.height = temp.Height();
	 this->info.nplanes = 1;
	 this->info.bitspp = 8;
	 this->info.compress_type = 0;
	 this->info.bmp_bytesz=temp.Width()*temp.Height();
	 this->info.hres = 2835;
	 this->info.vres = 2835;
	 this->info.ncolors = 0;
	 this->info.nimpcolors = 0;
	 return *this;
 }
 Bitmap::Bitmap(Bitmap&temp)
 {
	 this->image = new char[temp.Width()*temp.Height()];
	 for (int i = 0; i < temp.Width(); i++)
		 for (int j = 0; j < temp.Height(); j++)
			 this->SetPixel(i, j, temp.GetPixel(i, j));
	 this->header.filesz = sizeof(bmpHeader) + sizeof(bmpInfo) + (temp.Width()*temp.Height()) + 2 + 1024;
	 this->header.bmp_offset = sizeof(bmpHeader) + sizeof(bmpInfo) + 2 + 1024;
	 this->info.header_sz = sizeof(bmpInfo);
	 this->info.width = temp.Width();
	 this->info.height = temp.Height();
	 this->info.nplanes = 1;
	 this->info.bitspp = 8;
	 this->info.compress_type = 0;
	 this->info.bmp_bytesz = temp.Width()*temp.Height();
	 this->info.hres = 2835;
	 this->info.vres = 2835;
	 this->info.ncolors = 0;
	 this->info.nimpcolors = 0;
 }