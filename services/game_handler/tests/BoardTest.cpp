#include <gtest/gtest.h>

#include <game_handler/Board.hpp>
#include <game_handler/CardFactory.hpp>

using GameHandler::Board;
using GameHandler::Factory::card;

using enum GameHandler::HandRank;

class BoardTest : public ::testing::Test {};

/**
 ╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 ║                                                  Board combo check                                                  ║
 ╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 */

TEST(BoardTest, hasPossibleStraightShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPossibleStraight());
    EXPECT_FALSE(Board({card("7S"), card("8H"), card("QD"), card("KD"), card("KS")}).hasPossibleStraight());
    EXPECT_TRUE(Board({card("7S"), card("8H"), card("QD"), card("KD"), card("AS")}).hasPossibleStraight());
    EXPECT_TRUE(Board({card("2S"), card("3H"), card("QD"), card("KD"), card("AS")}).hasPossibleStraight());
}

TEST(BoardTest, hasPossibleFlushDrawShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPossibleFlushDraw());
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("3D")}).hasPossibleFlushDraw());
    EXPECT_TRUE(Board({card("AS"), card("AH"), card("KS")}).hasPossibleFlushDraw());
    EXPECT_TRUE(Board({card("2S"), card("3S"), card("3H"), card("6H")}).hasPossibleFlushDraw());
}

TEST(BoardTest, hasPossibleFlushShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPossibleFlush());
    EXPECT_FALSE(Board({card("2S"), card("2H"), card("7D"), card("7D")}).hasPossibleFlush());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("8D")}).hasPossibleFlush());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("9S"), card("7D"), card("AS")}).hasPossibleFlush());
}

TEST(BoardTest, hasPairShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasPair());
    EXPECT_FALSE(Board({card("2S"), card("AH"), card("7D"), card("8D"), card("9D")}).hasPair());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("8D")}).hasPair());
    EXPECT_TRUE(Board({card("2S"), card("AH"), card("9S"), card("7D"), card("AS")}).hasPair());
}

