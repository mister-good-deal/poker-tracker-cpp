#include "game_handler/Board.hpp"

namespace GameHandler {
    using std::ranges::all_of;
    using std::ranges::any_of;
    using std::ranges::copy;
    using std::ranges::count;
    using std::ranges::count_if;
    using std::ranges::distance;
    using std::ranges::find;
    using std::ranges::find_if;
    using std::ranges::for_each;
    using std::ranges::max_element;
    using std::ranges::sort;
    using std::views::counted;

    using enum Card::Rank;
    using enum Card::Suit;
    using enum HandRank;

    namespace {
        using eval_key_t = std::array<Card::Rank, COMPARISON_CARDS_NUMBER>;
        using counts_t   = std::array<int32_t, RANK_CARDS_NUMBER + 1>;  // Ranks occurrences, indexed by Card::Rank
        using present_t  = std::array<bool, RANK_CARDS_NUMBER + 1>;     // Ranks presence, index 0 is the low ace of the wheel

        auto toRank(int32_t value) -> Card::Rank { return static_cast<Card::Rank>(value); }

        auto rankCounts(const Board::all_cards_t& cards) -> counts_t {
            counts_t counts {};

            for (const auto& card : cards) {
                if (card.getRank() != UNDEFINED) { counts.at(card.getRank())++; }
            }

            return counts;
        }

        // The suit, if any, present at least five times
        auto flushSuit(const Board::all_cards_t& cards) -> Card::Suit {
            std::array<int32_t, SUIT_CARDS_NUMBER> suitCounts {};

            for (const auto& card : cards) {
                if (card.getSuit() != Card::Suit::UNKNOWN) { suitCounts.at(card.getSuit())++; }
            }

            auto suit = find_if(suitCounts, [](int32_t occurrences) { return occurrences >= FLUSH_SIZE; });

            return suit == suitCounts.end() ? Card::Suit::UNKNOWN : static_cast<Card::Suit>(distance(suitCounts.begin(), suit));
        }

        // Top rank of the highest straight described by `present`, wheel included, UNDEFINED if there is none
        auto highestStraightTop(const present_t& present) -> Card::Rank {
            present_t ranks = present;

            // Special Ace case, the ace also plays low in the wheel
            ranks[0] = present[ACE];

            for (int32_t start = RANK_CARDS_NUMBER + 1 - STRAIGHT_SIZE; start >= 0; --start) {
                auto window = counted(ranks.begin() + start, STRAIGHT_SIZE);

                if (all_of(window, [](bool rankIsPresent) { return rankIsPresent; })) { return toRank(start + STRAIGHT_SIZE - 1); }
            }

            return UNDEFINED;
        }

        auto straightKey(Card::Rank top) -> eval_key_t {
            // The wheel is keyed 5-4-3-2-2, it sorts below every other straight and ties only another wheel
            auto bottom = top == FIVE ? TWO : toRank(top - STRAIGHT_SIZE + 1);

            return {top, toRank(top - 1), toRank(top - 2), toRank(top - 3), bottom};
        }

        // Highest rank appearing exactly `occurrences` times, UNDEFINED if there is none
        auto rankWithCount(const counts_t& counts, int32_t occurrences) -> Card::Rank {
            for (int32_t rank = RANK_CARDS_NUMBER; rank >= TWO; --rank) {
                if (counts.at(rank) == occurrences) { return toRank(rank); }
            }

            return UNDEFINED;
        }

        // The `index`th rank (0 based, from the highest) appearing at least twice, it splits the two pairs apart
        auto nthPairRank(const counts_t& counts, int32_t index) -> Card::Rank {
            for (int32_t rank = RANK_CARDS_NUMBER; rank >= TWO; --rank) {
                if (counts.at(rank) >= PAIR_SIZE && index-- == 0) { return toRank(rank); }
            }

            return UNDEFINED;
        }

