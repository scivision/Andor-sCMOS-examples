
#include <iostream>
#include <fstream>
#include "saveAsBmp.h"

using namespace std;

const int MIN_COLOR = 02000;
const int MAX_COLOR = 62000;

const int COLOR_RANGE = 65536;

#pragma pack(push,1)
typedef struct {
	char id1;
	char id2;
	unsigned int filesize;
	unsigned short reserved[2];
	unsigned int headersize;
	unsigned int infoSize;
	unsigned int width;
	unsigned int depth;
	unsigned short biPlanes;
	unsigned short bits;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} TBmpHead;
#pragma pack(pop)
	    
typedef struct {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} TRgbQuad;
    	   
int saveAsBmp(char * _sz_filename, unsigned char * _puc_data, int _width, int _height, int _stride, int _minScale, int _maxScale)
{
  int i_err = 0;
  TBmpHead bmp;
  int i_dataSize = _width * _height;
  
  if (i_dataSize > 0) {
		//FalseXRange MUST be rounded to an even unsigned int boundary (mod4)
		unsigned int falseXRange = _width;
		if (falseXRange & 0x0003) {
		  falseXRange |= 0x0003;
		  ++falseXRange;
		}

		bmp.id1 = 'B';
		bmp.id2 = 'M';
		bmp.bits 					  = 16;
		bmp.headersize 			= 1078L;
		bmp.filesize 		   	= bmp.headersize + (unsigned int) falseXRange * _height * sizeof(unsigned char);
		bmp.reserved[0]     = bmp.reserved[1] = 0;
		bmp.infoSize				= 0x28L;
		bmp.width					  = _width;
		bmp.depth					  = _height;
		bmp.biPlanes				= 1;
		bmp.biCompression		= 0L;
		bmp.biSizeImage			= i_dataSize * sizeof(unsigned char);
		bmp.biXPelsPerMeter	= 0;
		bmp.biYPelsPerMeter	= 0;
		bmp.biClrUsed			  = COLOR_RANGE;
		bmp.biClrImportant	= COLOR_RANGE;

		// Construct Grey Palette
		TRgbQuad palette[COLOR_RANGE];
		for (int color = 0; color < COLOR_RANGE; color++) {
		  palette[color].rgbRed    = (unsigned char) color;
		  palette[color].rgbGreen  = (unsigned char) color;
		  palette[color].rgbBlue   = (unsigned char) color;
		  palette[color].rgbReserved  = 0;
		}

	  float f_scalar = (MAX_COLOR - MIN_COLOR + 1) / (float) (_maxScale - _minScale + 1);

    fstream fout;
		fout.open(_sz_filename, ios::out | ios::binary);

		if (fout.good()) {
		  fout.write((const char *) &bmp.id1, sizeof(char));
		  fout.write((const char *) &bmp.id2, sizeof(char));
		  fout.write((const char *) &bmp.filesize, sizeof(unsigned int));
		  fout.write((const char *) &bmp.reserved[0], sizeof(unsigned short));
		  fout.write((const char *) &bmp.reserved[1], sizeof(unsigned short));
		  fout.write((const char *) &bmp.headersize, sizeof(unsigned int));
		  fout.write((const char *) &bmp.infoSize, sizeof(unsigned int));
		  fout.write((const char *) &bmp.width, sizeof(unsigned int));
		  fout.write((const char *) &bmp.depth, sizeof(unsigned int));
		  fout.write((const char *) &bmp.biPlanes, sizeof(unsigned short));
		  fout.write((const char *) &bmp.bits, sizeof(unsigned short));
		  fout.write((const char *) &bmp.biCompression, sizeof(unsigned int));
		  fout.write((const char *) &bmp.biSizeImage, sizeof(unsigned int));
		  fout.write((const char *) &bmp.biXPelsPerMeter, sizeof(unsigned int));
		  fout.write((const char *) &bmp.biYPelsPerMeter, sizeof(unsigned int));
		  fout.write((const char *) &bmp.biClrUsed, sizeof(unsigned int));
		  fout.write((const char *) &bmp.biClrImportant, sizeof(unsigned int));
		  fout.write((const char*) palette, sizeof(TRgbQuad) * COLOR_RANGE);

	  	unsigned char * puc_temp = new unsigned char[falseXRange];
		
	  	for (unsigned int row = 0; row < _height; row++) {
		  	unsigned char * puc_currPixel = puc_temp;	  	
			  unsigned short * pus_data = reinterpret_cast<unsigned short*>(_puc_data);
	    	for (unsigned int col = 0; col < _width; col++) {
		      if (*pus_data < _minScale) {
		      	*puc_currPixel = MIN_COLOR;
		      }
		      else if (*pus_data > _maxScale) {
		      	*puc_currPixel = MAX_COLOR;
		      }
		      else {
		      	*puc_currPixel = (unsigned char) (f_scalar * (*pus_data - _minScale) + MIN_COLOR);
		      }
		      ++pus_data;
		      ++puc_currPixel;
		    }
	    	for (unsigned int ii = 0; ii < (falseXRange - _width); ii++) {
					*(puc_currPixel++) = MIN_COLOR;		    		
	    	}		
	    	fout.write((const char*) puc_temp, sizeof(unsigned char) * (int)falseXRange);
        _puc_data += _stride;
	  	}
	  	delete[] puc_temp;
	    fout.close();
	  }
	  else {
	  	i_err = -2; // File Error
	  }
	}
	else {
		i_err = -1; // incorrect Image size
	} 
	return i_err; 
}
