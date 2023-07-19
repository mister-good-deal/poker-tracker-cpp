#include "WinamaxScraper.hpp"

namespace Scraper {
    auto WinamaxScraper::operator=(WinamaxScraper&& other) noexcept -> WinamaxScraper& {
        if (this != &other)
        {
            // @todo, prefer using var = std::move(...);
        }

        return *this;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    auto WinamaxScraper::getBoardCard1Coordinate() const -> const cv::Rect {
        return {195, BOARD_CARDS_Y_COORDINATE, CARD_WIDTH, CARD_HEIGHT};
    }

    auto WinamaxScraper::getBoardCard2Coordinate() const -> const cv::Rect {
        return {getBoardCard1Coordinate().x + BOARD_CARDS_WIDTH_SEPARATION, BOARD_CARDS_Y_COORDINATE, CARD_WIDTH, CARD_HEIGHT};
    }

    auto WinamaxScraper::getBoardCard3Coordinate() const -> const cv::Rect {
        return {getBoardCard2Coordinate().x + BOARD_CARDS_WIDTH_SEPARATION, BOARD_CARDS_Y_COORDINATE, CARD_WIDTH, CARD_HEIGHT};
    }

    auto WinamaxScraper::getBoardCard4Coordinate() const -> const cv::Rect {
        return {getBoardCard3Coordinate().x + BOARD_CARDS_WIDTH_SEPARATION, BOARD_CARDS_Y_COORDINATE, CARD_WIDTH, CARD_HEIGHT};
    }

    auto WinamaxScraper::getBoardCard5Coordinate() const -> const cv::Rect {
        return {getBoardCard4Coordinate().x + BOARD_CARDS_WIDTH_SEPARATION, BOARD_CARDS_Y_COORDINATE, CARD_WIDTH, CARD_HEIGHT};
    }
}  // namespace Scraper