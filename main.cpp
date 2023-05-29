#include <iostream>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <assert.h>
#include <vector>
#include <set>
using namespace std;

unordered_map<string, string> parseStatement(string input);
string toLowerCase(string input);
string toUpperCase(string input);
string removeEdgeSpaces(string input);
string getTableName(unordered_map<string, string> statementComponents);
void parseColumnTable(string tableName);
void parseColumnsTableLine(string line, string tableName);
void parseDataTable(string tableName);
void parseDataTableLine(string line);
void applyWhereCondtion(string column, string op, string ref);
void parseWhereCondtion(unordered_map<string, string> statementComponents);
void parseSelectCondition(unordered_map<string, string> statementComponents);
void parseOrderByCondition(unordered_map<string, string> statementComponents);
void parseOrderByLine(string orderAttribute, int attributeNumber);
bool compare(unordered_map<string, string> x, unordered_map<string, string> y);
bool isEmpty(string input);
void applyOrder();
//create hashmap
unordered_map<int, vector<string>> columnsTable1;
unordered_map<string, vector<string>> columnsTable2;
unordered_map<int, vector<string>> orderByTable;
int numColumns = 0;
int numOrderByConditions = 0;

vector<unordered_map<string, string>> dataTable;


int main() {

    //get initial input
    string input;
    cout << "dbms> ";
    getline(cin, input);        //fixme make sure this accounts for \n and \r\n
    while(toLowerCase(input) != "exit"){
        columnsTable1.clear();
        columnsTable2.clear();
        dataTable.clear();
        orderByTable.clear();
        numColumns = 0;
        unordered_map<string, string> statementComponents = parseStatement(input);

        //determine table name then import data into hashmap in format number, [dno, int]
        //keep count of the number of columns
        //import file into hashmap


        //parse columns table
        string tableName = getTableName(statementComponents);
        parseColumnTable(tableName);

        //parse data table
        parseDataTable(tableName);

        //apply where condition
        parseWhereCondtion(statementComponents);

        //apply orderby
        parseOrderByCondition(statementComponents);

        //apply select
        parseSelectCondition(statementComponents);

        cout << "dbms> ";
        getline(cin, input);        //fixme make sure this accounts for \n and \r\n
    }
    return 0;
}

string toLowerCase(string input){
    int i;
    for(i = 0; i < input.size(); i++){
        transform(input.begin(),input.end(),input.begin(), ::tolower);
    }
    return input;
}

string toUpperCase(string input){
    int i;
    for(i = 0; i < input.size(); i++){
        transform(input.begin(),input.end(),input.begin(), ::toupper);
    }
    return input;
}
string removeSemicolon(string input){
    input.erase(remove(input.begin(),input.end(),';'),input.end()); //fixme what is input.end for?
    return input;
}

unordered_map<string, string> parseStatement(string input){
    //create hashmap
    unordered_map<string, string> statementComponents;
    //check if empty

    //change input to lowercase
    string newInput = removeSemicolon(input);

    //fixme remove semicolons

    //initial read in stuff
    string currentWord = "";
    stringstream stringStream;
    stringStream << newInput;

    //parse select part
    string selectComponent = "";

    while(stringStream >> currentWord && toLowerCase(currentWord) != "from"){
        if(toLowerCase(currentWord) != "select"){
            selectComponent.append(toLowerCase(currentWord));
        }
    }
    statementComponents["select"] = removeEdgeSpaces(selectComponent);

    //parse from part
    string fromComponent = "";
    while(stringStream >> currentWord && toLowerCase(currentWord) != "where" && toLowerCase(currentWord) != "orderby"){       //fixme make sure this stops at the appropriate place if where is last component
        if(toLowerCase(currentWord) != "from"){
            fromComponent.append(toLowerCase(currentWord));
        }
    }
    statementComponents["from"] = removeEdgeSpaces(fromComponent);

    //parse where part
    string whereComponent = "";
    while(toLowerCase(currentWord) != "orderby" && stringStream >> currentWord && toLowerCase(currentWord) != "orderby"){       //fixme make sure this stops at the appropriate place if where is last component
        if(toLowerCase(currentWord) != "where"){
            whereComponent.append(currentWord);
        }
    }
    statementComponents["where"] = removeEdgeSpaces(whereComponent);

    //parse order part
    string orderComponent = "";
    while(stringStream >> currentWord){
        if(toLowerCase(currentWord) != "orderby"){
            orderComponent.append(" "+ toLowerCase(currentWord));
        }
    }
    statementComponents["orderby"] = removeEdgeSpaces(orderComponent);
    return statementComponents;
}

