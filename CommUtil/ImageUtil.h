#pragma once
#include <list>
#include <vector>

namespace comm_util
{

using namespace std;

struct Point
{
	int x;
	int y;
};

struct Size
{
	int w;
	int h;
};

struct Rect
{
	int x;
	int y;
	int w;
	int h;
};

struct ImageData
{
	char *lpBits;
	int w;
	int h;
};

class ImageUtil
{
public:
	ImageUtil();
	~ImageUtil();

	static vector<float> getFingerprint(const ImageData &image, Rect rect);
	static void getFingerprint(const ImageData &image, Rect rect, vector<float> &result);

	static float findImage(const ImageData &image, Size rect, const vector<float> &sample, Point &point);
};

} // namespace comm_util
