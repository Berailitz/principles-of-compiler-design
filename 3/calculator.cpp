#include "calculator.h"

SymbolList *string_to_vector(const string &raw_string)
{
    istringstream iss(raw_string);
    return new SymbolList((istream_iterator<string>(iss)),
                          istream_iterator<string>());
}

float tryNum(const symbol word)
{
    float result;
    try
    {
        result = stof(word);
    }
    catch (const invalid_argument &e)
    {
        result = -1;
    }
    return result;
}

Calculator::Calculator()
{
    buildAnalyseTable();
    buildRuleList();
}

Calculator::~Calculator()
{
    delete table;
    delete ruleList;
}

void Calculator::buildAnalyseTable()
{
    AnalyseTableLine line;

    line["("] = {ShiftActionType, 4};
    line["num"] = {ShiftActionType, 5};
    line["E"] = {GotoActionType, 1};
    line["T"] = {GotoActionType, 2};
    line["F"] = {GotoActionType, 3};
    table->insert({0, line});

    line.clear();
    line["+"] = {ShiftActionType, 6};
    line["-"] = {ShiftActionType, 7};
    line["$"] = {AcceptActionType, -1};
    table->insert({1, line});

    line.clear();
    line["+"] = {ReduceActionType, 4};
    line["-"] = {ReduceActionType, 4};
    line["*"] = {ShiftActionType, 8};
    line["/"] = {ShiftActionType, 9};
    line[")"] = {ReduceActionType, 4};
    line["$"] = {ReduceActionType, 4};
    table->insert({2, line});

    line.clear();
    line["+"] = {ReduceActionType, 7};
    line["-"] = {ReduceActionType, 7};
    line["*"] = {ReduceActionType, 7};
    line["/"] = {ReduceActionType, 7};
    line[")"] = {ReduceActionType, 7};
    line["$"] = {ReduceActionType, 7};
    table->insert({3, line});

    line.clear();
    line["("] = {ShiftActionType, 4};
    line["num"] = {ShiftActionType, 5};
    line["E"] = {GotoActionType, 10};
    line["T"] = {GotoActionType, 2};
    line["F"] = {GotoActionType, 3};
    table->insert({4, line});

    line.clear();
    line["+"] = {ReduceActionType, 9};
    line["-"] = {ReduceActionType, 9};
    line["*"] = {ReduceActionType, 9};
    line["/"] = {ReduceActionType, 9};
    line[")"] = {ReduceActionType, 9};
    line["$"] = {ReduceActionType, 9};
    table->insert({5, line});

    line.clear();
    line["("] = {ShiftActionType, 4};
    line["num"] = {ShiftActionType, 5};
    line["T"] = {GotoActionType, 11};
    line["F"] = {GotoActionType, 3};
    table->insert({6, line});

    line.clear();
    line["("] = {ShiftActionType, 4};
    line["num"] = {ShiftActionType, 5};
    line["T"] = {GotoActionType, 12};
    line["F"] = {GotoActionType, 3};
    table->insert({7, line});

    line.clear();
    line["("] = {ShiftActionType, 4};
    line["num"] = {ShiftActionType, 5};
    line["F"] = {GotoActionType, 13};
    table->insert({8, line});

    line.clear();
    line["("] = {ShiftActionType, 4};
    line["num"] = {ShiftActionType, 5};
    line["F"] = {GotoActionType, 14};
    table->insert({9, line});

    line.clear();
    line["+"] = {ShiftActionType, 6};
    line["-"] = {ShiftActionType, 7};
    line[")"] = {ShiftActionType, 15};
    table->insert({10, line});

    line.clear();
    line["+"] = {ReduceActionType, 2};
    line["-"] = {ReduceActionType, 2};
    line["*"] = {ShiftActionType, 8};
    line["/"] = {ShiftActionType, 9};
    line[")"] = {ReduceActionType, 2};
    line["$"] = {ReduceActionType, 2};
    table->insert({11, line});

    line.clear();
    line["+"] = {ReduceActionType, 3};
    line["-"] = {ReduceActionType, 3};
    line["*"] = {ShiftActionType, 8};
    line["/"] = {ShiftActionType, 9};
    line[")"] = {ReduceActionType, 3};
    line["$"] = {ReduceActionType, 3};
    table->insert({12, line});

    line.clear();
    line["+"] = {ReduceActionType, 5};
    line["-"] = {ReduceActionType, 5};
    line["*"] = {ReduceActionType, 5};
    line["/"] = {ReduceActionType, 5};
    line[")"] = {ReduceActionType, 5};
    line["$"] = {ReduceActionType, 5};
    table->insert({13, line});

    line.clear();
    line["+"] = {ReduceActionType, 6};
    line["-"] = {ReduceActionType, 6};
    line["*"] = {ReduceActionType, 6};
    line["/"] = {ReduceActionType, 6};
    line[")"] = {ReduceActionType, 6};
    line["$"] = {ReduceActionType, 6};
    table->insert({14, line});

    line.clear();
    line["+"] = {ReduceActionType, 8};
    line["-"] = {ReduceActionType, 8};
    line["*"] = {ReduceActionType, 8};
    line["/"] = {ReduceActionType, 8};
    line[")"] = {ReduceActionType, 8};
    line["$"] = {ReduceActionType, 8};
    table->insert({15, line});
}

