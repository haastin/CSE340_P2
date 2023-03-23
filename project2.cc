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

            if (nonterminals_unordered.count(iterator->rhs[i]) == 1) //if its a nonterminal
            { 
                if (inserted_nonterminals.insert(iterator->rhs[i]).second) //and hasnt been inserted into our queue yet
                {
                    nonterminals.push_back(iterator->rhs[i]); 
                }
            }
            else
            {
                if (inserted_terminals.insert(iterator->rhs[i]).second) //if its a terminal
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
void CalculateFirstSets()
{
    cout << "3\n";
}

// Task 4
void CalculateFollowSets()
{
    cout << "4\n";
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
        CalculateFirstSets();
        break;

    case 4:
        CalculateFollowSets();
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
