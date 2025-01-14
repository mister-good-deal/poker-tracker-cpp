#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <opencv4/opencv2/videoio.hpp>

#include <game_session/Session.hpp>
#include <logger/Logger.hpp>
#include <utilities/GtestMacros.hpp>

using GameHandler::Game;
using GameSession::Session;
using nlohmann::json;
using Scraper::windowSize_t;
using std::chrono::milliseconds;
using std::filesystem::path;
using ::testing::AtLeast;
using ::testing::InvokeWithoutArgs;

using sharedConstMat_t = Scraper::Model::sharedConstMat_t;

class SessionMock : public Session {
    public:
        SessionMock(const std::string_view& roomName, uint64_t windowId, windowSize_t windowSize)
          : Session(roomName, windowId, windowSize) {}

        // Mocked methods
        MOCK_METHOD(sharedConstMat_t, _getScreenshot, (), (override));

        auto getNextFrame(const std::string& videoFileName, int fastForward = 0) -> sharedConstMat_t {
            if (!_video.isOpened()) {
                if (!_video.open(path(WINAMAX_RESOURCES_DIR) /= videoFileName)) {
                    LOG_ERROR(Logger::Quill::getLogger(),
                              "Failed to open video in `{}`",
                              std::string(WINAMAX_RESOURCES_DIR) + "/" + videoFileName);
                }
            }

            if (!_video.set(cv::CAP_PROP_POS_MSEC, GameSession::TICK_RATE.count() * (++_tickCount + fastForward))) {
                LOG_ERROR(Logger::Quill::getLogger(), "Failed to set video position");
            }

            cv::Mat frame;

            if (!_video.read(frame)) { LOG_ERROR(Logger::Quill::getLogger(), "Cannot read video frame"); }
            if (frame.empty()) { LOG_ERROR(Logger::Quill::getLogger(), "Empty frame"); }

            return std::make_shared<const cv::Mat>(frame);
        }

    private:
        cv::VideoCapture _video;
        int32_t          _tickCount = 0;
};

class SessionTest : public ::testing::Test {};

TEST(SessionTest, shouldReadTheWholeGameCorrectlyFromVideo_game1) {
    SessionMock session("Winamax", 0, {3840, 1080});

    session.getGame().setBuyIn(10);

    // Tell Google Mock to return consecutive frames
    EXPECT_CALL(session, _getScreenshot()).Times(AtLeast(1)).WillRepeatedly(InvokeWithoutArgs([&session] {
        return session.getNextFrame("game_1_3840x1080x8_25cts.mkv", 35);
    }));
    // Run the session
    session.run();  // Read expected json result from file
    std::ifstream fileReader(path(WINAMAX_RESOURCES_DIR) /= "game_1_3840x1080x8_25cts.json");
    json          expectedJson = json::parse(fileReader);

    EXPECT_JSON_EQ(session.getGame().toJson(), expectedJson);
}

TEST(SessionTest, shouldReadTheWholeGameCorrectlyFromVideo_game2) {
    SessionMock session("Winamax", 0, {3840, 1080});

    session.getGame().setBuyIn(10);

    // Tell Google Mock to return consecutive frames
    EXPECT_CALL(session, _getScreenshot()).Times(AtLeast(1)).WillRepeatedly(InvokeWithoutArgs([&session] {
        return session.getNextFrame("game_2.mkv", 20);
    }));
    // Run the session
    session.run();
    // Read expected json result from file
    //    std::ifstream fileReader(path(WINAMAX_RESOURCES_DIR) /= "game_1_3840x1080x8_25cts.json");
    //    json          expectedJson = json::parse(fileReader);
    //
    //    EXPECT_JSON_EQ(session.getGame().toJson(), expectedJson);
}
