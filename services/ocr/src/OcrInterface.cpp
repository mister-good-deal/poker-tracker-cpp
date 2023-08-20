#include "ocr/OcrInterface.hpp"

#include <logger/Logger.hpp>
#include <utilities/Image.hpp>
#include <utilities/Strings.hpp>

namespace OCR {
    using Utilities::Strings::fullTrim;
    using Utilities::Strings::removeChar;
    using Utilities::Strings::replaceChar;
    using Utilities::Strings::toFloat;
    using Utilities::Strings::toInt;
    using Utilities::Strings::trim;

    using Logger = Logger::Quill;

    using enum cv::text::ocr_engine_mode;
    using enum cv::text::page_seg_mode;

    OcrInterface::OcrInterface(int32_t cardWidth) : _cardWidth(cardWidth) {
        // DEFAULT datapath = "/usr/local/share/tessdata"
        _tesseractCard         = cv::text::OCRTesseract::create(nullptr, "eng", "0123456789TJQKA", OEM_CUBE_ONLY, PSM_SINGLE_CHAR);
        _tesseractWord         = cv::text::OCRTesseract::create(nullptr, "eng", ALL_CHARACTERS, OEM_CUBE_ONLY, PSM_SINGLE_BLOCK);
        _tesseractChar         = cv::text::OCRTesseract::create(nullptr, "eng", ALL_CHARACTERS, OEM_CUBE_ONLY, PSM_SINGLE_CHAR);
        _tesseractIntNumbers   = cv::text::OCRTesseract::create(nullptr, "eng", "0123456789 ", OEM_CUBE_ONLY, PSM_SINGLE_CHAR);
        _tesseractIntRange     = cv::text::OCRTesseract::create(nullptr, "eng", "0123456789- ", OEM_CUBE_ONLY, PSM_SINGLE_CHAR);
        _tesseractFloatNumbers = cv::text::OCRTesseract::create(nullptr, "eng", "0123456789,.B ", OEM_CUBE_ONLY, PSM_SINGLE_CHAR);
        _tesseractDuration     = cv::text::OCRTesseract::create(nullptr, "eng", "0123456789: ", OEM_CUBE_ONLY, PSM_SINGLE_CHAR);
    }

    auto OCR::OcrInterface::readBoardCard(const cv::Mat& cardImage) const -> Card {
        try {
            cv::Mat rankImage = cardImage(getRankCardArea());
            cv::Mat suitImage = cardImage(getSuitCardArea());

            return {readCardRank(rankImage), readCardSuit(suitImage)};
        } catch (const UnknownCardRankException& e) {
            throw CannotReadBoardCardRankImageException(e, cardImage);
        } catch (const UnknownCardSuitException& e) { throw CannotReadBoardCardSuitImageException(e, cardImage); }
    }

    auto OCR::OcrInterface::readPlayerCard(const cv::Mat& cardImage) const -> Card {
        try {
            cv::Mat rankImage = cardImage(getRankCardArea());
            cv::Mat suitImage = cardImage(getSuitCardArea());

            return {readCardRank(rankImage), readCardSuit(suitImage)};
        } catch (const UnknownCardRankException& e) {
            throw CannotReadPlayerCardRankImageException(e, cardImage);
        } catch (const UnknownCardSuitException& e) { throw CannotReadPlayerCardSuitImageException(e, cardImage); }
    }

    auto OcrInterface::readHand(const cv::Mat& handImage) const -> Hand {
        cv::Mat firstCardImage  = handImage({0, 0, _cardWidth, handImage.rows});
        cv::Mat secondCardImage = handImage({handImage.cols - _cardWidth, 0, _cardWidth, handImage.rows});

        return {readPlayerCard(firstCardImage), readPlayerCard(secondCardImage)};
    }

    auto OcrInterface::readWord(const cv::Mat& wordImage) const -> std::string {
        auto word = _tesseractWord->run(wordImage, OCR_MIN_CONFIDENCE, cv::text::OCR_LEVEL_TEXTLINE);

        trim(word);

        return word;
    }

