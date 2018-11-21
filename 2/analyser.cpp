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

template<class T>
string reversed_container_to_string(T &container, string separator, const int start_index)
{
    string result = "";
    if (container.size() > start_index)
    {
        auto it = container.rbegin();
        result = *it;
        it++;
        for (; it + 1 != container.rend(); it++)
        {
            result += (separator + *it);
        }
    }
    return result;
}

Nonterminal::Nonterminal(string name) : candidates(new CandidateList), firsts(new TerminalSet),
    follows(new TerminalSet), table(new AnalyseTable), name(name)
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
    name = old_nonterminal->name;
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

Analyser::Analyser() : terminals(new TerminalSet({END_MARK})), nonterminals(new NonterminalTable)
{
}

void Analyser::raise_error(string message)
{
    cout << message << endl;
    cin.get();
    exit(1);
}

void Analyser::clear_grammar()
{
    for (auto &n: *nonterminals)
    {
        delete n.second.table;
        n.second.table = new AnalyseTable();
        n.second.firsts = new TerminalSet();
        n.second.follows = new TerminalSet();
    }
}

void Analyser::create_grammar(istream &stream)
{
    receive_grammar(stream);
    print_grammar();
    calculate_firsts();
    print_firsts();
    calculate_follows();
    print_follows();
    if (build_table(true))
    {
        cout << "LL(1) grammar without recursion or common prefix detected." << endl;
        print_table();
    }
    else
    {
        clear_grammar();
        calculate_firsts();
        eliminate_empty();
        print_grammar();
        eliminate_recursion();
        print_grammar();
        eliminate_common_prefix();
        print_grammar();
        calculate_firsts();
        print_firsts();
        calculate_follows();
        print_follows();
        build_table(false);
        print_table();
    }
}

void Analyser::receive_grammar(istream &stream)
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
        nonterminals->insert({word, Nonterminal(word)});
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
    while (raw_string != "$")
    {
        rule = build_rule(raw_string);
        add_rule(*rule);
        delete rule;
        getline(stream, raw_string);
    }
    cout << endl;
}