TEST(BoardTest, hasTwoPairShouldBeCorrect) {
    EXPECT_FALSE(Board({card("2S"), card("3H"), card("7D")}).hasTwoPair());
    EXPECT_FALSE(Board({card("2S"), card("AH"), card("7D"), card("7D"), card("9D")}).hasTwoPair());
    EXPECT_TRUE(Board({card("2S"), card("2H"), card("7D"), card("7D"), card("8D")}).hasTwoPair());
    EXPECT_TRUE(Board({card("AS"), card("7H"), card("7S"), card("8D"), card("AH")}).hasTwoPair());
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

/**
 ╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 ║                                              Board + hand combo check                                               ║
 ╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 */

TEST(BoardTest, handRankHighCardShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D")}).getHandRank({card("9D"), card("4D")}), HIGH_CARD);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("8D")}).getHandRank({card("9D"), card("TD")}), HIGH_CARD);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("6D"), card("7H")}).getHandRank({card("9D"), card("TD")}), HIGH_CARD);
    EXPECT_EQ(Board({card("2S"), card("4H"), card("7D"), card("3S"), card("8D")}).getHandRank({card("KD"), card("JD")}), HIGH_CARD);
}

TEST(BoardTest, handRankPairShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D")}).getHandRank({card("9D"), card("3D")}), PAIR);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("8D")}).getHandRank({card("9D"), card("8D")}), PAIR);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("7H")}).getHandRank({card("9D"), card("TD")}), PAIR);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("3S"), card("8D")}).getHandRank({card("KD"), card("JD")}), PAIR);
}

TEST(BoardTest, handRankTwoPairShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D")}).getHandRank({card("2D"), card("7C")}), TWO_PAIR);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("8D")}).getHandRank({card("2D"), card("8D")}), TWO_PAIR);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("7H")}).getHandRank({card("9D"), card("9S")}), TWO_PAIR);
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D"), card("3S"), card("8D")}).getHandRank({card("KD"), card("2D")}), TWO_PAIR);
}

TEST(BoardTest, handRankTripsShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3H"), card("7D")}).getHandRank({card("7H"), card("7C")}), TRIPS);
    EXPECT_EQ(Board({card("2S"), card("8H"), card("7D"), card("8D")}).getHandRank({card("AD"), card("8C")}), TRIPS);
    EXPECT_EQ(Board({card("2S"), card("9H"), card("7D"), card("6H")}).getHandRank({card("9D"), card("9S")}), TRIPS);
    EXPECT_EQ(Board({card("AS"), card("AH"), card("7D"), card("3S"), card("8D")}).getHandRank({card("AD"), card("2D")}), TRIPS);
}

TEST(BoardTest, handRankStraightShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3H"), card("4D")}).getHandRank({card("5H"), card("6C")}), STRAIGHT);
    EXPECT_EQ(Board({card("8S"), card("8H"), card("7D"), card("6D")}).getHandRank({card("5D"), card("9C")}), STRAIGHT);
    EXPECT_EQ(Board({card("2S"), card("8H"), card("7D"), card("6H")}).getHandRank({card("9D"), card("TS")}), STRAIGHT);
    EXPECT_EQ(Board({card("AS"), card("AH"), card("KD"), card("QS"), card("AC")}).getHandRank({card("TD"), card("JD")}), STRAIGHT);
}

TEST(BoardTest, handRankFlushShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3S"), card("4S")}).getHandRank({card("5S"), card("7S")}), FLUSH);
    EXPECT_EQ(Board({card("8S"), card("8D"), card("7D"), card("6D")}).getHandRank({card("5D"), card("TD")}), FLUSH);
    EXPECT_EQ(Board({card("2S"), card("8S"), card("7S"), card("6S")}).getHandRank({card("KD"), card("TS")}), FLUSH);
    EXPECT_EQ(Board({card("AS"), card("AD"), card("KD"), card("QS"), card("QD")}).getHandRank({card("9D"), card("JD")}), FLUSH);
}

TEST(BoardTest, handRankFullShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("2H"), card("7D")}).getHandRank({card("7H"), card("7C")}), FULL);
    EXPECT_EQ(Board({card("2S"), card("8H"), card("7D"), card("8D")}).getHandRank({card("2D"), card("8C")}), FULL);
    EXPECT_EQ(Board({card("2S"), card("9H"), card("7D"), card("7H")}).getHandRank({card("9D"), card("9S")}), FULL);
    EXPECT_EQ(Board({card("AS"), card("AH"), card("7D"), card("2S"), card("8D")}).getHandRank({card("AD"), card("2D")}), FULL);
}

TEST(BoardTest, handRankQuadsShouldBeCorrect) {
    EXPECT_EQ(Board({card("7S"), card("3H"), card("7D")}).getHandRank({card("7H"), card("7C")}), QUADS);
    EXPECT_EQ(Board({card("2S"), card("8H"), card("8D"), card("8S")}).getHandRank({card("AD"), card("8C")}), QUADS);
    EXPECT_EQ(Board({card("2S"), card("9H"), card("9C"), card("6H")}).getHandRank({card("9D"), card("9S")}), QUADS);
    EXPECT_EQ(Board({card("AS"), card("AH"), card("7D"), card("3S"), card("8D")}).getHandRank({card("AD"), card("AC")}), QUADS);
}

TEST(BoardTest, handRankStraightFlushShouldBeCorrect) {
    EXPECT_EQ(Board({card("2S"), card("3S"), card("4S")}).getHandRank({card("5S"), card("6S")}), STRAIGHT_FLUSH);
    EXPECT_EQ(Board({card("8S"), card("8D"), card("7D"), card("6D")}).getHandRank({card("5D"), card("9D")}), STRAIGHT_FLUSH);
    EXPECT_EQ(Board({card("2S"), card("8S"), card("7S"), card("6S")}).getHandRank({card("9S"), card("TS")}), STRAIGHT_FLUSH);
    EXPECT_EQ(Board({card("AS"), card("AD"), card("KD"), card("QS"), card("QD")}).getHandRank({card("TD"), card("JD")}),
              STRAIGHT_FLUSH);
}

