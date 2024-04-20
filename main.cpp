/* FIFA World Cup Simulator - Version 1.0.1*/


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>
#include <iomanip> 
#include <chrono>
#include <string> // Include necessary header file

struct Goal {
    std::string scorer;
    int minute;

    Goal(const std::string& scorer, int minute) : scorer(scorer), minute(minute) {}

    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"scorer\":\"" << scorer << "\",\"minute\":" << minute << "}";
        return ss.str();
    }
};

class Player {
public:
    std::string team;
    int number;
    std::string position; // Add position information
    std::string name;
    std::string shortName;

    Player(const std::string& team, int number, const std::string& position, const std::string& name, const std::string& shortName)
        : team(team), number(number), position(position), name(name), shortName(shortName) {}

    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"team\":\"" << team << "\",\"number\":" << number << ",\"position\":\"" << position << "\",\"name\":\"" << name << "\",\"shortName\":\"" << shortName << "\"}";
        return ss.str();
    }

    // Method to check if the player is a forward
    bool isForward() const {
        return position == "FW";
    }
};

class Team {
public:
    std::string name;
    std::vector<Player> players;
    std::vector<Goal> goals; // Store goals scored by the team
    int points;
    int wins;
    int draws;
    int losses;

    // Default constructor
    Team() : points(0), wins(0), draws(0), losses(0) {}

    // Inside the Team class definition
bool operator==(const Team& other) const {
    return name == other.name;
}

    // Constructor with team name
    Team(const std::string& name) : name(name), points(0), wins(0), draws(0), losses(0) {}


    void addPlayer(const Player& player) {
        players.push_back(player);
    }

    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"name\":\"" << name << "\",\"players\":[";
        for (size_t i = 0; i < players.size(); ++i) {
            ss << players[i].toJSON();
            if (i < players.size() - 1) {
                ss << ",";
            }
        }
        ss << "]}";
        return ss.str();
    }
};

class Match {
private:
    std::vector<Goal> goals;
    std::string stage; // New member variable to store the stage information

public:
    Team team1;
    Team team2;

    Match(const Team& team1, const Team& team2) : team1(team1), team2(team2) {}
    // Constructor with stage information
    Match(const Team& team1, const Team& team2, const std::string& stage)
        : team1(team1), team2(team2), stage(stage) {}

    // Method to get the stage of the match
    std::string getStage() const {
        return stage;
    }


void playMatch() {
    int goals1 = rand() % 5; // Max 4 goals
    int goals2 = rand() % 5; // Max 4 goals

    // Check if the match is tied after regular time
    if (goals1 == goals2) {
        // Penalty shootout simulation
        int penalty1 = 0;
        int penalty2 = 0;
        for (int i = 0; i < 5; ++i) {
            // Simulate penalties for team 1
            penalty1 += rand() % 2; // Randomly add 0 or 1 to penalty1
            // Simulate penalties for team 2
            penalty2 += rand() % 2; // Randomly add 0 or 1 to penalty2
        }

        // Determine the winner based on penalties
        if (penalty1 > penalty2) {
            // Team 1 wins the penalty shootout
            team1.points += 3;
            team1.wins++;
            team2.losses++;
        } else if (penalty2 > penalty1) {
            // Team 2 wins the penalty shootout
            team2.points += 3;
            team2.wins++;
            team1.losses++;
        } else {
            // Penalty shootout tied as well, simulate sudden death
            while (penalty1 == penalty2) {
                penalty1 += rand() % 2; // Randomly add 0 or 1 to penalty1
                penalty2 += rand() % 2; // Randomly add 0 or 1 to penalty2
            }
            // Determine the winner based on sudden death
            if (penalty1 > penalty2) {
                team1.points += 2;
                team1.wins++;
                team2.losses++;
            } else {
                team2.points += 2;
                team2.wins++;
                team1.losses++;
            }
        }
    } else {
        // Regular time result
        if (goals1 > goals2) {
            team1.points += 3;
            team1.wins++;
            team2.losses++;
        } else {
            team2.points += 3;
            team2.wins++;
            team1.losses++;
        }
    }

    // Update goals scored by each team
    for (int i = 0; i < goals1; ++i) {
        int minute = rand() % 90 + 1; // Random minute between 1 and 90
        team1.goals.push_back(Goal(team1.players[rand() % team1.players.size()].name, minute));
    }
    for (int i = 0; i < goals2; ++i) {
        int minute = rand() % 90 + 1; // Random minute between 1 and 90
        team2.goals.push_back(Goal(team2.players[rand() % team2.players.size()].name, minute));
    }
}



