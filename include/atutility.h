//---------------------------------------------------------------------------

#ifndef atutilityH
#define atutilityH
//---------------------------------------------------------------------------
#include "atcore.h"

#define AT_ERR_INVALIDOUTPUTPIXELENCODING 1002
#define AT_ERR_INVALIDINPUTPIXELENCODING 1003
#define AT_ERR_INVALIDMETADATAINFO 1004
#define AT_ERR_CORRUPTEDMETADATA 1005
#define AT_ERR_METADATANOTFOUND 1006

#ifdef __cplusplus
extern "C" {
#endif

int AT_EXP_CONV AT_ConvertBuffer(AT_U8* inputBuffer,
                                 AT_U8* outputBuffer,
                                 AT_64 width,
                                 AT_64 height,
                                 AT_64 stride,
                                 const AT_WC * inputPixelEncoding,
                                 const AT_WC * outputPixelEncoding);
int AT_EXP_CONV AT_ConvertBufferUsingMetadata(AT_U8* inputBuffer,
                                              AT_U8* outputBuffer,
                                              AT_64 imagesizebytes,
                                              const AT_WC * outputPixelEncoding);
int AT_EXP_CONV AT_GetWidthFromMetadata(AT_U8* inputBuffer, AT_64 imagesizebytes, AT_64& width);
int AT_EXP_CONV AT_GetHeightFromMetadata(AT_U8* inputBuffer, AT_64 imagesizebytes, AT_64& height);
int AT_EXP_CONV AT_GetStrideFromMetadata(AT_U8* inputBuffer, AT_64 imagesizebytes, AT_64& stride);
int AT_EXP_CONV AT_GetPixelEncodingFromMetadata(AT_U8* inputBuffer, AT_64 imagesizebytes, AT_WC* pixelEncoding, int pixelEncodingSize);
int AT_EXP_CONV AT_GetTimeStampFromMetadata(AT_U8* inputBuffer, AT_64 imagesizebytes, AT_64& timeStamp);

int AT_EXP_CONV AT_InitialiseUtilityLibrary();
int AT_EXP_CONV AT_FinaliseUtilityLibrary();

#ifdef __cplusplus
}
#endif

#endif
