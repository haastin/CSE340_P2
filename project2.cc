/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <stack>
#include <unordered_map>

using namespace std;

struct rule
{

    string lhs;
    vector<string> rhs;
    struct rule *next;
    struct rule *prev;
};

static struct rule *head = NULL; // linked list of rules
static struct rule *tail = NULL;

bool validRuleList(LexicalAnalyzer *);
bool validRule(LexicalAnalyzer *);
bool validRHS(LexicalAnalyzer *);
bool validIDList(LexicalAnalyzer *);
unordered_set<string> findUnorderedNonterminals();
pair<deque<string>, deque<string>> findOrdered_TerminalsAndNonTerminals();
bool isNonterminal(string);
bool goesToEpsilonDirectly(string);
unordered_map<string, deque<string>> CalculateFirstSets();
void printFirstSets(unordered_map<string, deque<string>>);
deque<string> sortListOfTerminals(deque<string>);
deque<string> allTerminalsInOrder();
unordered_set<string> ListOfNonterminalsThatGoToEpsilonDirectly();
unordered_set<string> ListOfNonterminalsThatGoToEpsilon();
unordered_set<string> setOfGeneratingRules();
unordered_set<string> setOfReachableRules();
unordered_set<string> unorderedListOfRemovedUselessNonterminals();
// recursive procedures to check if the grammar for an input grammar is being followed

// read grammar
void ReadGrammar()
{

    LexicalAnalyzer *lexer = new LexicalAnalyzer();

    if (validRuleList(lexer) && lexer->GetToken().token_type == HASH)
    {
    }
    else
    { // if the input grammar does not follow our required format
        cout << "SYNTAX ERROR !!!";
        exit(0);
    }

    return;
}

