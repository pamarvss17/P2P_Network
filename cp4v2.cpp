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

bool in(int target, vector<int> v) {
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == target) {
            return true;
        }
    }
    return false;
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
    
    string filesindr_as_str = "";
    for (auto it : filesindirectory) {
        filesindr_as_str += it;
        filesindr_as_str += " ";
    }
    filesindr_as_str += ' ';

    /*
        ---------------  Establishing connections  ------------------
    */

    int nids[noofneighbour], nports[noofneighbour];
    copy(neighbourids.begin(), neighbourids.end(), nids);
    copy(neighbourports.begin(), neighbourports.end(), nports);

    string tmp = to_string(clientuniqueid);
    tmp = to_string(clientid) + " " + tmp;
    char const *num_char = tmp.c_str();

    int checker[noofneighbour] = {0};

    int position = next(nids,clientid,noofneighbour);
    //cout << position << endl;
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

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(clientportno);

    if (bind(sockfd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

    if (listen(sockfd, 5) < 0)		// checking for listen error
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
                        continue;
                    }
                    send(buf ,  num_char, 6 , 0 );
                    valuesread[i] = recv( buf , buffer, 1024,0);
                    if (valuesread[i]==6)
                    {
                        int j = 0;
                        for (;j < noofneighbour; j++) {
                            if (neighbourids[j] == buffer[0]-'0') {
                                checker[j] = 1;
                                usingsocks[j] = buf;
                                cout << "Accepted connection with " << neighbourids[j] << endl; 
                            }
                        }
                        privateids.push_back(buffer);
                    }  
                }
                else
                {
                    while ((connect(usingsocks[i], (struct sockaddr *)&serv_addr[i-position], sizeof(serv_addr[i-position])) < 0))
	                {
		                continue;
	                }
                    send(usingsocks[i] , num_char , 6 , 0 );
                    valuesread[i] = recv(usingsocks[i] , buffer, 1024,0);		// server side msg reading
                    privateids.push_back(buffer);
                    if (valuesread[i]==6)
                    {
                        checker[i]=1;
                    } 
                    
                }
            }
            
            
        }
        
    }

    sort(filesindirectory.begin(), filesindirectory.end());
    sort(privateids.begin(), privateids.end());

    for (auto& it : filesindirectory) 
    {
        cout << it << endl;
    }

    for (int i = 0; i < noofneighbour; i++)
    {
        string r = privateids[i].substr(2,5);
        cout<< "Connected to "<<neighbourids[i]<<" with unique-ID "<<r<<" on port "<< neighbourports[i] << endl;
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

    vector<vector<string> > filesatneighbours;
    vector<vector<string> > filesrequestedbyneighbours;

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
            while (sentbytes != query.length()) {
                sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
            }
            int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
            if (bytesreceived > 0) {
                string temp = "";
                for (int j = 2; j < 2+nooffilestosearch; j++) {
                    temp += bufferp2[j];
                }
                replyfromneighbours[i] = temp;
                string tmp = "";
                vector<string> buf;
                for (int j = 3 + nooffilestosearch; j < bytesreceived; j++) {
                    if (bufferp2[j] == ' ') {
                        buf.push_back(tmp);
                        cout << tmp << " ";
                        tmp = "";
                    }
                    else {
                        tmp += bufferp2[j];
                    }
                }
                filesatneighbours.push_back(buf);
                cout << "The above files are present at neighbour " << neighbourids[i] << endl;
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
                filesrequestedbyneighbours.push_back(wordstosearch);
                cout << "Should search the above words for " << neighbourids[i] << endl;
                for (int j = 0; j < noofwords; j++) {
                    if (searchword(wordstosearch[j], filesindirectory, nooffilesindir)) {
                        result += "1";
                    }
                    else {
                        result += "0";
                    }
                }
                result = result + " " + filesindr_as_str;
                int sentbytes = send(usingsocks[i], result.c_str(), result.length(), 0);
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
                for (int j = 2; j < 2+nooffilestosearch; j++) {
                    temp += bufferp2[j];
                }
                replyfromneighbours[i] = temp;
                string tmp = "";
                vector<string> buf;
                for (int j = 3 + nooffilestosearch; j < bytesreceived; j++) {
                    if (bufferp2[j] == ' ') {
                        buf.push_back(tmp);
                        cout << tmp << " ";
                        tmp = "";
                    }
                    else {
                        tmp += bufferp2[j];
                    }
                }
                filesatneighbours.push_back(buf);
                cout << "The above files are present at neighbour " << neighbourids[i] << endl;
                cout << "Reply " << temp << " received from neighbour " << neighbourids[i] << endl;
            }
            string temp = "5";
            sentbytes = send(usingsocks[i], temp.c_str(), 1, 0);
            while (sentbytes != 1) {
                sentbytes = send(usingsocks[i], temp.c_str(), 1, 0);
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
                filesrequestedbyneighbours.push_back(wordstosearch);
                cout << "Should search the above words for " << neighbourids[i] << endl;
                for (int j = 0; j < noofwords; j++) {
                    if (searchword(wordstosearch[j], filesindirectory, nooffilesindir)) {
                        result += "1";
                    }
                    else {
                        result += "0";
                    }
                }
                result = result + " " + filesindr_as_str ;
                int sentbytes = send(usingsocks[i], result.c_str(), result.length(), 0);
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
        //cout << "Reply from "<< neighbourids[i] << ": " << replyfromneighbours[i] << endl;
        for (int j = 0; j < nooffilestosearch; j++) {
            if (replyfromneighbours[i][j] == '1') {
                cout << "Found " << filenamestosearch[j] << " at " << neighbourids[i] << endl;
            }
        }
    }

    //Phase 3

    string downloaddirstr = "sample-data/files/client" + to_string(clientid)+ "/Downloaded";
    mkdir(downloaddirstr.c_str(), 0777);

    int wheretofind[nooffilestosearch];
    for (int i = 0; i < nooffilestosearch; i++) {
        wheretofind[i] = -1;
    }
    for (int i = 0; i < nooffilestosearch; i++) {
        for (int j = 0; j < noofneighbour; j++) {
            if (wheretofind[i] == -1) {
                if (replyfromneighbours[j][i] == '1') {
                    wheretofind[i] = j;
                }
            }
        }
    }

    vector<string> filestoask;
    vector<vector<string> > filestoask2;
    int nooffilestoask[noofneighbour];
    for (int i = 0; i < noofneighbour; i++) {
        string temp = "0 ";
        int count = 0;
        vector<string> dummy;
        for (int j = 0; j < nooffilestosearch; j++) {
            if (wheretofind[j] == i) {
                temp += filenamestosearch[j];
                dummy.push_back(filenamestosearch[j]);
                count++;
            }
        }
        temp = to_string(count) + temp.substr(1);
        temp = temp + " ";
        filestoask.push_back(temp);
        filestoask2.push_back(dummy);
        nooffilestoask[i] = count;
        cout << "Message for neighbour " << neighbourids[i] << " " << temp << endl;
    }

    char bufferp3[1024] = {0};
    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            while (sentbytes != filestoask[i].length()) {
                sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            }
            cout << "Successfully sent " << filestoask[i] << " to " << neighbourids[i] << endl;
            int cnt = nooffilestoask[i];
            int sizesoffiles[cnt] = {0};
            int filereceiving = 0;
            while (filereceiving < cnt) {
                if (sizesoffiles[filereceiving] == 0) {
                    int recvsize = recv(usingsocks[i], bufferp3, 1024, 0);
                    if (recvsize > 0) {
                        string temp = "";
                        for (int j = 0; j < recvsize; j++) {
                            temp += bufferp3[j];
                        } 
                        //cout << "Temp " << temp << endl;
                        send(usingsocks[i], to_string(filereceiving).c_str(), to_string(filereceiving).length(), 0);
                        cout << "File size " << temp << endl;
                        cout << "Starting to receive file" << endl;
                        int remainingdata = stoi(temp);
                        string filepath  = "sample-data/files/client" + to_string(clientid) + "/Downloaded/" + filestoask2[i][filereceiving];
                        FILE* filepathdesc = fopen(filepath.c_str(), "w");
                        while (remainingdata > 0) {
                            int bytesrecvd = recv(usingsocks[i], bufferp3, 1024, 0);
                            if (bytesrecvd > 0) {
                                remainingdata -= bytesrecvd;
                                fwrite(bufferp3, sizeof(char), bytesrecvd, filepathdesc);
                            }
                        }
                        sizesoffiles[filereceiving] = stoi(temp);
                    }
                }
                filereceiving++;
                //cout << "Size of " << filestoask[i][filereceiving-1] << " is " << sizesoffiles[filereceiving-1] << "bytes" << endl;
            }
            string ack = "6";
            send(usingsocks[i], ack.c_str(), 1, 0);
        }
        else {
            int filerequestmsg = recv(usingsocks[i], bufferp3, 1024, 0);
            if (filerequestmsg > 0) {
                //cout << "First bit " << bufferp3[0] << endl;
                if (bufferp3[0] != '0') {
                    cout << "Received request from " <<  neighbourids[i] << endl;
                    int x = 0;
                    string temp = "";
                    while (bufferp3[x] != ' ') {
                        temp += bufferp3[x];
                        x++;
                    }
                    x++;
                    int y = 0;
                    int nooffilestosend = stoi(temp);
                    cout << "Should send " << nooffilestosend << " files to " << neighbourids[i] << endl;
                    temp = "";
                    while (y < nooffilestosend) {
                        while (bufferp3[x] != ' ') {
                            temp += bufferp3[x];
                            x++;
                        }
                        //cout << temp << " ";
                        temp = "sample-data/files/client" + to_string(clientid) + "/" + temp;
                        FILE* fp = fopen(temp.c_str(), "rb");
                        fseek(fp, 0L, SEEK_END);
                        int fsize = ftell(fp);
                        fclose(fp);
                        fp = fopen(temp.c_str(), "rb");
                        string msg = to_string(fsize);
                        send(usingsocks[i], msg.c_str(), msg.length(), 0);
                        int filetosend = recv(usingsocks[i], bufferp3, 1024, 0);
                        int remainingdata = fsize;
                        if (filetosend > 0) {
                            int sentbytes = 0;
                            off_t offset = 0;
                            //FILE* fppp = fopen("Dummy.txt", "w");
                            while (remainingdata > 0) {
                                fread(bufferp3, sizeof(char), min(1024, remainingdata), fp);
                                //cout << bufferp3 << endl;
                                ///fwrite(bufferp3, sizeof(char), min(1024, remainingdata), fppp);
                                if ((sentbytes = send(usingsocks[i], bufferp3, min(1024, remainingdata), 0)) > 0) {
                                    remainingdata -= sentbytes;
                                    cout << "Sent " << sentbytes << " bytes of data" << endl; 
                                }
                                else {
                                    perror("Send failed");
                                }
                            }
                        }
                        else {
                            cout << "Didn't receive approval to send file" << endl;
                        }
                        temp = "";
                        y++;
                    }
                }
                else {
                    cout << "No files to send to " << neighbourids[i] << endl;
                }
            }
            if (recv(usingsocks[i], bufferp3, 1024, 0) > 0) {
                cout << "Successfully sent all files to " << neighbourids[i] << endl;
            }
            else {
                cout << "Still waiting for ack from " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            while (sentbytes != filestoask[i].length()) {
                sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            }
            cout << "Successfully sent " << filestoask[i] << " to " << neighbourids[i] << endl;
            int cnt = nooffilestoask[i];
            int sizesoffiles[cnt] = {0};
            int filereceiving = 0;
            while (filereceiving < cnt) {
                if (sizesoffiles[filereceiving] == 0) {
                    int recvsize = recv(usingsocks[i], bufferp3, 1024, 0);
                    if (recvsize > 0) {
                        string temp = "";
                        for (int j = 0; j < recvsize; j++) {
                            temp += bufferp3[j];
                        } 
                        //cout << "Temp " << temp << endl;
                        send(usingsocks[i], to_string(filereceiving).c_str(), to_string(filereceiving).length(), 0);
                        cout << "File size " << temp << endl;
                        cout << "Starting to receive file" << endl;
                        int remainingdata = stoi(temp);
                        string filepath  = "sample-data/files/client" + to_string(clientid) + "/Downloaded/" + filestoask2[i][filereceiving];
                        FILE* filepathdesc = fopen(filepath.c_str(), "w");
                        while (remainingdata > 0) {
                            int bytesrecvd = recv(usingsocks[i], bufferp3, 1024, 0);
                            if (bytesrecvd > 0) {
                                remainingdata -= bytesrecvd;
                                fwrite(bufferp3, sizeof(char), bytesrecvd, filepathdesc);
                            }
                        }
                        sizesoffiles[filereceiving] = stoi(temp);
                    }
                }
                filereceiving++;
                //cout << "Size of " << filestoask[i][filereceiving-1] << " is " << sizesoffiles[filereceiving-1] << "bytes" << endl;
            }
            string ack = "6";
            send(usingsocks[i], ack.c_str(), 1, 0);
        }
        else {
            int filerequestmsg = recv(usingsocks[i], bufferp3, 1024, 0);
            if (filerequestmsg > 0) {
                //cout << "First bit " << bufferp3[0] << endl;
                if (bufferp3[0] != '0') {
                    cout << "Received request from " <<  neighbourids[i] << endl;
                    int x = 0;
                    string temp = "";
                    while (bufferp3[x] != ' ') {
                        temp += bufferp3[x];
                        x++;
                    }
                    x++;
                    int y = 0;
                    int nooffilestosend = stoi(temp);
                    cout << "Should send " << nooffilestosend << " files to " << neighbourids[i] << endl;
                    temp = "";
                    while (y < nooffilestosend) {
                        while (bufferp3[x] != ' ') {
                            temp += bufferp3[x];
                            x++;
                        }
                        cout << temp << " ";
                        temp = "sample-data/files/client" + to_string(clientid) + "/" + temp;
                        FILE* fp = fopen(temp.c_str(), "rb");
                        fseek(fp, 0L, SEEK_END);
                        int fsize = ftell(fp);
                        fclose(fp);
                        fp = fopen(temp.c_str(), "rb");
                        string msg = to_string(fsize);
                        send(usingsocks[i], msg.c_str(), msg.length(), 0);
                        int filetosend = recv(usingsocks[i], bufferp3, 1024, 0);
                        int remainingdata = fsize;
                        if (filetosend > 0) {
                            int sentbytes = 0;
                            off_t offset = 0;
                            //FILE* fppp = fopen("Dummy.txt", "w");
                            while (remainingdata > 0) {
                                fread(bufferp3, sizeof(char), min(1024, remainingdata), fp);
                                //cout << bufferp3 << endl;
                                //fwrite(bufferp3, sizeof(char), min(1024, remainingdata), fppp);
                                if ((sentbytes = send(usingsocks[i], bufferp3, min(1024, remainingdata), 0)) > 0) {
                                    remainingdata -= sentbytes;
                                    cout << "Sent " << sentbytes << " bytes of data" << endl; 
                                }
                                else {
                                    perror("Send failed");
                                }
                            }
                        }
                        else {
                            cout << "Didn't receive approval to send file" << endl;
                        }
                        temp = "";
                        y++;
                    }
                }
                else {
                    cout << "No files to send to " << neighbourids[i] << endl;
                }
            }
            recv(usingsocks[i], bufferp3, 1024, 0);
        }
    }

    sleep(2);

    //Phase 4
    cout << "Starting phase 4" << endl;
    int w = 0;
    char bufferp4[1024];
    for (auto it : filesatneighbours) {
        cout << "Files at neighbour " << neighbourids[w] << ": " ;
        for (auto it2 : it) {
            cout << it2 << " ";
        }
        cout << endl;
        w++;
    }

    w = 0;
    for (auto it : filesrequestedbyneighbours) {
        cout << "Files requested by neighbour " << neighbourids[w] << ": " ;
        for (auto it2 : it) {
            cout << it2 << " ";
        }
        cout << endl;
        w++;
    }

    int wheretofind2[nooffilestosearch] = {0};
    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp4[j] == ' ') {
                        if (wheretofind2[i] == 0) {
                            wheretofind2[i] = stoi(temp);
                        }
                        else {
                            wheretofind2[i] = min(wheretofind2[i], stoi(temp));
                        }
                        cout << temp << "----------------------" << endl;
                        temp = "";
                    }
                    else {
                        temp += bufferp4[j];
                    }
                }
            }
            else {
                cout << "File info recv failed from neighbour " << neighbourids[i] << endl;
                perror("Recv failed");
            }
            string temp = "7";
            send(usingsocks[i], temp.c_str(), 1, 0);
        }
        else {
            string msgtosend = "";
            for (auto it: filesrequestedbyneighbours[i]) {
                if (searchword(it, filesindirectory, nooffilesindir)) {
                    msgtosend = msgtosend + to_string(clientid) + " ";
                }
                else {
                    string temp = "0";
                    for (int j = 0; j < noofneighbour; j++) {
                        if (neighbourids[j] != i) {
                            if (searchword(it, filesatneighbours[i], filesatneighbours[i].size())) {
                                if (temp  == "0") {
                                    temp = to_string(neighbourids[i]);
                                }
                            }
                        }
                    }
                    msgtosend = msgtosend + temp + " ";
                }
            }
            cout << "Message ----------- " << msgtosend << endl;
            if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) <  0) {
                cout << "Send failed :( :(" << endl;
            }
            if (recv(usingsocks[i], bufferp4, 1, 0) > 0) {
                cout << "File info successfully sent to " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp4[j] == ' ') {
                        if (wheretofind2[i] == 0) {
                            wheretofind2[i] = stoi(temp);
                        }
                        else {
                            wheretofind2[i] = min(wheretofind2[i], stoi(temp));
                        }
                        temp = "";
                    }
                    else {
                        temp += bufferp4[j];
                    }
                }
            }
            string temp = "7";
            send(usingsocks[i], temp.c_str(), 1, 0);
        }
        else {
            string msgtosend = "";
            for (auto it: filesrequestedbyneighbours[i]) {
                if (searchword(it, filesindirectory, nooffilesindir)) {
                    msgtosend = msgtosend + to_string(clientid) + " ";
                }
                else {
                    string temp = "0";
                    for (int j = 0; j < noofneighbour; j++) {
                        if (neighbourids[j] != i) {
                            if (searchword(it, filesatneighbours[i], filesatneighbours[i].size())) {
                                if (temp  == "0") {
                                    temp = to_string(neighbourids[i]);
                                }
                            }
                        }
                    }
                    msgtosend = msgtosend + temp + " ";
                }
            }
            if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) < 0) {
                perror("Send failed");
            }
            else {
                cout << "Semnt" << endl;
            }
            if (recv(usingsocks[i], bufferp4, 1, 0) > 0) {
                cout << "File info successfully sent to " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < nooffilestosearch; i++) {
        string depth = "0";
        if (wheretofind2[i] > 0 && !in(wheretofind2[i], neighbourids)) {
            depth = "2";
        }
        cout << "Found " << filenamestosearch[i] << " at " << wheretofind2[i] << " at depth " << depth << endl;
    }

    for (int i = 0; i < noofneighbour; i++) {
        close(usingsocks[i]);
    }

}