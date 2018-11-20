#include "analyser.hpp"

SymbolList *string_to_vector(const string &raw_string)
{
    istringstream iss(raw_string);
    SymbolList *words = new SymbolList((istream_iterator<string>(iss)),
                         istream_iterator<string>());
    return words;
}

bool merge_set(TerminalSet &destination, const TerminalSet &source)
{
    int old_size = destination.size();
    destination.insert(source.begin(), source.end());
    return destination.size() > old_size;
}

template<class T>
string container_to_string(T &container, string separator, const int start_index)
{
    string result = "";
    if (container.size() > start_index)
    {
        auto it = container.begin();
        advance(it, start_index);
        result = *it;
        it++;
        for (; it != container.end(); it++)
        {
            result += (separator + *it);
        }
    }
    return result;
}

Nonterminal::Nonterminal() : candidates(new CandidateList), firsts(new TerminalSet),
    follows(new TerminalSet), table(new AnalyseTable)
{
}

Nonterminal::Nonterminal(const Nonterminal *old_nonterminal)
{
    initialize(old_nonterminal);
}

Nonterminal::Nonterminal(const Nonterminal &old_nonterminal)
{
    initialize(&old_nonterminal);
}

Nonterminal::~Nonterminal()
{
    delete candidates;
    delete firsts;
    delete follows;
    delete table;
}

void Nonterminal::initialize(const Nonterminal *old_nonterminal)
{
    candidates = new CandidateList(*old_nonterminal->candidates);
    firsts = new TerminalSet(*old_nonterminal->firsts);
    follows = new TerminalSet(*old_nonterminal->follows);
    table = new AnalyseTable(*old_nonterminal->table);
}

Rule::Rule() : candidates(new CandidateList)
{
}

Rule::Rule(const Rule *rule) : candidates(new CandidateList(*rule->candidates))
{
}

Rule::Rule(const Rule &rule) : candidates(new CandidateList(*rule.candidates))
{
}

Rule::~Rule()
{
    delete candidates;
}

Analyser::Analyser() : terminals(new TerminalSet), nonterminals(new NonterminalTable)
{
}

void Analyser::create_grammar(istream &stream)
{
    string raw_string;
    SymbolList *inputs;
    Rule *rule;
    char temp;
    cout << "Note: all words should be separated by space, end with line break." << endl;
    cout << "Please enter the non-terminals: ";
    getline(stream, raw_string);
    inputs = string_to_vector(raw_string);
    cout << container_to_string(*inputs) << endl;
    for (symbol &word: *inputs)
    {
        nonterminals->insert({word, Nonterminal()});
    }
    cout << "Please enter the terminals: ";
    getline(stream, raw_string);
    inputs = string_to_vector(raw_string);
    cout << container_to_string(*inputs) << endl;
    for (symbol &word: *inputs)
    {
        terminals->insert(word);
    }
    cout << "Please enter the start symbol: ";
    stream >> start_symbol;
    getline(stream, raw_string);
    cout << "Please enter the rules, ome per line, enter `$` to exit:" << endl;
    getline(stream, raw_string);
    cout << raw_string << endl;
    while (raw_string != "$")
    {
        rule = build_rule(raw_string);
        add_rule(*rule);
        delete rule;
        getline(stream, raw_string);
        cout << raw_string << endl;
    }
    cout << "Grammar: " << endl;
    cout << "T:";
    for (auto &t: *terminals)
    {
        cout << " " << t;
    }
    cout << endl;
    cout << "N:";
    for (auto &n: *nonterminals)
    {
        cout << " " << n.first;
    }
    cout << endl;
    cout << "S: " << start_symbol << endl;
    cout << "Rules: " << endl;
    for (auto &n: *nonterminals)
    {
        bool is_first = true;
        cout << n.first << " ->";
        for (auto &r: *n.second.candidates)
        {
            string rule_text;
            if (is_first)
            {
                is_first = false;
            }
            else
            {
                cout << " | ";
            }
            rule_text = container_to_string(r);
            if (rule_text.size() > 0)
            {
                cout << rule_text;
            }
            else
            {
                cout << EMPTY_MARK;
            }
        }
        cout << " (" << to_string(n.second.candidates->size()) << " candidates)" << endl;
    }
    cout << endl;
}

