#include "ScraperInterface.hpp"

#include <ranges>

namespace Scraper {
    using std::ranges::ref_view;
    using std::views::keys;

    using Logger = Logger::Quill;

    auto ScraperInterface::operator=(ScraperInterface&& other) noexcept -> ScraperInterface& {
        if (this != &other) { _activeWindows = std::move(other._activeWindows); }

        return *this;
    }

    auto ScraperInterface::_parseActiveWindows() -> void {
#ifdef _WIN32
        HWND hwnd = nullptr;

        while ((hwnd = FindWindowEx(nullptr, hwnd, nullptr, nullptr)) != nullptr)
        {
            auto titleLength = GetWindowTextLength(hwnd);
            auto title       = static_cast<PSTR>(VirtualAlloc(nullptr, titleLength + 1, MEM_COMMIT, PAGE_READWRITE));
            auto titleSize   = GetWindowText(hwnd, title, titleLength);

            if (titleSize == 0)
            {
                LOG_DEBUG(Logger::getLogger(), "The windows's title could not be retrieved\n\n{}", GetLastError());
            } else {
                _activeWindows[title] = hwnd;
            }

            VirtualFree(title, 0, MEM_RELEASE);
        }

#elif __linux__
        Display*          display = XOpenDisplay(nullptr);
        Window            parent{}, root{}, *children = nullptr;
        XWindowAttributes attributes;
        uint32_t          childrenNumber = 0;

        if (XQueryTree(display, DefaultRootWindow(display), &root, &parent, &children, &childrenNumber))
        {
            // C++ way to avoid pointer arithmetic use
            auto childrenArray = std::span(children, childrenNumber);

            for (uint32_t i = 0; i < childrenNumber; ++i)
            {
                if (XGetWindowAttributes(display, childrenArray[i], &attributes) && attributes.map_state == IsViewable)
                {
                    char* title = nullptr;

                    if (XFetchName(display, childrenArray[i], &title))
                    {
                        _activeWindows[title] = childrenArray[i];
                        XFree(title);
                    }
                }
            }

            XFree(children);
        }

        XCloseDisplay(display);
#endif
    }

    auto ScraperInterface::getScreenshot(const std::string& windowName) -> cv::Mat {
        _parseActiveWindows();

        auto window = _activeWindows[windowName];

#ifdef _WIN32
        RECT rc;

        GetClientRect(window, &rc);

        HDC hdcScreen = GetDC(nullptr);
        HDC hdcWindow = GetDC(window);

        int width  = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        HDC     hdcMemDC  = CreateCompatibleDC(hdcScreen);
        HBITMAP hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);
        HGDIOBJ oldObj    = SelectObject(hdcMemDC, hbmScreen);

        BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

        cv::Mat screenshot(height, width, CV_8UC4);

        GetBitmapBits(hbmScreen, static_cast<LONG>(screenshot.total() * screenshot.elemSize()), screenshot.data);

        screenshot = cv::imdecode(screenshot, cv::IMREAD_COLOR);

        SelectObject(hdcMemDC, oldObj);
        DeleteDC(hdcMemDC);
        ReleaseDC(nullptr, hdcScreen);
        ReleaseDC(window, hdcWindow);
        DeleteObject(hbmScreen);

#elif __linux__
        Display*          display = XOpenDisplay(nullptr);
        XWindowAttributes attributes;
        XGetWindowAttributes(display, window, &attributes);
        XImage* img = XGetImage(display, window, 0, 0, attributes.width, attributes.height, AllPlanes, ZPixmap);

        cv::Mat screenshot(attributes.height, attributes.width, CV_8UC4, img->data);

        cv::cvtColor(screenshot, screenshot, cv::COLOR_BGRA2BGR);

        XDestroyImage(img);
        XCloseDisplay(display);
#endif

        return screenshot;
    }

    auto ScraperInterface::getWindowsName() -> std::vector<std::string_view> {
        _parseActiveWindows();

        std::vector<std::string_view> windowsName{};

        for (const auto& windowName : _activeWindows | keys)
        { windowsName.emplace_back(windowName); }

        return windowsName;
    }
}  // namespace Scraper
