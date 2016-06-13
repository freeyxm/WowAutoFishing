#include "ImageUtil.hpp"
#include "VectorUtil.hpp"

namespace comm_util
{

ImageUtil::ImageUtil()
{
}


ImageUtil::~ImageUtil()
{
}

/*
计算图像指纹。图像数据格式为ARGB。
*/
vector<float> ImageUtil::getFingerprint(const ImageData &image, Rect rect)
{
	vector<float> result(64);

	getFingerprint(image, rect, result);

	return result;
}

/*
计算图像指纹。图像数据格式为ARGB。
*/
void ImageUtil::getFingerprint(const ImageData &image, Rect rect, vector<float> &result)
{
	result.resize(64);
	memset(&result[0], 0, result.size() * sizeof(result[0]));

	int index;
	for (int i = 0; i < rect.h; ++i)
	{
		int basei = (i + rect.y) * image.w << 2;
		for (int j = 0; j < rect.w; ++j)
		{
			index = basei + ((j + rect.x) << 2);
			unsigned char r = image.lpBits[index + 2];
			unsigned char g = image.lpBits[index + 1];
			unsigned char b = image.lpBits[index + 0];
			index = (r / 64 << 4) + (g / 64 << 2) + b / 64;
			result[index]++;
		}
	}
}

/*
从图像中寻找小图。
*/
float ImageUtil::findImage(const ImageData &image, Size size, const vector<float> &sample, Point &point)
{
	Rect rect = { 0, 0, size.w, size.h };
	vector<float> finger;
	float value, max_value = FLT_MIN;

	for (int k = 0; k < 1; ++k)
	{
		int off_w = k * size.w / 2;
		int off_h = k * size.h / 2;
		for (int i = off_w; i < image.w; i += size.w)
		{
			for (int j = off_h; j < image.h; j += size.h)
			{
				rect.x = i;
				rect.y = j;
				getFingerprint(image, rect, finger);
				value = VectorUtil::getCosA(&sample[0], &finger[0], sample.size());
				if (max_value < value)
				{
					point.x = i;
					point.y = j;
					max_value = value;
				}
			}
		}
	}
	
	return max_value;
}

} // namespace comm_util