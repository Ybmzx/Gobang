#ifndef GOBANG_AI_HPP
#define GOBANG_AI_HPP

#include "GobangService.hpp"
#include <algorithm>
#include <climits>
#include <unordered_map>

class GobangAI {
public:
    enum Pattern {
        FIVE,          // 连五
        LIVE_FOUR,     // 活四
        CHONG_FOUR,    // 冲四
        LIVE_THREE,    // 活三
        SLEEP_THREE,   // 眠三
        LIVE_TWO,      // 活二
        SLEEP_TWO,     // 眠二
        NONE
    };

    struct Move {
        Vector2i pos;
        int score;
        Move(Vector2i p = Vector2i(), int s = 0) : pos(p), score(s) {}
        bool operator<(const Move& other) const { return score < other.score; }
    };

    GobangAI(GobangService& service, GobangService::CoordType aiColor, int depth = 2)
        : service(service), aiColor(aiColor), humanColor(aiColor == GobangService::BLACK ?
            GobangService::WHITE : GobangService::BLACK), searchDepth(depth) {
    }

    Vector2i getBestMove() {
        std::vector<Vector2i> emptyPositions = getEmptyPositions();
        if (emptyPositions.empty()) return Vector2i(-1, -1);

        Move bestMove;
        int maxScore = INT_MIN;

        for (auto& pos : emptyPositions) {
            service.board[pos.x][pos.y] = aiColor;
            int score = minimax(searchDepth - 1, false, INT_MIN, INT_MAX);
            service.board[pos.x][pos.y] = GobangService::NONE;

            if (score > maxScore) {
                maxScore = score;
                bestMove = Move(pos, score);
            }
        }
        return bestMove.pos;
    }

private:
    GobangService& service;
    GobangService::CoordType aiColor;
    GobangService::CoordType humanColor;
    int searchDepth;

    const std::unordered_map<Pattern, int> patternScore = {
        {FIVE, 1000000},     {LIVE_FOUR, 10000},
        {CHONG_FOUR, 5000},  {LIVE_THREE, 2000},
        {SLEEP_THREE, 500},  {LIVE_TWO, 100},
        {SLEEP_TWO, 30},     {NONE, 0}
    };

    std::vector<Vector2i> getEmptyPositions() {
        std::vector<Vector2i> positions;
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j++) {
                if (service.board[i][j] == GobangService::NONE) {
                    positions.emplace_back(i, j);
                }
            }
        }
        return positions;
    }

    int evaluatePosition(GobangService::CoordType player) {
        int totalScore = 0;
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 15; j++) {
                if (service.board[i][j] == player) {
                    totalScore += evaluatePoint(Vector2i(i, j), player);
                }
            }
        }
        return totalScore;
    }

    int evaluatePoint(Vector2i pos, GobangService::CoordType player) {
        int score = 0;
        for (auto& dir : DIRECTION) {
            Pattern pattern = analyzePattern(pos, dir, player);
            score += patternScore.at(pattern);
        }
        return score;
    }

    Pattern analyzePattern(Vector2i pos, Vector2i dir, GobangService::CoordType player) {
        int consecutive = 1;
        bool leftBlocked = false, rightBlocked = false;

        for (int i = 1; i <= 4; i++) {
            Vector2i next = pos + dir * i;
            if (!service.inBounds(next)) {
                rightBlocked = true;
                break;
            }
            if (service.board[next.x][next.y] != player) {
                if (service.board[next.x][next.y] != GobangService::NONE) rightBlocked = true;
                break;
            }
            consecutive++;
        }

        for (int i = 1; i <= 4; i++) {
            Vector2i prev = pos - dir * i;
            if (!service.inBounds(prev)) {
                leftBlocked = true;
                break;
            }
            if (service.board[prev.x][prev.y] != player) {
                if (service.board[prev.x][prev.y] != GobangService::NONE) leftBlocked = true;
                break;
            }
            consecutive++;
        }

        if (consecutive >= 5) return FIVE;
        if (consecutive == 4) {
            if (!leftBlocked && !rightBlocked) return LIVE_FOUR;
            if (leftBlocked != rightBlocked) return CHONG_FOUR;
        }
        if (consecutive == 3) {
            if (!leftBlocked && !rightBlocked) return LIVE_THREE;
            if (leftBlocked != rightBlocked) return SLEEP_THREE;
        }
        if (consecutive == 2) {
            if (!leftBlocked && !rightBlocked) return LIVE_TWO;
            if (leftBlocked != rightBlocked) return SLEEP_TWO;
        }
        return NONE;
    }

    int minimax(int depth, bool isMaximizing, int alpha, int beta) {
        if (depth == 0) {
            return evaluatePosition(aiColor) - evaluatePosition(humanColor);
        }

        std::vector<Vector2i> positions = getEmptyPositions();
        if (positions.empty()) return 0;

        if (isMaximizing) {
            int maxEval = INT_MIN;
            for (auto& pos : positions) {
                service.board[pos.x][pos.y] = aiColor;
                int eval = minimax(depth - 1, false, alpha, beta);
                service.board[pos.x][pos.y] = GobangService::NONE;
                maxEval = std::max(maxEval, eval);
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break;
            }
            return maxEval;
        }
        else {
            int minEval = INT_MAX;
            for (auto& pos : positions) {
                service.board[pos.x][pos.y] = humanColor;
                int eval = minimax(depth - 1, true, alpha, beta);
                service.board[pos.x][pos.y] = GobangService::NONE;
                minEval = std::min(minEval, eval);
                beta = std::min(beta, eval);
                if (beta <= alpha) break;
            }
            return minEval;
        }
    }
};

#endif