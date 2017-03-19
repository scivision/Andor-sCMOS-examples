// Reconnect during acquistion example.
// Shows how to handle the camera being disconnected/reconnected during an acquisition.  

#include "atcore.h"
#include <iostream> 
#include <sstream>
#include <vector>
#include "acquisitionmanager.h"

#ifndef RETURN_ON_FAILURE
#define RETURN_ON_FAILURE(command) { int result = command; if(result!=AT_SUCCESS) { std::cout<<__LINE__<<"\t: "<<#command<<" returned error code "<<result<<std::endl; return result;} }
#endif
#ifndef WARN_ON_FAILURE
#define WARN_ON_FAILURE(command) { int result = command; if(result!=AT_SUCCESS) { std::cout<<__LINE__<<"\t: "<<#command<<" returned error code "<<result<<std::endl;} }
#endif

const int IMAGES_TO_ACQUIRE = 1000;

int run();
void pauseForInput();

int InitialiseLibrary(int &deviceCount);
int OpenCamera(int device, AT_H &Handle);
int AT_EXP_CONV CameraPresentCallback(AT_H Hndl, const AT_WC* Feature, void* Context);

int main(int argc, char* argv[])
{
  if (run() == AT_SUCCESS)
    std::cout << "Test completed successfully" << std::endl;
  else
    std::cout << "Test Failed" << std::endl;

  pauseForInput();
  return 0;
}

int run()
{
  std::cout << "Initialising ..." << std::endl << std::endl;

  int deviceCount = 0;
  RETURN_ON_FAILURE(InitialiseLibrary(deviceCount));

  if (deviceCount == 0)
    return AT_SUCCESS;

  AT_H hndl;
  RETURN_ON_FAILURE(OpenCamera(0, hndl));

  AcquisitionManager manager(hndl);

  RETURN_ON_FAILURE(manager.ConfigureAcquisition());
  RETURN_ON_FAILURE(AT_RegisterFeatureCallback(hndl, L"CameraPresent", CameraPresentCallback, &manager));

  RETURN_ON_FAILURE(manager.StartAcquisition());

  std::cout << "Acquisition running, try disconnecting and reconnecting the camera" << std::endl;
  int imageCount = 0;
  while(1){
    int ret = manager.AcquireFrame();
    if (ret == AT_SUCCESS) {
      std::cout << "Acquired Image " << (++imageCount) << std::endl;
    }
    else {
      std::cout << "Acquisition failed with code " << ret << std::endl;
    }

    if (imageCount >= IMAGES_TO_ACQUIRE) {
      break;
    }
    Sleep(400);
  }

  WARN_ON_FAILURE(manager.StopAcquisition());
  RETURN_ON_FAILURE(AT_Close(hndl));
  RETURN_ON_FAILURE(AT_FinaliseLibrary());
  return AT_SUCCESS;
}

void pauseForInput()
{
  std::cout << "Press any key to continue ... " << std::endl;
  std::cin.get();
}


int AT_EXP_CONV CameraPresentCallback(AT_H Hndl, const AT_WC* Feature, void* Context) {
  
  if (wcscmp(Feature, L"CameraPresent") == 0 && Context != nullptr) {
    static AT_BOOL lastCameraPresentStatus = AT_TRUE;

    AcquisitionManager * manager = reinterpret_cast<AcquisitionManager*>(Context);
    
    AT_BOOL newStatus;
    WARN_ON_FAILURE(AT_GetBool(Hndl, L"CameraPresent", &newStatus));
    
    if (newStatus == AT_FALSE && lastCameraPresentStatus == AT_TRUE) {
      std::cout << "Camera disconnected!" << std::endl;
      manager->CameraDisconnected();
      //communicate with other hardware, e.g. resync stage/light sources
      //...
    }
    if (newStatus == AT_TRUE && lastCameraPresentStatus == AT_FALSE) {
      std::cout << "Camera reconnected!" << std::endl;
      manager->CameraReconnected();
      //communicate with other hardware, e.g. resync stage/light sources
      //...
    }
    lastCameraPresentStatus = newStatus;

  }
  return AT_SUCCESS;
}


int InitialiseLibrary(int &deviceCount)
{
  RETURN_ON_FAILURE(AT_InitialiseLibrary());
  AT_WC szValue[64];
  RETURN_ON_FAILURE(AT_GetString(AT_HANDLE_SYSTEM, L"SoftwareVersion", szValue, 64));
  std::wcout << "Software Version: " << szValue << std::endl;
  AT_64 deviceCount64 = 0;
  RETURN_ON_FAILURE(AT_GetInt(AT_HANDLE_SYSTEM, L"DeviceCount", &deviceCount64));
  std::cout << "Found " << deviceCount64 << " cameras" << std::endl << std::endl;
  deviceCount = static_cast<int> (deviceCount64);

  return AT_SUCCESS;
}



int OpenCamera(int device, AT_H &Handle)
{
  AT_WC szValue[64];
  RETURN_ON_FAILURE(AT_Open(device, &Handle));

  RETURN_ON_FAILURE(AT_GetString(Handle, L"SerialNumber", szValue, 64));
  std::wcout << "Serial: " << szValue;

  RETURN_ON_FAILURE(AT_GetString(Handle, L"CameraModel", szValue, 64));
  std::wcout << " Model: " << szValue;

  RETURN_ON_FAILURE(AT_GetString(Handle, L"FirmwareVersion", szValue, 64));
  std::wcout << " FPGA: " << szValue << std::endl;
  return AT_SUCCESS;
}





