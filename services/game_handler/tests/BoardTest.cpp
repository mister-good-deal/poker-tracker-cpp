#include <gtest/gtest.h>

#include "Board.hpp"
#include "CardFactory.hpp"

using GameHandler::Board;
using GameHandler::Factory::card;

class BoardTest : public ::testing::Test {};

TEST(BoardTest, gutShotsShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3S"), card("7S"), card("8S"), card("QS")}).getGutShots(), 0);
    EXPECT_EQ(Board({card("2S"), card("3S"), card("4S"), card("5S"), card("KS")}).getGutShots(), 2);
    EXPECT_EQ(Board({card("2S"), card("3S"), card("5S"), card("6S"), card("KS")}).getGutShots(), 1);
}

TEST(BoardTest, hasFlushDrawShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasFlushDraw());
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("3D")}).hasFlushDraw());
    EXPECT_TRUE(Board({card("AS"), card("AH"), card("KS")}).hasFlushDraw());
    EXPECT_TRUE(Board({card("2S"), card("3S"), card("3H"), card("6H"), card("KD")}).hasFlushDraw());
}

TEST(BoardTest, hasPossibleStraightShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPossibleStraight());
    EXPECT_FALSE(Board({card("7S"), card("8H"), card("QD"), card("KD"), card("KS")}).hasPossibleStraight());
    EXPECT_TRUE(Board({card("7S"), card("8H"), card("QD"), card("KD"), card("AS")}).hasPossibleStraight());
    EXPECT_TRUE(Board({card("2S"), card("3H"), card("QD"), card("KD"), card("AS")}).hasPossibleStraight());
}

TEST(BoardTest, hasPossibleFlushShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPossibleFlush());
    EXPECT_FALSE(Board({card("2S"), card("2H"), card("7D"), card("7D")}).hasPossibleFlush());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("8D")}).hasPossibleFlush());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("9S"), card("7D"), card("AS")}).hasPossibleFlush());
}

TEST(BoardTest, hasPaireShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPaire());
    EXPECT_FALSE(Board({card("2S"), card("AH"), card("7D"), card("8D"), card("9D")}).hasPaire());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("8D")}).hasPaire());
    EXPECT_TRUE(Board({card("2S"), card("AH"), card("9S"), card("7D"), card("AS")}).hasPaire());
}

TEST(BoardTest, hasDoublePaireShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasDoublePaire());
    EXPECT_FALSE(Board({card("2S"), card("AH"), card("7D"), card("7D"), card("9D")}).hasDoublePaire());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("8D")}).hasDoublePaire());
    EXPECT_TRUE(Board({card("AS"), card("7H"), card("7S"), card("8D"), card("AH")}).hasDoublePaire());
}

TEST(BoardTest, hasTripsShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasTrips());
    EXPECT_FALSE(Board({card("AS"), card("AH"), card("7D"), card("7D"), card("9D")}).hasTrips());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("2D")}).hasTrips());
    EXPECT_TRUE(Board({card("AS"), card("7H"), card("7S"), card("7D"), card("AH")}).hasTrips());
}

TEST(BoardTest, hasStraightShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasStraight());
    EXPECT_FALSE(Board({card("7S"), card("8H"), card("9D"), card("6D"), card("4D")}).hasStraight());
    EXPECT_TRUE(Board({card("AS"), card("2H"), card("3D"), card("4D"), card("5D")}).hasStraight());
    EXPECT_TRUE(Board({card("AS"), card("QH"), card("JS"), card("TD"), card("KH")}).hasStraight());
}

TEST(BoardTest, hasFlushShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasFlush());
    EXPECT_FALSE(Board({card("7S"), card("8S"), card("9S"), card("6S")}).hasFlush());
    EXPECT_TRUE(Board({card("AS"), card("KS"), card("3S"), card("4S"), card("5S")}).hasFlush());
    EXPECT_TRUE(Board({card("AH"), card("QH"), card("JH"), card("TH"), card("KH")}).hasFlush());
}

TEST(BoardTest, hasFullShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasFull());
    EXPECT_FALSE(Board({card("7S"), card("7H"), card("7D"), card("6S")}).hasFull());
    EXPECT_TRUE(Board({card("AH"), card("AS"), card("AD"), card("2C"), card("2S")}).hasFull());
    EXPECT_TRUE(Board({card("KH"), card("3S"), card("3D"), card("3C"), card("KD")}).hasFull());
}

TEST(BoardTest, hasQuadsShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasQuads());
    EXPECT_FALSE(Board({card("7S"), card("7H"), card("7D"), card("6S")}).hasQuads());
    EXPECT_TRUE(Board({card("AH"), card("AS"), card("AD"), card("AC"), card("2S")}).hasQuads());
    EXPECT_TRUE(Board({card("KH"), card("3S"), card("3D"), card("3C"), card("3H")}).hasQuads());
}

TEST(BoardTest, hasStraightFlushShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasStraightFlush());
    EXPECT_FALSE(Board({card("7S"), card("8S"), card("9S"), card("6S")}).hasStraightFlush());
    EXPECT_TRUE(Board({card("AS"), card("2S"), card("3S"), card("4S"), card("5S")}).hasStraightFlush());
    EXPECT_TRUE(Board({card("AH"), card("QH"), card("JH"), card("TH"), card("KH")}).hasStraightFlush());
}

TEST(BoardTest, jsonRepresentationShouldBeCorrect) {
    // language=json
    auto expectedJson = R"(
        {
            "cards": [
                {
                    "shortName": "AH",
                    "fullName": "Ace of Heart",
                    "rank": "Ace",
                    "suit": "Heart"
                },
                {
                    "shortName": "QH",
                    "fullName": "Queen of Heart",
                    "rank": "Queen",
                    "suit": "Heart"
                },
                {
                    "shortName": "JH",
                    "fullName": "Jack of Heart",
                    "rank": "Jack",
                    "suit": "Heart"
                },
                {
                    "shortName": "TH",
                    "fullName": "Ten of Heart",
                    "rank": "Ten",
                    "suit": "Heart"
                },
                {
                    "shortName": "KH",
                    "fullName": "King of Heart",
                    "rank": "King",
                    "suit": "Heart"
                }
            ],
            "properties": {
                "paire": false,
                "doublePaire": false,
                "trips": false,
                "straight": true,
                "possibleStraight": true,
                "gutShots": 0,
                "flush": true,
                "possibleFlush": true,
                "flushDraw": false,
                "full": false,
                "quads": false,
                "straightFlush": true
            }
        }
    )"_json;

    EXPECT_EQ(Board({card("AH"), card("QH"), card("JH"), card("TH"), card("KH")}).toJson(), expectedJson);
}
