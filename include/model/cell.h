#pragma once
#include <iostream>
#include <string>
#include <set>

class cell {
private:
    std::string question;
    std::string answer;
    bool is_revealed;
    int points;
    std::set<size_t> attempted_teams;  // Track which teams have attempted this question
    bool answer_revealed;  // Track if the answer has been shown to any team

public:
    cell(): is_revealed(false), points(0), answer_revealed(false) {};
    cell(std::string question, std::string answer, int points = 0, bool is_revealed = false):
        question(std::move(question)), answer(std::move(answer)), points(points), is_revealed(is_revealed), answer_revealed(false) {};
    
    void set_question(const std::string& question);
    void set_answer(const std::string& answer);
    void set_points(int points);
    void set_is_revealed(bool is_revealed);
    
    [[nodiscard]] const std::string& get_question() const;
    [[nodiscard]] const std::string& get_answer() const;
    [[nodiscard]] int get_points() const;
    [[nodiscard]] bool get_is_revealed() const;
    
    // Team attempt tracking
    void add_attempted_team(size_t team_index);
    [[nodiscard]] bool has_team_attempted(size_t team_index) const;
    [[nodiscard]] const std::set<size_t>& get_attempted_teams() const;
    void clear_attempted_teams();
    
    // Answer revelation tracking
    void set_answer_revealed(bool revealed);
    [[nodiscard]] bool is_answer_revealed() const;
    
    void reveal();
    void reset();
    
    ~cell() = default;
};
