

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#include "atcore.h"
#include "common.h"
#include "saveAsBmp.h"

//************************************
//*** Data
//************************************
const int MAX_FILENAME_LENGTH = 256;

bool b_verbose = false;

char sz_filename[MAX_FILENAME_LENGTH] = "image.bmp";

int i_deviceId = 0;

int i_handle = AT_HANDLE_UNINITIALISED;

double d_exposureTime = 1.;

AT_64 i64_aoiHeight = 0, i64_aoiWidth = 0;

AT_64 i64_max = 2047;
AT_64 i64_min = 0;

int i_minScale = -1;
int i_maxScale = -1;
    
//************************************
//*** Functions
//************************************
int showHelp()
{
  printf("Andor SDK 3 Image Capture Example Program\n"
         "\n"
         "Usage:\n"
         "    image [-?] [-vV] [-e <expTime>] [-f <filename>] [-d <device>]\n"
         "          [s <min> <max>]\n"
         "\n"
         "Synopsis:\n"
         "  Captures a single full frame image and saves it to a bitmap.\n"
         "\n"
         "Arguments:\n"
         "  -?             : Show this help\n"
         "  -v/-V          : Verbose mode\n"
         "  -e <exptime>   : Sets the exposure to the specified float value\n"
         "  -f <filename>  : Saves the bitmap to the specified file\n"
         "  -d <device>    : Acquires from the device number specified\n"
         "  -s <min> <max> : Scales the image to the output palette with\n"
         "                   the specified min and max count values. If\n"
         "                   unspecified, the max and min of the image is used\n"
         "\n"
         );     
  return EXIT_SUCCESS;
}

int printParams()
{
  printf("Device Id: %d\n", i_deviceId);
  printf("Exposure : %0.8f secs\n", d_exposureTime);
  printf("Filename : %s\n", sz_filename); 
  return EXIT_SUCCESS; 
}

int processArgs(int argc, char ** argv)
{
  int i_err = 0;
  char * sz_current;
  // Skip program name;
  argv++;
  argc--;

  while (argc > 0) {
    sz_current = *argv;
    
    if (sz_current[0] != '-' ) {
      printf("** Invalid command line at : '%s'\n", sz_current);
      showHelp();
      i_err = -1;
      break;
    }

    switch (sz_current[1]) {
    case '?':
      showHelp();
      break;      
    case 'v':
    case 'V':
      b_verbose = true;
      break;
    case 'e':
      if (argc > 1) {
        argc--;
        argv++;
        d_exposureTime = atof(*argv);
      }
      else {
        i_err = -4;
        printf("No exposure value given\n");
      }
      break;
    case 'f':
      if (argc > 1) {
        argc--;
        argv++;
        strncpy(sz_filename, *argv, MAX_FILENAME_LENGTH);
      }
      else {
        i_err = -5;
        printf("No filename given\n");
      }      
      break;
    case 'd':
      if (argc > 1) {
        argc--;
        argv++;
        i_deviceId = atoi(*argv);
      }
      else {
        i_err = -4;
        printf("No device id given\n");
      }
      break;
    case 's':
      if (argc > 2) {
        argc--;
        argv++;
        i_minScale = atoi(*argv);
        argc--;
        argv++;
        i_maxScale = atoi(*argv);        
      }
      else {
        i_err = -4;
        printf("No device id given\n");
      }
      break;   
    case '\0':
      printf("** Invalid empty option\n");
      showHelp();
      i_err = -2;
      break;      
    default:
      printf("** Unknown option : '%c'\n", sz_current[1]);
      showHelp();
      i_err = -3;
      break;
    }
    if (i_err < 0) {
      break;
    }
    argv++;
    argc--;
  }

  return i_err;
}

int init()
{
  int i_err = 0;
  i_err = AT_InitialiseLibrary();
  if (errorOk(i_err, "AT_InitialiseLibrary")) {
    if (b_verbose) {
      AT_64 i64_deviceCount = 0;
      i_err = AT_GetInt(AT_HANDLE_SYSTEM, L"Device Count", &i64_deviceCount);
      if (errorOk(i_err, "AT_GetInt 'Device Count'")) {
        std::cout << "Found " << i64_deviceCount << " Devices." << std::endl;
      }
    }
  
    i_err = AT_Open(i_deviceId, &i_handle);    
     errorOk(i_err, "AT_Open");
  }
  
  return i_err;
}

int shutdown()
{
  int i_err = 0;
  AT_Close(i_handle); // Don't check in case not opened.

  i_err = AT_FinaliseLibrary();
  errorOk(i_err, "AT_FinaliseLibrary");  
  return i_err;
}

