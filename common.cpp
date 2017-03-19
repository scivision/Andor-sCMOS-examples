
#include <stdlib.h>
#include <stdio.h>

#include "atcore.h"
#include "common.h"


const char * sdkErrorString(int _i_errorCode)
{
	const char * sz_ret = NULL;
	switch (_i_errorCode) {
		case AT_SUCCESS:
			sz_ret = "AT_SUCCESS";
			break;
		case AT_ERR_NOTINITIALISED:
			sz_ret = "AT_ERR_NOTINITIALISED";
			break;
		case AT_ERR_NOTIMPLEMENTED:
			sz_ret = "AT_ERR_NOTIMPLEMENTED";
			break;
		case AT_ERR_READONLY:
			sz_ret = "AT_ERR_READONLY";
			break;
		case AT_ERR_NOTREADABLE:
			sz_ret = "AT_ERR_NOTREADABLE";
			break;
		case AT_ERR_NOTWRITABLE:
			sz_ret = "AT_ERR_NOTWRITABLE";
			break;
		case AT_ERR_OUTOFRANGE:
			sz_ret = "AT_ERR_OUTOFRANGE";
			break;
		case AT_ERR_INDEXNOTAVAILABLE:
			sz_ret = "AT_ERR_INDEXNOTAVAILABLE";
			break;
		case AT_ERR_INDEXNOTIMPLEMENTED:
			sz_ret = "AT_ERR_INDEXNOTIMPLEMENTED";
			break;
		case AT_ERR_EXCEEDEDMAXSTRINGLENGTH:
			sz_ret = "AT_ERR_EXCEEDEDMAXSTRINGLENGTH";
			break;
		case AT_ERR_CONNECTION:
			sz_ret = "AT_ERR_CONNECTION";
			break;
		case AT_ERR_NODATA:
			sz_ret = "AT_ERR_NODATA";
			break;
		case AT_ERR_INVALIDHANDLE:
			sz_ret = "AT_ERR_INVALIDHANDLE";
			break;
		case AT_ERR_TIMEDOUT:
			sz_ret = "AT_ERR_TIMEDOUT";
			break;
		default:
			sz_ret = "Unknown code";
			break;
	}
	return sz_ret;
}

bool errorOk(int _i_err, const char * _sz_caller)
{
	bool b_ret = true;
	if (_i_err != AT_SUCCESS) {
		printf("Error returned from %s: %d - %s\n", _sz_caller, _i_err, sdkErrorString(_i_err));
		b_ret = false;
	}
	return b_ret;
}
