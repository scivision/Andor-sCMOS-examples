

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include "atutility.h"

#include "atcore.h"
#include "common.h"
#include "saveAsBmp.h"

//************************************
//*** Data
//************************************
const int MAX_FILENAME_LENGTH = 256;

bool b_verbose = false;

char sz_filename[MAX_FILENAME_LENGTH] = "image.bmp";
char index_sz_filename[MAX_FILENAME_LENGTH + 25];

time_t start,end;
time_t start1,end1;
time_t start2,end2;
time_t start3,end3;

bool b_set16bit = true; // defaults to 16 bit, settings are for 12 bit if false.
  


int i_deviceId = 0;
int i_imagesToCapture = 1;

int i_handle = AT_HANDLE_UNINITIALISED;

double d_exposureTime = 0.05;

AT_64 i64_aoiHeight = 2047, i64_aoiWidth = 2047;

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
         "  -P             : Whether or not to use 12bit packed bit mode on camera for faster frame rates. Defaults to using 16 bit mode.\n"
         "  -n             : How many images in a row to capture. Displays framerate after capture complete. Defaults to 1.\n"
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
  return 0;
}

int printParams()
{
  printf("Device Id: %d\n", i_deviceId);
  printf("Exposure : %0.8f secs\n", d_exposureTime);
  printf("Filename : %s\n", sz_filename); 
  return 0; 
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
    case 'n':
      if (argc > 1) {
        argc--;
        argv++;
        i_imagesToCapture = atoi(*argv);
      }
      else {
        i_err = -4;
        printf("No number of images given. \n");
      }
      break;
    case 'P':
      b_set16bit = false;
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
    //Open utility library used in 12 bit mode:
    AT_InitialiseUtilityLibrary();

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
  i_err = AT_FinaliseUtilityLibrary();
  errorOk(i_err, "AT_FinaliseUtilityLibrary");  
  
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

int setupAcq()
{
  int i_err = 0;
  
  i_err = AT_SetFloat(i_handle, L"Exposure Time", d_exposureTime);
  if (errorOk(i_err, "AT_SetInt 'Exposure Time'")) {
  
    int i_available = 0;
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

    AT_IsImplemented(i_handle, L"AOI Width", &i_available);    
    if (i_available) {
    
      AT_IsWritable(i_handle, L"AOI Width", &i_available);    
      if (i_available) {          
        i_err = AT_GetIntMax(i_handle, L"AOI Width", &i64_aoiWidth);
        if (errorOk(i_err, "AT_GetintMax 'AOI Width'")) {
          i_err = AT_SetInt(i_handle, L"AOI Width", i64_aoiWidth);    
          if (errorOk(i_err, "AT_SetInt 'AOI Width'") && b_verbose) {
            std::cout << "Set AOI Width to " << i64_aoiWidth << std::endl;
          }      
        }
      }
    }
    
    AT_IsImplemented(i_handle, L"SimplePreAmpGainControl", &i_available);
    if (i_available)
    {
      if (b_set16bit){
      i_err = AT_SetEnumString(i_handle, L"SimplePreAmpGainControl", L"16-bit (low noise & high well capacity)");  
      if (errorOk(i_err, "AT_SetEnumString 'SimplePreAmpGainControl'")) {
        if (b_verbose) {
          std::cout << "Set SimplePreAmpGainControl to " <<  "16-bit (low noise & high well capacity)" << std::endl;
        }
       }
     } else { //Setup for 12 bit low noise
          i_err = AT_SetEnumString(i_handle, L"SimplePreAmpGainControl", L"12-bit (low noise)");  
          if (errorOk(i_err, "AT_SetEnumString 'SimplePreAmpGainControl'")) {
                  if (b_verbose) {
                    std::cout << "Set SimplePreAmpGainControl to " <<  "12-bit (low noise)" << std::endl;
                  }
                  
                }
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
  return 0;
}

int acquire()
{
  double accum1 = 0;
  
  int i_err = 0;
   AT_64 i64_sizeInBytes, i64_aoiStride;
  
  AT_GetInt(i_handle, L"ImageSizeBytes", &i64_sizeInBytes);
  AT_GetInt(i_handle, L"AOIStride", &i64_aoiStride);
  updateImageSize();
  
  
  //Declare the number of buffers and the number of frames interested in
  int NumberOfBuffers = 250;
  int NumberOfFrames = i_imagesToCapture;

  //Reserved temp buffer for 12 bit mode:
  unsigned short* unpackedBuffer = new unsigned short[static_cast<size_t>(i64_aoiWidth* i64_aoiHeight)];
            
  //Allocate a number of memory buffers to store frames
  unsigned char** AcqBuffers = new unsigned char*[NumberOfBuffers];
  unsigned char** AlignedBuffers = new unsigned char*[NumberOfBuffers];
  for (int i=0; i < NumberOfBuffers; i++) {
   AcqBuffers[i] = new unsigned char[i64_sizeInBytes + 7];
   AlignedBuffers[i] = reinterpret_cast<unsigned char*>((reinterpret_cast<unsigned
  long>(AcqBuffers[i% NumberOfBuffers]) + 7) & ~7);
  }
  //Pass these buffers to the SDK
  for(int i=0; i < NumberOfBuffers; i++) {
   i_err = AT_QueueBuffer(i_handle, AlignedBuffers[i], i64_sizeInBytes);
  }
  if (errorOk(i_err, "AT_QueueBuffer")) {
    //Set the camera to continuously acquires frames
    AT_SetEnumString(i_handle, L"CycleMode", L"Continuous");

    time (&start);
    i_err = AT_Command(i_handle, L"Acquisition Start");
    if (errorOk(i_err, "AT_Command 'Acquisition Start'")) {
      //Sleep in this thread until data is ready, in this case set
      //the timeout to infinite for simplicity
      unsigned char* pBuf;
      int BufSize;
      for (int i=0; i < NumberOfFrames; i++) {
        time (&start1);
        i_err = AT_WaitBuffer(i_handle, &pBuf, &BufSize, AT_INFINITE);
        //Application specific data processing goes here..
        snprintf(index_sz_filename,sizeof(index_sz_filename),"%d_%s",i, sz_filename);
        
        if (b_set16bit){
        
          saveAsBmp(index_sz_filename, pBuf, i64_aoiWidth, i64_aoiHeight, i64_aoiStride, i_minScale, i_maxScale);
      
        } else {
          //AT_ConvertBuffer(pBuf, reinterpret_cast<unsigned char*>(unpackedBuffer), i64_aoiWidth, i64_aoiHeight, i64_aoiStride, L"Mono12Packed", L"Mono16");
          //saveAsBmp(index_sz_filename, reinterpret_cast<unsigned char*>(&unpackedBuffer), i64_aoiWidth, i64_aoiHeight, i64_aoiStride, i_minScale, i_maxScale);

        }
        //Re-queue the buffers
        AT_QueueBuffer(i_handle, AlignedBuffers[i%NumberOfBuffers], i64_sizeInBytes);
        
        time (&end1);
        accum1 += difftime(end1,start1);

      }
      //Stop the acquisition
      AT_Command(i_handle, L"AcquisitionStop");
      AT_Flush(i_handle); 


      //Application specific data processing goes here..
      //Free the allocated buffer
      for (int i=0; i < NumberOfBuffers; i++) {
       delete[] AcqBuffers[i];
      }
      delete[] AlignedBuffers;
      delete[] AcqBuffers;
   
    }
  }

  time (&end);
  double dif = i_imagesToCapture/difftime(end,start);
  printf ("Loop 1 framerate is %lf hz for %d images.\n", dif, i_imagesToCapture);
  double dif1 = i_imagesToCapture/accum1;
  printf ("saveAsBmp 1 framerate is %lf hz for %d images.\n", dif1, i_imagesToCapture);

  //delete[] unpackedBuffer;
      
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