Rule *Analyser::build_rule(string grammar_text)
{
    Rule *rule = new Rule;
    SymbolList *words = string_to_vector(grammar_text);
    cout << container_to_string(*words) << endl;
    if (words->size() >= 3 && (*words)[1] == PRODUCTION_MARK)
    {
        Candidate *candidate = new Candidate;
        cout << "Rule accepted." << endl;
        rule->nonterminal = (*words)[0];
        for (int i = 2; i < words->size(); i++)
        {
            symbol new_word = (*words)[i];
            if (new_word != CANDIDATE_DELIMITER)
            {
                candidate->push_back(new_word);
            }
            else
            {
                rule->candidates->push_back(*candidate);
                candidate = new Candidate;
            }
        }
        rule->candidates->push_back(*candidate);
        candidate = new Candidate;
        return rule;
    }
    else
    {
        cout << "Rule not accepted." << endl;
        return nullptr;
    }
}

void Analyser::add_rule(Rule &new_rule)
{
    Nonterminal *word = &nonterminals->find(new_rule.nonterminal)->second;
    for (Candidate &it : (*new_rule.candidates))
    {
        word->candidates->push_back(it);
    }
    cout << to_string(word->candidates->size()) << " rules added." << endl;
}

bool Analyser::is_nonterminal(symbol word) const
{
    return nonterminals->find(word) != nonterminals->end();
}

TerminalSet Analyser::get_firsts(symbol word) const
{
    if (is_nonterminal(word))
    {
        return *nonterminals->at(word).firsts;
    }
    else
    {
        return TerminalSet({word});
    }
}

TerminalSet Analyser::get_firsts(Candidate &candidate) const
{
    if (candidate.size() == 0)
    {
        return {EMPTY_MARK};
    }
    else
    {
        TerminalSet current_set = get_firsts(candidate[0]);
        if (current_set.find(EMPTY_MARK) != current_set.end() && candidate.size() > 1)
        {
            Candidate new_candidate(candidate.begin() + 1, candidate.end());
            merge_set(current_set, get_firsts(new_candidate));
        }
        return current_set;
    }
}

void Analyser::calculate_firsts()
{
    bool need_update = true;
    while (need_update)
    {
        cout << "FIRST set:" << endl;
        for (auto &nit: *nonterminals)
        {
            cout << nit.first << ":\t" << container_to_string(*nit.second.firsts) << endl;
        }
        need_update = false;
        for (auto &nit: *nonterminals)
        {
            TerminalSet temp;
            for (Candidate &cit: *nit.second.candidates)
            {
                TerminalSet new_set = get_firsts(cit);
                need_update = merge_set(*nit.second.firsts, new_set) || need_update;
            }
        }
    }
    cout << "FIRST set:" << endl;
    for (auto &nit: *nonterminals)
    {
        cout << nit.first << ":\t" << container_to_string(*nit.second.firsts) << endl;
    }
}

void Analyser::calculate_follows()
{
    bool need_update = true;
    nonterminals->at(start_symbol).follows->insert(END_MARK);
    while (need_update)
    {
        cout << "FOLLOW set:" << endl;
        for (auto &nit: *nonterminals)
        {
            cout << nit.first << ":\t" << container_to_string(*nit.second.follows) << endl;
        }
        for (auto &nit: *nonterminals)
        {
            need_update = false;
            TerminalSet temp;
            for (Candidate &cit: *nit.second.candidates)
            {
                int i = 1;
                for (symbol &word: cit)
                {
                    if (is_nonterminal(word))
                    {
                        if (i == cit.size())
                        {
                            need_update = merge_set(*nonterminals->at(word).follows, *nit.second.follows) || need_update;
                        }
                        else
                        {
                            Candidate endian(cit.begin() + i, cit.end());
                            TerminalSet new_set = get_firsts(endian);
                            TerminalSet::iterator tit = new_set.find(EMPTY_MARK);
                            if (tit != new_set.end())
                            {
                                new_set.erase(tit);
                                merge_set(new_set, *nit.second.follows);
                            }
                            need_update = merge_set(*nonterminals->at(word).follows, new_set) || need_update;
                        }
                    }
                    i++;
                }
            }
        }
    }
    cout << "FOLLOW set:" << endl;
    for (auto &nit: *nonterminals)
    {
        cout << nit.first << ":\t" << container_to_string(*nit.second.follows) << endl;
    }
}

