#include <regex>
#include "DeterministicFiniteAutomaton.h"

bool readRegex(const std::string& fileName, std::string& regex)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cout << "Fisierul "<< fileName<< "nu a putut fi deschis.\n";
        return false;
    }

    file >> regex;

    return true;
}

bool isValidRegex(const std::string& regex)
{
    if (regex.empty())
    {
        std::cout << "Regex vid.\n";
        return false;
    }

    try
    {
        std::regex regEx(regex);
    }
    catch (const std::regex_error&)
    {
        std::cout << "Expresie invalida.\n";
        return false;
    }

    for(int i = 0; i < regex.size(); i++)
        if(!isalnum(regex[i]) && !strchr("().|*+", regex[i]) || 
            regex[i] == '.' && (i == 0 || i == regex.size()-1 || strchr("*+|", regex[i + 1])))
        {
            std::cout << "Expresie invalida.\n";
            return false;
        }

    return true;
}

int main() 
{
    std::string regex;

    if (readRegex("regex.in", regex) && isValidRegex(regex))
    {
        DeterministicFiniteAutomaton automaton = DeterministicFiniteAutomaton::BuildDFA(DeterministicFiniteAutomaton::ConvertToPostfix(regex));
        automaton.RunMenu(regex);
    }

    return 0;
}
