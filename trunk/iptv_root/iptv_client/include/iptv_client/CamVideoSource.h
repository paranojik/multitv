#ifndef CAMVIDEOSOURCE_H
#define CAMVIDEOSOURCE_H

#include "VideoSource.h"

/** @brief Provides captured video frames.
 *
 * This VideoSource-derived class provides frames from video capture.
 *
 */
class CamVideoSource: public VideoSource
{
private:
	int m_camID;

public:
	CamVideoSource(int camID = -1);
	virtual ~CamVideoSource();

	void SetCamID(int camID);

	bool Init();
	void Start();
	void Stop();
	void Close();

	void NewFrame(const unsigned char *data, int width, int height);
};

#endif
