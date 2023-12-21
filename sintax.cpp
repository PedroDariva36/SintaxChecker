#include <bits/stdc++.h>
#include <fstream>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

#define MSIZE 200

using namespace std;



ifstream sTape("sintaxTape.txt");

string acT[] = {"e", "s", "r", "g", "ac"};
vector<string> itot;
map<string,int> ttoi;
vector<pair<int,int>> prodSize (MSIZE);
pair<int, int> sTable[99][48];
queue<int> inTape;
stack<int> inStack;

int main (int argc, char *argv[]){
    int n, x, y, z, k, l;
    string s;
    sTape >> n;
    while (n--){
        sTape >> x >> s;
        itot.push_back(s);
        ttoi[s] = x;
    }
    
    /*
    for(int i = 0; i < itot.size(); i++){
        cout << i << " " << itot[i] << endl; 
    }
    cout << endl;
    for(auto i: ttoi){
        cout << i.first << " " << i.second << endl;
    }
    */
    
    sTape >> n;
    while (n--){
        sTape >> x >> y >> z;
        prodSize[x] = {y,z};
    }
    
    /*
    for(int i = 0; i < prodSize.size(); i++){
        if(prodSize[i] != 0)
            cout << i << " " << prodSize[i] << endl; 
    }
    */
    
    sTape >> n;
    while (n--){
        sTape >> l >> k;
        while(k--){
            sTape >> x >> y >> z;
            sTable[l][x] = {y,z};
        }
    }
    
    /*
    for (auto i : itot) {
        cout << setw(2) << i;
    }
    cout << endl;
    
    
    for(int i = 0; i < 100; i++){
        for(int j = 0; j < 100; j++){

            if(sTable[i][j].first){
                cout << setw(2) << acT[sTable[i][j].first] << "|" << sTable[i][j].first;
            }
            else cout << setw(2) << "_";
        }
        cout << endl;
    }
    */
    
    for (int i = 0; i < 47; i++) {
        cout << itot[i] << "  ";
    }
    cout << endl;
    for (auto i : sTable[1]) {
        cout << acT[i.first] << i.second << " ";
    }
    cout << endl;

    
    inStack.push(0);
    //inTape.push(5); 
    inTape.push(5); 
    inTape.push(0);
    
    cout << sTable[0][5].first << " " << sTable[0][5].second;
    
    cout << endl;
    
    
    
    while (true) {
        auto action = sTable[inStack.top()][inTape.front()];

        
        if (action.first == 1) {
            

            cout << acT[action.first] << action.second << " Stacked : " << itot[inTape.front()]  << endl;

            


            inStack.push(inTape.front());
            inTape.pop();
            inStack.push(action.second);
            
            auto ps = inStack;
            auto pt = inTape;

            cout << "Stack: ";
            while(!ps.empty()){
                cout << ps.top() << " ";
                ps.pop();
            }
            cout << endl;
            
            cout << "Tape: ";
            while(!pt.empty()){
                cout << pt.front() << " ";
                pt.pop();
            }
            cout << endl;


        }

        else if (action.first == 2) {

            int aux = prodSize[action.second].first * 2;
            while (aux--) inStack.pop();
            
            aux = sTable[inStack.top()][prodSize[action.second].second].second;
            inStack.push(prodSize[action.second].second);
            inStack.push(aux);
            

            cout << acT[action.first] << action.second << " Reduction: " << prodSize[action.second].second << " " << aux << endl;
            auto ps = inStack;
            auto pt = inTape;
            cout << "Stack: ";
            while(!ps.empty()){
                cout << ps.top() << " ";
                ps.pop();
            }
            cout << endl;
            
            cout << "Tape: ";
            while(!pt.empty()){
                cout << pt.front() << " ";
                pt.pop();
            }
            cout << endl;



        }
        else if(action.first == 4) {
            cout << "Aceito" << endl;
            break;
        }


        else{
            cout << "error" << endl;
            cout << action.first << " " << action.second << endl;
            break;
        }
    }


    return 0;
}
