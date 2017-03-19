//Metadata.cpp
// SDK3 example for toggling metadata during an acquisition and parsing the metadata information - extracting the timestamp value and frame information from buffer

#include <iostream>
#include <stdlib.h>
#include "main.h"

#define LENGTH_FIELD_SIZE 4
#define CID_FIELD_SIZE 4

using namespace std;
static AT_H Handle;
AT_64 i_imageSizeBytes;
unsigned char* puc_image;
unsigned char* UserBuffer;

static TMetadataParser metadataParser;

int main()
{
  // Establish connection to camera and initialise SDK3 library. AT_Initialise must return true before any more API calls.
  if(metadataParser.Initialise() == false)
  {
    metadataParser.Finalise(EXIT_FAILURE);
  }

  //The metadata feature must be available on the chosen camera before proceeding with the rest of the program.
  if(!metadataParser.IsFeatureAvailable(L"MetadataEnable"))
  {
    metadataParser.Finalise(EXIT_FAILURE);
  }

  // If the feature is available, then perform an acquisition and parse the metadata obtained in the results. 
  if(!metadataParser.TakeAcquisition())
  {
    metadataParser.Finalise(EXIT_FAILURE);
  }

  //Parse metadata results
  //Timestamp values
  metadataParser.ParseTimestamp(puc_image, i_imageSizeBytes);
  
  // Frame information (USB 3 cameras only)
  metadataParser.ParseFrameInfo(puc_image, i_imageSizeBytes);

  metadataParser.StopAcquisition();
  metadataParser.Finalise(0);
}


bool TMetadataParser::Initialise()
{
  //Firtly, initialise SDK3 library. Check that there are cameras connected and then attempt to open handle to the camera. 
  int i_errorCode = 0;

  cout << "\nInitialising Library..." << endl;
  i_errorCode = AT_InitialiseLibrary();
  if(i_errorCode != AT_SUCCESS)
  {
    // note - no need to finalise library as not been initialised.
    cout << "Error. Failed to initialise library. Error code: " << i_errorCode << endl;
    metadataParser.Close(EXIT_FAILURE);
  }

  if(!metadataParser.GetNumberOfDevices())
    return false;

  if(!metadataParser.OpenHandle())
    return false;

  return true;
}

bool TMetadataParser::GetNumberOfDevices()
{
  AT_64 i_numberOfDevices = 0;
  // Use system handle as inidivdual handle to the camera hasn't been opened. 
  int i_errorCode = AT_GetInt(AT_HANDLE_SYSTEM, L"DeviceCount", &i_numberOfDevices);
  if(i_numberOfDevices <= 0)
  {
    cout <<"Error. No Cameras have been detected." << endl;
    return false;
  }
  else
  {
    cout << "Number of Cameras Detected: " << i_numberOfDevices << endl;
  }
  return true;
}

bool TMetadataParser::OpenHandle()
{
  int i_errorCode = 0;
  i_errorCode = AT_Open(0, &Handle);
  if(i_errorCode != AT_SUCCESS)
  {
    cout << "Error. Failed to open handle to camera. Error code: " << i_errorCode << endl;
    return false;
  }
  return true;
}

bool TMetadataParser::IsFeatureAvailable(AT_WC* FeatureName)
{
  if(metadataParser.IsFeatureImplemented(FeatureName) && metadataParser.IsFeatureWritable(FeatureName))
    return true;
  else 
    return false;
}

bool TMetadataParser::IsFeatureImplemented(AT_WC* FeatureName)
{
  AT_BOOL b_isImplemented = AT_FALSE;
  int i_errorCode = AT_IsImplemented(Handle, FeatureName, &b_isImplemented);
  cout << "\n";
  if(i_errorCode != AT_SUCCESS)
  {
    cout << "Error from AT_IsImplemented: " << i_errorCode << endl;
    return false;
  }
  else if(b_isImplemented != AT_TRUE)
  {
    cout << "Feature is not implemented." << endl; 
    return false;
  }
  else
  {
    cout <<"Feature is implemented." << endl;
    return true;
  }
}

