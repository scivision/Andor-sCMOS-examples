#include <stdlib.h>
#include <iostream>
#include "atcore.h"

int main()
{
  int iErr = AT_InitialiseLibrary();
  if (iErr != AT_SUCCESS) {
    std::cout << "Error from AT_Initialise : " << iErr << std::endl;
  }

  long long DeviceCount = 0;
  iErr = AT_GetInt(AT_HANDLE_SYSTEM, L"Device Count", &DeviceCount);
  if (iErr != AT_SUCCESS) {
    std::cout << "Error from AT_GetInt('Device Count') : " << iErr << std::endl;
  }

  std::cout << "Found " << DeviceCount << " Devices." << std::endl;

  for (long long i=0; i<DeviceCount; i++) {
    std::cout << "Device " << i << " : ";
    AT_H Hndl  = AT_HANDLE_UNINITIALISED;
    iErr = AT_Open(static_cast<int>(i), &Hndl);
    if (iErr != AT_SUCCESS) {
      std::cout << "Error from AT_Open() : " << iErr << std::endl;
    }
    else {
      AT_WC CameraModel[128];
      iErr = AT_GetString(Hndl, L"Camera Model", CameraModel, 128);
      if (iErr != AT_SUCCESS) {
        std::cout << "Error from AT_GetString('Camera Model') : " << iErr << std::endl;
      }

      if (iErr == AT_SUCCESS) {
        char szCamModel[128];
        wcstombs(szCamModel, CameraModel, 64);
        std::cout << szCamModel << std::endl;
      }

      iErr = AT_Close(Hndl);
      if (iErr != AT_SUCCESS) {
        std::cout << "Error from AT_Close() : " << iErr << std::endl;
      }
    }
  }

  AT_FinaliseLibrary();

  std::cout << "Press any key and enter to exit." << std::endl;
  char ch;
  std::cin >> ch;
}