/**
 ╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 ║                                                  Winner hand check                                                  ║
 ╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 */

TEST(BoardTest, highCardHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("5D"), card("9C"), card("TC"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("3C"), card("8D")}, {card("4H"), card("7D")}), 1);  // High card vs high card inferior
    EXPECT_EQ(board_1.compareHands({card("3C"), card("QD")}, {card("4H"), card("8D")}), 1);  // High card vs high card inferior
    EXPECT_EQ(board_1.compareHands({card("AC"), card("KD")}, {card("AH"), card("QD")}), 1);  // High card vs high card inferior
    // Equal hands
    EXPECT_EQ(board_1.compareHands({card("3C"), card("7D")}, {card("4H"), card("7S")}), 0);  // High card vs high card equal
    EXPECT_EQ(board_1.compareHands({card("3C"), card("8D")}, {card("4H"), card("8C")}), 0);  // High card vs high card equal
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("4H"), card("AC")}), 0);  // High card vs high card equal
    // Loser hands
    EXPECT_EQ(board_1.compareHands({card("3C"), card("KD")}, {card("3H"), card("AD")}), -1);  // High card vs high card superior
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("2H"), card("QD")}), -1);  // High card vs pair over card
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("QH"), card("QD")}), -1);  // High card vs pair superior
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("2H"), card("9D")}), -1);  // High card vs two pairs
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("2H"), card("2D")}), -1);  // High card vs trips
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("QH"), card("KD")}), -1);  // High card vs straight
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("4C"), card("8C")}), -1);  // High card vs flush
    EXPECT_EQ(board_1.compareHands({card("3C"), card("AD")}, {card("8C"), card("QC")}), -1);  // High card vs straight flush
}

TEST(BoardTest, pairHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("5C"), card("9H"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("3D")}, {card("4H"), card("AD")}), 1);  // Pair vs high card
    EXPECT_EQ(board_1.compareHands({card("9C"), card("3D")}, {card("5H"), card("6D")}), 1);  // Pair vs pair inferior
    EXPECT_EQ(board_1.compareHands({card("8C"), card("8D")}, {card("5H"), card("6D")}), 1);  // Pair vs pair inferior
    EXPECT_EQ(board_1.compareHands({card("7C"), card("JD")}, {card("6H"), card("JH")}), 1);  // Pair vs pair down card
    // Equal hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("3D")}, {card("2H"), card("4D")}), 0);  // Pair vs pair
    EXPECT_EQ(board_1.compareHands({card("JH"), card("3D")}, {card("JD"), card("4D")}), 0);  // Pair vs pair
    EXPECT_EQ(board_1.compareHands({card("2C"), card("6D")}, {card("2H"), card("7D")}), 0);  // Pair vs pair
    // Loser hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("3D")}, {card("2H"), card("QD")}), -1);  // Pair vs pair over card
    EXPECT_EQ(board_1.compareHands({card("3C"), card("TD")}, {card("QH"), card("QD")}), -1);  // Pair vs pair superior
    EXPECT_EQ(board_1.compareHands({card("3C"), card("TD")}, {card("2H"), card("9D")}), -1);  // Pair vs two pairs
    EXPECT_EQ(board_1.compareHands({card("3C"), card("TD")}, {card("2H"), card("2D")}), -1);  // Pair vs trips
    EXPECT_EQ(board_1.compareHands({card("3C"), card("TD")}, {card("QH"), card("KD")}), -1);  // Pair vs straight
    EXPECT_EQ(board_2.compareHands({card("3C"), card("8D")}, {card("4C"), card("8C")}), -1);  // Pair vs flush
    EXPECT_EQ(board_2.compareHands({card("3C"), card("8D")}, {card("9H"), card("9D")}), -1);  // Pair vs full
    EXPECT_EQ(board_2.compareHands({card("3C"), card("8D")}, {card("2C"), card("2D")}), -1);  // Pair vs quads
    EXPECT_EQ(board_2.compareHands({card("3C"), card("8D")}, {card("8C"), card("QC")}), -1);  // Pair vs straight flush
}

