#include "game_handler/Round.hpp"

#include <utility>

#include <ranges>

namespace GameHandler {
    using fmt::format;
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    using std::ranges::all_of;
    using std::ranges::any_of;
    using std::ranges::count_if;
    using std::ranges::find_if;
    using std::ranges::for_each;
    using std::ranges::sort;
    using std::views::filter;

    using enum Round::Street;

    namespace {
        auto playerIsInRound   = [](const PlayerStatus& player) { return player.inRound; };
        auto playerIsAllIn     = [](const PlayerStatus& player) { return player.inRound && player.isAllIn; };
        auto playerIsNotBusted = [](const PlayerStatus& player) { return !player.isEliminated(); };
        // Seat order starting left of the button: small blind, big blind then the dealer
        auto buttonOrder = [](const PlayerStatus& player) {
            return player.position == DEALER ? 3 : static_cast<int32_t>(player.position);
        };
    }  // namespace

    Round::Round(const Blinds& blinds, std::array<Player, 3>& players, Hand hand, int32_t dealerNumber)
      : _blinds(blinds)
      , _players(&players)
      , _lastActionTime(system_clock::now())
      , _hand(hand)
      , _dealerPlayerNum(dealerNumber)
      , _currentPlayerNum(dealerNumber)
      , _playersStatus(std::make_unique<players_status_t>(players_status_t {{PlayerStatus {&players[0], dealerNumber},
                                                                             PlayerStatus {&players[1], dealerNumber},
                                                                             PlayerStatus {&players[2], dealerNumber}}})) {
        auto remainingPlayers = count_if(*_playersStatus, playerIsNotBusted);

        if (remainingPlayers == 3) {
            _smallBlindPlayerNum = _getNextPlayerNum(dealerNumber);
        } else {
            _smallBlindPlayerNum = dealerNumber;
        }

        _bigBlindPlayerNum = _getNextPlayerNum(_smallBlindPlayerNum);

        _getPlayerStatus(1).hand = std::move(hand);
        _payBlinds();
    }

    auto Round::operator=(const Round& other) -> Round& {
        if (this != &other) {
            _actions             = other._actions;
            _board               = other._board;
            _ranking             = other._ranking;
            _playersRoundRecap   = other._playersRoundRecap;
            _blinds              = other._blinds;
            _pot                 = other._pot;
            _currentStreet       = other._currentStreet;
            _lastActionTime      = other._lastActionTime;
            _players             = other._players;
            _ended               = other._ended;
            _hand                = other._hand;
            _dealerPlayerNum     = other._dealerPlayerNum;
            _smallBlindPlayerNum = other._smallBlindPlayerNum;
            _bigBlindPlayerNum   = other._bigBlindPlayerNum;
            _currentPlayerNum    = other._currentPlayerNum;
            _playerGotBusted     = other._playerGotBusted;
            _playersStatus       = std::make_unique<players_status_t>(*_playersStatus);
        }

        return *this;
    }

    auto Round::operator=(Round&& other) noexcept -> Round& {
        if (this != &other) {
            _actions             = std::move(other._actions);
            _board               = std::move(other._board);
            _ranking             = std::move(other._ranking);
            _playersStatus       = std::move(other._playersStatus);
            _hand                = std::move(other._hand);
            _playersRoundRecap   = other._playersRoundRecap;
            _dealerPlayerNum     = other._dealerPlayerNum;
            _smallBlindPlayerNum = other._smallBlindPlayerNum;
            _bigBlindPlayerNum   = other._bigBlindPlayerNum;
            _currentPlayerNum    = other._currentPlayerNum;
            _blinds              = other._blinds;
            _pot                 = other._pot;
            _currentStreet       = other._currentStreet;
            _lastActionTime      = other._lastActionTime;
            _players             = other._players;
            _ended               = other._ended;
            _playerGotBusted     = other._playerGotBusted;
        }

        return *this;
    }

    auto Round::call(int32_t playerNum) -> void {
        auto& player         = _getPlayerStatus(playerNum);
        auto  computedAmount = std::min(_lastBetOrRaise - player.totalStreetBet, player.getStack());

        _setAction(playerNum, CALL, computedAmount);
    }

    auto Round::bet(int32_t playerNum, int32_t amount) -> void { _setAction(playerNum, BET, amount); }

    auto Round::raiseTo(int32_t playerNum, int32_t amount) -> void {
        auto& player = _getPlayerStatus(playerNum);

        // A raise is made to a total street bet, anything below the player's own street bet would take chips back out
        // of the pot and let the next calls invent chips
        if (amount <= player.totalStreetBet) {
            throw std::invalid_argument(format("Player {} cannot raise to {}, its street bet is already {}", playerNum, amount,
                                               player.totalStreetBet));
        }

        _setAction(playerNum, RAISE, amount - player.totalStreetBet);
    }

