#pragma once
#include <ostream>
#include "player.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>

using namespace std;

enum Weight {
    Weight_Semi_2 = 20,
    Weight_Open_2 = 50,
    Weight_Semi_3 = 100,
    Weight_Open_3 = 500,
    Weight_Semi_4 = 10000,
    Weight_Open_4 = 100000,
    Weight_5 = 10000000,
};
extern vector<Point> direction;


class Cell {
    Point coord;
    int weight;
public:
    Cell(const Point& p) :coord(p),weight(0){};
    bool operator<(const Cell& other) const;
    void evaluate(const GameView& game, Point& move, Mark player);
    int attack(const GameView& game, Point& move, Mark player);
    int defense(const GameView& game, Point& move, Mark player);
    int evaluate_move(const GameView& game, Point& move, Point directions, Mark player);
    int evaluate_length(int length, int open);
    friend class m_Player;
  
};

class m_Player :public Player {
    std::string _name;
    Point _last_move;
    Mark _my_mark;
    vector<Cell> cells;
public:
    m_Player(const std::string& name) : _name(name) {}
    std::string get_name() const override { return _name; }
    Point play(const GameView& game) override;

    Point result_Sorted_vector(const GameView& game);

    Point random_play(const GameView& game);

    void full_vector(const Point& result, const GameView& game);

    void evaluate_vector(const GameView& game);

    vector<Point>gip_move(const Point& center, const GameView& game);

    void add_to_vector_Point(vector<Point>& neighbors, const GameView& game, const Point& neighbor_next);

    bool is_valid_move(const GameView& game, const Point& cell);

    bool is_valid_cell(const GameView& game, const Point& cell);

    void assign_mark(Mark player_mark) override { _my_mark = player_mark; }

    void notify(const GameView& view, const Event& event) override;
};







/**
 * Simple observer, that logs every event in given output stream.
 */
class BasicObserver : public Observer {
    std::ostream& _out;

    std::ostream& _print_mark(Mark mark);
public:
    BasicObserver(std::ostream& out_stream): _out(out_stream) {}
    void notify(const GameView&, const Event& event) override;
};
