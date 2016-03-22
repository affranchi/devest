#include <unordered_map>
#include <iostream>
#include <vector>
#include <deque>
using namespace std;

// Define type long long int.
typedef long long int ll;

// Define DATA query types
enum QueryType {
    SET,
    GET,
    UNSET,
    NUMEQUALTO,
    INVALID
};

// Query struct
//      Data query type, variable name, data for the variable
struct Query {
    Query() {
        type = QueryType::INVALID;
    }
    QueryType type;
    string name;
    string data;
};

// Parse class
//      Parse query and return Query struct.
class QueryParser {

public:
    // Tokenize string for parsing
    static deque<string> tokenize(string& query) {
        string word;
        deque<string> words;
        for (auto ch : query) {
            if (ch != ' ') {
                word += ch;
            }
            else {
                words.push_back(word);
                word.clear();
            }
        }
        if (!word.empty())
            words.push_back(word);
        return words;
    }

    // Parsing tokens
    static Query parse(string& query) {
        Query result;
        deque<string> tokens = QueryParser::tokenize(query);

        // If no tokens, return with INVALID query type.
        if (tokens.size() == 0) {
            result.type = QueryType::INVALID;
            return result;
        }

        if (tokens[0] == "SET") {                   // If SET query,
            if (tokens.size() == 3) {               //   Need 3 tokens.
                result.type = QueryType::SET;       //      SET query type
                result.name = tokens[1];            //      Variable name
                result.data = tokens[2];            //      Data
            } else {                                //   If wrong token numbers
                result.type = QueryType::INVALID;   //      INVALID query type return
            }
        } else if (tokens[0] == "GET") {            // If GET query,
            if (tokens.size() == 2) {               //   Need 2 tokens.
                result.type = QueryType::GET;       //      GET query type
                result.name = tokens[1];            //      Variable name
            } else {                                //   If wrong token numbers
                result.type = QueryType::INVALID;   //      INVALID query type return
            }
        } else if (tokens[0] == "UNSET") {          // If UNSET query,
            if (tokens.size() == 2) {               //   Need 2 tokens.
                result.type = QueryType::UNSET;     //      UNSET query type
                result.name = tokens[1];            //      Variable name
            } else {                                //   If wrong token numbers
                result.type = QueryType::INVALID;   //      INVALID query type return
            }
        } else if (tokens[0] == "NUMEQUALTO") {       // If NUMEQUALTO query,
            if (tokens.size() == 2) {                 //   Need 2 tokens.
                result.type  = QueryType::NUMEQUALTO; //      NUMEQUALTO query type
                result.data = tokens[1];              //      Variable name
            } else {                                  //   If wrong token numbers
                result.type = QueryType::INVALID;     //      INVALID query type return
            }
        } else {                                    // If query type not matched
            result.type = QueryType::INVALID;       //      INVALID query type return
        }

        return result;
    }

};

// Data class
//  Query execute
//      Insert/Update/unset data if query needs to insert/update data
//      Print data               if query needs data retrieve.
//      Print the number of data if query needs the number of data

class Data {
public:
    // Execute query function
    void execute(Query& query) {
        switch (query.type) {
            case QueryType::SET :                           // If query is SET
                setData(query.name, query.data);            //  Set data associated with variable
                break;
            case QueryType::GET :                           // If query is GET
                cout << getData(query.name) << endl;        //  Print data associated with variable
                break;
            case QueryType::UNSET :                         // If query is UNSET
                unsetData(query.name);                      //  Unset data associated with variable
                break;
            case QueryType::NUMEQUALTO :                    // If query is NUMEQUALTO
                cout << getNumData(query.data) << endl;     //  Print the number of data
                break;
            default:
                return;
        }
    }

    // Get data from database
    string getData(string& name) {
        if (mData.find(name) != mData.end())                //  If data associated with the variable exists
            return mData[name];                             //      return data
        return "NULL";                                      //  Otherwise, return "NULL"
    }
private:
    // Set data for the variable
    void setData(string& name, string& data) {

        if (mNumData.find(mData[name]) != mNumData.end()) { // If the previous data for the name was existed
            mNumData[mData[name]]--;                        //      Decrease the number of previous data for the name
        }

        if (mNumData.find(data) != mNumData.end()) {        // If the count for the new data exists,
            mNumData[data]++;                               //      Increase the number of data
        }
        else {                                              // If the count for the new data doesn't exist
            mNumData[data] = 1;                             //      Initialize the number of data
        }
        mData[name] = data;                                 // Set data for the variable
    }

    // Unset(remove) data for the variable
    void unsetData(string& name) {
        unordered_map<string, string>::const_iterator it = mData.find(name);
        string data;
        if (it != mData.end()) {    // If the data for the variable exist
            data = it->second;      //      Get the data
            mNumData[data]--;       //      Decrease the count for the data
            mData.erase(it);        //      Remove the data for the variable
        } // else do nothing.
    }
    int getNumData(string& data) {
        if (mNumData.find(data) != mNumData.end())  // If the data exists,
            return mNumData[data];                  //      return the count for the data
        return 0;                                   // Otherwise, return 0
    }