    auto Round::check(int32_t playerNum) -> void { _setAction(playerNum, CHECK); }
    auto Round::fold(int32_t playerNum) -> void { _setAction(playerNum, FOLD); }

    auto Round::allIn(int32_t playerNum) -> void {
        const auto& player = _getPlayerStatus(playerNum);

        if (_streetPot == 0) {
            bet(playerNum, player.initialStack - player.totalBet);
        } else if (player.getStack() > _lastBetOrRaise) {
            raiseTo(playerNum, player.initialStack);
        } else {
            call(playerNum);
        }
    }

    auto Round::getInRoundPlayersNum() const -> std::vector<int32_t> {
        std::vector<int32_t> inRoundPlayersNum;

        for (const auto& player : *_playersStatus) {
            if (player.inRound) { inRoundPlayersNum.emplace_back(player.getNumber()); }
        }

        return inRoundPlayersNum;
    }

    auto Round::isNextActionTheLastStreetOne(int32_t playerNum) const -> bool {
        // All other players in the round are all in
        return !any_of(*_playersStatus, [playerNum](const auto& player) {
            return player.inRound && player.getNumber() != playerNum && !player.isAllIn;
        });
    }

    auto Round::getCurrentPlayerStack(int32_t playerNum) const -> int32_t { return _getPlayerStatus(playerNum).getStack(); }
    auto Round::waitingShowdown() const -> bool { return !_ended && _currentStreet == Street::SHOWDOWN; }
    auto Round::showdown() -> void { _endRound(); }

    auto Round::toJson() const -> json {
        if (_ranking.empty()) { throw std::runtime_error("The round's ranking has not been set"); }

        auto preFlopActions = json::array();
        auto flopActions    = json::array();
        auto turnActions    = json::array();
        auto riverActions   = json::array();
        auto hands          = json::object();

        for_each(_actions[PREFLOP], [&](const RoundAction& action) { preFlopActions.emplace_back(action.toJson()); });
        for_each(_actions[FLOP], [&](const RoundAction& action) { flopActions.emplace_back(action.toJson()); });
        for_each(_actions[TURN], [&](const RoundAction& action) { turnActions.emplace_back(action.toJson()); });
        for_each(_actions[RIVER], [&](const RoundAction& action) { riverActions.emplace_back(action.toJson()); });

        for (const auto& player : *_playersStatus) { hands.emplace(format("player_{}", player.getNumber()), player.hand.toJson()); }

        return {{"actions", {{"pre_flop", preFlopActions}, {"flop", flopActions}, {"turn", turnActions}, {"river", riverActions}}},
                {"board", _board.toJson()},
                {"hands", hands},
                {"blinds", {{"small", _blinds.SB()}, {"big", _blinds.BB()}}},
                {"pot", _pot},
                {"won", _hasWon()},
                {"positions",
                 {{"dealer", format("player_{}", _dealerPlayerNum)},
                  {"small_blind", format("player_{}", _smallBlindPlayerNum)},
                  {"big_blind", format("player_{}", _bigBlindPlayerNum)}}},
                {"stacks", toJson(_playersRoundRecap)},
                {"ranking", toJson(_ranking)}};
    }

    auto Round::toJson(const ranking_t& ranking) -> json {
        auto rankingJson = json::array();
        auto rankingCopy = ranking;

        while (!rankingCopy.empty()) {
            auto rankStepJson = json::array();
            auto rankStep     = rankingCopy.top();

            for (const auto& playerNum : rankStep) { rankStepJson.emplace_back(fmt::format("player_{}", playerNum)); }

            rankingJson.emplace_back(rankStepJson);
            rankingCopy.pop();
        }

        return rankingJson;
    }

    auto Round::toJson(const players_round_recap_t& playersRoundRecap) -> json {
        auto playersStack = json::array();

        for (const auto& recap : playersRoundRecap) {
            playersStack.emplace_back(json::object({{"player", fmt::format("player_{}", recap.playerNumber)},
                                                    {"stack", recap.endStack},
                                                    {"balance", recap.endStack - recap.startStack}}));
        }

        return playersStack;
    }

    auto Round::_getPlayerStatus(int32_t playerNum) -> PlayerStatus& {
        if (playerNum <= 0 || playerNum > 3) { throw std::invalid_argument("The given player number is invalid"); }

        return _playersStatus->at(playerNum - 1);
    }

    auto Round::_getPlayerStatus(int32_t playerNum) const -> PlayerStatus {
        if (playerNum <= 0 || playerNum > 3) { throw std::invalid_argument("The given player number is invalid"); }

        return _playersStatus->at(playerNum - 1);
    }

