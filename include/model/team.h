#pragma once
#include <iostream>


class team {
    private:
        std::string name;
        int score;

    public:
        team(const std::string& team_name, int initial_score = 0) 
        : name(std::move(team_name)), score(initial_score) {}
        
        const std::string& get_name() const { return name; }
        void set_name(const std::string& new_name) { name = new_name; }
        void set_name(std::string&& new_name) { name = std::move(new_name); }

        int get_score() const { return score; }
        void set_score(int new_score) { score = new_score; }
};