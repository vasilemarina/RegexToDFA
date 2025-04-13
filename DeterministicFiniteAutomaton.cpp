#include "DeterministicFiniteAutomaton.h"

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(const std::string& outputFileName)
    : initialState(), finalStates(), outputFileName(outputFileName) {}

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(
    const std::unordered_set<std::string>& states,
    const std::unordered_set<char>& alphabet,
    const std::map<std::pair<std::string, char>, std::string, CustomComparator>& transitionTable,
    const std::string& initialState,
    const std::unordered_set<std::string>& finalStates,
    const std::string& outputFileName)
    : states(states), alphabet(alphabet), transitionTable(transitionTable), initialState(initialState), finalStates(finalStates), outputFileName(outputFileName) {}

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(const LambdaNondeterministicAutomaton& lambdaAutomaton, const std::string& outputFileName)
    :
    outputFileName(outputFileName),
    alphabet(lambdaAutomaton.GetAlphabet())
{
    int statesNumber = 0;
    initialState = "q" + std::to_string(statesNumber++) + "'";
    states.insert(initialState);

    /* Mapping between the states of the Deterministic Finite Automaton and the states of the Lambda Nondeterministic Finite Automaton: */
    std::unordered_map<std::string, std::unordered_set<std::string>> statesMapping;
    statesMapping[initialState] = lambdaAutomaton.FindLambdaClosure({ lambdaAutomaton.GetInitialState() });;
    
    if (IsFinalState(statesMapping[initialState], lambdaAutomaton.GetFinalStates()))
        finalStates.insert(initialState);

    std::queue<std::string> unanalysedStates;
    unanalysedStates.push(initialState);

    while (!unanalysedStates.empty())
    {
        std::string currentState = unanalysedStates.front();
        unanalysedStates.pop();

        for (char symbol : alphabet)
        {
            std::unordered_set<std::string> newStateComponents = lambdaAutomaton.FindLambdaClosure(
                lambdaAutomaton.FindTransition(statesMapping[currentState], symbol));

            if (newStateComponents.empty())
                continue;

            auto newStateIterator = find_if(statesMapping.begin(), statesMapping.end(), 
                [newStateComponents](const auto& pair)
                {
                    return pair.second == newStateComponents;
                });

            std::string newState;
            if (newStateIterator == statesMapping.end())
            {
                newState = "q" + std::to_string(statesNumber++) + "'";
                states.insert(newState);
                if (IsFinalState(newStateComponents, lambdaAutomaton.GetFinalStates()))
                    finalStates.insert(newState);

                statesMapping.insert(std::make_pair(newState, newStateComponents));
                unanalysedStates.push(newState);
            }
            else
                newState = newStateIterator->first;
            
            transitionTable[std::make_pair(currentState, symbol)] = newState;
        }
    }
}

bool DeterministicFiniteAutomaton::VerifyAutomaton() const
{
    if (states.empty()) {
        std::cerr << "Eroare: Multimea de stari este goala.\n";
        return false;
    }

    if (alphabet.empty()) {
        std::cerr << "Eroare: Alfabetul este gol.\n";
        return false;
    }

    if (states.find(initialState) == states.end()) {
        std::cerr << "Eroare: Starea initiala nu apartine multimii de stari.\n";
        return false;
    }

    for (const auto& finalState : finalStates) {
        if (states.find(finalState) == states.end()) {
            std::cerr << "Eroare: Starea finala (" << finalState << ") nu apartine multimii de stari Q.\n";
            return false;
        }
    }

    for (const auto& state : states) {
        for (const auto& symbol : alphabet) {
            auto key = std::make_pair(state, symbol);
            if (transitionTable.find(key) == transitionTable.end()) {
                std::cerr << "Eroare: Functia de tranzitie nu este definita pentru starea "
                    << state << " si simbolul " << symbol << ".\n";
                return false;
            }
        }
    }

    std::map<std::pair<std::string, char>, std::string, CustomComparator> uniqueTransitions;
    for (const auto& transition : transitionTable) {
        const auto& key = transition.first;
        const auto& value = transition.second;

        const auto& state = key.first;
        const auto& symbol = key.second;

        if (uniqueTransitions.find(key) != uniqueTransitions.end() && uniqueTransitions[key] != value) {
            std::cerr << "Eroare: Functia de tranzitie nu este determinista pentru starea "
                << state << " si simbolul " << symbol << ".\n";
            return false;
        }
        uniqueTransitions[key] = value;
    }

    return true;
}