TEST(BoardTest, twoPairsHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("5C"), card("9H"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});
    Board board_3({card("2S"), card("2H"), card("9C"), card("9D"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("5D")}, {card("4H"), card("AD")}), 1);  // Two pairs vs high card
    EXPECT_EQ(board_1.compareHands({card("9C"), card("TD")}, {card("4H"), card("JD")}), 1);  // Two pairs vs pair
    EXPECT_EQ(board_1.compareHands({card("9C"), card("TD")}, {card("QH"), card("QD")}), 1);  // Two pairs vs pair
    EXPECT_EQ(board_2.compareHands({card("8C"), card("9D")}, {card("QH"), card("AD")}), 1);  // Two pairs vs pair
    EXPECT_EQ(board_1.compareHands({card("5D"), card("TD")}, {card("5H"), card("9H")}), 1);  // Two pairs vs two pairs inferior
    EXPECT_EQ(board_2.compareHands({card("3C"), card("JD")}, {card("4H"), card("TD")}), 1);  // Two pairs vs two pairs inferior
    EXPECT_EQ(board_2.compareHands({card("9D"), card("JD")}, {card("9H"), card("TD")}), 1);  // Two pairs vs two pairs inferior
    EXPECT_EQ(board_3.compareHands({card("JD"), card("KD")}, {card("JH"), card("QD")}), 1);  // Two pairs vs two pairs inferior
    EXPECT_EQ(board_3.compareHands({card("JD"), card("KD")}, {card("TH"), card("TD")}), 1);  // Two pairs vs two pairs inferior
    EXPECT_EQ(board_2.compareHands({card("9D"), card("AD")}, {card("9H"), card("KC")}), 1);  // Two pairs vs two pairs down card
    EXPECT_EQ(board_3.compareHands({card("3C"), card("QD")}, {card("4C"), card("8C")}), 1);  // Two pairs vs two pairs down card
    // Equal hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("5D")}, {card("2H"), card("5H")}), 0);  // Two pairs vs two pairs
    EXPECT_EQ(board_1.compareHands({card("TH"), card("JD")}, {card("TD"), card("JH")}), 0);  // Two pairs vs two pairs
    EXPECT_EQ(board_2.compareHands({card("8C"), card("9D")}, {card("9H"), card("3D")}), 0);  // Two pairs vs two pairs
    EXPECT_EQ(board_3.compareHands({card("3C"), card("8D")}, {card("3H"), card("TC")}), 0);  // Two pairs vs two pairs
    // Loser hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("TD")}, {card("2H"), card("JD")}), -1);  // Two pair vs two pairs superior
    EXPECT_EQ(board_1.compareHands({card("2C"), card("TD")}, {card("2H"), card("2D")}), -1);  // Two pair vs trips
    EXPECT_EQ(board_1.compareHands({card("2C"), card("TD")}, {card("QH"), card("KD")}), -1);  // Two pair vs straight
    EXPECT_EQ(board_2.compareHands({card("3C"), card("TD")}, {card("4C"), card("8C")}), -1);  // Two pair vs flush
    EXPECT_EQ(board_2.compareHands({card("3C"), card("TD")}, {card("9H"), card("9D")}), -1);  // Two pair vs full
    EXPECT_EQ(board_2.compareHands({card("3C"), card("TD")}, {card("2C"), card("2D")}), -1);  // Two pair vs quads
    EXPECT_EQ(board_2.compareHands({card("3C"), card("TD")}, {card("8C"), card("QC")}), -1);  // Two pair vs straight flush
}

