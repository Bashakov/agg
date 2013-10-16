/*!\file unkenc.h 
\brief Unknown encoding handler for expat declaration.
*/
#ifndef UNKENC_H__90EFD6F8_7F0E_4ebd_ABA7_FFC953598BEA
#define UNKENC_H__90EFD6F8_7F0E_4ebd_ABA7_FFC953598BEA

#include <expat.h>

#ifdef __cplusplus
extern "C" {
#endif

	/*! 
	\brief Unknown encoding handler for expat.

	Use this with XML_SetUnknownEncodingHandler function. 
	encodingHandlerData parameter is not used.
	*/
	int XMLCALL unknownEncoding( void * encodingHandlerData, const XML_Char * name, XML_Encoding * info );

#ifdef __cplusplus
}
#endif

#endif