    unordered_map<string,string> mData; // Container for data
    unordered_map<string,ll> mNumData;  // Container the count for data
};

// Transaction class
//      Handling transaction
//      BEGIN Transaction
//          Begin transaction
//      COMMIT TRANSACTION
//          Make all transaction permanent
//      ROLLBACK TRANSACTION
//          Restore database before executing the most recent transaction
class Transaction {

public:
    Transaction() {
        mActive = false; // Trasanction disabled by default
    }

    // Start transaction
    void begin() {
        mActive = true;                  // Mark it's started
        Query query;                     // Query with INVALID querytype is 
        query.type = QueryType::INVALID; // rollback point mark for the new transaction.
        queries.push_back(query);        // Push to the transaction to mark the new transaction.
    }

    // Commit all queries.
    //      Changes on database by all transactions
    //      would be premanent.
    void commit() {
        if (hasTransaction()) {                 // If some transactions exist,
            queries.clear();                    //      all queries would be permanent.
        } else {                                // Otherwise,
            cout << "NO TRANSACTION" << endl;   //      Show "NO TRANSACTION"
        }
        mActive = false;                        // Transaction cancelled
    }

    // Rollback all queries in the most recent transaction
    //      Revert all changes by the most recent transaction
    //      by running revert quries in the most recent transaction
    void rollback(Data& data) {
        if (hasTransaction()) {                             // If there are some transactions running,
            while (queries.size() > 0) {                    //      Running when the queries exist to be executed in the transaction
                Query current = queries.back();             //      Get the most recent query in the transaction
                queries.pop_back();                         //      Remove the query first.
                if (current.type == QueryType::INVALID) {   //      If it's transaction start point indicator,
                    break;                                  //          Stop rollback since we have processed all queries in the most current transacion.
                }
                data.execute(current);                      //      Otherwise execute roll back query to revert the change by the query in the transaction.
            }
            if (!hasTransaction())                          //      After rollback the most recent transaction,
                mActive = false;                            //      Mark no transaction if no transaction left.
        } else {
            cout << "NO TRANSACTION" << endl;               // If no transaction, show "NO TRANSACTION"
        }
    }

    // Execute query in the transaction
    //      Execute all queries.
    //      For SET/UNSET queries:
    //          Generate roll back query for reverting the change by the current query.
    //          Insert transaction for rollback
    void execute(Data& data, Query query) {
        // Generate rollback query
        Query rollbackQuery;
        string tempData = data.getData(query.name);         //  Get the data for the variable
        switch (query.type) {
            case QueryType::SET :                           //  If SET
                if (tempData != "NULL") {                   //      If the data for the variable exists
                    rollbackQuery.type = QueryType::SET;    //          Generate rollback query with SET
                    rollbackQuery.name = query.name;        //          Variable name
                    rollbackQuery.data = tempData;          //          Previous data for rollback
                } else {                                    //      If the data doesn't exist
                    rollbackQuery.type = QueryType::UNSET;  //          Generate rollback query with UNSET
                    rollbackQuery.name = query.name;        //          Variable name
                }
                queries.push_back(rollbackQuery);           //  Push the query into the transaction
                break;
            case QueryType::UNSET :                         //  If UNSET
                if (tempData != "NULL") {                   //      If the data for the variable exists
                    rollbackQuery.type = QueryType::SET;    //          Generate rollback query with SET
                    rollbackQuery.name = query.name;        //          Variable name
                    rollbackQuery.data = tempData;          //          Previous data for rollback
                }
                // else nothing to do
                queries.push_back(rollbackQuery);           //  Push the query into the transaction
                break;
            // Other queries would be ignored since it doesn't affect data.
        }
        data.execute(query);
    }

    // Return transaction running or not
    bool isActive() {
        return mActive;
    }

private:
    // Check if transaction has something to run
    bool hasTransaction() {
        return !queries.empty();
    }
    bool mActive;
    deque<Query> queries;
};

int main() 
{
    std::string queryStr;
    Transaction transaction;
    Data data;
    while (true) {
        getline(cin, queryStr);
        if (queryStr == "END") {             // If query is "END",
            break;                           //     Stop running
        }
        else if (queryStr == "BEGIN") {      // If query is "BEGIN"
            transaction.begin();             //     Start transaction
        }
        else if (queryStr == "COMMIT") {     // If query is "COMMIT"
            transaction.commit();            //     Commmit all quaries
        }
        else if (queryStr == "ROLLBACK") {   // If query is "ROLLBACK"
            transaction.rollback(data);      //     Rollback queries
        } else {                                                // Data Query handling 
            Query query = QueryParser::parse(queryStr);
            if (query.type != INVALID) {                        // Ignore invalid query
                if (transaction.isActive()) {                   // IF it's in transaction,
                    transaction.execute(data, query);           //      Execute query in transaction
                } else {                                        // ELSE
                    data.execute(query);                        //      Exectue query
                }
            } // else nothing to do for invalid query
        }
    }
    return 0;
}