TEST(BoardTest, tripsHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("5C"), card("9H"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});
    Board board_3({card("2S"), card("9H"), card("9C"), card("9D"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("2C"), card("2D")}, {card("4H"), card("AD")}), 1);  // Trips vs high card
    EXPECT_EQ(board_1.compareHands({card("9C"), card("9D")}, {card("QH"), card("QD")}), 1);  // Trips vs pair
    EXPECT_EQ(board_2.compareHands({card("2C"), card("AD")}, {card("QH"), card("AD")}), 1);  // Trips vs pair
    EXPECT_EQ(board_1.compareHands({card("5D"), card("5H")}, {card("5S"), card("9C")}), 1);  // Trips vs two pairs
    EXPECT_EQ(board_2.compareHands({card("2C"), card("8D")}, {card("4H"), card("JD")}), 1);  // Trips vs two pairs
    EXPECT_EQ(board_2.compareHands({card("2C"), card("8D")}, {card("KH"), card("KD")}), 1);  // Trips vs two pairs
    EXPECT_EQ(board_1.compareHands({card("TH"), card("TD")}, {card("9C"), card("9D")}), 1);  // Trips vs trips inferior
    EXPECT_EQ(board_2.compareHands({card("2C"), card("QD")}, {card("2D"), card("8D")}), 1);  // Trips vs trips down card
    // Equal hands
    EXPECT_EQ(board_2.compareHands({card("2C"), card("5D")}, {card("2D"), card("5H")}), 0);  // Trips vs trips
    EXPECT_EQ(board_2.compareHands({card("2C"), card("8D")}, {card("2D"), card("7H")}), 0);  // Trips vs trips
    EXPECT_EQ(board_3.compareHands({card("6D"), card("4H")}, {card("5D"), card("6S")}), 0);  // Trips vs trips
    // Loser hands
    EXPECT_EQ(board_2.compareHands({card("2D"), card("AH")}, {card("9D"), card("9S")}), -1);  // Trips vs trips superior
    EXPECT_EQ(board_1.compareHands({card("2C"), card("2D")}, {card("QH"), card("KD")}), -1);  // Trips vs straight
    EXPECT_EQ(board_2.compareHands({card("2C"), card("8D")}, {card("4C"), card("8C")}), -1);  // Trips vs flush
    EXPECT_EQ(board_2.compareHands({card("2C"), card("AD")}, {card("2D"), card("9D")}), -1);  // Trips vs full
    EXPECT_EQ(board_3.compareHands({card("TD"), card("AH")}, {card("9S"), card("3D")}), -1);  // Trips vs quads
    EXPECT_EQ(board_2.compareHands({card("2C"), card("8D")}, {card("8C"), card("QC")}), -1);  // Trips vs straight flush
}

