#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <utility>

using namespace std;

string convertToLowerCase(string str) {
    for (int x=0; x<str.length(); x++){
        str[x]=tolower(str[x]);
    }

    return str;
}

struct FlowLog {
    int version;
    string accountID;
    string networkInterfaceID;
    string sourceIP;
    string destinationIP;
    int destinationPort;
    int sourcePort;
    int protocol;
    int bytesSent;
    int bytesReceived;
    long startTime;
    long endTime;
    string action;
    string logStatus;
};

int main(){

    map<int, string> mp;
    mp[1] = "icmp";
    mp[2] = "igmp";
    mp[6] = "tcp";
    mp[17] = "udp";
    mp[41] = "ipv6 encapsulation";
    mp[50] = "esp";
    mp[51] = "ah";
    mp[89] = "ospf";
    mp[132] = "sctp";

    ifstream flowLogFile("sample.txt"); 
    string line;

    vector<FlowLog> logs;

    while (getline(flowLogFile, line)) {
        istringstream stream(line);
        FlowLog log;

       
        if (!(stream >> log.version >> log.accountID >> log.networkInterfaceID >> log.sourceIP
                  >> log.destinationIP >> log.destinationPort >> log.sourcePort
                  >> log.protocol >> log.bytesSent >> log.bytesReceived
                  >> log.startTime >> log.endTime >> log.action >> log.logStatus)) {
            continue; 
        }

        logs.push_back(log);
    }

    flowLogFile.close();

    map<pair<int, string>, string> protocolMap;
    
    ifstream lookup("lookup.csv");
    
    string csvline;

    getline(lookup, csvline);

    while (getline(lookup, csvline)) {
        stringstream ss(csvline);
        int dstport;
        string row;

        string protocol, tag;

        if (getline(ss, row, ',')) {
            cout<<row<<endl;
            dstport = stoi(row);
        }

    
        if (getline(ss, row, ',')) {
            cout<<row<<endl;
            protocol = row;
        }

        if (getline(ss, row)) {
            cout<<row<<endl;
            tag = row;
        }
        pair<int, string> p;
        p.first = dstport;
        p.second = convertToLowerCase(protocol);
        protocolMap[p] = tag;
        cout<<p.first<<" "<<p.second<<" "<<protocolMap[p]<<endl;
    }

    lookup.close();

    map<string,int> tagCount;
    map<pair<int, string>, int> portProtocolCount;
    string untagged="untagged";

    int n = logs.size();
    for(int i=0;i<n;i++){
        pair<int, string> p;
        p.first = logs[i].destinationPort;
        p.second = mp[logs[i].protocol];
        cout<<i<<" "<<p.first<<" "<<p.second<<endl;
        if(protocolMap.find(p) != protocolMap.end()){
            tagCount[protocolMap[p]]++;
        }
        else tagCount[untagged]++;

        portProtocolCount[p]++;
    }

    ofstream output("output.txt");

    output << "Tag Counts: \n";
    output << "Tag,Count \n";
    for (const auto& it : tagCount) {
        output << it.first << "," << it.second << "\n";
    }

    output << "\nPort/Protocol Combination Counts: \n";
    output << "Port,Protocol,Count \n";
    for (const auto& it : portProtocolCount) {
        output << it.first.first << "," << it.first.second << "," << it.second << "\n";
    }

    output.close();

    
}