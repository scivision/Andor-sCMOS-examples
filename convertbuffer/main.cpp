#include "atutility.h"

#include <iostream>
#include <cstddef>
using namespace std;


int main(int argc, char* argv[])
{
  int i_retCode;
  cout << "Initialising ..." << endl << endl;
  i_retCode = AT_InitialiseLibrary();
  if (i_retCode == AT_SUCCESS) {
    i_retCode = AT_InitialiseUtilityLibrary();
    if (i_retCode == AT_SUCCESS) {
      AT_64 iNumberDevices = 0;
      AT_GetInt(AT_HANDLE_SYSTEM, L"Device Count", &iNumberDevices);
      if (iNumberDevices > 0) {
        AT_H Hndl;
        i_retCode = AT_Open(0, &Hndl);
        if (i_retCode == AT_SUCCESS) {
          cout << "Successfully initialised camera" << endl << endl;
          AT_SetEnumeratedString(Hndl, L"Pixel Encoding", L"Mono12Packed");
          AT_SetFloat(Hndl, L"Exposure Time", 0.01);

          //Get the number of bytes required to store one frame
          AT_64 iImageSizeBytes;
          AT_GetInt(Hndl, L"Image Size Bytes", &iImageSizeBytes);
          int iBufferSize = static_cast<int>(iImageSizeBytes);

          //Allocate a memory buffer to store one frame
          unsigned char* UserBuffer = new unsigned char[iBufferSize];

          AT_QueueBuffer(Hndl, UserBuffer, iBufferSize);
          AT_Command(Hndl, L"Acquisition Start");
          cout << "Waiting for acquisition ..." << endl << endl;

          unsigned char* Buffer;
          if (AT_WaitBuffer(Hndl, &Buffer,  &iBufferSize, 10000) == AT_SUCCESS){
            cout << "Acquisition finished successfully" << endl;
            //Unpack the 12 bit packed data
            AT_64 ImageHeight;
            AT_GetInt(Hndl, L"AOI Height", &ImageHeight);
            AT_64 ImageWidth;
            AT_GetInt(Hndl, L"AOI Width", &ImageWidth);
            AT_64 ImageStride;
            AT_GetInt(Hndl, L"AOI Stride", &ImageStride);
            unsigned short* unpackedBuffer = new unsigned short[static_cast<size_t>(ImageHeight*ImageWidth)];
            AT_ConvertBuffer(Buffer, reinterpret_cast<unsigned char*>(unpackedBuffer), ImageWidth, ImageHeight, ImageStride, L"Mono12Packed", L"Mono16");

            cout << "Print out of first 20 pixels " << endl;
            for (int i=0; i < 20; i++) {
             cout << unpackedBuffer[i] << endl;
            }

            delete[] unpackedBuffer;
          }
          AT_Command(Hndl, L"Acquisition Stop");
          AT_Flush(Hndl);
          delete[] UserBuffer;
        }
        AT_Close(Hndl);
      }
    }
  }
  AT_FinaliseLibrary();
  AT_FinaliseUtilityLibrary();
  cout << endl << "Press any key then enter to close" << endl;
  char ch;
  cin >> ch;

  return 0;
}