string getTableName(unordered_map<string, string> statementComponents){
    string fromComponent = statementComponents["from"];
    return toLowerCase(fromComponent);
}

void parseColumnTable(string tableName) {
    ifstream input;
    input.open("TAB_COLUMNS.csv", ios::in);
    string currentLine = "";

    while(getline(input,currentLine)){
        parseColumnsTableLine(currentLine,tableName);
    }
}

string removeEdgeSpaces(string input){
    int startIndex = input.find_first_not_of(" \t"); //fixme is this delimeter ok?
    int endIndex = input.find_last_not_of(" \t"); //fixme is this delimeter ok?
    if(startIndex == -1 && endIndex == -1){ //empty attribute //fixme does this work?
        return input;
    }
    return input.substr(startIndex,endIndex-startIndex + 1);
}
bool isEmpty(string input){
    int startIndex = input.find_first_not_of(" \t"); //fixme is this delimeter ok?
    int endIndex = input.find_last_not_of(" \t"); //fixme is this delimeter ok?
    if(startIndex == -1 && endIndex == -1){ //empty attribute //fixme does this work?
        return true;
    }
    return false;
}
void parseColumnsTableLine(string line, string tableName){
    int columnNumber = 0;
    string currentWord = "";
    string currentTable = "";
    string currentAttributeName = "";
    string currentType = "";
    string currentAttributeNumber = "";
    stringstream currentLineStream;

    currentLineStream << line;
    while(getline(currentLineStream,currentWord,',')){
        if(columnNumber == 0){
            currentTable = toLowerCase(removeEdgeSpaces(currentWord));
        }
        else if(columnNumber == 1){
            currentAttributeName = toLowerCase(removeEdgeSpaces(currentWord));
        }
        else if(columnNumber == 2){
            currentType = toLowerCase(removeEdgeSpaces(currentWord));
        }
        else{
            currentAttributeNumber = toLowerCase(removeEdgeSpaces(currentWord));
            if(currentTable == tableName){
                if(stoi(currentAttributeNumber) > numColumns){      //only update number if for relevant table
                    numColumns = stoi(currentAttributeNumber);
                }

                columnsTable1[stoi(currentAttributeNumber)] = vector<string>();
                columnsTable1[stoi(currentAttributeNumber)].push_back(currentTable);
                columnsTable1[stoi(currentAttributeNumber)].push_back(currentAttributeName);
                columnsTable1[stoi(currentAttributeNumber)].push_back(currentType);

                columnsTable2[currentAttributeName] = vector<string>();
                columnsTable2[currentAttributeName].push_back(currentTable);
                columnsTable2[currentAttributeName].push_back(currentAttributeNumber);
                columnsTable2[currentAttributeName].push_back(currentType);
            }
        }
        columnNumber = (columnNumber + 1) % 4;
    }
}

void parseDataTable(string tableName){
    ifstream input;
    input.open(toUpperCase(tableName) + ".csv", ios::in);
    string currentLine = "";
    while(getline(input,currentLine)){
        parseDataTableLine(currentLine);
    }
}

void parseDataTableLine(string line){
    string currentWord = "";
    int currentAttributeNumber = 1;
    string currentAttributeName = "";
    stringstream currentLineStream;

    unordered_map<string, string> dataLine;

    currentLineStream << line;
    while(getline(currentLineStream,currentWord,',')) {
        currentAttributeName = columnsTable1[currentAttributeNumber].at(1);
        dataLine[currentAttributeName] = removeEdgeSpaces(currentWord);                     //fixme (may have issue elsewhere) make sure returning by value not by pointer?
        currentAttributeNumber++;
    }
    dataTable.push_back(dataLine);
}

