#include "LambdaNondeterministicAutomaton.h"

LambdaNondeterministicAutomaton::LambdaNondeterministicAutomaton(const std::unordered_set<std::string>& states, const std::unordered_set<char>& alphabet,
    const std::unordered_map<std::pair<std::string, char>, std::unordered_set<std::string>, pair_hash>& transitionTable,
    const std::string initialState, std::unordered_set<std::string>& finalStates)
	:
	states(states),
	alphabet(alphabet),
	transitionTable(transitionTable),
	initialState(initialState),
	finalStates(finalStates)
{
}

LambdaNondeterministicAutomaton::LambdaNondeterministicAutomaton(const std::string& postfixRegex)
{
    *this = BuildLambdaNFA(postfixRegex);
}

LambdaNondeterministicAutomaton LambdaNondeterministicAutomaton::Alternation(const LambdaNondeterministicAutomaton& A, const LambdaNondeterministicAutomaton& B, int& stateCounter)
{
    LambdaNondeterministicAutomaton result;
    
    std::string newInitialState = "q" + std::to_string(stateCounter++);
    std::string newFinalState = "q" + std::to_string(stateCounter++);
    
    result.states.insert(newInitialState);
    result.states.insert(newFinalState);
    result.states.insert(A.states.begin(), A.states.end());
    result.states.insert(B.states.begin(), B.states.end());
    
    result.transitionTable[{newInitialState, '\0'}].insert(A.initialState);
    result.transitionTable[{newInitialState, '\0'}].insert(B.initialState); 
    
    result.transitionTable.insert(A.transitionTable.begin(), A.transitionTable.end());
    result.transitionTable.insert(B.transitionTable.begin(), B.transitionTable.end());
    
    for (const auto& state : A.finalStates) {
        result.transitionTable[{state, '\0'}].insert(newFinalState);
    }
    for (const auto& state : B.finalStates) {
        result.transitionTable[{state, '\0'}].insert(newFinalState);
    }
    
    result.initialState = newInitialState;
    result.finalStates = { newFinalState };
    
    result.alphabet.insert(A.alphabet.begin(), A.alphabet.end());
    result.alphabet.insert(B.alphabet.begin(), B.alphabet.end());
    
    return result;
}

LambdaNondeterministicAutomaton LambdaNondeterministicAutomaton::Concatenation(const LambdaNondeterministicAutomaton& A, const LambdaNondeterministicAutomaton& B, int& stateCounter)
{
    LambdaNondeterministicAutomaton result;

    result.states.insert(A.states.begin(), A.states.end());
    result.states.insert(B.states.begin(), B.states.end());
    result.transitionTable = A.transitionTable;

    for (const auto& state : A.finalStates) {
        result.transitionTable[{state, '\0'}].insert(B.initialState);
    }

    result.transitionTable.insert(B.transitionTable.begin(), B.transitionTable.end());

    result.initialState = A.initialState;
    result.finalStates = B.finalStates;

    result.alphabet.insert(A.alphabet.begin(), A.alphabet.end());
    result.alphabet.insert(B.alphabet.begin(), B.alphabet.end());

    return result;
}

LambdaNondeterministicAutomaton LambdaNondeterministicAutomaton::KleeneStar(const LambdaNondeterministicAutomaton& A, int& stateCounter)
{
    LambdaNondeterministicAutomaton result;

    std::string newInitialState = "q" + std::to_string(stateCounter++);
    std::string newFinalState = "q" + std::to_string(stateCounter++);

    result.states.insert(newInitialState);
    result.states.insert(newFinalState);
    result.states.insert(A.states.begin(), A.states.end());

    result.transitionTable = A.transitionTable;

    result.transitionTable[{newInitialState, '\0'}].insert(A.initialState);
    result.transitionTable[{newInitialState, '\0'}].insert(newFinalState);
    for (const auto& state : A.finalStates) {
        result.transitionTable[{state, '\0'}].insert(A.initialState);
        result.transitionTable[{state, '\0'}].insert(newFinalState);
    }

    result.initialState = newInitialState;
    result.finalStates = { newFinalState };

    result.alphabet = A.alphabet;

    return result;
}

LambdaNondeterministicAutomaton LambdaNondeterministicAutomaton::KleenePlus(const LambdaNondeterministicAutomaton& A, int& stateCounter)
{
    LambdaNondeterministicAutomaton result;

    std::string newInitialState = "q" + std::to_string(stateCounter++);
    std::string newFinalState = "q" + std::to_string(stateCounter++);

    result.states.insert(newInitialState);
    result.states.insert(newFinalState);
    result.states.insert(A.states.begin(), A.states.end());
    
    result.transitionTable = A.transitionTable;

    result.transitionTable[{newInitialState, '\0'}].insert(A.initialState);
    for (const auto& state : A.finalStates) {
        result.transitionTable[{state, '\0'}].insert(A.initialState);
        result.transitionTable[{state, '\0'}].insert(newFinalState);
    }

    result.initialState = newInitialState;
    result.finalStates = { newFinalState };

    result.alphabet = A.alphabet;

    return result;
}

const std::unordered_set<char>& LambdaNondeterministicAutomaton::GetAlphabet() const
{
    return alphabet;
}

const std::string& LambdaNondeterministicAutomaton::GetInitialState() const
{
    return initialState;
}

const std::unordered_set<std::string>& LambdaNondeterministicAutomaton::GetFinalStates() const
{
    return finalStates;
}