    auto OcrInterface::readWordByChar(const cv::Mat& wordImage) const -> std::string {
        auto word = _tesseractChar->run(wordImage, OCR_MIN_CONFIDENCE);

        trim(word);

        return word;
    }

    auto OcrInterface::readIntNumbers(const cv::Mat& intNumberImage) const -> int32_t {
        auto number = _tesseractIntNumbers->run(intNumberImage, OCR_MIN_CONFIDENCE);

        fullTrim(number);

        return toInt(number);
    }

    auto OcrInterface::readIntRange(const cv::Mat& intRangeImage) const -> intRange {
        auto range = _tesseractIntRange->run(intRangeImage, OCR_MIN_CONFIDENCE);

        fullTrim(range);

        auto dashPos      = range.find('-');
        auto firstNumber  = range.substr(0, dashPos);
        auto secondNumber = range.substr(dashPos + 1);

        return {toInt(firstNumber), toInt(secondNumber)};
    }

    auto OcrInterface::readFloatNumbers(const cv::Mat& floatNumberImage) const -> double {
        auto number = _tesseractFloatNumbers->run(floatNumberImage, OCR_MIN_CONFIDENCE);

        fullTrim(number);
        removeChar(number, 'B');
        replaceChar(number, ',', '.');

        LOG_DEBUG(Logger::getLogger(), "readFloatNumbers string: {}", number);

        return toFloat(number);
    }

    auto OcrInterface::readDuration(const cv::Mat& clockImage) const -> seconds {
        auto clock = _tesseractDuration->run(clockImage, OCR_MIN_CONFIDENCE);

        fullTrim(clock);

        auto doubleDashPos = clock.find(':');
        auto minutes       = clock.substr(0, doubleDashPos);
        auto seconds       = clock.substr(doubleDashPos + 1);

        return std::chrono::seconds(toInt(minutes) * 60 + toInt(seconds));
    }

    auto OcrInterface::isSimilar(const cv::Mat& firstImage, const cv::Mat& secondImage, double threshold, cv::InputArray& mask) const
        -> bool {
        return _similarityScore(firstImage, secondImage, mask) <= threshold;
    }

    auto OcrInterface::_cropCentered(cv::Mat& firstImage, cv::Mat& secondImage) const -> void {
        // Determine smaller image, then crop the bigger one to the size of the smaller one. First image is the bigger one.
        if (secondImage.cols > firstImage.cols || secondImage.rows > firstImage.rows) { std::swap(firstImage, secondImage); }

        if (firstImage.cols < secondImage.cols || firstImage.rows < secondImage.rows) {
            throw std::runtime_error("The first image must be bigger than the second one.");
        }

        auto colsBorder = (firstImage.cols - secondImage.cols) / 2;
        auto rowsBorder = (firstImage.rows - secondImage.rows) / 2;

        firstImage(cv::Rect(colsBorder, rowsBorder, secondImage.cols, secondImage.rows)).copyTo(firstImage);
    }

    /**
     * The lower the score, the more similar the images are.
     *
     * @todo Use a better crop method (shape detection matching to center the crop).
     *
     * @param firstImage The first image to compare.
     * @param secondImage The second image to compare.
     * @param mask The mask to apply to the images.
     *
     * @return The similarity score.
     */
    auto OcrInterface::_similarityScore(const cv::Mat& firstImage, const cv::Mat& secondImage, cv::InputArray& mask) const -> double {
        cv::Mat firstImageCopy  = firstImage;
        cv::Mat secondImageCopy = secondImage;

        if (firstImage.rows != secondImage.rows || firstImage.cols != secondImage.cols) {
            LOG_DEBUG(Logger::getLogger(),
                      "The images size are not equals in similarity images computation ({}x{} != {}x{}), cropping the bigger one.",
                      firstImage.rows,
                      firstImage.cols,
                      secondImage.rows,
                      secondImage.cols);

            firstImageCopy  = firstImage.clone();
            secondImageCopy = secondImage.clone();
            _cropCentered(firstImageCopy, secondImageCopy);
        }

        double similarity = cv::norm(firstImageCopy, secondImageCopy, cv::NORM_RELATIVE | cv::NORM_L2SQR, mask);

        return similarity;
    }
}  // namespace OCR