bool DeterministicFiniteAutomaton::CheckWord(const std::string& word) const 
{
    std::string currentState = initialState;

    std::cout << "Verificam cuvantul: " << (word.empty() ? "CUVANTUL VID" : word) << "\n";
    std::cout << "Starea initiala: " << currentState << "\n";

    for (char symbol : word) {
        std::cout << "Procesam simbolul: " << symbol << " din starea: " << currentState << "\n";

        auto it = transitionTable.find({ currentState, symbol });
        if (it == transitionTable.end()) {
            std::cout << "Tranzitie lipsa pentru simbolul: " << symbol << "\n";
            return false;
        }

        currentState = it->second;
        std::cout << "Trecem in starea: " << currentState << "\n";
    }

    if (finalStates.find(currentState) != finalStates.end()) {
        std::cout << "Cuvantul este acceptat. Am ajuns in starea finala: " << currentState << "\n";
        return true;
    }

    std::cout << "Cuvantul NU este acceptat. Nu am ajuns intr-o stare finala.\n";
    return false;
}

void DeterministicFiniteAutomaton::RunMenu(const std::string& regex) const
{
    char key;
    do {
        std::cout << "MENIU\n";
        std::cout << "(a) Afisare regex\n";
        std::cout << "(b) Afisare automat\n";
        std::cout << "(c) Verificare cuvant in automat\n";
        std::cout << "(d) Iesire\n\n";

        std::cin >> key;
        switch (key)
        {
        case 'a':
            std::cout << regex << "\n\n";
            break;
        case 'b':
        {
            Print();
            break;
        }
        case 'c':
        {
            std::string word;
            std::cout << "Introduceti cuvantul care va fi verificat: ";
            std::getline(std::cin, word);
            std::getline(std::cin, word);
            if (CheckWord(word))
                std::cout << "\nCuvantul " << (word.empty() ? "$" : word) << " este acceptat.\n\n";
            else
                std::cout << "\nCuvantul " << (word.empty() ? "$" : word) << " NU este acceptat.\n\n";
            break;
        }
        case 'd':
            break;
        default:
            std::cout << "Tasta invalida.\n";
        }
    } while (key != 'd');
}

std::string DeterministicFiniteAutomaton::ConvertToPostfix(const std::string& regex)
{
    std::string postfix;
    std::stack<char> operators;

    std::unordered_map<char, int> priority = {
        {'*', 3}, 
        {'+', 3},
        {'.', 2}, 
        {'|', 1} 
    };

    for (char ch : regex) {
        if (std::isalnum(ch)) {
            postfix += ch;  
        }
        else if (ch == '(') {
            operators.push(ch);
        }
        else if (ch == ')') {
            while (!operators.empty() && operators.top() != '(') {
                postfix += operators.top();
                operators.pop();
            }
            operators.pop();  
        }
        else { 
            while (!operators.empty() && priority[operators.top()] >= priority[ch]) {
                postfix += operators.top();
                operators.pop();
            }
            operators.push(ch);
        }
    }

    while (!operators.empty()) {
        postfix += operators.top();
        operators.pop();
    }

    return postfix;
}

DeterministicFiniteAutomaton DeterministicFiniteAutomaton::BuildDFA(const std::string& postfixRegex)
{
    LambdaNondeterministicAutomaton lambdaNFA(postfixRegex);
    return DeterministicFiniteAutomaton(lambdaNFA);
}

bool DeterministicFiniteAutomaton::IsFinalState(std::unordered_set<std::string> stateComponents, std::unordered_set<std::string> lambdaAutomatonFinalStates)
{
    for (const std::string& state : stateComponents)
        if (lambdaAutomatonFinalStates.contains(state))
            return true;

    return false;
}

void DeterministicFiniteAutomaton::Print() const
{
    std::cout << *this << '\n';
    std::ofstream file(outputFileName);
    if (file.is_open())
        file << *this;
    else
        std::cout << "Fisierul de iesire nu a putut fi deschis.\n";
}

std::ostream& operator<<(std::ostream& os, const DeterministicFiniteAutomaton& automaton) {
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
        const auto& value = transition.second;

        const auto& state = key.first;
        const auto& symbol = key.second;

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

