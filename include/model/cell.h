#pragma once
#include <iostream>
#include <string>

class cell {
private:
    std::string question;
    std::string answer;
    bool is_revealed;
    int points;

public:
    cell(): is_revealed(false), points(0) {};
    cell(std::string question, std::string answer, int points = 0, bool is_revealed = false):
        question(std::move(question)), answer(std::move(answer)), points(points), is_revealed(is_revealed) {};
    
    void set_question(const std::string& question);
    void set_answer(const std::string& answer);
    void set_points(int points);
    void set_is_revealed(bool is_revealed);
    
    [[nodiscard]] const std::string& get_question() const;
    [[nodiscard]] const std::string& get_answer() const;
    [[nodiscard]] int get_points() const;
    [[nodiscard]] bool get_is_revealed() const;
    
    void reveal();
    void reset();
    
    ~cell() = default;
};