TEST(BoardTest, straightHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("5C"), card("9H"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QD")}, {card("4H"), card("AD")}), 1);  // Straight vs high card
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QD")}, {card("QH"), card("QC")}), 1);  // Straight vs pair
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QD")}, {card("5S"), card("9C")}), 1);  // Straight vs two pairs
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QD")}, {card("9C"), card("9D")}), 1);  // Straight vs trips
    EXPECT_EQ(board_1.compareHands({card("QC"), card("KD")}, {card("8C"), card("QS")}), 1);  // Straight vs straight inferior
    // Equal hands
    EXPECT_EQ(board_1.compareHands({card("QC"), card("KD")}, {card("QD"), card("KS")}), 0);  // Straight vs straight
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QD")}, {card("8D"), card("QS")}), 0);  // Straight vs straight
    // Loser hands
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QS")}, {card("QC"), card("KD")}), -1);  // Straight vs straight superior
    EXPECT_EQ(board_2.compareHands({card("QC"), card("KD")}, {card("4C"), card("8C")}), -1);  // Straight vs flush
    EXPECT_EQ(board_2.compareHands({card("QC"), card("KD")}, {card("2D"), card("9D")}), -1);  // Straight vs full
    EXPECT_EQ(board_2.compareHands({card("QC"), card("KD")}, {card("2C"), card("2D")}), -1);  // Straight vs quads
    EXPECT_EQ(board_2.compareHands({card("QD"), card("KD")}, {card("8C"), card("QC")}), -1);  // Straight vs straight flush
}

TEST(BoardTest, flushHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("7C"), card("9C"), card("TC"), card("JC")});
    Board board_3({card("5C"), card("7C"), card("9C"), card("TC"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("7C"), card("QC")}, {card("4H"), card("AD")}), 1);  // Flush vs high card
    EXPECT_EQ(board_1.compareHands({card("7C"), card("QC")}, {card("QH"), card("QC")}), 1);  // Flush vs pair
    EXPECT_EQ(board_1.compareHands({card("7C"), card("QC")}, {card("5S"), card("9C")}), 1);  // Flush vs two pairs
    EXPECT_EQ(board_1.compareHands({card("7C"), card("QC")}, {card("2C"), card("8D")}), 1);  // Flush vs trips
    EXPECT_EQ(board_1.compareHands({card("7C"), card("KC")}, {card("8C"), card("QS")}), 1);  // Flush vs straight
    EXPECT_EQ(board_1.compareHands({card("7C"), card("KC")}, {card("5C"), card("QC")}), 1);  // Flush vs flush inferior
    // Equal hands
    EXPECT_EQ(board_3.compareHands({card("2D"), card("TD")}, {card("QD"), card("KD")}), 0);  // Flush vs flush
    EXPECT_EQ(board_3.compareHands({card("2C"), card("QD")}, {card("4C"), card("QS")}), 0);  // Flush vs flush
    // Loser hands
    EXPECT_EQ(board_2.compareHands({card("4C"), card("KC")}, {card("QC"), card("AC")}), -1);  // Flush vs flush superior
    EXPECT_EQ(board_1.compareHands({card("QC"), card("AC")}, {card("2D"), card("9D")}), -1);  // Flush vs full
    EXPECT_EQ(board_1.compareHands({card("QC"), card("AC")}, {card("2C"), card("2D")}), -1);  // Flush vs quads
    EXPECT_EQ(board_1.compareHands({card("5C"), card("KC")}, {card("8C"), card("QC")}), -1);  // Flush vs straight flush
}

TEST(BoardTest, fullHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("2H"), card("2C"), card("7C"), card("JC")});
    Board board_3({card("KS"), card("KH"), card("KC"), card("7C"), card("7D")});
    Board board_4({card("8S"), card("8H"), card("KC"), card("7C"), card("9D")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("QH"), card("AC")}), 1);  // Full vs pair
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("KS"), card("KC")}), 1);  // Full vs two pairs
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("2D"), card("8D")}), 1);  // Full vs trips
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("QC"), card("KS")}), 1);  // Full vs straight
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("5C"), card("QC")}), 1);  // Full vs flush
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("9C"), card("2D")}), 1);  // Full vs full inferior
    EXPECT_EQ(board_4.compareHands({card("9C"), card("9H")}, {card("KD"), card("8C")}), 1);  // Full vs full inferior
    // Equal hands
    EXPECT_EQ(board_2.compareHands({card("7D"), card("AD")}, {card("7S"), card("3H")}), 0);  // Full vs full
    EXPECT_EQ(board_3.compareHands({card("5D"), card("5C")}, {card("6D"), card("6C")}), 0);  // Full vs full
    EXPECT_EQ(board_3.compareHands({card("7H"), card("5D")}, {card("7S"), card("6D")}), 0);  // Full vs full
    // Loser hands
    EXPECT_EQ(board_2.compareHands({card("7D"), card("7H")}, {card("JD"), card("JH")}), -1);  // Full vs full superior
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("2C"), card("2D")}), -1);  // Full vs quads
    EXPECT_EQ(board_1.compareHands({card("TD"), card("2C")}, {card("8C"), card("QC")}), -1);  // Full vs straight flush
}