bool TMetadataParser::IsFeatureWritable(AT_WC* FeatureName)
{
  AT_BOOL b_isWritable = AT_FALSE;
  int i_errorCode = AT_IsWritable(Handle, FeatureName, &b_isWritable);
  if(i_errorCode != AT_SUCCESS)
  {
    cout << "Error from AT_IsWritable: " << i_errorCode << endl;
    return false;
  }
  else if(b_isWritable != AT_TRUE)
  {
    cout << "Feature is not writable." << endl;
    return false;
  }
  else
  {
    cout <<"Feature is writable." << endl;
    return true;
  }
}

void TMetadataParser::Finalise(int i_errorCode)
{
  // Closes handle to camera and finalises library. 
  cout <<"Finalising..." << endl;
  int i_ret = AT_Close(Handle);
  if(i_ret != AT_SUCCESS)
  {
    cout << "Error when closing handle to the camera. Error code: " << i_ret << endl;
  }

  i_ret = AT_FinaliseLibrary();
  if(i_ret != AT_SUCCESS)
  {
    cout << "Error when finalising library. Error code: " << i_ret << endl;
  }
  metadataParser.Close(i_errorCode);
}

void TMetadataParser::Close(int i_errorCode)
{
  //System calls to terminate program.
  cout << "\n\n******Press any key then ENTER to close******"<<endl;
  char ch;
  cin >> ch;
  exit(i_errorCode);
}

bool TMetadataParser::TakeAcquisition()
{
  int i_bufferSize;
  cout << "Preparing to take acquisition..." << endl;

  //Prior to taking an acquisition, the camera must have certain configurations applied. 
  if(!metadataParser.ApplySDKConfigurations())
  {
    cout << "Warning! One or more configurations failed to set correctly." <<  endl;
  }

  i_bufferSize = metadataParser.GetImageSizeBytes();
  UserBuffer = new unsigned char[i_bufferSize];

  //Pass this buffer to the SDK
  int i_retCode = AT_QueueBuffer(Handle, UserBuffer, i_bufferSize);
  if(i_retCode != AT_SUCCESS)
  {
    // Failed to queue buffers - abort acquisition.
    cout << "Return code generated from AT_QueueBuffer: " << i_retCode << endl;
    return false;
  }

  //Start the Acquisition running
  i_retCode = AT_Command(Handle, L"AcquisitionStart");
  if(i_retCode != 0)
  {
    cout << "Error generated whilst attempting to start an acquisition: " << i_retCode <<endl;
    metadataParser.Finalise(EXIT_FAILURE);
  }
  cout <<"\nStarting acquisition...." <<endl;

  //Wait for the next frame
  puc_image = new unsigned char[i_bufferSize];
  i_retCode = AT_WaitBuffer(Handle, &puc_image,  &i_bufferSize, 10000);
  if (i_retCode == AT_SUCCESS)
  {
    cout << "Acquisition finished successfully" << endl;
    cout << "Number bytes received " << i_imageSizeBytes << endl << endl;
    return true;
  }
  else
  {
    cout << "Acquisition finished unsuccessfully. Return Code: "<< i_retCode << endl;
    return false;
  }
}

int TMetadataParser::GetImageSizeBytes()
{
  //Get the number of bytes required to store one frame
  AT_GetInt(Handle, L"ImageSizeBytes", &i_imageSizeBytes);
  int _i_bufferSize = static_cast<int>(i_imageSizeBytes);
  return _i_bufferSize;
}

