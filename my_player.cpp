#include "my_player.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

static field_index_t rand_int(field_index_t min, field_index_t max) {
    return min + rand() % (max - min+1 );
}



//Cell


bool Cell::operator<(const Cell& other)const  {
    return weight > other.weight;
}

void Cell::evaluate(const GameView& game, Point& move, Mark player) {
    weight = attack(game, move, player) +defense(game, move, player);
}                

int Cell::attack(const GameView& game, Point& move, Mark player) {
    int attack = 0;
    
    for (int i = 0; i < 4; i++) {
        attack += evaluate_move(game, move, direction[i], player);
    }
    return attack;
};

int Cell::defense(const GameView& game, Point& move, Mark player) {
    Mark  player_1 = player == Mark::Cross ? Mark::Zero : Mark::Cross;

    int defense = 0;
    
    for (int i = 0; i < 4; i++) {
        defense += evaluate_move(game, move, direction[i], player_1);
    }
    return defense;
};



int Cell::evaluate_move(const GameView& game, Point& move, Point dir, Mark player) {

    int open = 0;
    int length = 1;
    int middle = 0;

    Point next_move;
    Point prev_move;
    Boundary b = game.get_settings().field_size;

    int Is_prev = 0;
    int Is_next = 0;
    for (int i = 1; i < game.get_settings().win_length; i++) {
        prev_move = {
                .x = move.x - dir.x * i,
                .y = move.y - dir.y * i,
        };
        if (prev_move.y < b.min.y || prev_move.x < b.min.x || prev_move.y >= b.max.y || prev_move.x >= b.max.x || (game.get_state().field->get_value(prev_move) != player && game.get_state().field->get_value(prev_move) != Mark::None))
        {
            break;
        }
        if (game.get_state().field->get_value(prev_move) == player) {
            length++;

        }

        if (game.get_state().field->get_value(prev_move) == Mark::None) {
            open++;
            break;
        }
   
    }
    for (int i = 1; i < game.get_settings().win_length; i++) {
        next_move = {
               .x = move.x + dir.x * i,
               .y = move.y + dir.y * i,
        };
        if (next_move.y < b.min.y || next_move.x < b.min.x || next_move.y >= b.max.y || next_move.x >= b.max.x || (game.get_state().field->get_value(next_move) != player && game.get_state().field->get_value(next_move) != Mark::None))
        {
            break;
        }
        if (game.get_state().field->get_value(next_move) == player) {
            length++;

        }

        if (game.get_state().field->get_value(next_move) == Mark::None) {
            open++;
            break;
        }
    }

    return evaluate_length(length, open);

}

int Cell::evaluate_length(int length, int open) {
    if (length == 2 && open == 1) {
        return Weight_Semi_2;
    }
    if (length == 2 && open == 2) {
        return Weight_Open_2;
    }
    if (length == 3 && open == 1) {
        return Weight_Semi_3;
    }
    if (length == 3 && open == 2) {
        return Weight_Open_3;
    }
    if (length == 4 && open == 1) {
        return Weight_Semi_4;
    }
    if (length == 4 && open == 2) {
        return Weight_Open_4;
    }
    if (length == 5) {
        return Weight_5;
    }

    return 0;
}



//m_Player_cell_A_D

Point m_Player::play(const GameView& game)  {

    Point result;
    const bool is_first_move = game.get_state().number_of_moves == 0;

    if (is_first_move) {

        Boundary b = game.get_settings().field_size;

          result = random_play(game);
        full_vector(result, game);
        return result;
    }

    full_vector(_last_move, game);
    evaluate_vector(game);
    result = result_Sorted_vector(game);

    if (cells.empty() && result == _last_move) {
        result = random_play(game);
    }

    full_vector(result, game);

    return result;

}

Point m_Player::result_Sorted_vector(const GameView& game) {
    Point result;
    while (!cells.empty()) {
        std::sort(cells.begin(), cells.end());
        result = cells.front().coord;
        if (!is_valid_move(game, cells.front().coord)) {
            cells.erase(cells.begin());
        }
        else {

            cells.erase(cells.begin());
            return result;
        }
    }
    return _last_move;
}