        auto highestPairExcluding(const counts_t& counts, Card::Rank excluded) -> Card::Rank {
            for (int32_t rank = RANK_CARDS_NUMBER; rank >= TWO; --rank) {
                if (counts.at(rank) >= PAIR_SIZE && toRank(rank) != excluded) { return toRank(rank); }
            }

            return UNDEFINED;
        }

        // Highest present rank that is not excluded, UNDEFINED if there is none (never happens on seven cards)
        auto highestRankExcluding(const counts_t& counts, std::initializer_list<Card::Rank> excluded) -> Card::Rank {
            for (int32_t rank = RANK_CARDS_NUMBER; rank >= TWO; --rank) {
                if (counts.at(rank) >= 1 && find(excluded, toRank(rank)) == excluded.end()) { return toRank(rank); }
            }

            return UNDEFINED;
        }

        auto flushKey(const Board::all_cards_t& cards, Card::Suit suit) -> eval_key_t {
            std::vector<Card::Rank> ranks;

            for (const auto& card : cards) {
                if (card.getSuit() == suit) { ranks.push_back(card.getRank()); }
            }

            sort(ranks, [](Card::Rank A, Card::Rank B) { return A > B; });

            return {ranks[0], ranks[1], ranks[2], ranks[3], ranks[4]};
        }

        /**
         * @brief Canonical seven cards evaluation: the hand rank plus its five tiebreak ranks, highest first.
         *
         * For high card, pair, two pair, trips, full and quads the key is the made cards then the kickers, for a
         * straight or a straight flush the five ranks of the highest qualifying run and for a flush the five highest
         * cards of the flushed suit. Two hands then compare as (HandRank, key) lexicographically, which is the whole of
         * compareHands: there is no variable length combo truncated into a five cards array anymore, so the best five
         * cards no longer depend on the order the board and the hole cards happen to be iterated in.
         */
        auto evaluate(const Board::all_cards_t& cards) -> std::pair<HandRank, eval_key_t> {
            auto counts = rankCounts(cards);
            auto suit   = flushSuit(cards);

            if (suit != Card::Suit::UNKNOWN) {
                present_t suited {};

                for (const auto& card : cards) {
                    if (card.getSuit() == suit && card.getRank() != UNDEFINED) { suited.at(card.getRank()) = true; }
                }

                // The straight flush run must be looked up among the flushed cards only, a higher mixed suits straight
                // is not one
                if (auto top = highestStraightTop(suited); top != UNDEFINED) { return {STRAIGHT_FLUSH, straightKey(top)}; }
            }

            if (auto quads = rankWithCount(counts, QUADS_SIZE); quads != UNDEFINED) {
                return {QUADS, eval_key_t {quads, quads, quads, quads, highestRankExcluding(counts, {quads})}};
            }

            auto trips = rankWithCount(counts, TRIPS_SIZE);

            if (trips != UNDEFINED) {
                if (auto pair = highestPairExcluding(counts, trips); pair != UNDEFINED) {
                    return {FULL, eval_key_t {trips, trips, trips, pair, pair}};
                }
            }

            if (suit != Card::Suit::UNKNOWN) { return {FLUSH, flushKey(cards, suit)}; }

            present_t present {};

            for (int32_t rank = TWO; rank <= RANK_CARDS_NUMBER; ++rank) { present.at(rank) = counts.at(rank) >= 1; }

            if (auto top = highestStraightTop(present); top != UNDEFINED) { return {STRAIGHT, straightKey(top)}; }

            if (trips != UNDEFINED) {
                auto first  = highestRankExcluding(counts, {trips});
                auto second = highestRankExcluding(counts, {trips, first});

                return {TRIPS, eval_key_t {trips, trips, trips, first, second}};
            }

            auto highPair = nthPairRank(counts, 0);
            auto lowPair  = nthPairRank(counts, 1);

            if (lowPair != UNDEFINED) {
                auto kicker = highestRankExcluding(counts, {highPair, lowPair});

                return {TWO_PAIR, eval_key_t {highPair, highPair, lowPair, lowPair, kicker}};
            }

            if (highPair != UNDEFINED) {
                auto first  = highestRankExcluding(counts, {highPair});
                auto second = highestRankExcluding(counts, {highPair, first});
                auto third  = highestRankExcluding(counts, {highPair, first, second});

                return {PAIR, eval_key_t {highPair, highPair, first, second, third}};
            }

            auto first  = highestRankExcluding(counts, {});
            auto second = highestRankExcluding(counts, {first});
            auto third  = highestRankExcluding(counts, {first, second});
            auto fourth = highestRankExcluding(counts, {first, second, third});
            auto fifth  = highestRankExcluding(counts, {first, second, third, fourth});

            return {HIGH_CARD, eval_key_t {first, second, third, fourth, fifth}};
        }
    }  // namespace