int updateImageSize()
{
  int i_available = 0, i_err = 0;
  const AT_WC * wsz_featureName = L"AOI Width";
  AT_IsImplemented(i_handle, wsz_featureName, &i_available);    
  if (!i_available) {
    wsz_featureName = L"Sensor Width";  
  }
  i_err = AT_GetInt(i_handle, wsz_featureName, &i64_aoiWidth);    
  if (errorOk(i_err, "AT_GetInt 'Width'")) {
  
    wsz_featureName = L"AOI Height";
    AT_IsImplemented(i_handle, wsz_featureName, &i_available);    
    if (!i_available) {
      wsz_featureName = L"Sensor Height";  
    }
    i_err = AT_GetInt(i_handle, wsz_featureName, &i64_aoiHeight);    
    errorOk(i_err, "AT_GetInt 'Height'");
  }  
  return i_err;
}

///////////////////////////////////////////////////////////////////////////////
int setupAcq()
{
  int i_err = 0;
  bool b_set16bit = false;
  
  i_err = AT_SetFloat(i_handle, L"Exposure Time", d_exposureTime);
  if (errorOk(i_err, "AT_SetInt 'Exposure Time'")) {
  
    int i_available = 0;
//////////// HEIGHT /////////////////////
    AT_IsImplemented(i_handle, L"AOI Height", &i_available);    
    if (i_available) {
      AT_IsWritable(i_handle, L"AOI Height", &i_available);    
      if (i_available) {      
        i_err = AT_GetIntMax(i_handle, L"AOI Height", &i64_aoiHeight);
        if (errorOk(i_err, "AT_GetIntMax 'AOI Height'")) {
          i_err = AT_SetInt(i_handle, L"AOI Height", i64_aoiHeight);    
          if (errorOk(i_err, "AT_SetInt 'AOI Height'") && b_verbose) {
            std::cout << "Set AOI height to " << i64_aoiHeight << std::endl;
          }
        }
      }
    }
//////////// WIDTH //////////////
    AT_IsImplemented(i_handle, L"AOI Width", &i_available);    
    if (i_available) {
    
      AT_IsWritable(i_handle, L"AOI Width", &i_available);    
      if (i_available) {          
        i_err = AT_GetIntMax(i_handle, L"AOI Width", &i64_aoiWidth);
        if (errorOk(i_err, "AT_GetintMax 'AOI Width'")) {
          i_err = AT_SetInt(i_handle, L"AOI Width", i64_aoiWidth);
          if (errorOk(i_err, "AT_SetInt 'AOI Width'") && b_verbose)
            {
              std::cout << "Set AOI Width to " << i64_aoiWidth << std::endl;
            }
        }
      }
    }
////////// BINNING //////////////////
    AT_IsImplemented(i_handle, L"AOIBinning", &i_available);
    if (i_available) {
      AT_IsWritable(i_handle, L"AOIBinning", &i_available);    
        if (i_available) { 
          i_err = AT_SetEnumString(i_handle, L"AOIBinning", L"4x4");  
          if (errorOk(i_err, "AT_SetEnumString 'AOIBinning'") && b_verbose)
           {
            std::cout << "Set AOIBinning to 4x4" << std::endl;
           }
         }
      }
/////////////// PREAMP //////////////////
    AT_IsImplemented(i_handle, L"SimplePreAmpGainControl", &i_available);
    if (i_available)
    {
      i_err = AT_SetEnumString(i_handle, L"SimplePreAmpGainControl", L"16-bit (low noise & high well capacity)");  
      if (errorOk(i_err, "AT_SetEnumString 'SimplePreAmpGainControl'")) {
        if (b_verbose) {
          std::cout << "Set SimplePreAmpGainControl to " <<  "16-bit (low noise & high well capacity)" << std::endl;
        }
        b_set16bit = true;
      }
      else {
        b_set16bit = false;
      }
        
    }
    
    AT_IsImplemented(i_handle, L"PixelEncoding", &i_available);    
    if (i_available) {
    
      AT_IsWritable(i_handle, L"PixelEncoding", &i_available);    
      if (i_available) {  
                
        if (b_set16bit) {
          i_err = AT_SetEnumString(i_handle, L"PixelEncoding", L"Mono16");    
          if (errorOk(i_err, "AT_SetEnumString 'PixelEncoding'") && b_verbose) {
            std::cout << "Set PixelEncoding to " <<  "Mono16" << std::endl;
          }      
        }
        else {
          i_err = AT_SetEnumString(i_handle, L"PixelEncoding", L"Mono12Packed");
          if (errorOk(i_err, "AT_SetEnumString 'PixelEncoding'") && b_verbose) {
            std::cout << "Set PixelEncoding to " <<  "Mono12Packed" << std::endl;
          }      
        }
        
      }
    }
  }
  if (i_err == 0) {
    i_err = updateImageSize();
  }    
  return i_err;
}