    auto Round::_getNextPlayerNum(int32_t currentPlayerNum) const -> int32_t {
        if (currentPlayerNum <= 0 || currentPlayerNum > 3) { throw std::invalid_argument("The given player number is invalid"); }

        int32_t nextPlayerNum = (currentPlayerNum % 3) + 1;

        while (nextPlayerNum != currentPlayerNum) {
            if (_getPlayerStatus(nextPlayerNum).inRound) { return nextPlayerNum; }

            nextPlayerNum = (nextPlayerNum % 3) + 1;
        }

        throw std::runtime_error("No next player found");
    }

    auto Round::_getAndResetLastActionTime() -> seconds {
        auto now            = system_clock::now();
        auto lastActionTime = duration_cast<seconds>(now - _lastActionTime);

        _lastActionTime = now;

        return lastActionTime;
    }

    auto Round::_isStreetOver() const -> bool {
        auto hasChecked = [](const PlayerStatus& player) { return !player.inRound || player.isAllIn || player.lastAction == CHECK; };
        auto hasPlayed  = [](const PlayerStatus& player) { return !player.inRound || player.isAllIn || player.lastAction != NONE; };
        // All in players are considered in the round, hasPlayed and hasChecked
        auto roundPlayersCount    = count_if(*_playersStatus, playerIsInRound);
        auto allPlayersPlayed     = all_of(*_playersStatus, hasPlayed);
        auto allPlayersHasChecked = all_of(*_playersStatus, hasChecked);

        bool currentPlayerCalled          = _currentAction.getAction() == CALL;
        bool currentPlayerFold            = _currentAction.getAction() == FOLD;
        bool currentPlayerCalledOrChecked = currentPlayerCalled || _currentAction.getAction() == CHECK;
        bool currentPlayerDidNotRaise     = _lastAction.getAction() != RAISE && _lastAction.getAction() != BET;
        bool lastPlayerCalledOrFold       = _lastAction.getAction() == CALL || _lastAction.getAction() == FOLD;
        bool lastPlayerDidNotRaise        = _lastAction.getAction() != RAISE && _lastAction.getAction() != BET;

        // clang-format off
        return  roundPlayersCount == 1 
            || (roundPlayersCount == 2 && allPlayersPlayed && currentPlayerCalledOrChecked)
            || (roundPlayersCount == 2 && allPlayersPlayed && currentPlayerFold && lastPlayerCalledOrFold)
            || (roundPlayersCount == 3 && allPlayersPlayed && currentPlayerDidNotRaise && lastPlayerDidNotRaise)
            || allPlayersHasChecked;
        // clang-format on
    }

    auto Round::_setAction(int32_t playerNum, ActionType actionType, int32_t amount) -> void {
        auto& player = _getPlayerStatus(playerNum);

        _lastAction    = _currentAction;
        _currentAction = _actions.at(_currentStreet).emplace_back(actionType, player, _getAndResetLastActionTime(), amount);

        if (amount != 0) {
            _pot       += amount;
            _streetPot += amount;
        }

        switch (actionType) {
            case CALL: player.hasCalled(amount); break;
            case BET:
                player.hasBet(amount);
                _lastBetOrRaise = amount;
                break;
            case RAISE:
                player.hasRaised(amount);
                _lastBetOrRaise = player.totalStreetBet;
                break;
            case CHECK: player.hasChecked(); break;
            case FOLD:
                player.hasFolded();
                _ranking.emplace(std::vector<int32_t> {player.getNumber()});
                break;
            default: break;
        }

        if ((actionType == FOLD || actionType == CHECK || actionType == CALL) && _isStreetOver()) {
            _endStreet();
            return;
        }

        _currentPlayerNum = _getNextPlayerNum(_currentPlayerNum);
    }

    auto Round::_determineRoundOver() -> void {
        if (count_if(*_playersStatus, playerIsInRound) == 1) { _endRound(); }
    }

    auto Round::_endStreet() -> void {
        _updatePlayersMaxWinnable();

        // An all-in player that folded is out of the round, counting it would make the difference below lie
        auto playersInRound = count_if(*_playersStatus, playerIsInRound);
        auto playersAllIn   = count_if(*_playersStatus, playerIsAllIn);

        if (playersInRound >= 2 && playersInRound - playersAllIn <= 1) { _currentStreet = SHOWDOWN; }

        switch (_currentStreet) {
            case SHOWDOWN: break;
            case PREFLOP: _currentStreet = FLOP; break;
            case FLOP: _currentStreet = TURN; break;
            case TURN: _currentStreet = RIVER; break;
            case RIVER: _currentStreet = SHOWDOWN; break;
        }

        _determineRoundOver();

        if (!_ended) {
            _lastAction       = RoundAction();
            _streetPot        = 0;
            _frozenPot        = _pot;
            _currentPlayerNum = _getNextPlayerNum(_dealerPlayerNum);

            for (auto& player : *_playersStatus | filter(playerIsInRound)) { player.streetReset(); }
        }
    }