    auto Board::operator=(Board&& other) noexcept -> Board& {
        if (this != &other) {
            _cards             = std::move(other._cards);
            _rankFrequencies   = other._rankFrequencies;
            _suitFrequencies   = other._suitFrequencies;
            _possibleStraight  = other._possibleStraight;
            _possibleFlushDraw = other._possibleFlushDraw;
            _possibleFlush     = other._possibleFlush;
            _pair              = other._pair;
            _twoPair           = other._twoPair;
            _trips             = other._trips;
            _straight          = other._straight;
            _flush             = other._flush;
            _full              = other._full;
            _quads             = other._quads;
            _straightFlush     = other._straightFlush;
        }

        return *this;
    }

    auto Board::setCards(const Board::board_t& cards) -> void {
        _cards = cards;

        _updateStats();
    }

    auto Board::setFlop(const std::array<Card, FLOP_CARDS_NUMBER>& cards) -> void {
        copy(cards, _cards.begin());

        _updateStats();
    }

    auto Board::setTurn(const Card& card) -> void {
        constexpr int32_t TURN_CARD_INDEX = 3;

        _cards[TURN_CARD_INDEX] = card;

        _updateStats();
    }
    auto Board::setRiver(const Card& card) -> void {
        constexpr int32_t RIVER_CARD_INDEX = 4;

        _cards[RIVER_CARD_INDEX] = card;

        _updateStats();
    }

    auto Board::isFlopEmpty() const -> bool {
        return any_of(getFlop(), [](const Card& card) { return card.isUnknown(); });
    }

    auto Board::getHighCardRank() -> Card::Rank {
        return max_element(_cards, [](const Card& A, const Card& B) { return A.getRank() < B.getRank(); })->getRank();
    }

    auto Board::getHandRank(const Hand& hand) -> HandRank {
        HandRank rank     = HIGH_CARD;
        auto     rankF    = _computeRankFrequencies(hand);
        auto     suitF    = _computeSuitFrequencies(hand);
        bool     straight = _straight || _countPossibleStraights(0, rankF) >= 1;

        auto cards = _combineWithHand(hand);

        if (_pair || count(rankF, 2) == 1) { rank = PAIR; }
        if (_twoPair || count(rankF, 2) == 2) { rank = TWO_PAIR; }
        if (_trips || count(rankF, 3) == 1) { rank = TRIPS; }
        if (straight) { rank = STRAIGHT; }
        if (_flush || any_of(suitF, [](const auto& value) { return value >= FLUSH_SIZE; })) { rank = FLUSH; }
        if (_full || (count(rankF, 3) == 2 || (count(rankF, 3) == 1 && count(rankF, 2) >= 1))) { rank = FULL; }
        if (_quads || count(rankF, 4) == 1) { rank = QUADS; }
        if (_straightFlush || (rank == FLUSH && straight && _isStraightFlush(cards, rankF, suitF))) { rank = STRAIGHT_FLUSH; }

        return rank;
    }

