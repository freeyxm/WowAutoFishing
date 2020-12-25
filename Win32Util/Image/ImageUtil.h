#pragma once
#pragma execution_character_set("utf-8")
#include <Windows.h>
#include <list>
#include <vector>
#include <atlimage.h>

#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define RGB_R(color) (color & 0xff)
#define RGB_G(color) ((color >> 8) & 0xff)
#define RGB_B(color) ((color >> 16) & 0xff)

class ImageUtil
{
public:
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
        char* lpBits;
        int w;
        int h;
    };

public:
    typedef bool (*MatchColorProc) (char r, char g, char b);

public:
    ImageUtil();
    ~ImageUtil();

    static std::vector<float> GetFingerprint(const ImageData& image, Rect rect);
    static void GetFingerprint(const ImageData& image, Rect rect, std::vector<float>& result);

    static float FindImage(const ImageData& image, Size rect, const std::vector<float>& sample, Point& point);

    static bool GetWindowSnapshot(HWND hwnd, int x, int y, int w, int h, char *lpBits, BITMAPINFOHEADER *pbi = NULL);
    static bool CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, char *lpBits);

    static bool TransToGray(BITMAP bitmap, LPTSTR pszFile);

    static void FindColor(const CImage& image, MatchColorProc match, std::list<POINT> &points);
    static void FindColor(char *lpBits, int w, int h, MatchColorProc match, std::list<POINT> &points);
    static void FindColor(char *lpBits, int w, int h, int color, int range, std::list<POINT> &points);
    static void FindGray(char *lpBits, int w, int h, int gray, int range, std::list<POINT> &points, unsigned int maxCount = 0);
    static bool SelectBestPoint(std::list<POINT> points, int radius, POINT &p);

    static int GetColor(char *lpBits, int w, int h, int x, int y);
    static bool IsColorAlike(int c1, int c2, int range);
};