void Calculator::buildRuleList()
{
    ruleList->push_back({"", 0});   // 占位，因为从1开始编号
    ruleList->push_back({"E'", 1}); // #1
    ruleList->push_back({"E", 3});  // #2
    ruleList->push_back({"E", 3});  // #3
    ruleList->push_back({"E", 1});  // #4
    ruleList->push_back({"T", 3});  // #5
    ruleList->push_back({"T", 3});  // #6
    ruleList->push_back({"T", 1});  // #7
    ruleList->push_back({"F", 3});  // #8
    ruleList->push_back({"F", 1});  // #9
}

AnalyseAction Calculator::SearchAnalyseTable(const DFAState state, const symbol &word)
{
    int intWord = tryNum(word);
    symbol realWord;
    if (intWord >= 0)
    {
        realWord = "num";
    }
    else
    {
        realWord = word;
    }
    AnalyseTableLine::const_iterator it = table->find(state)->second.find(realWord);
    if (it == AnalyseTableLine::const_iterator{})
    {
        return defaultAction;
    }
    else
    {
        return it->second;
    }
}

bool Calculator::performAction(const AnalyseAction &action)
{
    const symbol word = words->front();
    Rule rule;
    AnalyseAction nextAction;
    int lexval;
    const int topIndex = valStack.size() - 1;
    switch (action.first)
    {
    case ErrorActionType:
        cout << "Invalid sentence" << endl;
        throw "Invalid sentence";
    case AcceptActionType:
        cout << "Finished, result is " << valStack.back() << endl;
        return true;
    case ShiftActionType:
        cout << "Shift " << word << " to state " << action.second << endl;
        symbolStack.push_back(word);
        stateStack.push_back(action.second);
        lexval = tryNum(word);
        if (lexval >= 0)
        {
            valStack.push_back(lexval);
        }
        else
        {
            valStack.push_back(EMPTY_VAL);
        }
        words->pop_front();
        return false;
    case ReduceActionType:
        rule = ruleList->at(action.second);
        cout << "Reduce by " << action.second << endl;
        switch (action.second)
        {
        case 2:
            valStack[topIndex - 2] = valStack[topIndex - 2] + valStack[topIndex];
            break;
        case 3:
            valStack[topIndex - 2] = valStack[topIndex - 2] - valStack[topIndex];
            break;
        case 5:
            valStack[topIndex - 2] = valStack[topIndex - 2] * valStack[topIndex];
            break;
        case 6:
            valStack[topIndex - 2] = valStack[topIndex - 2] / valStack[topIndex];
            break;
        case 8:
            valStack[topIndex - 2] = valStack[topIndex - 1];
            break;
        }
        for (int i = 0; i < rule.second - 1; i++)
        {
            valStack.pop_back();
        }
        for (int i = 0; i < rule.second; i++)
        {
            symbolStack.pop_back();
            stateStack.pop_back();
        }
        nextAction = SearchAnalyseTable(stateStack.back(), rule.first);
        symbolStack.push_back(rule.first);
        stateStack.push_back(nextAction.second);
        return false;
    case GotoActionType:
        throw "Invalid operation";
    }
}

bool Calculator::calculate(const string stringText)
{
    words = string_to_vector(stringText + " " + END_MARK);
    symbolStack = {EMPTY_SYMBOL};
    stateStack = {0};
    valStack = {EMPTY_VAL};
    bool isFinished = false;
    int i = 1;
    cout << setw(4) << "No." << setw(35) << "StateStack" << setw(35) << "SymbolStack" << setw(35) << "valStack" << setw(35) << "Input" << setw(25) << "Output" << endl;
    while (!isFinished)
    {
        const symbol word = words->front();
        AnalyseAction action = SearchAnalyseTable(stateStack.back(), word);
        cout << setw(4) << to_string(i);
        cout << setw(35) << container_to_string(stateStack, "|");
        cout << setw(35) << container_to_string(symbolStack, "|");
        cout << setw(35) << container_to_string(valStack, "|");
        cout << setw(35) << container_to_string(*words, "");
        cout << setw(25);
        try
        {
            isFinished = performAction(action);
        }
        catch (const exception &e)
        {
            cout << e.what() << endl;
            return false;
        }
        i++;
    }
    return true;
}

void Calculator::receive_text(istream &stream)
{
    string raw_string;
    cout << "Please enter a piece of text to analyse, enter `" << END_MARK << "` to exit: ";
    getline(stream, raw_string);
    while (raw_string.size() > 0 && raw_string != END_MARK)
    {
        calculate(raw_string);
        cout << "Please enter a piece of text to analyse, enter `" << END_MARK << "` to exit: ";
        getline(stream, raw_string);
    }
}

template <class T>
string container_to_string(T &container, string separator, const int start_index)
{
    ostringstream oss;
    if (container.size() > start_index)
    {
        auto it = container.begin();
        advance(it, start_index);
        oss << *it;
        it++;
        for (; it != container.end(); it++)
        {
            oss << separator << *it;
        }
    }
    return oss.str();
}