    // Method to select scorer (only forwards can score)
    const Player& selectScorer(const Team& team) const {
        const std::vector<Player>& players = team.players;
        std::vector<const Player*> forwards;
        for (const Player& player : players) {
            if (player.isForward()) {
                forwards.push_back(&player);
            }
        }
        if (!forwards.empty()) {
            int index = rand() % forwards.size();
            return *forwards[index];
        } else {
            // If no forwards found, select a random player
            int index = rand() % players.size();
            return players[index];
        }
    }

    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\"team1\":" << team1.toJSON() << ",\"team2\":" << team2.toJSON() << ",\"score1\":" << team1.goals.size() << ",\"score2\":" << team2.goals.size() << ",\"goals\":[";
        for (size_t i = 0; i < goals.size(); ++i) {
            ss << goals[i].toJSON();
            if (i < goals.size() - 1) {
                ss << ",";
            }
        }
        ss << "]}";
        return ss.str();
    }
};

class WorldCup {
private:
    std::vector<Team> teams;
    std::vector<Match> groupStageMatches;
    std::vector<Match> knockoutStageMatches;
    std::string goldTeam;
    std::string silverTeam;
    std::string bronzeTeam;
    std::string topScorer;

    std::map<std::string, std::vector<Match>> groupStageResults;

    void simulateGroupStage() {
    // Shuffle the teams to ensure random distribution
    std::random_shuffle(teams.begin(), teams.end());

    // Divide teams into groups
    std::vector<std::vector<Team>> groups(8, std::vector<Team>(4));
    int groupIndex = 0;
    for (size_t i = 0; i < teams.size(); ++i) {
        groups[groupIndex][i % 4] = teams[i];
        if ((i + 1) % 4 == 0) {
            groupIndex++;
        }
    }

    // Play matches in each group
    for (auto& group : groups) {
        for (size_t i = 0; i < group.size(); ++i) {
            for (size_t j = i + 1; j < group.size(); ++j) {
                Match match(group[i], group[j], "Group Stage"); // Include stage information
                match.playMatch();
                groupStageMatches.push_back(match);
                groupStageResults[group[i].name].push_back(match);
                groupStageResults[group[j].name].push_back(match);
            }
        }
    }
}

// Inside WorldCup class
void displayGroupStageResults() {
    std::cout << "--- Group Stage Results ---\n";
    for (const auto& groupMatches : groupStageResults) {
        std::cout << groupMatches.first << ":\n";
        std::vector<Team> groupTeams;
        for (const auto& match : groupMatches.second) {
            // Add teams to the vector if not already added
            if (std::find(groupTeams.begin(), groupTeams.end(), match.team1) == groupTeams.end()) {
                groupTeams.push_back(match.team1);
            }
            if (std::find(groupTeams.begin(), groupTeams.end(), match.team2) == groupTeams.end()) {
                groupTeams.push_back(match.team2);
            }
        }
        // Sort teams based on points
        std::sort(groupTeams.begin(), groupTeams.end(), [](const Team& a, const Team& b) {
            return a.points > b.points;
        });
        // Display team names and points
        for (size_t i = 0; i < groupTeams.size(); ++i) {
            std::cout << "  " << groupTeams[i].name << " - Points: " << groupTeams[i].points << "\n";
        }
    }
}


