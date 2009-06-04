#include "iptv_kernel/Base64.h"
#include <cmath>

using namespace br::com::sbVB::VBLib;

Base64::Base64()
{

}

Base64::~Base64()
{
	while(m_bufferList.size() > 0)
	{
		delete[] m_bufferList.front();
		m_bufferList.pop_front();
	}
}

int Base64::Encode64(char *in, int inSize, VBString &out)
{
	int outSize;
	outSize = (4 * ((unsigned) ceil(inSize / 3.0))) + 1;
	int outSizeCopy = outSize;

	char *outBuffer = new char[outSize];
	outBuffer[outSize] = '\0';

	if(encode64(in, inSize, outBuffer, &outSizeCopy) == 0)
	{
		out = outBuffer;
		delete[] outBuffer;
		return outSize;
	}

	delete[] outBuffer;
	return 0;
}

int Base64::Decode64(const VBString &in, char **out)
{
	int padCount = 0;

	for(int i = in.size(); i > 0; i--)
	{
		if(in.c_str()[i-1] == '=')
			padCount++;
		else
			break;
	}

	if(padCount > 2)
		return 0;

	if((in.size() % 4) != 0)
		return 0;

	int outSize;
	outSize = ((in.size() / 4) * 3) - padCount;
	int outSizeCopy = outSize;

	char *outBuffer = new char[outSize];
	m_bufferList.push_back(outBuffer);

	if(decode64(in.c_str(), (int)in.size(), outBuffer, &outSizeCopy) == 0)
	{

		*out = outBuffer;
		return outSize;
	}

	return 0;
}
