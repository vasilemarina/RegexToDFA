#pragma once

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <map>
#include <set>
#include <string>
#include <stack>
#include <utility>
#include "LambdaNondeterministicAutomaton.h"

/*struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        std::size_t h1 = std::hash<T1>{}(pair.first);
        std::size_t h2 = std::hash<T2>{}(pair.second);
        return h1 ^ (h2 << 1); 
    }
};
*/
struct CustomComparator {
    bool operator()(const std::pair<std::string, char>& lhs, const std::pair<std::string, char>& rhs) const {
        if (lhs.first != rhs.first) {
            return lhs.first < rhs.first; 
        }
        return lhs.second < rhs.second; 
    }
};


class DeterministicFiniteAutomaton
{
public:
    DeterministicFiniteAutomaton(const std::string& outputFileName = "automaton.out");
    DeterministicFiniteAutomaton(
        const std::unordered_set<std::string>& states,
        const std::unordered_set<char>& alphabet,
        const std::map<std::pair<std::string, char>, std::string, CustomComparator>& transitionTable,
        const std::string& initialState,
        const std::unordered_set<std::string>& finalStates,
        const std::string& outputFileName = "automaton.out");
    DeterministicFiniteAutomaton(const LambdaNondeterministicAutomaton& lambdaAutomaton, const std::string& outputFileName = "automaton.out");

    bool VerifyAutomaton() const;
    bool CheckWord(const std::string& word) const;
    void RunMenu(const std::string& regex) const;

    static std::string ConvertToPostfix(const std::string& regex);
    static DeterministicFiniteAutomaton BuildDFA(const std::string& postfixRegex);

    friend std::ostream& operator<<(std::ostream& os, const DeterministicFiniteAutomaton& automaton);

private:
    bool IsFinalState(std::unordered_set<std::string> stateComponents, std::unordered_set<std::string> lambdaAutomatonFinalStates);
    void Print() const;

    std::unordered_set<std::string> states;
    std::unordered_set<std::string> finalStates;
    std::map<std::pair<std::string, char>, std::string, CustomComparator> transitionTable;
    std::string initialState;
    std::unordered_set<char> alphabet;

    std::string outputFileName;
};