    void simulateKnockoutStage() {
    std::sort(teams.begin(), teams.end(), [](const Team& a, const Team& b) {
        return a.points > b.points;
    });

    for (size_t i = 0; i < 4; ++i) {
        // Semi-finals
        Match semiFinal1(teams[i], teams[7 - i], "Semifinal"); // Include stage information
        semiFinal1.playMatch();
        knockoutStageMatches.push_back(semiFinal1);

        Match semiFinal2(teams[15 - i], teams[8 + i], "Semifinal"); // Include stage information
        semiFinal2.playMatch();
        knockoutStageMatches.push_back(semiFinal2);
    }

    // Final
    Match finalMatch(teams[0], teams[7], "Final"); // Include stage information
    finalMatch.playMatch();
    if (finalMatch.team1.goals.size() == finalMatch.team2.goals.size()) {
        // If the final match ends in a tie, simulate a penalty shootout
        simulatePenaltyShootout(finalMatch);
    } else {
        // Determine winners based on goals scored
        if (finalMatch.team1.goals.size() > finalMatch.team2.goals.size()) {
            finalMatch.team1.wins++;
            finalMatch.team2.losses++;
        } else {
            finalMatch.team2.wins++;
            finalMatch.team1.losses++;
        }
    }
    knockoutStageMatches.push_back(finalMatch);

    // Third-place match
    Match thirdPlaceMatch(teams[4], teams[11], "Third-place Playoff"); // Include stage information
    thirdPlaceMatch.playMatch();
    if (thirdPlaceMatch.team1.goals.size() == thirdPlaceMatch.team2.goals.size()) {
        // If the third-place match ends in a tie, simulate a penalty shootout
        simulatePenaltyShootout(thirdPlaceMatch);
    } else {
        // Determine winners based on goals scored
        if (thirdPlaceMatch.team1.goals.size() > thirdPlaceMatch.team2.goals.size()) {
            thirdPlaceMatch.team1.wins++;
            thirdPlaceMatch.team2.losses++;
        } else {
            thirdPlaceMatch.team2.wins++;
            thirdPlaceMatch.team1.losses++;
        }
    }
    knockoutStageMatches.push_back(thirdPlaceMatch);


    // Determine winners
    if (finalMatch.team1.goals.size() > finalMatch.team2.goals.size()) {
        goldTeam = finalMatch.team1.name;
        silverTeam = finalMatch.team2.name;
    } else if (finalMatch.team2.goals.size() > finalMatch.team1.goals.size()) {
        goldTeam = finalMatch.team2.name;
        silverTeam = finalMatch.team1.name;
    } else {
        // If the final match ends in a tie, randomly select Gold and Silver teams
        int randIndex = rand() % 2;
        goldTeam = (randIndex == 0) ? finalMatch.team1.name : finalMatch.team2.name;
        silverTeam = (randIndex == 0) ? finalMatch.team2.name : finalMatch.team1.name;
    }

    // Determine Bronze team
    if (thirdPlaceMatch.team1.goals.size() > thirdPlaceMatch.team2.goals.size()) {
        bronzeTeam = thirdPlaceMatch.team1.name;
    } else if (thirdPlaceMatch.team2.goals.size() > thirdPlaceMatch.team1.goals.size()) {
        bronzeTeam = thirdPlaceMatch.team2.name;
    } else {
        // If the third-place match ends in a tie, randomly select the Bronze team
        int randIndex = rand() % 2;
        bronzeTeam = (randIndex == 0) ? thirdPlaceMatch.team1.name : thirdPlaceMatch.team2.name;
    }

    
}
void simulatePenaltyShootout(Match& match) {
    int score1 = 0;
    int score2 = 0;
    for (int i = 0; i < 5; ++i) {
        if (rand() % 2 == 0) {
            score1++;
        } else {
            score2++;
        }
    }
    if (score1 == score2) {
        // If still tied after 5 rounds, simulate sudden death
        while (score1 == score2) {
            if (rand() % 2 == 0) {
                score1++;
            } else {
                score2++;
            }
        }
    }
    // Update match result based on penalty shootout
    if (score1 > score2) {
        match.team1.wins++;
        match.team2.losses++;
    } else {
        match.team2.wins++;
        match.team1.losses++;
    }
}



    void determineTopScorer() {
        std::map<std::string, int> goalsScored; // Map to store player name and their goals scored

        // Iterate through all matches
        for (const auto& match : groupStageMatches) {
            for (const auto& goal : match.team1.goals) {
                goalsScored[goal.scorer]++;
            }
            for (const auto& goal : match.team2.goals) {
                goalsScored[goal.scorer]++;
            }
        }
        for (const auto& match : knockoutStageMatches) {
            for (const auto& goal : match.team1.goals) {
                goalsScored[goal.scorer]++;
            }
            for (const auto& goal : match.team2.goals) {
                goalsScored[goal.scorer]++;
            }
        }

        // Find the maximum number of goals scored
        int maxGoals = 0;
        for (const auto& pair : goalsScored) {
            maxGoals = std::max(maxGoals, pair.second);
        }

        // Find all players with the maximum number of goals
        std::vector<std::string> topScorers;
        for (const auto& pair : goalsScored) {
            if (pair.second == maxGoals) {
                topScorers.push_back(pair.first); // Store player name
            }
        }

        // Combine top scorer names into a single string
        topScorer = "";
        for (size_t i = 0; i < topScorers.size(); ++i) {
            topScorer += topScorers[i];
            if (i < topScorers.size() - 1) {
                topScorer += ", ";
            }
        }

        // Debugging output to verify the top scorer
        //std::cout << "DEBUG: Top Scorer: " << topScorer << std::endl;
    }

public:
    void loadTeamsFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string teamName, position, name, shortName;
            int number;
            std::getline(ss, teamName, ',');
            ss >> number;
            ss.ignore(); // Ignore the comma
            std::getline(ss, position, ',');
            std::getline(ss, name, ',');
            std::getline(ss, shortName, ',');