void Analyser::build_table()
{
    for (auto &nit: *nonterminals)
    {
        int i = 0;
        for (Candidate &cit: *nit.second.candidates)
        {
            TerminalSet new_set = get_firsts(cit);
            for (Terminal const &t: new_set)
            {
                if (t == EMPTY_MARK)
                {
                    for (Terminal const &follower: *nit.second.follows)
                    {
                        nit.second.table->insert({follower, i});
                    }
                }
                else
                {
                    nit.second.table->insert({t, i});
                }
            }
            i++;
        }
        for (Terminal const &follower: *nit.second.follows)
        {
            if (nit.second.table->find(follower) == nit.second.table->end())
            {
                nit.second.table->insert({follower, SYNCH_MARK_INDEX});
            }
        }
    }
}

void Analyser::print_table() const
{
    for (const symbol &word: *terminals)
    {
        cout << COLUMN_DELIMITER << word;
    }
    cout << endl;
    for (const auto &nit: *nonterminals)
    {
        cout << nit.first;
        for (const symbol &word: *terminals)
        {
            cout << COLUMN_DELIMITER;
            if (nit.second.table->find(word) != nit.second.table->end())
            {
                string rule_text;
                int rule_index = nit.second.table->at(word);
                if (rule_index == -1)
                {
                    cout << SYNCH_MARK;
                }
                else
                {
                    rule_text = container_to_string(nit.second.candidates->at(rule_index), "");
                    if (rule_text.size() == 0)
                    {
                        rule_text = "@";
                    }
                    cout << nit.first << PRODUCTION_MARK << rule_text;
                }
            }
        }
        cout << endl;
    }
}

void Analyser::receive_text(istream &stream)
{
    string raw_string;
    cout << "Please enter a piece of text to analyse: ";
    getline(stream, raw_string);
    analyse(raw_string);
}

void Analyser::analyse(string code_text)
{
    int i = 1;
    SymbolStack stack;
    SymbolList *words = string_to_vector(code_text);
    SymbolList::iterator wit = words->begin();
    words->push_back(END_MARK);
    stack.push_back(END_MARK);
    stack.push_back(start_symbol);
    cout << endl << "No.\tStack\tInput\tOutput" << endl;
    while (true)
    {
        if (wit == words->end())
        {
            cout << "ERROR" << endl;
        }
        else
        {
            cout << to_string(i) << "\t" << container_to_string(stack, "") << "\t" << container_to_string(*words, "", wit - words->begin()) <<  "\t";
            if (stack.back() == END_MARK && *wit == END_MARK)
            {
                cout << "FINISHED";
                break;
            }
            else
            {
                symbol top = stack.back();
                if (is_nonterminal(top))
                {
                    Nonterminal nonterminal = nonterminals->at(top);
                    AnalyseTable::iterator ait = nonterminal.table->find(*wit);
                    if (ait == nonterminal.table->end())
                    {
                        cout << "Error prasing.";
                    }
                    else
                    {
                        Candidate candidate = nonterminal.candidates->at(ait->second);
                        string candidate_text = container_to_string(candidate);
                        stack.pop_back();
                        for (Candidate::reverse_iterator rit = candidate.rbegin(); rit != candidate.rend(); rit++)
                        {
                            stack.push_back(*rit);
                        }
                        if (candidate_text.size() == 0)
                        {
                            candidate_text = EMPTY_MARK;
                        }
                        cout << top << PRODUCTION_MARK << candidate_text;
                    }
                }
                else
                {
                    if (*wit == top)
                    {
                        stack.pop_back();
                        cout << "Match";
                        wit++;
                    }
                    else
                    {
                        cout << "Error prasing.";
                    }
                }
                i++;
            }
            cout << endl;
        }
    }
    cout << endl;
    delete words;
}
