#pragma once

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <iostream>

struct pair_hash
{
	std::size_t operator () (std::pair<std::string, char> pair) const {
		return std::hash<std::string>() (pair.first) ^
			std::hash<char>() (pair.second);
	}
};

class LambdaNondeterministicAutomaton
{
public:
	LambdaNondeterministicAutomaton(const std::unordered_set<std::string>& states, const std::unordered_set<char>& alphabet,
		const std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pair_hash>& transitionTable,
		const std::string initialState, std::unordered_set<std::string>& finalStates);
	LambdaNondeterministicAutomaton(const std::string& postfixRegex);
	LambdaNondeterministicAutomaton() = default;

	const std::unordered_set<char>& GetAlphabet() const;
	const std::string& GetInitialState() const;
	const std::unordered_set<std::string>& GetFinalStates() const;

	std::unordered_set<std::string> FindLambdaClosure(const std::unordered_set<std::string>& states) const;
	std::unordered_set<std::string> FindTransition(const std::unordered_set<std::string>& states, char symbol) const;

	static LambdaNondeterministicAutomaton BuildLambdaNFA(const std::string& postfixRegex);
private:
	static LambdaNondeterministicAutomaton Alternation(const LambdaNondeterministicAutomaton& A, const LambdaNondeterministicAutomaton& B, int& stateCounter);
	static LambdaNondeterministicAutomaton Concatenation(const LambdaNondeterministicAutomaton& A, const LambdaNondeterministicAutomaton& B, int& stateCounter);
	static LambdaNondeterministicAutomaton KleeneStar(const LambdaNondeterministicAutomaton& A, int& stateCounter);
	static LambdaNondeterministicAutomaton KleenePlus(const LambdaNondeterministicAutomaton& A, int& stateCounter);

	friend std::ostream& operator<<(std::ostream& os, const LambdaNondeterministicAutomaton& automaton);

	std::unordered_set<std::string> states;
	std::unordered_set<char> alphabet;
	std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pair_hash> transitionTable;
	std::string initialState;
	std::unordered_set<std::string> finalStates;
};

