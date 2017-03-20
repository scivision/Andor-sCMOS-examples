#include "atcore.h"
#include <iostream> 
#include <chrono>
#include <thread>
using namespace std;
using namespace std::literals::chrono_literals;

unsigned char *acqBuffer=NULL;
unsigned char *acqBuffer1=NULL;
unsigned char *acqBuffer2=NULL;
unsigned char *acqBuffer3=NULL;
unsigned char *acqBuffer4=NULL;

unsigned char* QueueBuffer(int _handle);
void createBuffers(int _handle);
void deleteBuffers();
int doLoopOfAcquisition(int _handle, int _i_count);
int performAcquisition(int _handle, int _numberAcquisitions);
void extract2from3(unsigned char* _buffer, int *_i_returns);

int getUserSettings(int _handle, int &_i_numAcqs);

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
    AT_GetInt(AT_HANDLE_SYSTEM, L"DeviceCount", &iNumberDevices);
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

        AT_WC szValue[64];
        i_retCode= AT_GetString(Hndl, L"Serial Number", szValue, 64);
        if (i_retCode == AT_SUCCESS) {
          wcout << L"The serial number is " << szValue << endl;
        }
        else {
          cout << "Error obtaining Serial number" << endl << endl;
        }

        i_retCode = AT_SetEnumeratedString(Hndl, L"PixelEncoding", L"Mono12Packed");
        if (i_retCode == AT_SUCCESS) {
          cout << "Pixel Encoding set to Mono12Packed" << endl << endl;
        }

        int iret = AT_SetEnumeratedString(Hndl, L"TriggerMode",L"Software");
        if (iret != AT_SUCCESS ) {
          cout << "Error setting trigger mode to Software, retcode=" << iret << endl;
        }
        else {
          cout << "Trigger mode set to Software" << endl << endl;
        }

        iret = AT_SetEnumeratedString(Hndl, L"CycleMode", L"Continuous");
        if (iret != AT_SUCCESS ) {
          cout << "Error setting Cycle Mode to Continuous, retcode=" << iret << endl;
        }
        else {
          cout << "CycleMode set to Continuous" << endl << endl;
        }

        int i_numAcqs;
        getUserSettings(Hndl, i_numAcqs);

        cout << endl << "About to perform "  << i_numAcqs << " acquisitions" << endl;

        performAcquisition(Hndl,i_numAcqs);
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

int getUserSettings(int _handle, int &_i_numAcqs)
{
  int i_retCode;
  cout << endl << "Enter the pixel Readout rate, 100, 200 or 280" << endl;
  int i_rate;
  cin >> i_rate;
  if (i_rate == 100) {
    i_retCode = AT_SetEnumeratedString(_handle, L"PixelReadoutRate", L"100 MHz");
  }
  else if (i_rate == 200) {
    i_retCode = AT_SetEnumeratedString(_handle, L"PixelReadoutRate", L"200 MHz");
  }
  else {
    i_retCode = AT_SetEnumeratedString(_handle, L"PixelReadoutRate", L"280 MHz");
  }

  if (i_retCode != AT_SUCCESS) {
    cout << "Error setting Pixel Readout Rate " << i_rate << endl << endl;
  }

  int i_index;
  i_retCode = AT_GetEnumerated(_handle,L"PixelReadoutRate", &i_index);
  if (i_retCode != AT_SUCCESS) {
    cout << "Error getting PixelReadoutRate index " << i_retCode << endl << endl;
  }

  AT_WC szValue[64];
  i_retCode = AT_GetEnumeratedString(_handle,L"PixelReadoutRate", i_index, szValue, 64);
  if (i_retCode != AT_SUCCESS) {
    cout << "Error getting PixelReadoutRate string " << i_retCode << endl << endl;
  }
  wcout << "PixelReadoutRate set to " << szValue << endl;


  cout << endl << "Enter the Exposure time in seconds, eg 0.01" << endl;
  float f_exp;
  cin >> f_exp;

  //Set the exposure time for this camera to 10 milliseconds
  i_retCode = AT_SetFloat(_handle, L"ExposureTime", f_exp);
  if (i_retCode != AT_SUCCESS) {
    cout << "Error setting Exposure time to " << f_exp << " Error code "<< i_retCode << endl << endl;
  }
  double d_actual;
  i_retCode = AT_GetFloat(_handle, L"ExposureTime", &d_actual);
  if (i_retCode != AT_SUCCESS) {
    cout << "Error getting Exposure time, Error code "<< i_retCode << endl << endl;
  }
  cout << "Exposure time set to " << d_actual << " second(s)" << endl;

  cout << endl << "Enter the number of acquisitions to perform" << endl;
  cin >> _i_numAcqs;

  return i_retCode;

}

