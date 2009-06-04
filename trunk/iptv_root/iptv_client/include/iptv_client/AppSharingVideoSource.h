#ifndef APP_SHARING_VIDEO_SOURCE_H
#define APP_SHARING_VIDEO_SOURCE_H

#include "VideoSource.h"
/** @brief Provides Application Sharing video frames.
 *
 * This VideoSource-derived class provides frames from the Application Sharing module.
 *
 */
class AppSharingVideoSource: public VideoSource
{
public:
	~AppSharingVideoSource();
	
	bool Init();
	void Start();
	void Stop();
	void Close();
	
	void NewFrame(const unsigned char *data, int width, int height);
};

#endif
