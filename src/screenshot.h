#pragma once
#include "includes.h"

using namespace Gdiplus;

// TAKEN FROM
// https://docs.microsoft.com/en-ca/windows/win32/gdiplus/-gdiplus-retrieving-the-class-identifier-for-an-encoder-use
inline int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;   // number of image encoders
    UINT size = 0;  // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

// https://docs.microsoft.com/en-ca/windows/win32/gdi/capturing-an-image
inline cv::Mat takeScreenshot() {
    // get device context of main screen
    HDC screen = GetDC(NULL);

    // create compatible DC for screen so you can draw to this DC
    HDC screenCompatibleDC = CreateCompatibleDC(screen);

    int width = GetDeviceCaps(screen, HORZRES);
    int height = GetDeviceCaps(screen, VERTRES);

    // bitmap to hold this image
    HBITMAP bitmapToStoreImage = CreateCompatibleBitmap(screen, width, height);

    // make bitmap the active target in our DC
    // store previous object to rebind later cuz we can't use the new bitmap if it's still attatched
    auto previouslySelectedObject = SelectObject(screenCompatibleDC, bitmapToStoreImage);

    // top left corner of main desktop, it could be negative so you gotta do this
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    if (!BitBlt(screenCompatibleDC, 0, 0, width, height, screen, x, y, SRCCOPY)) {
        cout << "Failed to bitblt image" << endl;
        DeleteObject(bitmapToStoreImage);
        ReleaseDC(NULL, screen);
        DeleteDC(screenCompatibleDC);

        return std::move(cv::Mat());
    }

    SelectObject(screenCompatibleDC, previouslySelectedObject);

    // convert to gdiplus bitmap so it's easy to save/work with
    Gdiplus::Bitmap bitmap(bitmapToStoreImage, NULL);

    // store in cv mat
    Gdiplus::BitmapData rawData;

    Rect lockRegion = {0, 0, width, height};
    if (bitmap.LockBits(&lockRegion, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &rawData) != Gdiplus::Ok) {
        cout << "failed to lock bitmap pixels" << endl;

        DeleteObject(bitmapToStoreImage);
        ReleaseDC(NULL, screen);
        DeleteDC(screenCompatibleDC);
        return std::move(cv::Mat());
    }

    // clone because grabbing from data pointer doesn't copy the pointer
    cv::Mat image = cv::Mat(height, width, CV_8UC3, rawData.Scan0).clone();

    bitmap.UnlockBits(&rawData);
    DeleteObject(bitmapToStoreImage);
    ReleaseDC(NULL, screen);
    DeleteDC(screenCompatibleDC);

    return std::move(image);
}

// CAREFUL, SMALL REGIONS ARE BLURRY AF AND WON"T WORK WELL
inline cv::Mat takeScreenshot(cv::Rect region) {
    // get device context of main screen
    HDC screen = GetDC(NULL);

    // create compatible DC for screen so you can draw to this DC
    HDC screenCompatibleDC = CreateCompatibleDC(screen);

    int width = region.width;
    int height = region.height;

    // bitmap to hold this image
    HBITMAP bitmapToStoreImage = CreateCompatibleBitmap(screen, width, height);

    // make bitmap the active target in our DC
    // store previous object to rebind later cuz we can't use the new bitmap if it's still attatched
    auto previouslySelectedObject = SelectObject(screenCompatibleDC, bitmapToStoreImage);

    // top left corner of main desktop, it could be negative so you gotta do this
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    if (!BitBlt(screenCompatibleDC, 0, 0, width, height, screen, region.x + x, region.y + y, SRCCOPY)) {
        DeleteObject(bitmapToStoreImage);
        ReleaseDC(NULL, screen);
        DeleteDC(screenCompatibleDC);
        cout << "Failed to bitblt image" << endl;
        return std::move(cv::Mat());
    }

    SelectObject(screenCompatibleDC, previouslySelectedObject);

    // convert to gdiplus bitmap so it's easy to save/work with
    Gdiplus::Bitmap bitmap(bitmapToStoreImage, NULL);

    // store in cv mat
    Gdiplus::BitmapData rawData;

    Rect lockRegion = {0, 0, width, height};
    if (bitmap.LockBits(&lockRegion, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &rawData) != Gdiplus::Ok) {
        DeleteObject(bitmapToStoreImage);
        ReleaseDC(NULL, screen);
        DeleteDC(screenCompatibleDC);
        cout << "failed to lock bitmap pixels" << endl;
        return std::move(cv::Mat());
    }

    // clone because grabbing from data pointer doesn't copy the pointer
    cv::Mat image = cv::Mat(height, width, CV_8UC3, rawData.Scan0).clone();

    bitmap.UnlockBits(&rawData);
    DeleteObject(bitmapToStoreImage);
    ReleaseDC(NULL, screen);
    DeleteDC(screenCompatibleDC);

    return std::move(image);
}