    auto Board::compareHands(const Hand& hand1, const Hand& hand2) -> int {
        if (!hand1.isSet() && !hand2.isSet()) { throw std::invalid_argument("Both hands are not set"); }
        if (!hand1.isSet()) { return -1; }
        if (!hand2.isSet()) { return 1; }

        auto [rank1, key1] = evaluate(_combineWithHand(hand1));
        auto [rank2, key2] = evaluate(_combineWithHand(hand2));

        if (rank1 != rank2) { return rank1 > rank2 ? 1 : -1; }

        // The tiebreak ranks are sorted in descending comparison order
        for (int32_t i = 0; i < COMPARISON_CARDS_NUMBER; ++i) {
            if (key1.at(i) != key2.at(i)) { return key1.at(i) > key2.at(i) ? 1 : -1; }
        }

        return 0;  // Both best hands are equal
    }

    auto Board::toJson() const -> json {
        auto cardsArray = json::array();

        for_each(_cards, [&cardsArray](const Card& card) {
            if (!card.isUnknown()) { cardsArray.emplace_back(card.toJson()); }
        });

        return cardsArray;
    }

    auto Board::toDetailedJson() const -> json {
        auto cardsArray = json::array();

        for_each(_cards, [&cardsArray](const Card& card) {
            if (!card.isUnknown()) { cardsArray.emplace_back(card.toJson()); }
        });

        return {{"cards", cardsArray},
                {"properties",
                 {{"possibleStraight", _possibleStraight},
                  {"possibleFlush", _possibleFlush},
                  {"possibleFlushDraw", _possibleFlushDraw},
                  {"paire", _pair},
                  {"doublePaire", _twoPair},
                  {"trips", _trips},
                  {"straight", _straight},
                  {"flush", _flush},
                  {"full", _full},
                  {"quads", _quads},
                  {"straightFlush", _straightFlush}}}};
    }

    auto Board::_computeRankFrequencies(std::optional<Hand> hand) -> rank_f_t {
        rank_f_t frequences {};

        all_cards_t cards;

        copy(_cards, cards.begin());

        if (hand != std::nullopt) {
            const auto& handCards = hand.value().getCards();

            copy(handCards, cards.begin() + BOARD_CARDS_NUMBER);
        }

        // Construct cards rank frequences sequence
        for_each(cards, [&frequences](const Card& card) {
            if (card.getRank() != UNDEFINED) { frequences.at(card.getRank())++; }
        });

        return frequences;
    }

    auto Board::_computeSuitFrequencies(std::optional<Hand> hand) -> suit_f_t {
        suit_f_t frequences {};

        std::array<Card, BOARD_CARDS_NUMBER + HAND_CARDS_NUMBER> cards;

        copy(_cards, cards.begin());

        if (hand != std::nullopt) {
            const auto& handCards = hand.value().getCards();

            copy(handCards, cards.begin() + BOARD_CARDS_NUMBER);
        }

        // Construct cards suit frequences sequence
        for_each(cards, [&frequences](const Card& card) {
            if (card.getSuit() != Card::Suit::UNKNOWN) { frequences.at(card.getSuit())++; }
        });

        return frequences;
    }

    // @todo check std::views::adjacent_transform compilers implementation status
    auto Board::_countPossibleStraights(int32_t additionalCards, std::optional<rank_f_t> rankFrequenciesOpt) -> int32_t {
        int32_t   possibleStraights = 0;
        rank_f_t& rankFrequencies   = rankFrequenciesOpt == std::nullopt ? _rankFrequencies : rankFrequenciesOpt.value();

        // Special Ace case
        rankFrequencies[0] = rankFrequencies[ACE];

        const int32_t WINDOW_SIZE = 5;

        for (int32_t i = 0; i < static_cast<int32_t>(rankFrequencies.size() - WINDOW_SIZE + 1); ++i) {
            auto window = counted(rankFrequencies.begin() + i, WINDOW_SIZE);

            if (count_if(window, [](const auto& value) { return value >= 1; }) >= STRAIGHT_SIZE - additionalCards) {
                possibleStraights++;
            }
        }

        // Reset special Ace case
        rankFrequencies[0] = 0;

        return possibleStraights;
    }

