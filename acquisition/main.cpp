#include "atcore.h"
#include <iostream> 
using namespace std;

#define EXTRACTLOWPACKED(SourcePtr) ( (SourcePtr[0] << 4) + (SourcePtr[1] & 0xF) )
#define EXTRACTHIGHPACKED(SourcePtr) ( (SourcePtr[2] << 4) + (SourcePtr[1] >> 4) )


int main(int argc, char* argv[])
{
  int i_retCode;
  cout << "Initialising ..." << endl << endl;
  i_retCode = AT_InitialiseLibrary();
  if (i_retCode != AT_SUCCESS) {
    cout << "Error initialising library" << endl << endl;
  }
  else {
    AT_64 iNumberDevices = 0;
    AT_GetInt(AT_HANDLE_SYSTEM, L"Device Count", &iNumberDevices);
    if (iNumberDevices <= 0) {
      cout << "No cameras detected"<<endl;
    }
    else {
      AT_H Hndl;
      i_retCode = AT_Open(0, &Hndl);
      if (i_retCode != AT_SUCCESS) {
        cout << "Error condition, could not initialise camera" << endl << endl;
      }
      else {
        cout << "Successfully initialised camera" << endl << endl;

        //Set the pixel Encoding to the desired settings Mono12Packed Data
        AT_SetEnumeratedString(Hndl, L"Pixel Encoding", L"Mono12Packed");

        //Set the pixel Readout Rate to slowest
        i_retCode = AT_SetEnumeratedString(Hndl, L"Pixel Readout Rate", L"100 MHz");
        if (i_retCode == AT_SUCCESS) {
          cout << "Pixel Readout Rate set to 100 MHz" << endl << endl;
        }

        //Set the exposure time for this camera to 10 milliseconds
        AT_SetFloat(Hndl, L"Exposure Time", 0.01);

        //Get the number of bytes required to store one frame
        AT_64 ImageSizeBytes;
        AT_GetInt(Hndl, L"Image Size Bytes", &ImageSizeBytes);

        int BufferSize = static_cast<int>(ImageSizeBytes);

        //Allocate a memory buffer to store one frame
        unsigned char* UserBuffer = new unsigned char[BufferSize];

        //Pass this buffer to the SDK
        AT_QueueBuffer(Hndl, UserBuffer, BufferSize);

        //Start the Acquisition running
        AT_Command(Hndl, L"Acquisition Start");

        cout << "Waiting for acquisition ..." << endl << endl;
        //Sleep in this thread until data is ready, in this case set
        //the timeout to infinite for simplicity
        unsigned char* Buffer;
        if (AT_WaitBuffer(Hndl, &Buffer,  &BufferSize, 10000) == AT_SUCCESS){
          cout << "Acquisition finished successfully" << endl;
          cout << "Number bytes received " << ImageSizeBytes << endl;
          cout << "Print out of first 20 pixels " << endl;
          //Unpack the 12 bit packed data
          for (int i=0; i < 3 * 10; i+=3) {
            AT_64 LowPixel = EXTRACTLOWPACKED(Buffer);
            AT_64 HighPixel = EXTRACTHIGHPACKED(Buffer);
            cout << HighPixel << endl << LowPixel << endl;
            Buffer += 3;
          }
          //Free the allocated buffer
          delete [] UserBuffer;
        }
        else {
          cout << "Timeout occurred check the log file ..." << endl << endl;
        }

        //Stop the Acquisition
        AT_Command(Hndl, L"Acquisition Stop");
        AT_Flush(Hndl);
      }
      AT_Close(Hndl);
    }
    AT_FinaliseLibrary();
  }

  cout << endl << "Press any key then enter to close" << endl;
  char ch;
  cin >> ch;

  return 0;
}