void extract2from3(unsigned char* _buffer, int *_i_returns)
{
  _i_returns[1] = (_buffer[0] << 4) + (_buffer[1] & 0xF);
  _i_returns[0] = (_buffer[2] << 4) + (_buffer[1] >> 4) ;
}

void createBuffers(int _handle)
{
  acqBuffer = QueueBuffer(_handle);
  acqBuffer1 = QueueBuffer(_handle);
  acqBuffer2 = QueueBuffer(_handle);
  acqBuffer3 = QueueBuffer(_handle);
  acqBuffer4 = QueueBuffer(_handle);
}

void deleteBuffers()
{
  delete []acqBuffer;
  acqBuffer=NULL;
  delete []acqBuffer1;
  acqBuffer1=NULL;
  delete []acqBuffer2;
  acqBuffer2=NULL;
  delete []acqBuffer3;
  acqBuffer3=NULL;
  delete []acqBuffer4;
  acqBuffer4=NULL;
}

unsigned char* QueueBuffer(int _handle) {
  int iError;
  // Get the number of bytes required to store one frame
  AT_64 ImageSizeBytes = 0;
  iError = AT_GetInt(_handle, L"ImageSizeBytes", &ImageSizeBytes);
  if (iError != AT_SUCCESS) {
    cout << "AT_GetInt failed - ImageSizeBytes - return code " << iError << endl;
  }

  unsigned char* acqBuffer = NULL;
  if (iError == AT_SUCCESS) {
    // Allocate a memory buffer to store one frame
    acqBuffer = new unsigned char[ImageSizeBytes + 7];
  // Pass this buffer to the SDK
    iError = AT_QueueBuffer(_handle, acqBuffer, static_cast<int>(ImageSizeBytes));
    if (iError != AT_SUCCESS) {
      cout << "AT_QueueBuffer failed - Image Size Bytes - return code " << iError << endl;
    }
  }
  return acqBuffer;
}

int doLoopOfAcquisition(int _handle,int _i_count)
{
  int iret;

  AT_64 ImageSizeBytes = 0;
  int data[2];
  AT_GetInt(_handle, L"ImageSizeBytes", &ImageSizeBytes);

  for (int i=0;i<_i_count;i++) {
    //Now there should be no images until we call the softwaretrigger command
    unsigned char* pBuf;
    int BufSize;

    //issue software trigger command
    iret = AT_Command(_handle, L"SoftwareTrigger");
    if (iret!=AT_SUCCESS){
      cout << "Error:Return from Software trigger command not success " << iret << endl;
      return 1;
    }
    //now do a wait and if its not success then error
    iret = AT_WaitBuffer(_handle, &pBuf, &BufSize, 10000);
    if (iret!=AT_SUCCESS){
      cout << "Error:Acquisition timeout when not expecting, retcode " << iret << endl;
      return 1;
    }
    cout << "Got image " << i+1 << " out of "<< _i_count << endl;

    extract2from3(pBuf,data);
    cout << "    First 2 pixels " << data[0] << " " << data[1] << endl;

    //requeue buffer
    AT_QueueBuffer(_handle, pBuf, static_cast<int>(ImageSizeBytes));

  }
  return 0;
}

int performAcquisition(int _handle,int _numberAcquisitions)
{
  int iret = 0;

  createBuffers(_handle);

  AT_Command(_handle, L"AcquisitionStart");
  std::this_thread::sleep_for(100ms); //To Give acquisition time to start

  if (iret ==0) {
    if (doLoopOfAcquisition(_handle,_numberAcquisitions) != 0) {
      iret = 1;
    }
  }
  AT_Command(_handle, L"AcquisitionStop");
  AT_Flush(_handle);
  deleteBuffers();

  return iret;
}
