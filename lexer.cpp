#include <algorithm>
#include <atomic>
#include <bits/stdc++.h>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#define state int
#define token string
#define adress pair<state,token>
#define tr map<token,set<state>>
#define af map<state,tr>
#define wm map<state,string>
using namespace std;

ifstream tokenFile("lexerTokens.txt");
ifstream codeFile("code.txt");
ofstream lexerErrorFile("lexerErrorFile.txt", std::ofstream::out | std::ofstream::trunc);
ofstream tapeMapFile("tapeMapFile.txt", std::ofstream::out | std::ofstream::trunc);
ofstream tapeFile("tapeFile.txt", std::ofstream::out | std::ofstream::trunc);

state error = -1;
state sid = 1;
set<token> usedTokens;
token stopToken = "ε";
vector<string> localState = {"S","A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "T", "U", "V", "W", "X", "Y", "Z"};
set<state> term;
af afnd;

af afd;
set<token> afdUsedTokens;
set<state> afdTerm;
state afdSid = 0;

wm wordMap;

void trimSpace(string &aux){
    aux.erase(aux.find_last_not_of(" ") + 1);
    aux.erase(0, aux.find_first_not_of(" "));
}

void trimRule(string &aux){
    aux.erase(aux.find_last_not_of("> ") + 1);
    aux.erase(0, aux.find_first_not_of(" <"));
}

void loadLocal(set<state> &termLocal, af &afLocal, set<token> &localUsedTokens){ 
    usedTokens.insert(localUsedTokens.begin(),localUsedTokens.end());
    int offset = (sid == 1)? 0 : sid-1;
    

    for (auto &i : termLocal){
        afnd[offset+i];
        term.insert(offset+i);
    }

    for (auto i : afLocal[0])
        for (auto &j : i.second) 
            afnd[0][i.first].insert(j+offset);

    afLocal.erase(0);

    for (auto &i : afLocal){ 
        for (auto &j : afLocal[i.first])
            for (auto &k : j.second) 
                afnd[sid][j.first].insert(k+offset);
        sid++;
    }
}

string getRule(string &aux){ 
    string rule, delimiter = "::=";
    int pos = 0;
    if((pos = aux.find(delimiter)) != string::npos) {
        rule = aux.substr(0, pos);
        trimRule(rule); 
        aux.erase(0, pos + delimiter.length());
    } 
    return rule;
}

pair<string, string> separateTransition(string aux){
    string t, delimiter = "<";
    int pos = 0;
    
    if (aux[0] == '<') {
        trimRule(aux);
        return {stopToken, aux};
    }

    if((pos = aux.find(delimiter)) != string::npos) {
        t = aux.substr(0, pos);
        aux.erase(0, pos-1 + delimiter.length());
    } 
    trimRule(aux);
    return {t,aux};
}

vector<string> separateTransitionList(string &aux){
    string sub,delimiter = "|";
    vector<string> pipeSeparated;
    int pos = 0; 
    while ((pos = aux.find(delimiter)) != std::string::npos) {
        sub = aux.substr(0, pos);
        trimSpace(sub);
        pipeSeparated.push_back(sub);
        aux.erase(0, pos + delimiter.length());
    }
    trimSpace(aux); 
    pipeSeparated.push_back(aux);
    return pipeSeparated;    
}

void loadAfndWord(){
    vector<string> inputStrings;
    string s; 
    while (getline(tokenFile,s) && !s.empty()) inputStrings.push_back(s);
    set<token> localUsedTokens;
    af afLocal;
    set<state> termLocal;
    int lsid = 0;

    for (auto &line : inputStrings) {
        lsid++;
        string c;
        c = line[0];
        localUsedTokens.insert(c);
        afLocal[0][c].insert(lsid);
        for (int i = 1; i < line.size(); i++) {
            c = line[i];
            localUsedTokens.insert(c);
            afLocal[lsid][c].insert(lsid+1);
            lsid++;
        }
        afLocal[lsid];
        termLocal.insert(lsid);
    }
    loadLocal(termLocal, afLocal, localUsedTokens);
}

void defineStates(){
    queue<adress> q;
    do{
        while(!q.empty()) {
            adress a = q.front(); q.pop();
            for (auto &i : afnd[a.first][a.second]){
                if (term.count(i)) term.insert(sid);
                for (auto &j : afnd[i]) afnd[sid][j.first].insert(j.second.begin(), j.second.end());
            }
            afnd[a.first][a.second].clear();
            afnd[a.first][a.second].insert(sid);
            sid++;
        }

        for (auto &i : afnd)
            for (auto &j : afnd[i.first]) 
                if (j.second.size() > 1)
                    q.push({i.first,j.first});

    }
    while(!q.empty());
}

void removeUnreachable(){
    bool marked[sid];
    for (auto &i : marked) i = false;
    marked[0] = true;
    queue<state> q;

    for (auto &j : afnd[0])
        for (auto &k : j.second) 
            q.push(k);

    while (!q.empty()) {
        state c = q.front(); q.pop();
        if (marked[c]) continue;

        marked[c] = true;
        for (auto &i : afnd[c]) {
            for (auto &j : i.second){
                q.push(j);
            }
        }
    }
    for (int i = 0; i < sid; i++){
        if (marked[i]) continue;
        afnd.erase(i);
        term.erase(i);
    }
}

bool searchTerminal(state s){
    bool marked[sid];
    for (auto &i : marked) i = false;
    marked[s] = true;

    queue<state> q;
    for (auto &j : afnd[s])
        for (auto &k : j.second) 
            q.push(k);

    while (!q.empty()) {
        state c = q.front(); q.pop(); 
        if (term.count(c)) return true;
        if (marked[c]) continue;
        marked[c] = true;
        for (auto &i : afnd[c]) {
            for (auto &j : i.second){
                q.push(j);
            }
        }
    }

    return false;
}

void removeDeadStates(){
    vector<state> ls;
    for (auto &st : afnd) if (!term.count(st.first) && !searchTerminal(st.first)) ls.push_back(st.first);
    for (auto &i : ls) afnd.erase(i);

    for (auto &i : afnd)
        for (auto &j : i.second)
            for (auto &k : ls)
                j.second.erase(k);
}


void loadAfndRule(){
    vector<string> inputStrings; 
    map<string,int> conv;
    string s;
    int counter = 0;
    for (auto &i : localState)
        conv[i] = counter++;

    while (getline(tokenFile,s) && !s.empty()) inputStrings.push_back(s);

    set<token> localUsedTokens;
    af afLocal;
    set<state> termLocal;
    int lsid = inputStrings.size();

    for (auto &line : inputStrings) {
        string rule = getRule(line); 
        vector<string> pipeSeparated = separateTransitionList(line);

        int r = conv[rule];
        pair<string, string> h;
        for (auto &i : pipeSeparated) { 
            h = separateTransition(i);
            if (h.first.empty()) {
                if (h.second == stopToken){
                    termLocal.insert(r);
                    continue;
                }
                localUsedTokens.insert(h.second);

                afLocal[r][h.second].insert(lsid);
                termLocal.insert(lsid);

                afLocal[lsid];

                lsid++;
                continue;
            }
            afLocal[r][h.first].insert(conv[h.second]);
            afLocal[conv[h.second]];
            localUsedTokens.insert(h.first);
        }
    }
    
    loadLocal(termLocal, afLocal,localUsedTokens);
}

void convertAfndToAfd(){
    
    map<int,int> conv;
    for(auto &i : afnd){
        conv[i.first] = afdSid; 
        afd[afdSid++];
    }

    for (auto &i : afnd) {
        for (auto &j : i.second) {
            if (afnd[i.first][j.first].empty()) continue; 

            afdUsedTokens.insert(j.first);            
            for (auto &k : j.second) {
                afd[conv[i.first]][j.first].insert(conv[k]);
            }
        }
    }
    for (auto &i : term) afdTerm.insert(conv[i]); 

    afd[afdSid];
    afdTerm.insert(afdSid);

    for (auto &i : afd) {
        for (auto &k: afdUsedTokens) {
            if(afd[i.first][k].empty())
                afd[i.first][k].insert(afdSid);
        }
    }
    error = afdSid++;
}

void printAfd(){
    cout << "# ";
    for (auto &i : afdUsedTokens) 
        (i == stopToken)? cout << setw(5) << i :cout << setw(4) << i; 
    cout << endl;
    for (auto &i : afdTerm) cout << i << " "; cout << endl;

    for (auto &i : afd) {
        string w = (i.first == error)? "&" :localState[i.first];
        if(afdTerm.count(i.first)) 
            cout << w << "*";
        else 
            cout << w << " ";

        for (auto &j : i.second)
            for (auto &k : j.second){ 
                (k == error)? cout << setw(4) << "-" :cout << setw(4) << localState[k];
            }
        cout << endl;
    }
}

void printAfnd(){
    cout << "# ";
    for (auto i : afdUsedTokens) 
        cout << setw(4) << i; 
    cout << endl;

    for (auto i : afnd) {
        auto w = i.first;
        if(term.count(i.first)) 
            cout << w << "*";
        else 
            cout << w << " ";

        for (auto &j : i.second)
            for (auto &k : j.second){ 
                cout << setw(4) << k;
            }
        cout << endl;
    }
}


int main (int argc, char *argv[]){


    loadAfndWord();
    loadAfndRule();
    loadAfndRule();
    defineStates();
    removeUnreachable();
    removeDeadStates();
    convertAfndToAfd();

    string nums[] = {"1","2","3","4","5","6","7","8","9","0","1.0","1.1","1.2","1.3","1.4","1.5","1.6","1.7","1.8","1.9","0.0"};

    string aux,s,conv;
    tokenFile.clear();
    tokenFile.seekg(0);
    while (getline(tokenFile,s) && !s.empty()) {
            int st = 0;
            for (auto i : s) {
                conv = i;
                st = *(afd[st][conv].begin());
            }
        wordMap[st] = s;
    } 
    
    for(auto f: nums) {
            int st = 0;
            for (auto i : f) {
                conv = i;
                st = *(afd[st][conv].begin());
            }
        wordMap[st] = "#Num";
    } 


    wordMap[error] = "#Error";
    

    for (auto i : afdTerm) 
        if (wordMap[i].empty())
            wordMap[i] = "#str";


    for (auto i : wordMap) tapeMapFile << i.first << " " << i.second << endl;

    vector <pair<int, string>> tape;
    int line = 0;
    while (codeFile.peek() != EOF) {
        getline(codeFile,aux);
        line++;
        stringstream str(aux);
        while (str >> s) {
            int st = 0;
            for (auto i : s) {
                conv = i;
                st = *(afd[st][conv].begin());
                if(st == error) break;
            }
            if (st == error) {
                lexerErrorFile << "line " << line << " " << s << "\n";
            }
            tapeFile << st << " " << s << " " << wordMap[st] <<  endl;
        } 
    }



    return 0;
}