int printAcqSettings()
{
  int i_err = 0;
  int i_available = 0;
  AT_64 i64_value = 0;
  AT_IsImplemented(i_handle, L"AOI Height", &i_available);    
  if (i_available) {
    i_err = AT_GetInt(i_handle, L"AOI Height", &i64_value);    
    if (errorOk(i_err, "AT_GetInt 'AOI Height'")) {
      std::cout << "AOI Height = " << i64_value << std::endl;
    }
  }

  AT_IsImplemented(i_handle, L"AOI Width", &i_available);    
  if (i_available) {
    i_err = AT_GetInt(i_handle, L"AOI Width", &i64_value);    
    if (errorOk(i_err, "AT_GetInt 'AOI Width'")) {
      std::cout << "AOI Width = " << i64_value << std::endl;
    }
  }
  double d_value = 0;
  AT_IsImplemented(i_handle, L"Actual Exposure Time", &i_available);    
  if (i_available) {
    i_err = AT_GetFloat(i_handle, L"Actual Exposure Time", &d_value);    
    if (errorOk(i_err, "AT_GetFloat 'Actual Exposure Time'")) {
      std::cout << "Actual Exposure Time = " << d_value << std::endl;
    }
  }
  return i_err;
}

int collectStats(unsigned char * _puc_image, AT_64 _i64_width, AT_64 _i64_height, AT_64 _i64_stride)
{
  unsigned short* pus_image = reinterpret_cast<unsigned short*>(_puc_image);
  i64_max = *pus_image;
  i64_min = *pus_image;
  AT_64 i64_total = 0;

  for (AT_64 jj = 0; jj < _i64_height; jj++) {
    pus_image = reinterpret_cast<unsigned short*>(_puc_image);
    for (AT_64 ii = 0; ii < _i64_width; ii++) {
      unsigned short ui_current = *(pus_image++) & 0x07FF;
      if (ui_current < i64_min) {
        i64_min = ui_current;
      }
      else if (ui_current > i64_max) {
        i64_max = ui_current;
      }
      i64_total += ui_current;            
    }
    _puc_image += _i64_stride;
  }
  if (b_verbose) {
    std::cout << "Average pixel value = " << (i64_total / (_i64_width * _i64_height)) << std::endl;
    std::cout << "Min pixel value     = " << i64_min << std::endl;
    std::cout << "Max pixel value     = " << i64_max << std::endl;    
  }      
  return EXIT_SUCCESS;
}

int acquire()
{
  int i_err = 0;
  AT_64 i64_sizeInBytes, i64_aoiStride;
  AT_GetInt(i_handle, L"ImageSizeBytes", &i64_sizeInBytes);
  AT_GetInt(i_handle, L"AOIStride", &i64_aoiStride);
  
  unsigned char * puc_image = new unsigned char[i64_sizeInBytes];
  
  i_err = AT_QueueBuffer(i_handle, puc_image, i64_sizeInBytes);
  if (errorOk(i_err, "AT_QueueBuffer")) {
    i_err = AT_Command(i_handle, L"Acquisition Start");
    if (errorOk(i_err, "AT_Command 'Acquisition Start'")) {
      unsigned char * puc_returnBuf = NULL;
      int i64_bufSize = 0;
      unsigned int ui_timeout = static_cast<unsigned int>(3 * d_exposureTime * 1000);
      if (ui_timeout < 500) {
        ui_timeout = 500;
      }
      
      i_err = AT_WaitBuffer(i_handle, &puc_returnBuf, &i64_bufSize, ui_timeout);
      if (errorOk(i_err, "AT_WaitBuffer")) {
        if (puc_returnBuf != puc_image) {
          i_err = -1;
          std::cout << "Returned buffer not equal to queued buffer" << std::endl;
        }
        else if (i64_bufSize != i64_sizeInBytes) {
          i_err = -2;
          std::cout << "Returned buffer size not correct :  Expected " << i64_sizeInBytes 
                    << ", Actual " << i64_bufSize << std::endl;
        }
        else {
          collectStats(puc_returnBuf, i64_aoiWidth, i64_aoiHeight, i64_aoiStride);
          if (i_minScale < 0) {
            i_minScale = i64_min;
            i_maxScale = i64_max;
          }
          saveAsBmp(sz_filename, puc_returnBuf, i64_aoiWidth, i64_aoiHeight, i64_aoiStride, i_minScale, i_maxScale);
        }
      }
    }
    AT_Command(i_handle, L"AcquisitionStop");
    AT_Flush(i_handle);
  } 
  
  delete [] puc_image;
  return i_err;
}

int main(int argc, char ** argv)
{
  int i_err = processArgs(argc, argv);
  
  if (i_err == 0) {
    if (b_verbose) {
      printParams();
    }
    
    i_err = init();
    if (i_err == 0) {
      i_err = setupAcq();
    }

    if (i_err == 0 && b_verbose) {
      i_err = printAcqSettings();
    }
        
    if (i_err == 0) {
      i_err = acquire();
    }
    
    if (i_err == 0) {
      i_err = shutdown();
    }
    
  }
  exit(i_err);
}