            Player player(teamName, number, position, name, shortName);
            bool teamExists = false;
            for (auto& team : teams) {
                if (team.name == teamName) {
                    team.addPlayer(player);
                    teamExists = true;
                    break;
                }
            }
            if (!teamExists) {
                Team newTeam(teamName);
                newTeam.addPlayer(player);
                teams.push_back(newTeam);
            }
        }
        file.close();
    }

    void simulate(bool saveToFile = false) {
    simulateGroupStage();
    displayGroupStageResults(); // Display group stage results before knockout phase
    simulateKnockoutStage();
    determineTopScorer();


    if (saveToFile) {
        std::ofstream outputFile("simulation_results.txt");
        if (outputFile.is_open()) {
            outputFile << "--- Group Stage Results ---\n";
            for (const auto& teamMatches : groupStageResults) {
                outputFile << teamMatches.first << ":\n";
                for (const auto& match : teamMatches.second) {
                    outputFile << "  " << match.team1.name << " vs " << match.team2.name << " (" << match.team1.goals.size() << "-" << match.team2.goals.size() << ") - " << match.getStage() << "\n";
                    for (const auto& goal : match.team1.goals) {
                        outputFile << "    " << goal.scorer << " (" << goal.minute << "')\n";
                    }
                    for (const auto& goal : match.team2.goals) {
                        outputFile << "    " << goal.scorer << " (" << goal.minute << "')\n";
                    }
                }
            }
            outputFile << "\n--- Knockout Stage Results ---\n";
            for (const auto& match : knockoutStageMatches) {
                outputFile << match.team1.name << " vs " << match.team2.name << " (" << match.team1.goals.size() << "-" << match.team2.goals.size() << ") - " << match.getStage() << "\n";
                for (const auto& goal : match.team1.goals) {
                    outputFile << "  " << goal.scorer << " (" << goal.minute << "')\n";
                }
                for (const auto& goal : match.team2.goals) {
                    outputFile << "  " << goal.scorer << " (" << goal.minute << "')\n";
                }
                // Print penalty shootout outcome for tie matches
                if (match.team1.goals.size() == match.team2.goals.size()) {
                    outputFile << "  Penalty shootout:\n";
                    while (true) {
                        int goalsTeam1 = rand() % 5;
                        int goalsTeam2 = rand() % 5;
                        outputFile << "    " << match.team1.name << " - " << match.team2.name << " (" << goalsTeam1 << "-" << goalsTeam2 << ")\n";
                        if (goalsTeam1 != goalsTeam2) {
                            break;
                        }
                    }
                }
            }
            outputFile << "\n--- Final Standings ---\n";
            outputFile << std::setw(10) << "Gold:" << std::setw(20) << goldTeam << "\n";
            outputFile << std::setw(10) << "Silver:" << std::setw(20) << silverTeam << "\n";
            outputFile << std::setw(10) << "Bronze:" << std::setw(20) << bronzeTeam << "\n";
            outputFile << std::setw(10) << "Top Scorer:" << std::setw(20) << topScorer << "\n";
            outputFile.close(); // Close the file stream
            std::cout << "Simulation results written to simulation_results.txt" << std::endl;
        } else {
            std::cerr << "Error: Unable to open file for writing" << std::endl;
        }
    }
    
}




};


int main() {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    srand(static_cast<unsigned>(seed));
    int choice;
    do {
        // Display menu
        std::cout << " " << std::endl;
        std::cout << " ************************************************" << std::endl;
        std::cout << " ********     FIFA World Cup Simulator    *******" << std::endl;
        std::cout << " ********     1. Simulate World Cup       *******" << std::endl;
        std::cout << " ********     2. See Results              *******" << std::endl;
        std::cout << " ********     3. Exit                     *******" << std::endl;
        std::cout << " Enter your choice: ";
        std::cin >> choice;
        std::cout << " ************************************************" << std::endl;
        // Handle user choice
        switch (choice) {
            case 1: {
                WorldCup worldCup;
                worldCup.loadTeamsFromFile("fifa.csv");
                worldCup.simulate(true); // Save simulation results to file
                break;
            }
            case 2: {
                // Load and display results from file
                std::ifstream inputFile("simulation_results.txt");
                if (inputFile.is_open()) {
                    std::string line;
                    while (std::getline(inputFile, line)) {
                        std::cout << line << std::endl;
                    }
                    inputFile.close();
                } else {
                    std::cerr << "Error: Unable to open file for reading" << std::endl;
                }
                break;
            }
            case 3:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please enter a valid option." << std::endl;
        }
    } while (choice != 3);

    return 0;
}