bool validRuleList(LexicalAnalyzer *lexer)
{

    bool isValidRule = true;
    TokenType next_token = lexer->peek(1).token_type;

    while (next_token == ID && isValidRule)
    {
        isValidRule = validRule(lexer);
        next_token = lexer->peek(1).token_type;
    }

    if (isValidRule)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*we build the structs here because once a LexicalAnalyzer is created it makes it so you cant change its index field, so once you move
past a certain index in tokenList you can't see it again, youd have to create a new lexer, but not sure if the stdin would be fully consumed
and then potentially unavailable? The design is fucking weird*/
bool validRule(LexicalAnalyzer *lexer)
{

    struct rule *new_rule = new rule();

    if (head == NULL)
    { // our first rule in our linked list
        head = new_rule;
        new_rule->prev = NULL;
        new_rule->next = NULL;
        tail = head;
    }
    else
    { // insert our new rule into our new rule linked list
        tail->next = new_rule;
        new_rule->prev = tail;
        new_rule->next = NULL;
        tail = new_rule;
    }

    new_rule->lhs = lexer->GetToken().lexeme;

    if (lexer->GetToken().token_type != ARROW)
    {
        return false;
    }

    if (validRHS(lexer) && lexer->GetToken().token_type == STAR)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool validRHS(LexicalAnalyzer *lexer)
{

    bool isValidIDList = true;

    while (isValidIDList && lexer->peek(1).token_type == ID)
    {
        isValidIDList = validIDList(lexer);
    }

    if (isValidIDList)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool validIDList(LexicalAnalyzer *lexer)
{

    if (lexer->peek(1).token_type == ID)
    {
        tail->rhs.push_back(lexer->GetToken().lexeme);
    }

    return true;
}

// Task 1
void printTerminalsAndNoneTerminals()
{
    pair<deque<string>, deque<string>> terms_nonterms = findOrdered_TerminalsAndNonTerminals();
    deque<string> terminals = terms_nonterms.first;
    deque<string> nonterminals = terms_nonterms.second;

    int p;
    for (p = 0; p < terminals.size(); p++)
    {
        cout << terminals[p] << " ";
    }
    for (p = 0; p < nonterminals.size(); p++)
    {
        cout << nonterminals[p] << " ";
    }

    return;
}

unordered_set<string> findUnorderedNonterminals()
{
    struct rule *iterator = head;
    unordered_set<string> nonterminals_unordered;
    while (iterator != NULL)
    {

        nonterminals_unordered.insert(iterator->lhs);

        iterator = iterator->next;
    }
    return nonterminals_unordered;
}

pair<deque<string>, deque<string>> findOrdered_TerminalsAndNonTerminals()
{

    pair<deque<string>, deque<string>> terms_nonterms;

    struct rule *iterator = head;

    deque<string> terminals;
    unordered_set<string> inserted_terminals;

    deque<string> nonterminals;
    unordered_set<string> inserted_nonterminals;

    unordered_set<string> nonterminals_unordered = findUnorderedNonterminals();

    while (iterator != NULL)
    {
        /*the order that the rubric wants is to print terminals nonterminals, and the order of each being the order you discover them, going
        from the first rule down, and from left to right in each rule; thus, we start by checking the lhs, then moving to check all in rhs*/
        if (inserted_nonterminals.insert(iterator->lhs).second)
        {
            nonterminals.push_back(iterator->lhs);
        }

        for (int i = 0; i < iterator->rhs.size(); i++)
        {

            if (nonterminals_unordered.count(iterator->rhs[i]) == 1) // if its a nonterminal
            {
                if (inserted_nonterminals.insert(iterator->rhs[i]).second) // and hasnt been inserted into our queue yet
                {
                    nonterminals.push_back(iterator->rhs[i]);
                }
            }
            else
            {
                if (inserted_terminals.insert(iterator->rhs[i]).second) // if its a terminal
                {
                    terminals.push_back(iterator->rhs[i]);
                }
            }
        }

        iterator = iterator->next;
    }
    terms_nonterms.first = terminals;
    terms_nonterms.second = nonterminals;
    return terms_nonterms;
}

void printListOfRemovedUselessNonterminals()
{

    unordered_set<string> unordered_nonuseless_rules = unorderedListOfRemovedUselessNonterminals();

    struct rule *iterator = head;
    while (iterator != NULL)
    {

        for (auto it = unordered_nonuseless_rules.begin(); it != unordered_nonuseless_rules.end(); it++)
        {

            if (iterator->lhs == *it)
            {
                bool containsAllNonuselessRules = true;
                for (int f = 0; f < iterator->rhs.size(); f++)
                {
                    if (isNonterminal(iterator->rhs[f]) && unordered_nonuseless_rules.count(iterator->rhs[f]) == 0)
                    {
                        containsAllNonuselessRules = false;
                        break;
                    }
                }
                if (containsAllNonuselessRules)
                {
                    cout << *it << " -> ";

                    for (int f = 0; f < iterator->rhs.size(); f++)
                    {
                        cout << iterator->rhs[f] << " ";
                    }
                    if (iterator->rhs.size() == 0)
                    {
                        cout << "#";
                    }
                    cout << endl;
                }
            }
        }

        iterator = iterator->next;
    }
}

// Task 2
unordered_set<string> unorderedListOfRemovedUselessNonterminals()
{
    unordered_set<string> generating_rules = setOfGeneratingRules();

    unordered_set<string> reachable_rules = setOfReachableRules();

    unordered_set<string> unordered_nonuseless_rules;

    for (auto gen_it = generating_rules.begin(); gen_it != generating_rules.end(); gen_it++)
    {
        // cout << *gen_it << " ";
        for (auto reach_it = reachable_rules.begin(); reach_it != reachable_rules.end(); reach_it++)
        {
            if (*gen_it == *reach_it)
            {
                unordered_nonuseless_rules.insert(*gen_it);
            }
        }
    }
    unordered_set<string> rules_containing_only_nonuseless_symbols;
    string start_symbol = head->lhs;
    deque<string> rules_to_explore;
    rules_to_explore.push_front(start_symbol);

    while (!rules_to_explore.empty())
    {

        string curr_rule = rules_to_explore.front();
        struct rule *iterator = head;
        while (iterator != NULL)
        {
            if (iterator->lhs == curr_rule)
            {
                bool allNonuseless = true;

                for (size_t x = 0; x < iterator->rhs.size(); x++)
                {

                    if (isNonterminal(iterator->rhs[x]) && unordered_nonuseless_rules.count(iterator->rhs[x]) == 0)
                    {
                        allNonuseless = false;
                        break;
                    }
                }
                if (allNonuseless)
                {
                    rules_containing_only_nonuseless_symbols.insert(iterator->lhs);
                    for (size_t x = 0; x < iterator->rhs.size(); x++)
                    {
                        if (isNonterminal(iterator->rhs[x]) && rules_containing_only_nonuseless_symbols.insert(iterator->rhs[x]).second)
                        {
                            rules_to_explore.push_back(iterator->rhs[x]);
                        }
                    }
                }
            }
            iterator = iterator->next;
        }

        rules_to_explore.pop_front();
    }

    return rules_containing_only_nonuseless_symbols;
}

unordered_set<string> setOfReachableRules()
{

    string start_symbol = head->lhs;
    unordered_set<string> reachable_rules;

    deque<string> rules_to_explore;
    reachable_rules.insert(start_symbol);
    rules_to_explore.push_front(start_symbol);

    while (!rules_to_explore.empty())
    {

        string curr_rule = rules_to_explore.front();
        struct rule *iterator = head;
        while (iterator != NULL)
        {
            if (iterator->lhs == curr_rule)
            {
                for (size_t x = 0; x < iterator->rhs.size(); x++)
                {

                    if (isNonterminal(iterator->rhs[x]) && reachable_rules.insert(iterator->rhs[x]).second)
                    {
                        rules_to_explore.push_back(iterator->rhs[x]);
                    }
                }
            }
            iterator = iterator->next;
        }

        rules_to_explore.pop_front();
    }

    return reachable_rules;
}

unordered_set<string> setOfGeneratingRules()
{

    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();
    // find generating symbols

    unordered_set<string> generating_rules;

    // all rules that go to epsilon are generating, so add them first

    struct rule *iterator = head;
    while (iterator != NULL)
    {

        if (nonterms_goto_epsilon.count(iterator->lhs) == 1)
        {
            generating_rules.insert(iterator->lhs);
        }
        iterator = iterator->next;
    }
    bool addedGeneratingRules = true;

    while (addedGeneratingRules)
    {
        addedGeneratingRules = false;
        struct rule *iterator = head;
        while (iterator != NULL)
        {

            bool rhsAllGenerating = true;
            for (size_t y = 0; y < iterator->rhs.size(); y++)
            {
                if (generating_rules.count(iterator->rhs[y]) == 1 || !isNonterminal(iterator->rhs[y]))
                {
                }
                else
                {
                    rhsAllGenerating = false;
                    break;
                }
            }
            if (rhsAllGenerating)
            {
                if (generating_rules.insert(iterator->lhs).second)
                    addedGeneratingRules = true;
            }

            iterator = iterator->next;
        }
    }

    return generating_rules;
}

// Task 3

void printFirstSets(unordered_map<string, deque<string>> first_sets)
{
    deque<string> nonterminals = findOrdered_TerminalsAndNonTerminals().second;
    deque<string> nonterminals_copy = nonterminals;

    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();

    // add epsilons to first set and order it the way bazzi wants it to be, then print
    for (int i = 0; i < first_sets.size(); i++)
    {
        first_sets[nonterminals[i]] = sortListOfTerminals(first_sets[nonterminals[i]]);
    }

    while (!nonterminals_copy.empty())
    {
        if (nonterms_goto_epsilon.count(nonterminals_copy.front()) == 1)
        {
            first_sets[nonterminals_copy.front()].push_front("#");
        }
        nonterminals_copy.pop_front();
    }

    // printing starts here

    for (int k = 0; k < nonterminals.size(); k++)
    {
        deque<string> first_set = first_sets[nonterminals[k]];
        cout << "FIRST(" << nonterminals[k] << ") = { ";
        // cout << first_set.size();
        for (int i = 0; i < first_set.size(); i++)
        {
            if (first_set.size() == 1)
            {
                cout << " " << first_set[first_set.size() - 1];
            }
            else if (i == first_set.size() - 1)
            {
                cout << first_set[first_set.size() - 1];
            }
            else
            {
                cout << first_set[i] << ", ";
            }
        }
        cout << " }" << endl;
    }
}

unordered_map<string, deque<string>> CalculateFirstSets() // gonna fix order at the end cuz the order thing is fucking stupid
{

    deque<string> rules = findOrdered_TerminalsAndNonTerminals().second;

    deque<string> update_first_sets;
    unordered_set<string> update_first_sets_inserted;

    unordered_map<string, deque<string>> first_sets;
    unordered_map<string, unordered_set<string>> first_sets_inserted;
    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();

    while (!rules.empty()) // can skip over the eles that already have entries in the map
    {
        string curr_rule = rules.front();
        update_first_sets.push_back(curr_rule);
        update_first_sets_inserted.insert(curr_rule);
        rules.pop_front();

        while (!update_first_sets.empty())
        {
            struct rule *iterator = head;

            // sort update_first_sets to be in order that the nonterms appear in the grammar

            while (iterator != NULL)
            { // find all terms and nonterms for rule at front of queue

                if (iterator->lhs == update_first_sets.front())
                { // the rule at front of queue

                    if (iterator->rhs.size() == 0)
                    {
                    }
                    else
                    {

                        if (!isNonterminal(iterator->rhs[0]))
                        {                                                                       // if its first element is a terminal
                            if (first_sets_inserted[curr_rule].insert(iterator->rhs[0]).second) // check if we've already added it
                                first_sets[curr_rule].push_back(iterator->rhs[0]);              // then add if not
                        }
                        else
                        { // else if its a nonterminal, we need to check if we can go past it

                            if (nonterms_goto_epsilon.count(iterator->rhs[0]) == 1)
                            {
                                if (update_first_sets_inserted.insert(iterator->rhs[0]).second)
                                {
                                    update_first_sets.push_back(iterator->rhs[0]);
                                }

                                for (int i = 1; i < iterator->rhs.size(); i++)
                                {
                                    if (isNonterminal(iterator->rhs[i]))
                                    {
                                        if (nonterms_goto_epsilon.count(iterator->rhs[i]) == 1)
                                        {
                                            if (update_first_sets_inserted.insert(iterator->rhs[i]).second)
                                            {
                                                update_first_sets.push_back(iterator->rhs[i]);
                                            }
                                        }
                                        else
                                        {
                                            if (update_first_sets_inserted.insert(iterator->rhs[i]).second)
                                            {
                                                update_first_sets.push_back(iterator->rhs[i]);
                                            }
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        if (first_sets_inserted[curr_rule].insert(iterator->rhs[i]).second)
                                        {
                                            first_sets[curr_rule].push_back(iterator->rhs[i]);
                                        }
                                        break;
                                    }
                                }
                            }

                            else if (update_first_sets_inserted.insert(iterator->rhs[0]).second)
                            {
                                update_first_sets.push_back(iterator->rhs[0]);
                            }
                        }
                    }
                }

                iterator = iterator->next;
            }
            update_first_sets.pop_front();
        }
        update_first_sets_inserted.clear();
        // each unique ule has its vector of first terms
        // for each rule
        // if first rhs ele is term, add it to rule's first set and break
        // could keep your own stack frame with a stack of vectors you need to go back and update
        // else, while rhs ele is a nonterminal
        //        go find the rule(s) for that nonterminal, and if all rules for this nonterm dont have first rhs as term
    }
    return first_sets;
}

deque<string> allTerminalsInOrder()
{

    deque<string> terminals;
    unordered_set<string> terminals_inserted;
    struct rule *iterator = head;
    while (iterator != NULL)
    {

        for (int i = 0; i < iterator->rhs.size(); i++)
        {

            if (!isNonterminal(iterator->rhs[i]))
            {
                if (terminals_inserted.insert(iterator->rhs[i]).second)
                {
                    terminals.push_back(iterator->rhs[i]);
                }
            }
        }

        iterator = iterator->next;
    }

    return terminals;
}

deque<string> sortListOfTerminals(deque<string> terminals)
{

    deque<string> sorted_terminals;
    deque<string> all_terms_ordered = allTerminalsInOrder();

    for (int i = 0; i < all_terms_ordered.size(); i++)
    {

        string curr_first = all_terms_ordered.front();
        for (int j = 0; j < terminals.size(); j++)
        {
            if (terminals[j] == all_terms_ordered[i])
            {
                sorted_terminals.push_back(terminals[j]);
            }
        }
    }

    for (int k = 0; k < terminals.size(); k++)
    {
        if (terminals[k] == "$")
        {
            sorted_terminals.push_front("$");
            break;
        }
        else if (terminals[k] == "#")
        {
            sorted_terminals.push_front("#");
            break;
        }
    }
    return sorted_terminals;
}

bool goesToEpsilonDirectly(string rule)
{
    struct rule *iterator = head;
    while (iterator != NULL)
    {

        if (iterator->lhs == rule)
        {
            if (iterator->rhs.size() == 0)
            {
                return true;
            }
        }
        iterator = iterator->next;
    }
    return false;
}

unordered_set<string> ListOfNonterminalsThatGoToEpsilonDirectly()
{

    unordered_set<string> nonterminals_direct;

    unordered_set<string> nonterminals_list = findUnorderedNonterminals();

    for (auto i = nonterminals_list.begin(); i != nonterminals_list.end(); i++)
    {

        if (goesToEpsilonDirectly(*i))
        {
            nonterminals_direct.insert(*i);
        }
    }

    return nonterminals_direct;
}

unordered_set<string> ListOfNonterminalsThatGoToEpsilon()
{

    bool updated_queue = true;

    unordered_set<string> nonterms_have_epsilon = ListOfNonterminalsThatGoToEpsilonDirectly();

    while (updated_queue)
    {
        updated_queue = false;

        struct rule *iterator = head;
        while (iterator != NULL)
        {
            bool allGoEpsilon = true;

            for (int i = 0; i < iterator->rhs.size(); i++) // if everything on the rhs is in the queue
            {
                if (nonterms_have_epsilon.count(iterator->rhs[i]) == 0)
                {
                    allGoEpsilon = false;
                    break;
                }
            }
            if (allGoEpsilon)
            {
                if (nonterms_have_epsilon.insert(iterator->lhs).second)
                {
                    updated_queue = true;
                }
            }

            iterator = iterator->next;
        }
    }
    return nonterms_have_epsilon;
}

bool isNonterminal(string element)
{
    unordered_set<string> nonterminals_unordered = findUnorderedNonterminals();
    if (nonterminals_unordered.insert(element).second)
    {
        return false;
    }
    return true;
}

// Task 4
void printFollowSets(unordered_map<string, deque<string>> all_follow_set)
{

    deque<string> nonterminals = findOrdered_TerminalsAndNonTerminals().second;

    for (int i = 0; i < all_follow_set.size(); i++)
    {
        all_follow_set[nonterminals[i]] = sortListOfTerminals(all_follow_set[nonterminals[i]]);
    }

    for (int k = 0; k < nonterminals.size(); k++)
    {

        deque<string> follow_set = all_follow_set[nonterminals[k]];
        cout << "FOLLOW(" << nonterminals[k] << ") = { ";

        for (int i = 0; i < follow_set.size(); i++)
        {
            if (i == follow_set.size() - 1)
            {
                cout << follow_set[follow_set.size() - 1];
            }
            else
            {
                cout << follow_set[i] << ", ";
            }
        }
        cout << " }" << endl;
    }
}

unordered_map<string, deque<string>> CalculateFollowSets()
{
    // initiailize follow sets
    unordered_map<string, deque<string>> follow_set;
    unordered_map<string, unordered_set<string>> follow_set_inserted;

    unordered_map<string, deque<string>> first_sets = CalculateFirstSets();

    unordered_map<string, unordered_set<string>> union_with_follow_sets; // nonterms at the end of a rule that union with the rule's follow set

    unordered_set<string> incomplete_follow_set_nonterms;

    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();

    if (follow_set_inserted[head->lhs].insert("$").second)
    {
        follow_set[head->lhs].push_back("$");
    }

    struct rule *iterator = head;
    while (iterator != NULL) // for every rule in the grammar
    {
        if (iterator->rhs.size() > 0) // if the rhs isn't epsilon
        {
            for (size_t i = 0; i < iterator->rhs.size() - 1; i++)
            {

                if (isNonterminal(iterator->rhs[i]))
                {

                    int x = i + 1;
                    while (x < iterator->rhs.size() && isNonterminal(iterator->rhs[x]))
                    {
                        for (int j = 0; j < first_sets[iterator->rhs[x]].size(); j++)
                        {
                            if (follow_set_inserted[iterator->rhs[i]].insert(first_sets[iterator->rhs[x]][j]).second)
                            {

                                follow_set[iterator->rhs[i]].push_back(first_sets[iterator->rhs[x]][j]);
                            }
                        }

                        if (nonterms_goto_epsilon.count(iterator->rhs[x]) != 1)
                        {
                            break;
                        }
                        x++;
                    }
                    if (x < iterator->rhs.size())
                    {
                        if (follow_set_inserted[iterator->rhs[i]].insert(iterator->rhs[x]).second)
                        {

                            follow_set[iterator->rhs[i]].push_back(iterator->rhs[x]);
                        }
                    }
                }
            }
        }
        if (iterator->rhs.size() > 0)
        {
            int i = iterator->rhs.size() - 1;
            while (i >= 0 && isNonterminal(iterator->rhs[i]))
            {
                if (iterator->lhs != iterator->rhs[i])
                {
                    union_with_follow_sets[iterator->rhs[i]].insert(iterator->lhs);
                    incomplete_follow_set_nonterms.insert(iterator->rhs[i]);
                }
                if (nonterms_goto_epsilon.count(iterator->rhs[i]) == 0)
                {
                    break;
                }
                i--;
            }
        }
        iterator = iterator->next;
    }

    // resolve completed follow sets

    unordered_set<string> complete_follow_set_nonterms = findUnorderedNonterminals();
    // find t
    for (auto it = incomplete_follow_set_nonterms.begin(); it != incomplete_follow_set_nonterms.end(); it++)
    {
        complete_follow_set_nonterms.erase(*it);
    }

    // resolve cycles
    for (auto it = incomplete_follow_set_nonterms.begin(); it != incomplete_follow_set_nonterms.end();) // go through each currently incomplete rule
    {
        deque<string> union_with_nonterms;
        unordered_set<string> possible_follow_nonterms;

        possible_follow_nonterms.insert(*it);
        union_with_nonterms.push_front(*it);

        string y = union_with_nonterms.front();

        while (!union_with_nonterms.empty())
        {
            for (auto m = union_with_follow_sets[y].begin(); m != union_with_follow_sets[y].end(); m++)
            {
                if (possible_follow_nonterms.insert(*m).second)
                    union_with_nonterms.push_back(*m); // place this dependency from the dependency list of the dependency we are resolving into the queue
            }

            union_with_nonterms.pop_front(); // and then delete the dependency we just resolved

            if (!union_with_nonterms.empty())
            {
                y = union_with_nonterms.front();
            }
        }

        for (auto union_iter = possible_follow_nonterms.begin(); union_iter != possible_follow_nonterms.end(); union_iter++)
        {
            if (follow_set[*union_iter].size() > 0 && *union_iter != *it)
            {
                for (auto h = follow_set[*union_iter].begin(); h != follow_set[*union_iter].end(); h++)
                {

                    if (follow_set_inserted[*it].insert(*h).second)
                    {
                        follow_set[*it].push_back(*h);
                    }
                }
            }
        }

        complete_follow_set_nonterms.insert(*it);
        it = incomplete_follow_set_nonterms.erase(it);
    }

    return follow_set;

    // add dollars to S and the symbols that immediately come after S
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    unordered_set<string> nonuseless_rules = unorderedListOfRemovedUselessNonterminals();

    unordered_map<string, deque<string>> first_sets = CalculateFirstSets();
    unordered_map<string, deque<string>> follow_sets = CalculateFollowSets();

    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();

    unordered_map<string, unordered_set<string>> already_inserted;

    if (nonuseless_rules.size() == 0)
    {
        cout << "NO" << endl;
        return;
    }

    struct rule *iterator = head;
    while (iterator != NULL)
    {

        if (iterator->rhs.size() != 0)
        {
            unordered_set<string> inserted_for_this_rule;
            for (int j = 0; j < iterator->rhs.size(); j++)
            {

                if (isNonterminal(iterator->rhs[j]))
                {

                    for (int i = 0; i < first_sets[iterator->rhs[j]].size(); i++)
                    {
                        if (!already_inserted[iterator->lhs].insert(first_sets[iterator->rhs[j]][i]).second)
                        {
                            if (inserted_for_this_rule.insert(first_sets[iterator->rhs[j]][i]).second)
                            {
                                cout << "NO" << endl;
                                return;
                            }
                        }
                        else
                        {
                            inserted_for_this_rule.insert(first_sets[iterator->rhs[j]][i]);
                        }
                    }
                    if (nonterms_goto_epsilon.count(iterator->rhs[j]) == 0)
                    {
                        break;
                    }
                }
                else
                {
                    if (!already_inserted[iterator->lhs].insert(iterator->rhs[j]).second)
                    {
                        if (inserted_for_this_rule.insert(iterator->rhs[j]).second)
                        {
                            cout << "NO" << endl;
                            return;
                        }
                    }
                    else
                    {
                        inserted_for_this_rule.insert(iterator->rhs[j]);
                    }
                    break;
                }
            }
        }
        else
        {
            if (first_sets[iterator->lhs].size() > follow_sets[iterator->lhs].size())
            {
                for (int i = 0; i < first_sets[iterator->lhs].size(); i++)
                {
                    for (int k = 0; k < follow_sets[iterator->lhs].size(); k++)
                    {
                        if (first_sets[iterator->lhs][i] == follow_sets[iterator->lhs][k])
                        {

                            cout << "NO" << endl;
                            return;
                        }
                    }
                }
            }
            else
            {
                for (int i = 0; i < follow_sets[iterator->lhs].size(); i++)
                {
                    for (int k = 0; k < first_sets[iterator->lhs].size(); k++)
                    {
                        if (first_sets[iterator->lhs][k] == follow_sets[iterator->lhs][i])
                        {

                            cout << "NO" << endl;
                            return;
                        }
                    }
                }
            }
        }
        iterator = iterator->next;
    }

    cout << "YES" << endl;
    return;
}

int main(int argc, char *argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    ReadGrammar(); // Reads the input grammar from standard input
                   // and represent it internally in data structures
                   // ad described in project 2 presentation file

    switch (task)
    {
    case 1:
        printTerminalsAndNoneTerminals();
        break;

    case 2:
        printListOfRemovedUselessNonterminals();
        break;

    case 3:
        printFirstSets(CalculateFirstSets());
        break;

    case 4:
        printFollowSets(CalculateFollowSets());
        break;

    case 5:
        CheckIfGrammarHasPredictiveParser();
        break;

    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