void Analyser::print_grammar() const
{
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
        cout << n.first << " -> ";
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
    cout << "Calculating FIRSTs..." << endl;
    while (need_update)
    {
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
    cout << "Done." << endl;
}

void Analyser::add_candidates(CandidateList &candidates, Candidate prefix, Candidate postfix)
{
    if (postfix.size() == 0)
    {
        if (prefix.size() > 0)
        {
            candidates.push_back(prefix);
        }
    }
    else
    {
        symbol next_word = postfix[0];
        postfix.erase(postfix.begin());
        if (is_nonterminal(next_word))
        {
            TerminalSet *firsts = nonterminals->at(next_word).firsts;
            if (firsts->find(EMPTY_MARK) != firsts->end())
            {
                add_candidates(candidates, prefix, postfix);
            }
        }
        prefix.push_back(next_word);
        add_candidates(candidates, prefix, postfix);
    }
}

void Analyser::eliminate_empty()
{
    cout << "Eliminating empty rules..." << endl;
    for (auto &nit: *nonterminals)
    {
        CandidateList *old_candidates = nit.second.candidates;
        nit.second.candidates = new CandidateList;
        for (Candidate &candidate: *old_candidates)
        {
            Candidate prefix;
            add_candidates(*nit.second.candidates, prefix, candidate);
        }
        delete old_candidates;
        if (nit.first == start_symbol && nit.second.firsts->find(EMPTY_MARK) != nit.second.firsts->end())
        {
            nit.second.candidates->push_back({EMPTY_MARK});
        }
    }
    cout << "Done." << endl;
}

bool Analyser::eliminate_direct_recursion(Nonterminal &nonterminal)
{
    bool has_direct_recursion = false;
    symbol name = nonterminal.name;
    SymbolSet set;
    for (Candidate &candidate: *nonterminal.candidates)
    {
        set.insert(candidate[0]);
    }
    if (set.find(name) != set.end())
    {
        CandidateList *old_candidates = nonterminal.candidates;
        nonterminal.candidates = new CandidateList;
        Nonterminal new_nonterminal(name);
        symbol new_name = name;
        has_direct_recursion = true;
        while (is_nonterminal(new_name))
        {
            new_name += "'";
        }
        new_nonterminal.name = new_name;
        new_nonterminal.candidates->push_back({EMPTY_MARK});
        for (Candidate &candidate: *old_candidates)
        {
            if (candidate[0] == name)
            {
                candidate.erase(candidate.begin());
                candidate.push_back(new_name);
                new_nonterminal.candidates->push_back(candidate);
            }
            else
            {
                candidate.push_back(new_name);
                nonterminal.candidates->push_back(candidate);
            }
        }
        nonterminals->insert({new_name, new_nonterminal});
        delete old_candidates;
    }
    return has_direct_recursion;
}

void Analyser::eliminate_recursion()
{
    bool has_new_nonterminals = false;
    cout << "Eliminating recursion..." << endl;
    for (auto rnit = nonterminals->begin(); rnit != nonterminals->end(); rnit++)
    {
        if (rnit->second.name != start_symbol)
        {
            for (auto &lnit: *nonterminals)
            {
                if (lnit.second.name == start_symbol || lnit.second.name < rnit->second.name)
                {
                    bool has_replacement = true;
                    CandidateList *rcandidates = rnit->second.candidates;
                    while (has_replacement)
                    {
                        has_replacement = false;
                        for (CandidateList::iterator cit = rcandidates->begin(); cit != rcandidates->end(); cit++)
                        {
                            if (cit->at(0) == lnit.second.name)
                            {
                                Candidate right_part = *cit;
                                cit = rcandidates->erase(cit);
                                has_replacement = true;
                                right_part.erase(right_part.begin());
                                for (const Candidate &left_candidate: *lnit.second.candidates)
                                {
                                    Candidate new_candidate = left_candidate;
                                    new_candidate.insert(new_candidate.end(), right_part.begin(), right_part.end());
                                    rcandidates->push_back(new_candidate);
                                }
                                cit = rcandidates->begin();
                            }
                        }
                    }
                }
            }
        }
        has_new_nonterminals = eliminate_direct_recursion(rnit->second) || has_new_nonterminals;
        if (has_new_nonterminals)
        {
            rnit = nonterminals->begin();
            has_new_nonterminals = false;
        }
    }
    cout << "Done." << endl;
}

void Analyser::eliminate_common_prefix()
{
    bool has_new_nonterminals = false;
    cout << "Eliminating common prefix..." << endl;
    for (auto nit = nonterminals->begin(); nit != nonterminals->end(); nit++)
    {
        has_new_nonterminals = false;
        CandidateList *old_candidates = nit->second.candidates;
        nit->second.candidates = new CandidateList;
        PrefixMap map;
        int i = 0;
        for (Candidate &candidate: *old_candidates)
        {
            map.insert({candidate[0], i});
            i++;
        }
        for (PrefixMap::iterator mit = map.begin(), end = map.end(); mit != end; mit = map.upper_bound(mit->first))
        {
            if (map.count(mit->first) == 1)
            {
                nit->second.candidates->push_back((*old_candidates)[mit->second]);
            }
            else
            {
                auto range = map.equal_range(mit->first);
                symbol new_name = nit->first;
                Nonterminal new_nonterminal(new_name);
                while (is_nonterminal(new_name))
                {
                    new_name += "'";
                }
                new_nonterminal.name = new_name;
                for (auto cit = range.first; cit != range.second; cit++)
                {
                    Candidate new_candidate = (*old_candidates)[cit->second];
                    new_candidate.erase(new_candidate.begin());
                    if (new_candidate.size() == 0)
                    {
                        new_candidate.push_back(EMPTY_MARK);
                    }
                    new_nonterminal.candidates->push_back(new_candidate);
                }
                nonterminals->insert({new_name, new_nonterminal});
                has_new_nonterminals = true;
                nit->second.candidates->push_back({mit->first, new_name});
            }
        }
        if (has_new_nonterminals)
        {
            nit = nonterminals->begin();
        }
        delete old_candidates;
    }
    cout << "Done." << endl;
}

void Analyser::calculate_follows()
{
    bool need_update = true;
    cout << "Calculating FOLLOWs..." << endl;
    nonterminals->at(start_symbol).follows->insert(END_MARK);
    while (need_update)
    {
        need_update = false;
        for (auto &nit: *nonterminals)
        {
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
    cout << "Done." << endl;
}

bool Analyser::build_table(bool no_error)
{
    cout << "Building table..." << to_string(nonterminals->size()) << endl;
    for (auto &nit: *nonterminals)
    {
        int i = 0;
        AnalyseTable *table = nit.second.table;
        cout << "Inserting rules for `" << nit.first << "`." << endl;
        int empty_index = -1;
        for (Candidate &cit: *nit.second.candidates)
        {
            TerminalSet new_set = get_firsts(cit);
            for (Terminal const &t: new_set)
            {
                if (t == EMPTY_MARK)
                {
                    empty_index = i;
                }
                else
                {
                    AnalyseTable::iterator tit = table->find(t);
                    if (tit == table->end())
                    {
                        table->insert({t, i});
                    }
                    else
                    {
                        if (no_error)
                        {
                            cout << "Try again." << endl;
                            return false;
                        }
                        else
                        {
                            raise_error("ERROR: Non-LL(1) grammar detected.");
                        }
                    }
                }
            }
            i++;
        }
        if (empty_index != -1)
        {
            for (Terminal const &follower: *nit.second.follows)
            {
                AnalyseTable::iterator tit = table->find(follower);
                if (tit == table->end())
                {
                    table->insert({follower, empty_index});
                }
                else
                {
                    if (no_error)
                    {
                        cout << "Try again." << endl;
                        return false;
                    }
                    else
                    {
                        raise_error("ERROR: Non-LL(1) grammar detected.");
                    }
                }
            }
        }
        for (Terminal const &follower: *nit.second.follows)
        {
            if (table->find(follower) == table->end())
            {
                table->insert({follower, SYNCH_MARK_INDEX});
            }
        }
    }
    cout << "Done." << endl;
    return true;
}

void Analyser::print_table() const
{
    const int column_width = 10;
    cout << "Analyse table:" << endl;
    cout << setw(5) << "Symbol";
    for (const symbol &word: *terminals)
    {
        cout << setw(column_width) << word;
    }
    cout << endl;
    for (const auto &nit: *nonterminals)
    {
        cout << setw(5) << nit.first;
        for (const symbol &word: *terminals)
        {
            string rule_text;
            if (nit.second.table->find(word) != nit.second.table->end())
            {
                int rule_index = nit.second.table->at(word);
                if (rule_index == -1)
                {
                    rule_text = SYNCH_MARK;
                }
                else
                {
                    rule_text = container_to_string(nit.second.candidates->at(rule_index), "");
                    if (rule_text.size() == 0)
                    {
                        rule_text = "@";
                    }
                }
                // rule_text = nit.first + PRODUCTION_MARK + rule_text;
            }
            else
            {
                rule_text = "--";
            }
            cout << setw(column_width) << rule_text;
        }
        cout << endl;
    }
}

void Analyser::print_firsts() const
{
    cout << "FIRST set:" << endl;
    for (auto &nit: *nonterminals)
    {
        cout << nit.first << ":\t" << container_to_string(*nit.second.firsts) << endl;
    }
}

void Analyser::print_follows() const
{
    cout << "FOLLOW set:" << endl;
    for (auto &nit: *nonterminals)
    {
        cout << nit.first << ":\t" << container_to_string(*nit.second.follows) << endl;
    }
}

void Analyser::receive_text(istream &stream)
{
    string raw_string;
    cout << "Please enter a piece of text to analyse, type `$` to exit: ";
    getline(stream, raw_string);
    while (raw_string.size() > 0 && raw_string != END_MARK)
    {
        analyse(raw_string);
        getline(stream, raw_string);
    }
}

void Analyser::analyse(string code_text)
{
    int i = 1;
    SymbolStack stack;
    string sentence = "";
    SymbolList *words = string_to_vector(code_text + " " + END_MARK);
    SymbolList::iterator wit = words->begin();
    cout << endl;
    cout << "Analysing:" << endl;
    stack.push_back(END_MARK);
    stack.push_back(start_symbol);
    cout << setw(5) << "No." << setw(15) << "Stack" << setw(20) << "Input" << setw(15) << "Sentence" << setw(40) << "Output" << endl;
    while (true)
    {
        symbol top = stack.back();
        cout << setw(5) << to_string(i) << setw(15) << container_to_string(stack, "");
        cout << setw(20) << container_to_string(*words, "", wit - words->begin());
        cout << setw(15) << sentence << reversed_container_to_string(stack, "", 1) << setw(40);
        if (top == END_MARK)
        {
            if (*wit == END_MARK)
            {
                cout << "FINISHED";
                break;
            }
            else
            {
                cout << "ERROR: end of string expected.";
                break;
            }
        }
        else
        {
            if (is_nonterminal(top))
            {
                Nonterminal nonterminal = nonterminals->at(top);
                AnalyseTable::iterator ait = nonterminal.table->find(*wit);
                if (ait == nonterminal.table->end())
                {
                    cout << "Error: skip unexpected word `" << *wit << "`.";
                    wit++;
                }
                else
                {
                    const int candidate_index = ait->second;
                    if (candidate_index == SYNCH_MARK_INDEX)
                    {
                        cout << "ERROR: pop finished nonterminal `" << top << "`";
                        stack.pop_back();
                    }
                    else
                    {
                        Candidate candidate = nonterminal.candidates->at(ait->second);
                        string candidate_text = container_to_string(candidate);
                        stack.pop_back();
                        if (candidate_text != EMPTY_MARK)
                        {
                            for (Candidate::reverse_iterator rit = candidate.rbegin(); rit != candidate.rend(); rit++)
                            {
                                stack.push_back(*rit);
                            }
                            if (candidate_text.size() == 0)
                            {
                                candidate_text = EMPTY_MARK;
                            }
                        }
                        cout << top << PRODUCTION_MARK << candidate_text;
                    }
                }
            }
            else
            {
                if (*wit == top)
                {
                    sentence += top;
                    stack.pop_back();
                    cout << "Match";
                    wit++;
                }
                else
                {
                    cout << "ERROR: pop finished terminal `" << top << "`";
                    stack.pop_back();
                }
            }
            i++;
        }
        cout << endl;
    }
    cout << endl;
    delete words;
}
