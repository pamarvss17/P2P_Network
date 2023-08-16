#include<bits/stdc++.h>
#include<dirent.h>
#include<sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include<filesystem>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

using namespace std;


int next(int arr[], int target, int end)
{
    int start = 0;
    int ans = -1;
    while (start <= end)
    {
        int mid = (start + end) / 2;
        if (arr[mid] <= target)
        {
            start = mid + 1;
        }
        else
        {
            ans = mid;
            end = mid - 1;
        }
    }
    if (ans == -1) {
        ans = end;
    }
    return ans;
}

bool check(int* a, int n)
{
    for (int i = 0; i < n; i++)
    {
        if(a[i] == 0 )
        {
            return true;
        }
    }

    return false;
    
}

bool searchword(string word, vector<string> words, int len) {
    for (int i = 0; i < len; i++) {
        if (word == words[i]) {
            return true;
        }
    }   
    return false;
}

vector<string> returnwords(string sentence)
{
    vector<string> words;
    string word;
    stringstream X(sentence);
    while (getline(X, word, ' '))
    {
        words.push_back(word);
    }
        return words;
}

int main(int argc, char* argv[])
{
    /*
        ---------------------- Parsing the arguments-----------------------------
    */

    if (argc != 3)
    {
        cout << "Usage: executable argument1-config-file argument2-directory-path"<< endl;
    }

    /*
        ---------------------- Reading the contents of the file and directory------------------
    */

    ifstream configfile(argv[1]);
    string line, words;
    int lineno = 1,clientid, clientportno,clientuniqueid,noofneighbour,nooffilestosearch;
    vector<string> filenamestosearch;
    vector<int> neighbourids, neighbourports;
    while(getline(configfile,line))
    {
        int wordcount = 0;
        stringstream X(line);
        while (getline(X, words, ' ')) 
        {
            if(lineno == 1)
            {
                if(wordcount == 0)
                {
                    clientid = stoi(words);
                }
                else if (wordcount == 1)
                {
                    clientportno = stoi(words);
                }
                else if (wordcount == 2)
                {
                    clientuniqueid = stoi(words);
                }
            }
            else if (lineno == 2)
            {
                noofneighbour = stoi(words);
            }
            else if (lineno == 3)
            {
                if(wordcount< 2*noofneighbour)
                {
                    if(wordcount%2 == 0)
                    {
                        neighbourids.push_back(stoi(words));
                    }
                    else
                    {
                        neighbourports.push_back(stoi(words));
                    }
                }
            }
            else if (lineno == 4)
            {
                nooffilestosearch = stoi(words);
            }
            else if (lineno >= 5)
            {

                words.erase(remove(words.begin(), words.end(), '\n'), words.end());
                words.erase(remove(words.begin(), words.end(), '\r'), words.end());
                words.erase(remove(words.begin(), words.end(), ' '), words.end());
                filenamestosearch.push_back(words);
            }
            wordcount += 1;
        }  
        lineno += 1;
    }
    configfile.close();


    /*
       ----- Printing the files the client contains -----
    */

    vector<string> filesindirectory;
    int nooffilesindir = 0;

    DIR *dr;
    struct dirent *en;
    struct stat s;
    dr = opendir(argv[2]);  //open all directory
    string path = argv[2];
    if (dr) 
    {
       while ((en = readdir(dr)) != NULL) 
       {    
            string name = en->d_name;
            name = path + '/' + name;
            if ( lstat(name.c_str(), &s) == 0)      // error checking 
            {
                if (!(S_ISDIR(s.st_mode)))  // Checking if the path is directory or not, if not directory, print the name 
                {
                    filesindirectory.push_back(en->d_name);
                    nooffilesindir++;
                }
            }
       }
       closedir(dr); //close all directory
    }

    /*
        ---------------  Establishing connections  ------------------
    */

    int nids[noofneighbour], nports[noofneighbour];
    copy(neighbourids.begin(), neighbourids.end(), nids);
    copy(neighbourports.begin(), neighbourports.end(), nports);

    string tmp = to_string(clientuniqueid);
    tmp = to_string(clientid) + " " + tmp;
    char const *num_char = tmp.c_str();
    // cout<< sizeof(num_char)<<endl;

    int checker[noofneighbour] = {0};

    int position = next(nids,clientid,noofneighbour);
    cout << "Position " << position << endl;
    int usingsocks[noofneighbour], valuesread[noofneighbour];
    struct sockaddr_in serv_addr[noofneighbour-position];

    char buffer[1024] = {0};
    char buffer1[1024] = {0};

    int sockfd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)		// server side of socket
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

    cout << "Neighbourids << ";
    for (auto it : neighbourids) {
        cout << it << " ";
    }
    cout << endl;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(clientportno);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

    if (listen(sockfd, 100) < 0)		// checking for listen error
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

    for (int i = position; i < noofneighbour; i++)
    {
        usingsocks[i] = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr[i-position].sin_family = AF_INET;
	    serv_addr[i-position].sin_port = htons(neighbourports[i]);
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr[i-position].sin_addr)<=0)
        {
		    printf("\nInvalid address/ Address not supported \n");
        }
    }
    vector<string> privateids;

    while (check(checker,noofneighbour))
    {
        for (int i = 0; i < noofneighbour; i++)
        {
            if(checker[i]==0)
            {
                if (i<position)
                {
                    int buf;
                    while (((buf = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0))
                    {
                        //perror("Accept failed");
                        continue;
                    }
                    send(buf ,  num_char, sizeof(num_char) , 0 );
                    valuesread[i] = recv( buf , buffer, 1024,0);
                    if (valuesread[i]>0)
                    {
                        int j = 0;
                        for (;j < noofneighbour; j++) 
                        {
                            if (neighbourids[j] == buffer[0]-'0') 
                            {
                                checker[j] = 1;
                                usingsocks[j] = buf;
                                cout << "Accepted connection with " << neighbourids[j] << endl;
                                string temp = "5";
                                send(usingsocks[j], tmp.c_str(), 1, 0); 
                            }
                        }
                        privateids.push_back(buffer);
                    }  
                }
                else
                {
                    while ((connect(usingsocks[i], (struct sockaddr *)&serv_addr[i-position], sizeof(serv_addr[i-position])) < 0))
	                {
                        //perror("Connect error");
                        //cout << neighbourids[i] << endl;
		                continue;
	                }
                    recv(usingsocks[i], buffer, 1024, 0);
                    send(usingsocks[i] , num_char , sizeof(num_char) , 0 );
                    valuesread[i] = recv(usingsocks[i] , buffer, 1024,0);		// server side msg reading
                    privateids.push_back(buffer);
                    if ((valuesread[i]>0) && (neighbourids[i] == buffer[0]-'0'))
                    {
                        checker[i]=1;
                    }            
                }
            }
        }
    }

    sort(filesindirectory.begin(), filesindirectory.end());
    sort(privateids.begin(), privateids.end());
    sort(filenamestosearch.begin(), filenamestosearch.end());

    for (auto& it : filesindirectory) 
    {
        cout << it << endl;
    }

    for (int i = 0; i < noofneighbour; i++)
    {
        vector<string> r = returnwords(privateids[i]);
        cout<< "Connected to "<<neighbourids[i]<<" with unique-ID "<<r[1]<<" on port "<< neighbourports[i] << endl;
    }

    //Starting phase 2
    int requestsent[noofneighbour] = {0};
    int requestreceived[noofneighbour] = {0};
    int preplysent[noofneighbour] = {0};
    int replysent[noofneighbour] = {0};
    int replyreceived[noofneighbour] = {0};

    int filesLocation[nooffilestosearch*noofneighbour] = {0};
    
    vector<string> replyfromneighbours;
    vector<string> replytoneighbours;

    for (int i = 0; i < noofneighbour; i++) {
        replyfromneighbours.push_back("");
        replytoneighbours.push_back("");
    }

    string query = "1 ";
    for (auto& it : filenamestosearch){
        query += it;
        query += " ";
    }
    //cout << query << endl;
    char const* query_as_c_str = query.c_str();

    //for (int i = 0; i < noofneighbour; i++) {
    //    valuesread[i] = 0;
    //}

    char bufferp2[1024] = {0};

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
            while (sentbytes != query.length()) {
                sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
            }
            int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
            if (bytesreceived > 0) {
                string temp = "";
                for (int j = 2; j < bytesreceived; j++) {
                    temp += bufferp2[j];
                }
                replyfromneighbours[i] = temp;
                cout << "Reply " << temp << " received from neighbour " << neighbourids[i] << endl;
            }
            string temp = "5";
            sentbytes = send(usingsocks[i], temp.c_str(), 1, 0);
            while (sentbytes != 1) {
                sentbytes = send(usingsocks[i], temp.c_str(), 1, 0);
            }
        }
        if (clientid > neighbourids[i]) {
            int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
            if (bytesreceived > 0) {
                cout << "Request received from neighbour " << neighbourids[i] << endl;
                string temp = "";
                vector<string> wordstosearch;
                int noofwords = 0;
                string result = "2 ";
                for (int j = 2; j < bytesreceived; j++) {
                    if (bufferp2[j] == ' ') {
                        wordstosearch.push_back(temp);
                        cout << "File " << temp << endl;
                        temp = "";
                        noofwords++;
                    }
                    else {
                        temp += bufferp2[j];
                    }
                }
                cout << "Should search the above words for " << neighbourids[i] << endl;
                for (int j = 0; j < noofwords; j++) {
                    if (searchword(wordstosearch[j], filesindirectory, nooffilesindir)) {
                        result += "1";
                    }
                    else {
                        result += "0";
                    }
                }
                int sentbytes = send(usingsocks[i], result.c_str(), noofwords+2, 0);
                while (sentbytes != noofwords+2) {
                    sentbytes = send(usingsocks[i], result.c_str(), noofwords+2, 0);
                }
            }
            int br = recv(usingsocks[i], bufferp2, 1024, 0);
            while (!(br > 0)) {
                br = recv(usingsocks[i], bufferp2, 1024, 0);
            }
            if (bufferp2[0] == '5') {
                cout << "Received ACK from " << neighbourids[i] << ". Hence reply successfully sent to " << neighbourids[i] << endl;
            }
        }
    }


    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
            while (sentbytes != query.length()) {
                sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
            }
            int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
            if (bytesreceived > 0) {
                string temp = "";
                for (int j = 2; j < bytesreceived; j++) {
                    temp += bufferp2[j];
                }
                replyfromneighbours[i] = temp;
                cout << "Reply " << temp << " received from neighbour " << neighbourids[i] << endl;
            }
        }
        if (clientid < neighbourids[i]) {
            int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
            if (bytesreceived > 0) {
                cout << "Request received from neighbour " << neighbourids[i] << endl;
                string temp = "";
                vector<string> wordstosearch;
                int noofwords = 0;
                string result = "2 ";
                for (int j = 2; j < bytesreceived; j++) {
                    if (bufferp2[j] == ' ') {
                        wordstosearch.push_back(temp);
                        cout << "File " << temp << endl;
                        temp = "";
                        noofwords++;
                    }
                    else {
                        temp += bufferp2[j];
                    }
                }
                cout << "Should search the above words for " << neighbourids[i] << endl;
                for (int j = 0; j < noofwords; j++) {
                    if (searchword(wordstosearch[j], filesindirectory, nooffilesindir)) {
                        result += "1";
                    }
                    else {
                        result += "0";
                    }
                }
                int sentbytes = send(usingsocks[i], result.c_str(), noofwords+2, 0);
                while (sentbytes != noofwords+2) {
                    sentbytes = send(usingsocks[i], result.c_str(), noofwords+2, 0);
                }
            }
        }
    }


    for (int i = 0; i < noofneighbour; i++) {
        //cout << "Reply from "<< neighbourids[i] << ": " << replyfromneighbours[i] << endl;
        for (int j = 0; j < nooffilestosearch; j++) {
            if (replyfromneighbours[i][j] == '1') {
                cout << "Found " << filenamestosearch[j] << " at " << neighbourids[i] << endl;
            }
        }
    }


    for (int i = 0; i < noofneighbour; i++) {
        close(usingsocks[i]);
    }

}