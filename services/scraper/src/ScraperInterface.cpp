#include "ScraperInterface.hpp"

#include <functional>

#include <fmt/core.h>
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
        LOG_DEBUG(Logger::getLogger(), "Enter _parseActiveWindows");
        HWND hwnd = nullptr;
        LOG_DEBUG(Logger::getLogger(), "Call FindWindowEx");
        while ((hwnd = FindWindowEx(nullptr, hwnd, nullptr, nullptr)) != nullptr)
        {
            LOG_DEBUG(Logger::getLogger(), "Call GetWindowTextLength");
            auto titleLength = GetWindowTextLength(hwnd);
            LOG_DEBUG(Logger::getLogger(), "Call VirtualAlloc");
            auto title = static_cast<PSTR>(VirtualAlloc(nullptr, titleLength + 1, MEM_COMMIT, PAGE_READWRITE));
            LOG_DEBUG(Logger::getLogger(), "Call GetWindowText");
            auto titleSize = GetWindowText(hwnd, title, titleLength);

            if (titleSize == 0)
            {
                LOG_DEBUG(Logger::getLogger(), "The windows's title could not be retrieved\n\n{}", GetLastError());
            } else {
                WindowInfo window(title, hwnd);

                _activeWindows.emplace(window.id, window);
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
                    char*         title = nullptr;
                    XTextProperty windowName;

                    if (XFetchName(display, childrenArray[i], &title))
                    {
                        WindowInfo window(title, childrenArray[i]);

                        // Core dump on mutter guard window manager when trying to get the screenshot
                        if (window.title != "mutter guard window") { _activeWindows.emplace(window.id, window); }

                        XFree(title);
                    } else if (XGetWMName(display, childrenArray[i], &windowName)) {
                        WindowInfo window(reinterpret_cast<char*>(windowName.value), childrenArray[i]);

                        _activeWindows.emplace(window.id, window);

                        XFree(windowName.value);
                    } else {
                        WindowInfo window("undefined", childrenArray[i]);

                        _activeWindows.emplace(window.id, window);
                    }
                }
            }

            XFree(children);
        }

        XCloseDisplay(display);
#endif
    }

    auto ScraperInterface::getScreenshot(uint64_t windowId) -> cv::Mat {
        _parseActiveWindows();

        if (!_activeWindows.contains(windowId))
        { throw std::invalid_argument(fmt::format("The window's ID {} is not found", windowId)); }

        auto window = _activeWindows.at(windowId);

#ifdef _WIN32
        RECT rc;

        GetClientRect(window.ref, &rc);

        HDC hdcScreen = GetDC(nullptr);
        HDC hdcWindow = GetDC(window.ref);

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
        ReleaseDC(window.ref, hdcWindow);
        DeleteObject(hbmScreen);

#elif __linux__
        Display*          display = XOpenDisplay(nullptr);
        XWindowAttributes attributes;
        XGetWindowAttributes(display, window.ref, &attributes);
        XImage* img = XGetImage(display, window.ref, 0, 0, attributes.width, attributes.height, AllPlanes, ZPixmap);

        cv::Mat screenshot(attributes.height, attributes.width, CV_8UC4, img->data);

        cv::cvtColor(screenshot, screenshot, cv::COLOR_BGRA2BGR);

        XDestroyImage(img);
        XCloseDisplay(display);
#endif

        return screenshot;
    }

    auto ScraperInterface::getActiveWindows() -> windows_t {
        _activeWindows.clear();

        _parseActiveWindows();

        return _activeWindows;
    }

    auto ScraperInterface::getWindowElementsView(const cv::Mat& img) -> cv::Mat {
        cv::Mat               elementsView = img.clone();
        std::vector<cv::Rect> elements     = {
            getFirstCardCoordinate(),     getSecondCardCoordinate(),    getPotCoordinate(),           getPrizePoolCoordinate(),
            getBoardCard1Coordinate(),    getBoardCard2Coordinate(),    getBoardCard3Coordinate(),    getBoardCard4Coordinate(),
            getBoardCard5Coordinate(),    getPlayer1NameCoordinate(),   getPlayer2NameCoordinate(),   getPlayer3NameCoordinate(),
            getPlayer1ButtonCoordinate(), getPlayer2ButtonCoordinate(), getPlayer3ButtonCoordinate(), getPlayer1StackCoordinate(),
            getPlayer2StackCoordinate(),  getPlayer3StackCoordinate(),  getPlayer1BetCoordinate(),    getPlayer2BetCoordinate(),
            getPlayer3BetCoordinate(),    getPlayer2HandCoordinate(),   getPlayer3HandCoordinate(),   getBlindLevelCoordinate(),
            getBlindAmountCoordinate(),   getGameTimeCoordinate()};

        for (const auto& element : elements)
        { cv::rectangle(elementsView, element, cv::Scalar(0, 255, 0), 2); }

        return elementsView;
    }

    auto ScraperInterface::toJson() const -> json {
        return {{"firstCard", _rectToJson(getFirstCardCoordinate())},
                {"secondCard", _rectToJson(getSecondCardCoordinate())},
                {"pot", _rectToJson(getPotCoordinate())},
                {"prizePool", _rectToJson(getPrizePoolCoordinate())},
                {"boardCard1", _rectToJson(getBoardCard1Coordinate())},
                {"boardCard2", _rectToJson(getBoardCard2Coordinate())},
                {"boardCard3", _rectToJson(getBoardCard3Coordinate())},
                {"boardCard4", _rectToJson(getBoardCard4Coordinate())},
                {"boardCard5", _rectToJson(getBoardCard5Coordinate())},
                {"player1Name", _rectToJson(getPlayer1NameCoordinate())},
                {"player2Name", _rectToJson(getPlayer2NameCoordinate())},
                {"player3Name", _rectToJson(getPlayer3NameCoordinate())},
                {"player1Button", _rectToJson(getPlayer1ButtonCoordinate())},
                {"player2Button", _rectToJson(getPlayer2ButtonCoordinate())},
                {"player3Button", _rectToJson(getPlayer3ButtonCoordinate())},
                {"player1Stack", _rectToJson(getPlayer1StackCoordinate())},
                {"player2Stack", _rectToJson(getPlayer2StackCoordinate())},
                {"player3Stack", _rectToJson(getPlayer3StackCoordinate())},
                {"player1Bet", _rectToJson(getPlayer1BetCoordinate())},
                {"player2Bet", _rectToJson(getPlayer2BetCoordinate())},
                {"player3Bet", _rectToJson(getPlayer3BetCoordinate())},
                {"player2Hand", _rectToJson(getPlayer2HandCoordinate())},
                {"player3Hand", _rectToJson(getPlayer3HandCoordinate())},
                {"blindLevel", _rectToJson(getBlindLevelCoordinate())},
                {"blindAmount", _rectToJson(getBlindAmountCoordinate())},
                {"gameTime", _rectToJson(getGameTimeCoordinate())}};
    }

    auto ScraperInterface::_rectToJson(const cv::Rect& rect) const -> json {
        return {{"topLeft", {{"x", rect.x}, {"y", rect.y}}}, {"width", rect.width}, {"height", rect.height}};
    };
}  // namespace Scraper