    auto Board::_hasPaire() -> bool { return count(_rankFrequencies, 2) >= 1; }
    auto Board::_hasDoublePaire() -> bool { return count(_rankFrequencies, 2) >= 2; }
    auto Board::_hasTrips() -> bool { return count(_rankFrequencies, 3) == 1; }
    auto Board::_hasStraight() -> bool { return _countPossibleStraights(0) == 1; }
    auto Board::_hasPossibleStraight() -> bool { return _straight || _countPossibleStraights(2) > 0; }
    auto Board::_hasFlush() -> bool { return count(_suitFrequencies, FLUSH_SIZE) == 1; }
    auto Board::_hasPossibleFlushDraw() -> bool { return _flush || count(_suitFrequencies, 2) >= 1; }
    auto Board::_hasPossibleFlush() -> bool { return _flush || count(_suitFrequencies, 3) == 1; }
    auto Board::_hasFull() -> bool { return count(_rankFrequencies, 2) >= 1 && count(_rankFrequencies, 3) >= 1; }
    auto Board::_hasQuads() -> bool { return count(_rankFrequencies, 4) == 1; }

    auto Board::_updateStats() -> void {
        _rankFrequencies = _computeRankFrequencies();
        _suitFrequencies = _computeSuitFrequencies();
        // The order is important, possible flush or possible straight use flush and straight values for optimisation
        _pair              = _hasPaire();
        _twoPair           = _hasDoublePaire();
        _trips             = _hasTrips();
        _straight          = _hasStraight();
        _possibleStraight  = _hasPossibleStraight();
        _flush             = _hasFlush();
        _possibleFlush     = _hasPossibleFlush();
        _possibleFlushDraw = _hasPossibleFlushDraw();
        _full              = _hasFull();
        _quads             = _hasQuads();
        _straightFlush     = _straight && _flush;
    }

    auto Board::_isStraightFlush(const all_cards_t& cards, rank_f_t& rankF, suit_f_t& suitF) -> bool {
        auto suit = static_cast<Card::Suit>(distance(suitF.begin(), find_if(suitF, [](int32_t val) { return val >= STRAIGHT_SIZE; })));

        auto extractCards = [&](int32_t index, std::vector<Card>& extractedCards) {
            for (const auto& card : cards) {
                if (card.getRank() == index) { extractedCards.emplace_back(card); }
            }
        };

        // Special Ace case
        rankF[0] = rankF[ACE];

        for (int32_t i = 0; i < static_cast<int32_t>(rankF.size() - STRAIGHT_SIZE + 1); ++i) {
            auto window = counted(rankF.begin() + i, STRAIGHT_SIZE);

            if (count_if(window, [](const auto& value) { return value >= 1; }) >= STRAIGHT_SIZE) {
                bool    isStraightFlush = true;
                int32_t j               = 0;

                while (isStraightFlush && j < STRAIGHT_SIZE) {
                    std::vector<Card> straightCards;

                    extractCards(i + j++, straightCards);

                    if (all_of(straightCards, [&](const auto& card) { return card.getSuit() != suit; })) { isStraightFlush = false; }
                }

                if (isStraightFlush) { return true; }
            }
        }

        // Reset special Ace case
        rankF[0] = 0;

        return false;
    }

    auto Board::_combineWithHand(const Hand& hand) const -> all_cards_t {
        all_cards_t cards;

        copy(_cards, cards.begin());
        copy(hand.getCards(), cards.begin() + BOARD_CARDS_NUMBER);

        return cards;
    }
}  // namespace GameHandler