TEST(BoardTest, quadsHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});
    Board board_2({card("8S"), card("8H"), card("8C"), card("7C"), card("7D")});
    Board board_3({card("8S"), card("8H"), card("8C"), card("TC"), card("TD")});
    Board board_4({card("8S"), card("8H"), card("8C"), card("8D"), card("TD")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("QH"), card("AC")}), 1);  // Quads vs pair
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("KS"), card("KC")}), 1);  // Quads vs two pairs
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("9D"), card("9H")}), 1);  // Quads vs trips
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("QC"), card("KS")}), 1);  // Quads vs straight
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("5C"), card("QC")}), 1);  // Quads vs flush
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("9C"), card("2D")}), 1);  // Quads vs full
    EXPECT_EQ(board_4.compareHands({card("KD"), card("3C")}, {card("QH"), card("TC")}), 1);  // Quads vs quads down card
    EXPECT_EQ(board_1.compareHands({card("2D"), card("8D")}, {card("7H"), card("7S")}), 1);  // Quads vs quads inferior
    // Equal hands
    EXPECT_EQ(board_4.compareHands({card("5D"), card("7D")}, {card("TH"), card("TS")}), 0);  // Quads vs quads
    EXPECT_EQ(board_4.compareHands({card("9D"), card("9C")}, {card("2H"), card("3S")}), 0);  // Quads vs quads
    // Loser hands
    EXPECT_EQ(board_4.compareHands({card("5D"), card("7D")}, {card("TH"), card("JS")}), -1);  // Quads vs quads up card
    EXPECT_EQ(board_3.compareHands({card("AD"), card("8D")}, {card("TH"), card("TS")}), -1);  // Quads vs quads superior
    EXPECT_EQ(board_1.compareHands({card("2D"), card("2C")}, {card("8C"), card("QC")}), -1);  // Quads vs straight flush
}

TEST(BoardTest, straightFlushHandComparisonShouldBeCorrect) {
    Board board_1({card("2S"), card("5C"), card("9H"), card("TC"), card("JC")});
    Board board_2({card("2S"), card("2H"), card("9C"), card("TC"), card("JC")});
    Board board_3({card("7C"), card("8C"), card("9C"), card("TC"), card("JC")});

    // Winner hands
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QC")}, {card("4H"), card("AD")}), 1);  // Straight flush vs high card
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QC")}, {card("JH"), card("AC")}), 1);  // Straight flush vs pair
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QC")}, {card("9S"), card("TD")}), 1);  // Straight flush vs two pairs
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QC")}, {card("9D"), card("9H")}), 1);  // Straight flush vs trips
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QC")}, {card("QC"), card("KS")}), 1);  // Straight flush vs straight
    EXPECT_EQ(board_1.compareHands({card("8C"), card("QC")}, {card("5C"), card("QC")}), 1);  // Straight flush vs flush
    EXPECT_EQ(board_2.compareHands({card("8C"), card("QC")}, {card("9C"), card("2D")}), 1);  // Straight flush vs full
    EXPECT_EQ(board_2.compareHands({card("8C"), card("QC")}, {card("2D"), card("2C")}), 1);  // Straight flush vs quads
    EXPECT_EQ(board_2.compareHands({card("QC"), card("KC")}, {card("8C"), card("7C")}), 1);  // Straight flush vs SF inferior
    // Equal hands
    EXPECT_EQ(board_3.compareHands({card("AC"), card("7D")}, {card("TH"), card("TS")}), 0);  // Straight flush vs straight flush
    EXPECT_EQ(board_3.compareHands({card("9D"), card("9H")}, {card("6C"), card("3S")}), 0);  // Straight flush vs straight flush
    // Loser hands
    EXPECT_EQ(board_3.compareHands({card("5D"), card("6C")}, {card("QC"), card("JS")}), -1);  // Straight flush vs SF superior
}

/**
 ╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 ║                                            Winner hand additional check                                             ║
 ╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 */

