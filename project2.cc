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
// recursive procedures to check if the grammar for an input grammar is being followed

// read grammar
void ReadGrammar()
{

    LexicalAnalyzer *lexer = new LexicalAnalyzer();

    if (validRuleList(lexer) && lexer->GetToken().token_type == HASH)
    {

        // just looping through the linked list to check if i split the input correctly
        /*struct rule *iterator = head;
        while(iterator != NULL){
            cout << iterator->lhs << ": ";
            for(int i = 0; i < iterator->rhs.size(); i++){
                cout << iterator->rhs[i] << " ";
            }
            cout << endl;

            iterator = iterator->next;
        }*/
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

// Task 2
void RemoveUselessSymbols()
{

    cout << "2\n";
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
    // cout << "this: " << first_sets[nonterminals[1]].front() << endl;
}

unordered_map<string, deque<string>> CalculateFirstSets() // gonna fix order at the end cuz the order thing is fucking stupid
{

    deque<string> rules = findOrdered_TerminalsAndNonTerminals().second;

    deque<string> update_first_sets;
    unordered_set<string> update_first_sets_inserted;

    unordered_map<string, deque<string>> first_sets;
    unordered_map<string, unordered_set<string>> first_sets_inserted;

    /*struct rule *iterator = head;
    while(iterator != NULL){
        cout << "testing rule: " << iterator->lhs << endl;

        if(goesToEpsilonDirectly(iterator->lhs)){
            cout << iterator->lhs << "goes to epsilon" << endl;
            if(first_sets_inserted[iterator->lhs].insert("#").second){
                cout << "inserted epsilon for " << iterator->lhs << endl;
                first_sets[iterator->lhs].push_front("#");
            }
        }

        iterator = iterator->next;
    }*/

    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();

    while (!rules.empty()) // can skip over the eles that already have entries in the map
    {
        string curr_rule = rules.front();
        update_first_sets.push_back(curr_rule);
        update_first_sets_inserted.insert(curr_rule);
        rules.pop_front();

        while (!update_first_sets.empty())
        {
            // cout << "rule: " << curr_rule << " with the queue of its first set eles:";
            /*for (int q = 0; q < update_first_sets.size(); q++)
            {
                cout << update_first_sets[q] << " ";
            }
            cout << endl;*/
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
                        // cout << "found a rule that matches first node in our queue: " << iterator->lhs << endl;
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
        /*cout << "current queue: ";
        for (auto p = nonterms_have_epsilon.begin(); p != nonterms_have_epsilon.end(); p++)
        {
            cout << *p << " ";
        }
        cout << endl;*/

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

    /*cout << "final queue: ";
       for (auto p = nonterms_have_epsilon.begin(); p != nonterms_have_epsilon.end(); p++)
       {
           cout << *p << " ";
       }
       cout << endl;*/
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
        // cout << "got to printing, just doesn't exist" << endl;
        deque<string> follow_set = all_follow_set[nonterminals[k]];
        cout << "FOLLOW(" << nonterminals[k] << ") = { ";
        // cout << first_set.size();
        for (int i = 0; i < follow_set.size(); i++)
        {
            if (follow_set.size() == 1)
            {
                cout << " " << follow_set[follow_set.size() - 1];
            }
            else if (i == follow_set.size() - 1)
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
    // cout << "here -4? ";

    unordered_map<string, deque<string>> first_sets = CalculateFirstSets();

    // cout << "here -3? ";

    unordered_map<string, unordered_set<string>> union_with_follow_sets; // nonterms at the end of a rule that union with the rule's follow set

    unordered_set<string> incomplete_follow_set_nonterms;

    unordered_set<string> nonterms_goto_epsilon = ListOfNonterminalsThatGoToEpsilon();

    // cout << "here -2? ";

    if (follow_set_inserted[head->lhs].insert("$").second)
    {
        follow_set[head->lhs].push_back("$");
    }
    // cout << "here -1? ";
    struct rule *iterator = head;
    while (iterator != NULL)
    {
        // cout << "here 0? ";
        if (iterator->rhs.size() > 0)
        {
            for (size_t i = 0; i < iterator->rhs.size() - 1; i++)
            {
                // cout << "here 1? ";
                if (isNonterminal(iterator->rhs[i]))
                {
                    // cout << "here 2? ";
                    if (isNonterminal(iterator->rhs[i + 1]))
                    {
                        int x = i + 1;
                        bool addNext = true;
                        while (addNext) //if the nonterm next to rhs[i] goes to epsilon, the nonterms/terms after it also follow rhs[i]
                        {
                            if (isNonterminal(iterator->rhs[x]))
                            {
                                //cout << "into " << iterator->rhs[i] << " insert: ";
                                for (int j = 0; j < first_sets[iterator->rhs[i + 1]].size(); j++)
                                {
                                    if (follow_set_inserted[iterator->rhs[i]].insert(first_sets[iterator->rhs[i + 1]][j]).second)
                                    {
                                        //cout << first_sets[iterator->rhs[i + 1]][j] << " ";
                                        follow_set[iterator->rhs[i]].push_back(first_sets[iterator->rhs[i + 1]][j]);
                                    }
                                }
                                //cout << endl;
                                addNext = (nonterms_goto_epsilon.count(iterator->rhs[x]) == 1);
                                x++;
                            }
                            else
                            {
                                if (follow_set_inserted[iterator->rhs[i]].insert(iterator->rhs[x]).second)
                                {
                                    //cout << iterator->rhs[x] << " ";
                                    follow_set[iterator->rhs[i]].push_back(iterator->rhs[x]);
                                }
                                addNext = false;
                            }
                        }
                    }
                    else
                    {

                        if (follow_set_inserted[iterator->rhs[i]].insert(iterator->rhs[i + 1]).second)
                        {
                            //cout << "into " << iterator->rhs[i] << "insert: " << iterator->rhs[i + 1];
                            follow_set[iterator->rhs[i]].push_back(iterator->rhs[i + 1]);
                        }
                        //cout << endl;
                    }
                }
            }
            cout << endl;
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

    bool some_nonterms_updated = true;
    while(some_nonterms_updated)
    some_nonterms_updated = false;
    for (auto it = incomplete_follow_set_nonterms.begin(); it != incomplete_follow_set_nonterms.end();)
    { // for each unfinished follow set

        cout << "incomplete follow set for rule " << *it << " needs to union with the follow sets of: ";
        for (auto union_iter = union_with_follow_sets[*it].begin(); union_iter != union_with_follow_sets[*it].end();)
        { // for each follow set it is supposed to union with

            // cout << *union_iter << " ";
            if (complete_follow_set_nonterms.count(*union_iter) == 1)
            {

                cout << "union with first(" << *union_iter << "): ";

                for (size_t h = 0; h < follow_set[*union_iter].size(); h++)
                {
                    cout << follow_set[*union_iter][h] << " ";

                    if (follow_set_inserted[*it].insert(follow_set[*union_iter][h]).second)
                    {

                        follow_set[*it].push_back(follow_set[*union_iter][h]);
                    }
                }

                union_iter = union_with_follow_sets[*it].erase(union_iter);
            }
            else
            {
                ++union_iter;
            }
        }
        cout << endl;
        if (union_with_follow_sets[*it].size() == 0)
        {
            complete_follow_set_nonterms.insert(*it);
            it = incomplete_follow_set_nonterms.erase(it);
            some_nonterms_updated = true; //any time a rule's dependencies are resolved, we need to check to see if any other unresolved rules depend on the now-resolved rule
        }
        else
        {
            it++;
        }
    }
    // cout << endl
    //cout << "new rules!" << endl;
    for (auto it = incomplete_follow_set_nonterms.begin(); it != incomplete_follow_set_nonterms.end(); it++)
    { // for each unfinished follow set

        cout << "incomplete follow set for rule " << *it << " needs to union with the follow sets of: ";
        if (union_with_follow_sets[*it].size() > 0)
        {
            for (auto union_iter = union_with_follow_sets[*it].begin(); union_iter != union_with_follow_sets[*it].end(); union_iter++)
            { // for each follow set it is supposed to union with

                cout << *union_iter << " ";
            }
        }
        else
        {
            cout << "no entry anymore for rule " << *it;
        }
        cout << endl;
    }

    // resolve cycles by
    /* while(incomplete_follow_set_nonterms.size() != 0){

     }*/

    for (auto it = incomplete_follow_set_nonterms.begin(); it != incomplete_follow_set_nonterms.end(); it++)
    { // for each unfinished follow set

        if (union_with_follow_sets[*it].size() > 0)
        {
            for (auto union_iter = union_with_follow_sets[*it].begin(); union_iter != union_with_follow_sets[*it].end(); union_iter++)
            { // for each follow set it is supposed to union with
            }
        }
    }

    return follow_set;

    // add dollars to S and the symbols that immediately come after S
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    cout << "5\n";
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
        RemoveUselessSymbols();
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