bool TMetadataParser::ApplySDKConfigurations()
{
  // The following acts as a flag - will be false if one or more configurations failed to set. 
  bool b_allConfigsSetCorrectly = true;
  int i_errorCode = 0;
  cout << "\n";
  //Metadata
  i_errorCode = AT_SetBool(Handle, L"MetadataEnable",  AT_TRUE);
  if(i_errorCode != 0)
  {
    cout << "Error. Failed to enable metadata. Error Code: " << i_errorCode << endl;
    b_allConfigsSetCorrectly = false;
  }
  else
  {
    cout <<"Successfully enabled metadata." << endl;
  }

  //Simple Pre Amp Gain Control
  i_errorCode = AT_SetEnumeratedString(Handle, L"SimplePreAmpGainControl", L"12-bit (low noise)");
  if(i_errorCode != 0)
  {
    cout << "Error. Failed to set SimplePreAmpGainControl. Error Code: " << i_errorCode << endl;
    b_allConfigsSetCorrectly = false;
  }
  else
  {
    cout << "SimplePreAmpGainControl: 12-bit (low noise)." <<endl;
  }

  //Pixel Read Out Rate
  i_errorCode = AT_SetEnumeratedString(Handle, L"PixelReadoutRate", L"280 MHz");
  if(i_errorCode != 0)
  {
    cout << "Failed to set Pixel Readout Rate to 280MHz. Error Code: " << i_errorCode << endl;
    b_allConfigsSetCorrectly = false;
  }
  else
  {
    cout << "Pixel Readout Rate: 280MHz." <<endl;
  }

  //Exposure Time 
  double d_minExposureTime = 0;
  AT_GetFloatMin(Handle, L"ExposureTime", &d_minExposureTime);
  i_errorCode = AT_SetFloat(Handle, L"ExposureTime", d_minExposureTime);
  if(i_errorCode != 0)
  {
    cout << "Failed to set Exposure Time to " << d_minExposureTime << ". Error Code: " << i_errorCode << endl;
    b_allConfigsSetCorrectly = false;
  }
  else
  {
    cout << "Exposure Time: " << d_minExposureTime << "." << endl;
  }

  //Frame Rate
  double d_maxFrameRate = 0;
  AT_GetFloatMax(Handle, L"FrameRate", &d_maxFrameRate);
  i_errorCode = AT_SetFloat(Handle, L"FrameRate", d_maxFrameRate);
  if(i_errorCode != 0)
  {
    cout << "Failed to set Frame Rate to " << d_maxFrameRate << ". Error Code: " << i_errorCode << endl;
    b_allConfigsSetCorrectly = false;
  }
  else
  {
    cout << "Frame Rate: " << d_maxFrameRate << "." << endl;
  }

  return b_allConfigsSetCorrectly;
}

void TMetadataParser::StopAcquisition()
{
  AT_Command(Handle, L"AcquisitionStop");

  delete[] UserBuffer;
  AT_Flush(Handle);
}

unsigned char* TMetadataParser::ParseTimestamp(unsigned char* puc_image, AT_64 i_imageSizeBytes)
{
  puc_image = SearchForCIDStartLocation(puc_image,i_imageSizeBytes , 1);
  if(puc_image == NULL)
  {
    cout << "Error! Unable to locate timestamp CID. \nThis will occur whenever metadata is not enabled in test configuration." << endl << endl;
    metadataParser.Finalise(EXIT_FAILURE);
  }
  else
  {
    cout << "\n-----------------Timestamp Metadata Value -------------------" << endl << endl;
  }

  AT_64 tickCount = metadataParser.Extract64BitNumber(puc_image);
  cout << "Tick count: " << tickCount << endl;

  return puc_image;
}

