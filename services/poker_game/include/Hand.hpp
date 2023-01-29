#pragma once

#include <vector>

#include "Card.hpp"

using Rank = Card::Rank;

class Hand {
    public:
        Hand() : firstCard(Card()), secondCard(Card()){};
        Hand(const Card& firstCard, const Card& secondCard);

        auto operator==(const Hand& rhs) const -> bool;
        auto operator!=(const Hand& rhs) const -> bool;
        auto operator=(Hand other) -> Hand&;

        const std::vector<Rank> BROADWAY = {Rank::TEN, Rank::JACK, Rank::QUEEN, Rank::KING, Rank::ACE};

        class invalid_hand : public std::runtime_error {
            public:
                explicit invalid_hand(const std::string& arg) : runtime_error(arg){};
        };

        auto cards() -> std::tuple<Card, Card> { return {firstCard, secondCard}; }

    protected:
        virtual auto isSuited() -> bool;
        virtual auto isBroadway() -> bool;
        virtual auto isPlur() -> bool;
        virtual auto isConnected() -> bool;

    private:
        Card firstCard;
        Card secondCard;
        bool suited    = false;
        bool broadway  = false;
        bool plur      = false;
        bool connected = false;
};