    auto Round::_endRound() -> void {
        _processRanking();
        _updateStacks();

        for (auto& player : *_playersStatus) {
            // Bust players with no stack left
            if (!player.isEliminated() && player.getStack() == 0) {
                player.bust();
                _playerGotBusted = true;
            }
            // Store the stacks variation for each player
            _playersRoundRecap[player.getNumber() - 1] = {player.getNumber(), player.initialStack, player.getStack()};
        }

        _ended = true;
    }

    auto Round::_hasWon() const -> bool {
        auto rankFirst = _ranking.top();

        return find_if(rankFirst, [&](int32_t playerNum) { return playerNum == 1; }) != rankFirst.end();
    }

    auto Round::_processRanking() -> void {
        std::vector<PlayerStatus> players;
        players.reserve(_playersStatus->size());
        // Add in round players to the players vector
        for (const auto& playerStatus : *_playersStatus | filter(playerIsInRound)) { players.push_back(playerStatus); }
        // Sort players by hand strength desc
        sort(players, [&](const PlayerStatus& p1, const PlayerStatus& p2) { return _board.compareHands(p2.hand, p1.hand) > 0; });
        // Add the first player to the _ranking stack
        _ranking.emplace(std::vector<int32_t> {players.front().getNumber()});
        // Iterate through the rest of players in round from last to first and add them to the _ranking stack
        for (int32_t i = 1; i < players.size(); ++i) {
            if (_board.compareHands(players[i].hand, _getPlayerStatus(_ranking.top().front()).hand) == 0) {
                _ranking.top().emplace_back(players[i].getNumber());
            } else {
                _ranking.emplace(std::vector<int32_t> {players[i].getNumber()});
            }
        }
    }

    auto Round::_payBlinds() -> void {
        auto& SBPlayer = _getPlayerStatus(_smallBlindPlayerNum);
        auto& BBPlayer = _getPlayerStatus(_bigBlindPlayerNum);  // Can be the dealer if there are only 2 players

        SBPlayer.paySmallBlind(_blinds.SB());
        BBPlayer.payBigBlind(_blinds.BB());

        _lastBetOrRaise  = std::max(SBPlayer.totalBet, BBPlayer.totalBet);
        _streetPot      += SBPlayer.totalBet + BBPlayer.totalBet;
        _pot             = _streetPot;
    }

    auto Round::_updateStacks() -> void {
        // Use a copy of _ranking and _pot because it will be modified
        auto ranking = _ranking;
        auto pot     = _pot;
        // Add pot to winner(s) stack
        while (!ranking.empty()) {
            auto playersNum = ranking.top();
            // Pay the smallest max winnable first so that a capped share flows back to the other winners. At equal max
            // winnable pay in reverse seat order: the integer division leaves the odd chips to the last players paid,
            // and those must be the earliest seats left of the button, not the biggest stack.
            sort(playersNum, [&](int32_t p1Num, int32_t p2Num) {
                const auto& p1 = _getPlayerStatus(p1Num);
                const auto& p2 = _getPlayerStatus(p2Num);

                return p1.maxWinnable != p2.maxWinnable ? p1.maxWinnable < p2.maxWinnable : buttonOrder(p1) > buttonOrder(p2);
            });
            // Number of remaining players to share the pot
            auto remainingPlayers = static_cast<int32_t>(playersNum.size());

            for (int32_t playerNum : playersNum) {
                auto& player    = _getPlayerStatus(playerNum);
                auto  winAmount = std::min(player.maxWinnable, pot / remainingPlayers--);

                player.winChips(winAmount);

                pot -= winAmount;
            }

            ranking.pop();
        }
    }

    /**
     * @brief Update the max winnable amount for each player at the end of a street
     */
    auto Round::_updatePlayersMaxWinnable() -> void {
        for (auto& playerStatus : *_playersStatus) {
            if (!playerStatus.isAllIn) {
                playerStatus.maxWinnable = _frozenPot + _streetPot;
            } else if (playerStatus.totalStreetBet != 0) {
                int32_t streetWinnable = 0;

                for (const auto& player : *_playersStatus) {
                    streetWinnable += std::min(player.totalStreetBet, playerStatus.totalStreetBet);
                }

                playerStatus.maxWinnable = _frozenPot + streetWinnable;
            }
        }
    }
}  // namespace GameHandler