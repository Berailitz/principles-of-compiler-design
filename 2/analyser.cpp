#include "analyser.hpp"

bool merge_set(TerminalSet &destination, const TerminalSet &source)
{
    int old_size = destination.size();
    destination.insert(source.begin(), source.end());
    return destination.size() > old_size;
}

Rule *Analyser::build_rule(string grammar_text)
{
    Rule *rule = new Rule;
    istringstream iss(grammar_text);
    vector<string> words((istream_iterator<string>(iss)),
                         istream_iterator<string>());
    if (words[1] == PRODUCTION_MARK)
    {
        Candidate *candidate = new Candidate;
        cout << "ok";
        rule->nonterminal = words[0];
        for (int i = 1; i < words.size(); i++)
        {
            symbol new_word = words[i];
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
        cout << "Invalid grammar.";
        return nullptr;
    }
}

void Analyser::add_rule(Rule &new_rule)
{
    Nonterminal word = nonterminals->at(new_rule.nonterminal);
    for (Candidate &it : (*new_rule.candidates))
    {
        word.candidates->push_back(it);
    }
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
    TerminalSet current_set = get_firsts(candidate[0]);
    if (current_set.find(EMPTY_MARK) != current_set.end() && candidate.size() > 1)
    {
        Candidate new_candidate(candidate.begin() + 1, candidate.end());
        merge_set(current_set, get_firsts(new_candidate));
    }
    return current_set;
}

void Analyser::calculate_firsts()
{
    bool need_update = true;
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
}

void Analyser::calculate_follows()
{
    bool need_update = true;
    nonterminals->at(start_symbol).follows->insert(END_MARK);
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
                    if (i == nit.second.candidates->size())
                    {
                        need_update = merge_set(*nonterminals->at(word).follows, *nit.second.follows) || need_update;
                    }
                    else
                    {
                        Candidate endian(cit.begin() + i, cit.end());
                        TerminalSet new_set = get_firsts(endian);
                        if (new_set.find(EMPTY_MARK) != new_set.end())
                        {
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
                        nit.second.table->at(follower) = i;
                    }
                }
                else
                {
                    nit.second.table->at(t) = i;
                }
            }
            i++;
        }
    }
}

void Analyser::analyse(string code_text)
{
    string::iterator it = code_text.begin();
    SymbolStack stack;
    stack.push(END_MARK);
    stack.push(start_symbol);
    while (stack.top() != END_MARK)
    {
        symbol top = stack.top();
        string it_string = to_string(*it);
        if (is_nonterminal(top))
        {
            Nonterminal nonterminal = nonterminals->at(top);
            try
            {
                int candidate_index = nonterminal.table->at(it_string);
                Candidate candidate = nonterminal.candidates->at(candidate_index);
                stack.pop();
                for (Candidate::reverse_iterator rit = candidate.rbegin(); rit != candidate.rend(); rit++)
                {
                    stack.push(*rit);
                }
            }
            catch (const std::out_of_range &e)
            {
                cout << "Error prasing.";
            }
        }
        else
        {
            if (it_string == top)
            {
                stack.pop();
                it++;
            }
            else
            {
                cout << "Error prasing.";
            }
        }
    }
}