Point m_Player::random_play(const GameView& game) {
    Point result;
    Boundary b = game.get_settings().field_size;
    do {
        result = {
                 .x = rand_int(b.min.x, b.max.x),
                 .y = rand_int(b.min.y, b.max.y),
        };
    } while (game.get_state().field->get_value(result) != Mark::None);
    return result;
}

void m_Player::full_vector(const Point& result, const GameView& game) {
 
    vector <Point> gip = gip_move(result, game);
    for (const Point& p : gip) {
        auto it = std::find_if(cells.begin(), cells.end(), [&p](const Cell& c) { return c.coord == p; });

        if (it == cells.end()) {
            Cell new_ceil(p);

            cells.push_back(new_ceil);
        }
    }
}

void m_Player::evaluate_vector(const GameView& game) {
    //vector<Point> direction = { { 0,1 }, { 1,0 }, { 1,1 }, {-1,1} };
    int final_cell = 0;
    for (int i = 0; i < cells.size(); i++) {
        cells[i].evaluate(game, cells[i].coord, _my_mark);
    }

}

vector<Point>m_Player::gip_move(const Point& center, const GameView& game) {//, vector<Point>direction
    vector<Point> neighbors;
    for (int i = 0; i < 4; i++) {
        Point neighbor_next = { center.x + direction[i].x , center.y + direction[i].y };
        add_to_vector_Point(neighbors, game, neighbor_next);
        Point neighbor_prev = { center.x - direction[i].x , center.y - direction[i].y };
        add_to_vector_Point(neighbors, game, neighbor_prev);
    }
    return neighbors;
}

void m_Player::add_to_vector_Point(vector<Point>& neighbors, const GameView& game, const Point& neighbor_next) {
    if (is_valid_cell(game, neighbor_next) && is_valid_move(game, neighbor_next)) {
        neighbors.push_back(neighbor_next);
    }
}

bool m_Player::is_valid_move(const GameView& game, const Point& cell) {
    return game.get_state().field->get_value(cell) == Mark::None;
}

bool m_Player::is_valid_cell(const GameView& game, const Point& cell) {
    Boundary b = game.get_settings().field_size;
    return cell.x >= b.min.x && cell.x <= b.max.x &&
        cell.y >= b.min.y && cell.y <= b.max.y;
}

void  m_Player::notify(const GameView& view, const Event& event)  {
    if (event.get_type() == MoveEvent::TYPE) {
        auto& data = get_data<MoveEvent>(event);
        if (data.mark != _my_mark) {
            _last_move = data.point;
        }
        return;
    }
}



void BasicObserver::notify(const GameView& game, const Event& event) {
    
    if (event.get_type() == MoveEvent::TYPE) {
    
        auto &data = get_data<MoveEvent>(event);
        _out << "Move:\tx = " << data.point.x
            << ",\ty = " << data.point.y << ":\t";
        _print_mark(data.mark) << '\n';
        
        return;
    }
    if (event.get_type() == PlayerJoinedEvent::TYPE) {
       auto& data = get_data<PlayerJoinedEvent>(event);
        _out << "Player '" << data.name << "' joined as ";
        _print_mark(data.mark) << '\n';
        return;
    }
    if (event.get_type() == GameStartedEvent::TYPE) {
        _out << "Game started\n";
        return;
    }
    if (event.get_type() == WinEvent::TYPE) {
        auto& data = get_data<WinEvent>(event);
        _out << "Player playing ";
        _print_mark(data.winner_mark) << " has won\n";

      
        return;
    }
    if (event.get_type() == DrawEvent::TYPE) {
        auto& data = get_data<DrawEvent>(event);
        _out << "Draw happened, reason: '" << data.reason << "'\n";
        return;
    }
    if (event.get_type() == DisqualificationEvent::TYPE) {
        auto& data = get_data<DisqualificationEvent>(event);
        _out << "Player playing ";
        _print_mark(data.player_mark) << " was disqualified, reason: '" 
            << data.reason << "'\n";
        return;
    }
}

std::ostream& BasicObserver::_print_mark(Mark m) {
    if (m == Mark::Cross) return _out << "X";
    if (m == Mark::Zero) return _out << "O";
    return _out << "?";
}
