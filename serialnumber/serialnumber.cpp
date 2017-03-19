#include "atcore.h"
#include <iostream> 
using namespace std;

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
      }
      AT_WC szValue[64];
      i_retCode= AT_GetString(Hndl, L"Serial Number", szValue, 64);
      if (i_retCode == AT_SUCCESS) {
        //The serial number of the camera is szValue
        wcout << L"The serial number is " << szValue << endl;
      }
      else {
        cout << "Error obtaining Serial number" << endl << endl;
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