std::unordered_set<std::string> LambdaNondeterministicAutomaton::FindLambdaClosure(const std::unordered_set<std::string>& states) const
{
    std::unordered_set<std::string> lambdaClosure(states);
    std::queue<std::string> unanalysedStates;
    for (const std::string& state : states)
        unanalysedStates.push(state);

    while (!unanalysedStates.empty())
    {
        std::string currentState = unanalysedStates.front();
        unanalysedStates.pop();

        if(auto it = transitionTable.find(std::make_pair(currentState, '\0')); it != transitionTable.end())
            for (const std::string& state : it->second)
            {
                lambdaClosure.insert(state);
                unanalysedStates.push(state);
            }
    }
    
    return lambdaClosure;
}

std::unordered_set<std::string> LambdaNondeterministicAutomaton::FindTransition(const std::unordered_set<std::string>& states, char symbol) const
{
    std::unordered_set<std::string> transition;

    for (const std::string& state : states)
        if(auto it = transitionTable.find(std::make_pair(state, symbol)); it != transitionTable.end())
            transition.insert(it->second.begin(), it->second.end());

    return transition;
}

LambdaNondeterministicAutomaton LambdaNondeterministicAutomaton::BuildLambdaNFA(const std::string& postfixRegex)
{
    std::stack<LambdaNondeterministicAutomaton> stack;
    int stateCounter = 0;

    //std::cout << "Construim AFN din expresia regulata: " << postfixRegex << "\n";

    for (char symbol : postfixRegex) {
        if (isalnum(symbol)) {
            // Automat pentru un simbol
            LambdaNondeterministicAutomaton simpleAutomaton;
            std::string startState = "q" + std::to_string(stateCounter++);
            std::string endState = "q" + std::to_string(stateCounter++);

            simpleAutomaton.states = { startState, endState };
            simpleAutomaton.alphabet.insert(symbol);
            simpleAutomaton.transitionTable[{startState, symbol}] = { endState };
            simpleAutomaton.initialState = startState;
            simpleAutomaton.finalStates = { endState };

            //std::cout << "Creat automat pentru simbolul '" << symbol << "' intre starile " << startState << " si " << endState << ".\n";

            stack.push(simpleAutomaton);
        }
        else if (symbol == '.') {
            // Concatenare
            LambdaNondeterministicAutomaton B = stack.top(); stack.pop();
            LambdaNondeterministicAutomaton A = stack.top(); stack.pop();

            /*std::cout << "Concatenam automate:\n";
            std::cout << " - Automat A cu starea initiala: " << A.initialState << " si finala: ";
            for (const auto& state : A.finalStates) std::cout << state << " ";
            std::cout << "\n";
            std::cout << " - Automat B cu starea initiala: " << B.initialState << " si finala: ";
            for (const auto& state : B.finalStates) std::cout << state << " ";
            std::cout << "\n";
            */
            auto concatenatedAutomaton = Concatenation(A, B, stateCounter);
            stack.push(concatenatedAutomaton);
        }
        else if (symbol == '|') {
            // Alternare
            LambdaNondeterministicAutomaton B = stack.top(); stack.pop();
            LambdaNondeterministicAutomaton A = stack.top(); stack.pop();

            /*std::cout << "Realizam alternarea intre automate:\n";
            std::cout << " - Automat A cu starea initiala: " << A.initialState << " si finala: ";
            for (const auto& state : A.finalStates) std::cout << state << " ";
            std::cout << "\n";
            std::cout << " - Automat B cu starea initiala: " << B.initialState << " si finala: ";
            for (const auto& state : B.finalStates) std::cout << state << " ";
            std::cout << "\n";
            */

            auto alternatedAutomaton = Alternation(A, B, stateCounter);
            stack.push(alternatedAutomaton);
        }
        else if (symbol == '*') {
            // Închiderea Kleene
            LambdaNondeterministicAutomaton A = stack.top(); stack.pop();

            /*std::cout << "Aplicam inchiderea Kleene pe automat cu starea initiala: " << A.initialState << " și finală: ";
            for (const auto& state : A.finalStates) std::cout << state << " ";
            std::cout << "\n";
            */

            auto kleeneAutomaton = KleeneStar(A, stateCounter);
            stack.push(kleeneAutomaton);
        }
        else if (symbol == '+')
        {
            // Plus Kleene
            LambdaNondeterministicAutomaton A = stack.top(); stack.pop();
            auto kleenePlusAutomaton = KleenePlus(A, stateCounter);
            stack.push(kleenePlusAutomaton);
        }

        //std::cout << "Valoarea contorului dupa procesare: " << stateCounter << "\n";
    }

    /*std::cout << "Automatul final construit are starea initiala: " << stack.top().initialState
        << " si starile finale: ";
    for (const auto& state : stack.top().finalStates) std::cout << state << " ";
    std::cout << "\n";
    */

    return stack.top();
}

std::ostream& operator<<(std::ostream& os, const LambdaNondeterministicAutomaton& automaton)
{
    os << "Stari: ";
    for (const auto& state : automaton.states) {
        os << state << " ";
    }
    os << "\n";

    os << "Alfabet: ";
    for (const auto& symbol : automaton.alphabet) {
        os << symbol << " ";
    }
    os << "\n";

    os << "Tranzitii:\n";
    for (const auto& transition : automaton.transitionTable) {
        const auto& key = transition.first;
        const auto& values = transition.second;

        const auto& state = key.first;
        const auto& symbol = key.second;

        for(const std::string& value : values)
            os << state << " -" << (symbol == '\0' ? "$" : std::string(1, symbol)) << "-> " << value << "\n";
    }

    os << "Starea initiala: " << automaton.initialState << "\n";

    os << "Stari finale: ";
    for (const auto& state : automaton.finalStates) {
        os << state << " ";
    }
    os << "\n";

    return os;
}