TEST(BoardTest, HandsComparisonShouldBeCorrect) {
    Board board({card("6C"), card("9S"), card("6H"), card("2S"), card("3C")});

    EXPECT_EQ(board.compareHands({card("KS"), card("7S")}, {card("8D"), card("8C")}), -1);
}

/**
 ╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 ║                                        Best five cards selection regressions                                        ║
 ╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝

 The four deals below all used to pick the wrong winner: the best five cards were the first five met while iterating
 the board then the hole cards, not the five best ranked ones. Found by brute forcing the Rust port against an
 independent C(7,5) evaluator, roughly one deal in 450 was decided wrongly.
 */

TEST(BoardTest, flushShouldKeepItsHighestCards) {
    Board board({card("7H"), card("TH"), card("KC"), card("JH"), card("QH")});

    // A-Q-J-T-7 flush beats Q-J-T-8-7, the ace is a hole card and used to be truncated away
    EXPECT_EQ(board.compareHands({card("6H"), card("AH")}, {card("4D"), card("8H")}), 1);
}

TEST(BoardTest, straightShouldNotBeEvictedByADuplicatedRank) {
    Board board({card("9C"), card("4H"), card("JH"), card("QH"), card("TS")});

    // Both play the same Q-J-T-9-8 straight, the duplicated ten used to take the eight's slot
    EXPECT_EQ(board.compareHands({card("TD"), card("8S")}, {card("8D"), card("QC")}), 0);
}

TEST(BoardTest, fullHouseShouldCompareItsTripsFirst) {
    Board board({card("AD"), card("7S"), card("KD"), card("AC"), card("7H")});

    // Kings full of aces loses to aces full of sevens
    EXPECT_EQ(board.compareHands({card("KS"), card("KC")}, {card("AH"), card("4S")}), -1);
}

TEST(BoardTest, straightFlushShouldIgnoreHigherMixedSuitsStraights) {
    Board board({card("5H"), card("6H"), card("7H"), card("8H"), card("9H")});

    // Both play the board's nine high straight flush, the off suit ten used to point the run one rank too high
    EXPECT_EQ(board.compareHands({card("TS"), card("2C")}, {card("4H"), card("2D")}), 0);
}

/**
 ╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗
 ║                                              JSON representation check                                              ║
 ╚═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╝
 */

TEST(BoardTest, jsonRepresentationShouldBeCorrect) {
    // language=json
    auto expectedJson = R"(
        [
            { "shortName": "AH", "rank": "Ace", "suit": "Heart" },
            { "shortName": "QH", "rank": "Queen", "suit": "Heart" },
            { "shortName": "JH", "rank": "Jack", "suit": "Heart" },
            { "shortName": "TH", "rank": "Ten", "suit": "Heart" },
            { "shortName": "KH", "rank": "King", "suit": "Heart" }
        ]
    )"_json;

    EXPECT_EQ(Board({card("AH"), card("QH"), card("JH"), card("TH"), card("KH")}).toJson(), expectedJson);
}

TEST(BoardTest, jsonDetailedRepresentationShouldBeCorrect) {
    // language=json
    auto expectedJson = R"(
        {
            "cards": [
                { "shortName": "AH", "rank": "Ace", "suit": "Heart" },
                { "shortName": "QH", "rank": "Queen", "suit": "Heart" },
                { "shortName": "JH", "rank": "Jack", "suit": "Heart" },
                { "shortName": "TH", "rank": "Ten", "suit": "Heart" },
                { "shortName": "KH", "rank": "King", "suit": "Heart" }
            ],
            "properties": {
                "paire": false,
                "doublePaire": false,
                "trips": false,
                "straight": true,
                "possibleStraight": true,
                "flush": true,
                "possibleFlush": true,
                "possibleFlushDraw": true,
                "full": false,
                "quads": false,
                "straightFlush": true
            }
        }
    )"_json;

    EXPECT_EQ(Board({card("AH"), card("QH"), card("JH"), card("TH"), card("KH")}).toDetailedJson(), expectedJson);
}