unsigned char* TMetadataParser::ParseFrameInfo(unsigned char* puc_image, AT_64 i_imageSizeBytes)
{
  cout << "\n-----------------Frame Info Metadata Values -----------------" << endl << endl;

  // FrameInfo only available for USB3 cameras. Check feature is available before continuing. 
  if (!metadataParser.IsFeatureAvailable(L"MetadataFrameInfo")) {
    cout << "Frame information not available on this camera." << endl << endl;
    return puc_image;
  }

  puc_image = SearchForCIDStartLocation(puc_image,i_imageSizeBytes , 7);
  if(puc_image == NULL)
  {
    cout << "Error! Unable to locate frame CID. \nThis will occur whenever metadata is not enabled in test configuration." << endl << endl;
    metadataParser.Finalise(EXIT_FAILURE);
  }

  AT_64 metadataSection = metadataParser.Extract64BitNumber(puc_image);
  cout << "MD Section=" << metadataSection << endl;
  AT_64 extractedValue = static_cast<AT_64>(metadataSection);
  extractedValue = metadataSection & 0xFFFF; 
  cout << "AOI Stride: " << extractedValue << endl;

  extractedValue = static_cast<AT_64>(metadataSection >> 16);
  extractedValue = extractedValue & 0xFF; 
  switch (extractedValue) {
  case 0:
    cout << "Pixel Encoding: Mono16 ("<<extractedValue<<")"<<endl;
    break;
  case 1:
    cout << "Pixel Encoding: Mono12 ("<<extractedValue<<")"<<endl;
    break;
  case 2:
    cout << "Pixel Encoding: Mono12Packed ("<<extractedValue<<")"<<endl;
    break;
  case 3:
    cout << "Pixel Encoding: Mono32 ("<<extractedValue<<")"<<endl;
    break;
  default:
    cout <<"Error. Pixel Encoding Value not recognised. " << extractedValue << endl;
    break;
  }

  extractedValue = static_cast<AT_64>(metadataSection >> 32);
  extractedValue = extractedValue & 0xFFFF; 
  cout << "AOI Width: " << extractedValue << endl;

  extractedValue = static_cast<AT_64>(metadataSection >> 48);
  extractedValue = extractedValue & 0xFFFF; 
  cout << "AOI Height: " << extractedValue << endl;

  return puc_image;
}

unsigned char* TMetadataParser::SearchForCIDStartLocation(unsigned char* _puc_frame, AT_64 _au64_imageSizeBytes, unsigned long _ui_CID)
{
  bool b_CIDFound = false;
  unsigned char* puc_metadata = TMetadataParser::FindStartOfMetadata(_puc_frame, _au64_imageSizeBytes);
  while (!b_CIDFound) {
    unsigned long u32_length = TMetadataParser::ExtractLengthField(puc_metadata);
    unsigned long u32_cid = TMetadataParser::ExtractCID(puc_metadata);
    if (u32_cid == _ui_CID) {
      b_CIDFound = true;
    }
    else {
      if (!TMetadataParser::IsLengthValid(puc_metadata, _puc_frame, u32_length))
        break;
      if (!TMetadataParser::GoToNextChunk(puc_metadata, _puc_frame, u32_length))
        break;
    } 
  }

  return b_CIDFound?puc_metadata:NULL;
}

unsigned char* TMetadataParser::FindStartOfMetadata(unsigned char* _puc_image, int _i_imageSize)
{
  return _puc_image + (_i_imageSize);
}

unsigned long TMetadataParser::ExtractLengthField(unsigned char* _puc_metadata)
{
  return *(reinterpret_cast<unsigned long*>(_puc_metadata - LENGTH_FIELD_SIZE));
}

unsigned long TMetadataParser::ExtractCID(unsigned char* _puc_metadata)
{
  return *(reinterpret_cast<unsigned long*>(_puc_metadata - (LENGTH_FIELD_SIZE + CID_FIELD_SIZE)));
}

bool TMetadataParser::IsLengthValid(unsigned char* _puc_metadata, unsigned char* _puc_image, unsigned long _u32_length)
{
  return static_cast<unsigned long>(_puc_metadata - _puc_image) >= _u32_length + LENGTH_FIELD_SIZE;
}

bool TMetadataParser::GoToNextChunk(unsigned char*& _puc_metadata, unsigned char* _puc_image, unsigned long _u32_length)
{
  _puc_metadata -= _u32_length + LENGTH_FIELD_SIZE;
  if (metadataParser.IsEndOfMetaData(_puc_metadata, _puc_image)) {
    return false;
  }
  else {
    return true;
  }
}

bool TMetadataParser::IsEndOfMetaData(unsigned char* _puc_metadata, unsigned char* _puc_image)
{
  return _puc_metadata == _puc_image;
}

AT_64 TMetadataParser::Extract64BitNumber(unsigned char* _puc_metadata)
{
  unsigned long u32_datasize = metadataParser.ExtractLengthField(_puc_metadata);
  return *(reinterpret_cast<AT_64*>(_puc_metadata - (CID_FIELD_SIZE + u32_datasize)));
}
