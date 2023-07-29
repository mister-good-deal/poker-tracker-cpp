#pragma once

#include <game_handler/Hand.hpp>

namespace GameHandler {
    static const int32_t BOARD_CARDS_NUMBER      = 5;
    static const int32_t COMPARISON_CARDS_NUMBER = 5;
    static const int32_t FLOP_CARDS_NUMBER       = 3;
    static const int32_t STRAIGHT_SIZE           = 5;
    static const int32_t FLUSH_SIZE              = 5;
    static const int32_t QUADS_SIZE              = 4;
    static const int32_t FULL_SIZE               = 5;
    static const int32_t TRIPS_SIZE              = 3;
    static const int32_t TWO_PAIR_SIZE           = 4;
    static const int32_t PAIR_SIZE               = 2;
    static const int32_t TOTAL_CARDS_SIZE        = BOARD_CARDS_NUMBER + HAND_CARDS_NUMBER;

    enum class HandRank : int32_t { HIGH_CARD = 0, PAIR, TWO_PAIR, TRIPS, STRAIGHT, FLUSH, FULL, QUADS, STRAIGHT_FLUSH };

    class Board {
        public:
            using board_t            = std::array<Card, BOARD_CARDS_NUMBER>;
            using best_hand_t        = std::array<Card, COMPARISON_CARDS_NUMBER>;
            using all_cards_t        = std::array<Card, TOTAL_CARDS_SIZE>;
            using rank_frequencies_t = std::array<int32_t, RANK_CARDS_NUMBER + 1>;  // +1 for the ace in the straight
            using suit_frequencies_t = std::array<int32_t, SUIT_CARDS_NUMBER>;
            using combo_t            = std::vector<Card>;

            Board()                   = default;
            Board(const Board& other) = default;
            Board(Board&& other) noexcept { *this = std::move(other); };
            explicit Board(const board_t& cards) { setCards(cards); }

            virtual ~Board() = default;

            auto operator=(const Board& other) -> Board& = default;
            auto operator=(Board&& other) noexcept -> Board&;

            [[nodiscard]] auto getCards() const -> board_t { return _cards; }
            [[nodiscard]] auto hasPossibleStraight() const -> bool { return _possibleStraight; }
            [[nodiscard]] auto hasPossibleFlush() const -> bool { return _possibleFlush; }
            [[nodiscard]] auto hasPossibleFlushDraw() const -> bool { return _possibleFlushDraw; }
            [[nodiscard]] auto hasPair() const -> bool { return _pair; }
            [[nodiscard]] auto hasTwoPair() const -> bool { return _twoPair; }
            [[nodiscard]] auto hasTrips() const -> bool { return _trips; }
            [[nodiscard]] auto hasStraight() const -> bool { return _straight; }
            [[nodiscard]] auto hasFlush() const -> bool { return _flush; }
            [[nodiscard]] auto hasFull() const -> bool { return _full; }
            [[nodiscard]] auto hasQuads() const -> bool { return _quads; }
            [[nodiscard]] auto hasStraightFlush() const -> bool { return _straightFlush; }

            auto setCards(const board_t& cards) -> void;
            auto setFlop(const std::array<Card, FLOP_CARDS_NUMBER>& cards) -> void;
            auto setTurn(const Card& card) -> void;
            auto setRiver(const Card& card) -> void;

            auto getHighCardRank() -> Card::Rank;
            auto getHandRank(const Hand& hand) -> HandRank;
            auto compareHands(const Hand& hand1, const Hand& hand2) -> int;

            [[nodiscard]] auto toJson() const -> json;
            [[nodiscard]] auto toDetailedJson() const -> json;

        private:
            board_t            _cards;
            rank_frequencies_t _rankFrequencies{};
            suit_frequencies_t _suitFrequencies{};
            bool               _possibleStraight  = false;
            bool               _possibleFlush     = false;
            bool               _possibleFlushDraw = false;
            bool               _pair              = false;
            bool               _twoPair           = false;
            bool               _trips             = false;
            bool               _straight          = false;
            bool               _flush             = false;
            bool               _full              = false;
            bool               _quads             = false;
            bool               _straightFlush     = false;

            auto _countPossibleStraights(int32_t otherCards, std::optional<rank_frequencies_t> frequencies = std::nullopt) -> int32_t;
            auto _computeRankFrequencies(std::optional<Hand> hand = std::nullopt) -> rank_frequencies_t;
            auto _computeSuitFrequencies(std::optional<Hand> hand = std::nullopt) -> suit_frequencies_t;

            auto _hasPossibleFlush() -> bool;
            auto _hasPossibleFlushDraw() -> bool;
            auto _hasPossibleStraight() -> bool;
            auto _hasPaire() -> bool;
            auto _hasDoublePaire() -> bool;
            auto _hasTrips() -> bool;
            auto _hasStraight() -> bool;
            auto _hasFlush() -> bool;
            auto _hasFull() -> bool;
            auto _hasQuads() -> bool;

            auto _updateStats() -> void;
            auto _extractComboFromPairsLike(const all_cards_t& cards, HandRank rank, rank_frequencies_t& rankFrequencies) -> combo_t;
            auto _extractComboFromStraightOrFlush(const all_cards_t& cards, HandRank rank, rank_frequencies_t& rankFrequencies,
                                                  suit_frequencies_t& suitFrequencies) -> combo_t;
            auto _trimCombo(HandRank rank, combo_t& combo) -> void;
            auto _extractCombo(const all_cards_t& cards, HandRank rank, rank_frequencies_t& rankF, suit_frequencies_t& suitF)
                -> combo_t;
            auto _extractHigherCards(all_cards_t& cards, const std::vector<Card>& combo) -> best_hand_t;
            auto _getHandRankAndBestCombo(const Hand& value) -> std::pair<HandRank, best_hand_t>;
    };
}  // namespace GameHandler