void parseWhereCondtion(unordered_map<string, string> statementComponents){
    string where = statementComponents["where"];
    if(where == ""){
        return;
    }

    string column;
    string op;
    string referenceValue;
    bool operatorRetrieved = false;
    int opSize = 0;

    //parse
    for(int i = 0; i < where.size(); i++){
        if(where[i] == ','){
            if(column != ""){
                applyWhereCondtion(toLowerCase(column),op,referenceValue);
            }
            column = "";
            op = "";
            referenceValue = "";
            operatorRetrieved = false;
            opSize = 0;
            continue;
        }
        if(where[i] == '=' || where[i] == '<' ||where[i] == '>'){
            op.push_back(where[i]);
            opSize++;
            continue; //don't want to add op to column
        }
        if(opSize == 2 || ((where[i] != '=' && where[i] != '<' && where[i] != '>') && opSize == 1)){
            operatorRetrieved = true;
        }
        if(operatorRetrieved == false){
            column.push_back(where[i]);
        }
        else{
            referenceValue.push_back(where[i]);
        }
    }
    if(column != ""){
        applyWhereCondtion(toLowerCase(column),op,referenceValue);
    }
}


void applyWhereCondtion(string column, string op, string ref){
    set<int> deleteIndexes;
    string columnType = columnsTable2[column].at(2);
    for(int i = 0; i < dataTable.size(); i++){
        if(op == "="){
            if(columnType == "char"){
                if(dataTable[i][column][0] != ref[0]){
                    deleteIndexes.insert(i);
                }
            }
            else if(columnType == "int" || columnType == "float"){
                if(isEmpty(dataTable[i][column])){
                    deleteIndexes.insert(i);
                }
                else if(stof(dataTable[i][column]) != stof(ref)){
                    deleteIndexes.insert(i);
                }
            }
            else{
                if(dataTable[i][column] != ref){
                    deleteIndexes.insert(i);
                }
            } //string
        }
        else if(op == "<>"){
            if(columnType == "char"){
                if(dataTable[i][column][0] == ref[0]){
                    deleteIndexes.insert(i);
                }
            }
            else if(columnType == "int" || columnType == "float"){
                if(isEmpty(dataTable[i][column])){
                    deleteIndexes.insert(i);
                }
                else if(stof(dataTable[i][column]) == stof(ref)){
                    deleteIndexes.insert(i);
                }
            }
            else{
                if(dataTable[i][column] == ref){
                    deleteIndexes.insert(i);
                }
            } //string
        }
        else if(op == ">="){
            if(columnType == "char"){
                if(dataTable[i][column][0] < ref[0]){
                    deleteIndexes.insert(i);
                }
            }
            else if(columnType == "int" || columnType == "float"){
                if(isEmpty(dataTable[i][column])){
                    deleteIndexes.insert(i);
                }
                else if(stof(dataTable[i][column]) < stof(ref)){
                    deleteIndexes.insert(i);
                }
            }
            else{
                if(dataTable[i][column] < ref){
                    deleteIndexes.insert(i);
                }
            } //string
        }
        else if(op == "<="){
            if(columnType == "char"){
                if(dataTable[i][column][0] > ref[0]){
                    deleteIndexes.insert(i);
                }
            }
            else if(columnType == "int" || columnType == "float"){
                if(isEmpty(dataTable[i][column])){
                    deleteIndexes.insert(i);
                }
                else if(stof(dataTable[i][column]) > stof(ref)){
                    deleteIndexes.insert(i);
                }
            }
            else{
                if(dataTable[i][column] > ref){
                    deleteIndexes.insert(i);
                }
            } //string
        }
    }

    //delete vectors that don't match

    set<int>::reverse_iterator r;
    for(r = deleteIndexes.rbegin(); r != deleteIndexes.rend(); r++){
        dataTable.erase(dataTable.begin()+ *r);
    }
    string hello;
}

