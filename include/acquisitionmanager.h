#pragma once

#ifndef RETURN_ON_FAILURE
#define RETURN_ON_FAILURE(command) { int result = command; if(result!=AT_SUCCESS) { std::cout<<__LINE__<<"\t: "<<#command<<" returned error code "<<result<<std::endl; return result;} }
#endif
#ifndef WARN_ON_FAILURE
#define WARN_ON_FAILURE(command) { int result = command; if(result!=AT_SUCCESS) { std::cout<<__LINE__<<"\t: "<<#command<<" returned error code "<<result<<std::endl;} }
#endif

const int NUMBER_OF_BUFFERS = 1;

class AcquisitionManager {
public:
  AcquisitionManager(AT_H hndl) :
    m_hndl(hndl)
  {
    buffers.clear();
  }

  ~AcquisitionManager() {
    ReleaseBuffers();
  }

  void CameraDisconnected() {
    m_cameraPresent = false;
  }

  void CameraReconnected() {
    if (m_running) {
      StopAcquisition();
      StartAcquisition();
    }
    m_cameraPresent = true;
  }

  int StartAcquisition() {
    m_running = true;
    std::cout << "Starting acquisition" << std::endl;
    AllocateBuffers(NUMBER_OF_BUFFERS);
    RETURN_ON_FAILURE(AT_Command(m_hndl, L"AcquisitionStart"));
    return AT_SUCCESS;
  }

  int StopAcquisition() {
    m_running = false;
    std::cout << "Stopping acquisition" << std::endl;
    RETURN_ON_FAILURE(AT_Command(m_hndl, L"AcquisitionStop"));
    return AT_SUCCESS;
  }

  int AcquireFrame()
  {
    if (!m_cameraPresent) {
      std::cout << "Waiting for camera to reconnect ... " << std::endl;
      while (!m_cameraPresent) {
        Sleep(100);
      }
    }

    RETURN_ON_FAILURE(AT_Command(m_hndl, L"SoftwareTrigger"));
    AT_U8* image;
    int imageSize;
    RETURN_ON_FAILURE(AT_WaitBuffer(m_hndl, &image, &imageSize, 2000));
    
    //Save frame/pass to image processing core

    RETURN_ON_FAILURE(AT_QueueBuffer(m_hndl, image, imageSize));
    return AT_SUCCESS;
  }

  int ConfigureAcquisition()
  {
    RETURN_ON_FAILURE(AT_SetEnumString(m_hndl, L"TriggerMode", L"Software"));
    RETURN_ON_FAILURE(AT_SetEnumString(m_hndl, L"CycleMode", L"Continuous"));
    return AT_SUCCESS;
  }

private:

  int AllocateBuffers(int count) {
    ReleaseBuffers();
    AT_64 imageSizeBytes = 0;
    RETURN_ON_FAILURE(AT_GetInt(m_hndl, L"ImageSizeBytes", &imageSizeBytes));
    RETURN_ON_FAILURE(AT_Flush(m_hndl));
    for (int i = 0; i < count; i++) {
      AT_U8 * buffer = new AT_U8[static_cast<size_t>(imageSizeBytes)];
      RETURN_ON_FAILURE(AT_QueueBuffer(m_hndl, buffer, static_cast<int>(imageSizeBytes)));
      buffers.push_back(buffer);
    }
    return AT_SUCCESS;
  }

  int ReleaseBuffers() {
    RETURN_ON_FAILURE(AT_Flush(m_hndl));
    for (const auto& buffer : buffers) {
      delete[] buffer;
    }
    buffers.clear();
    return AT_SUCCESS;
  }

  AT_H m_hndl;
  bool m_cameraPresent = true;
  bool m_running = false;
  std::vector<AT_U8*> buffers;
};