void parseSelectCondition(unordered_map<string, string> statementComponents){
    stringstream ss;
    vector<string> attributeList;
    string currentWord;
    ss << statementComponents["select"];
    while(getline(ss,currentWord,',')){
        attributeList.push_back(currentWord);
    }

    if(attributeList.at(0) == "*"){
        //print header of attributes
        for(int k = 0; k < numColumns; k++){
            if(k < numColumns -1){
                cout << toUpperCase(columnsTable1[k+1].at(1)) << ",";
            }
            else{
                cout << toUpperCase(columnsTable1[k+1].at(1)) << endl;
            }
        }
        for(int j = 0; j < dataTable.size(); j++){
            for(int i = 0; i < numColumns; i++){
                if(i < numColumns - 1){
                    cout << dataTable.at(j)[columnsTable1[i+1].at(1)] << ",";
                }
                else{
                    cout << dataTable.at(j)[columnsTable1[i+1].at(1)] << endl;
                }
            }
        }
    }
    else{
        //print header of attributes
        for(int k = 0; k < attributeList.size(); k++){
            if(k < attributeList.size() -1){
                cout << toUpperCase(attributeList.at(k)) << ",";
            }
            else{
                cout << toUpperCase(attributeList.at(k)) << endl;
            }
        }
        for(int j = 0; j < dataTable.size(); j++){
            for(int i = 0; i < attributeList.size(); i++){
                if(i < attributeList.size() - 1){
                    cout << dataTable.at(j)[attributeList.at(i)] << ",";
                }
                else{
                    cout << dataTable.at(j)[attributeList.at(i)] << endl;
                }
            }
        }
    }
}

void parseOrderByCondition(unordered_map<string, string> statementComponents){
    string orderBy = statementComponents["orderby"];
    if(orderBy == ""){
        return;                     //FIXME make sure this is always ""
    }
    int attributeNumber = 0;
    stringstream ss;
    ss << orderBy;
    string currentOrderAttribute;

    while(getline(ss,currentOrderAttribute,',')){
        parseOrderByLine(currentOrderAttribute, attributeNumber);
        attributeNumber++;
    }
    numOrderByConditions = attributeNumber; //fixme make sure this works ok with loop below
    //apply order
    applyOrder();
}

void parseOrderByLine(string orderAttribute, int attributeNumber){
    string column;
    string ascOrDesc;
    stringstream ss;
    string currentWord;

    ss << removeEdgeSpaces(orderAttribute);     //remove space after comma

    ss  >> column;
    ss  >> ascOrDesc;

    orderByTable[attributeNumber] = vector<string>();
    orderByTable[attributeNumber].push_back(column);
    orderByTable[attributeNumber].push_back(toLowerCase(ascOrDesc));
}

void applyOrder(){
    sort(dataTable.begin(),dataTable.end(),compare);
}

//make template here


bool compare(unordered_map<string, string> x, unordered_map<string, string> y){
    string columnType;  //assuming the duplicate tuples here
    string columnName;
    for(int i = 0; i < numOrderByConditions; i++){
        columnName = orderByTable[i].at(0);
        columnType = columnsTable2[columnName].at(2);
        if(columnType == "char"){
            if(x[columnName][0] != y[columnName][0]){
                if(orderByTable[i].at(1) == "asc") {
                    return x[columnName][0] < y[columnName][0];
                }
                return x[columnName][0] > y[columnName][0];
            }
            continue;   //go to next ordering attribute
        }
        else if((columnType == "int" || columnType == "float") && !isEmpty(x[columnName]) && !isEmpty(y[columnName])){
            if(stof(x[columnName]) != stof(y[columnName])){
                if(orderByTable[i].at(1) == "asc") {
                    return stof(x[columnName]) < stof(y[columnName]);
                }
                return stof(x[columnName]) > stof(y[columnName]);
            }
            continue;
        }
        else{
            if(x[columnName] != y[columnName]){
                if(orderByTable[i].at(1) == "asc") {
                    if(isEmpty(x[columnName]) || isEmpty(y[columnName])){
                        return x[columnName] > y[columnName];
                    }
                    return x[columnName] < y[columnName];
                }
                if(isEmpty(x[columnName]) || isEmpty(y[columnName])){
                    return x[columnName] < y[columnName];
                }
                return x[columnName] > y[columnName];
            }
            continue;
        }
    }
    return false;   //fixme these are equal?
}
