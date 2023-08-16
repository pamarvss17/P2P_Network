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
#include <sys/sendfile.h>

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

string vectos(vector<int> v) {
    string result = "";
    for (int i = 0; i < v.size(); i++) {
        result += to_string(v[i]);
        result += " ";
    }
    return result;
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

    string filesindir_as_str;
    for (auto it : filesindirectory) {
        filesindir_as_str += it;
        filesindir_as_str += " ";
    }

    string neighbours_as_str = "";
    for (auto it : neighbourids) {
        neighbours_as_str += to_string(it);
        neighbours_as_str += " ";
    }
    cout << "Neighbour string " << neighbours_as_str << endl;

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

    vector<vector<string> > filereqbyneighbours(noofneighbour);

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
                filereqbyneighbours[i] = wordstosearch;
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
            int br = recv(usingsocks[i], bufferp2, 1, 0);
            while (!(br > 0)) {
                br = recv(usingsocks[i], bufferp2, 1, 0);
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
            string temp = "5";
            send(usingsocks[i], temp.c_str(), 1, 0);
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
                filereqbyneighbours[i] = wordstosearch;
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
            int br = recv(usingsocks[i], bufferp2, 1, 0);
            if (br > 0) {
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
    cout << "---------------------Phase3------------------------" << endl;
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
        if (clientid > neighbourids[i]) {
            int sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            while (sentbytes != filestoask[i].length()) {
                sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            }
            cout << "Successfully sent request " << filestoask[i] << " to " << neighbourids[i] << endl;
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
            if (cnt == 0) {
                recv(usingsocks[i], bufferp3, 1, 0);
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
                    string temp = "9";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
            }
            if (recv(usingsocks[i], bufferp3, 1, 0) > 0) {
                cout << "Successfully sent all files to " << neighbourids[i] << endl;
            }
            else {
                cout << "Still waiting for ack from " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            while (sentbytes != filestoask[i].length()) {
                sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
            }
            cout << "Successfully sent request " << filestoask[i] << " to " << neighbourids[i] << endl;
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
            if (cnt == 0) {
                recv(usingsocks[i], bufferp3, 1, 0);
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
                    string temp = "9";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
            }
            if (recv(usingsocks[i], bufferp3, 1, 0) > 0) {
                cout << "Successfully sent all files to " << neighbourids[i] << endl;
            }
            else {
                cout << "Still waiting for ack from " << neighbourids[i] << endl;
            }
        }
    }

    //Phase 4
    cout << "--------------------------Phase4-------------------------" << endl;
    char bufferp4[1024];    
    vector<vector<string> > filesatneighbours;

    //Sending and receiving file info from neighbours
    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
            if (recvdbytes > 0) {
                cout << "Recv success from neighbour " << neighbourids[i] << endl;
                string temp = "";
                vector<string> filesatnbr;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp4[j] == ' ') {
                        filesatnbr.push_back(temp);
                        cout << temp << " ";
                        temp = "";
                    }
                    else {
                        temp += bufferp4[j];
                    }
                }
                cout << "Found the above files at neighbour " << neighbourids[i] << endl; 
                filesatneighbours.push_back(filesatnbr);
            }
            else {
                cout << "Recv file info1 failed from neighbour " << neighbourids[i] << endl;
            }
            string temp = "5";
            send(usingsocks[i], temp.c_str(), 1, 0);
        }
        else {
            send(usingsocks[i], filesindir_as_str.c_str(), filesindir_as_str.length(), 0);
            if (recv(usingsocks[i], bufferp4, 1, 0) > 0) {
                cout << "File info1 successfully sent to neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "File info1 sending failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                vector<string> filesatnbr;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp4[j] == ' ') {
                        filesatnbr.push_back(temp);
                        cout << temp << " ";
                        temp = "";
                    }
                    else {
                        temp += bufferp4[j];
                    }
                }
                cout << "Found the above files at neighbour " << neighbourids[i] << endl; 
                filesatneighbours.push_back(filesatnbr);
            }
            string temp = "5";
            send(usingsocks[i], temp.c_str(), 1, 0);
        }
        else {
            send(usingsocks[i], filesindir_as_str.c_str(), filesindir_as_str.length(), 0);
            if (recv(usingsocks[i], bufferp4, 1, 0) > 0) {
                cout << "File info1 successfully sent to neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "File info1 sending failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }

    int wheretofind2[nooffilestosearch] = {0};

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                int count = 0;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp4[j] == ' ') {
                        if (wheretofind2[count] == 0) {
                            wheretofind2[count] = stoi(temp);
                        }
                        else {
                            if (stoi(temp) > 0) {
                                wheretofind2[count] = min(wheretofind2[count], stoi(temp));
                            }
                        }
                        temp = "";
                        count++;
                    }
                    else {
                        temp += bufferp4[j];
                    }
                }
                temp = "7";
                send(usingsocks[i], temp.c_str(), 1, 0);
                cout << "File info successfully received from " << neighbourids[i] << endl;
            }
            else {
                cout << "File info receiving failed from " << neighbourids[i] << endl;
                perror("Receive error: ");
            }
        }
        else {
            string msgtosend = "";
            for (auto it: filereqbyneighbours[i]) {
                if (searchword(it, filesindirectory, nooffilesindir)) {
                    msgtosend = msgtosend + to_string(clientid) + " ";
                }
                else {
                    string temp = "0";
                    for (int j = 0; j < noofneighbour; j++) {
                        if (j != i) {
                            if (searchword(it, filesatneighbours[j], filesatneighbours[j].size())) {
                                if (temp  == "0") {
                                    temp = to_string(neighbourids[j]);
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
                cout << "Semnt " << msgtosend << " to neighbour " << neighbourids[i] << endl;
            }
            if (recv(usingsocks[i], bufferp4, 1, 0) > 0) {
                cout << "File info successfully sent to " << neighbourids[i] << endl;
            }
            else {
                cout << "File info sending failed to " << neighbourids[i] << endl;
                perror("Send error: ");
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                int count = 0;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp4[j] == ' ') {
                        //cout << temp << " ";
                        cout << wheretofind2[i] << endl;
                        if (wheretofind2[count] == 0) {
                            wheretofind2[count] = stoi(temp);
                        }
                        else {
                            if (stoi(temp) > 0) {
                                wheretofind2[count] = min(wheretofind2[count], stoi(temp));
                            }
                        }
                        temp = "";
                        count++;
                        //cout << wheretofind2[i] << endl;
                    }
                    else {
                        temp += bufferp4[j];
                    }
                }
                temp = "7";
                send(usingsocks[i], temp.c_str(), 1, 0);
                cout << "File info successfully received from " << neighbourids[i] << endl;
            }
            else {
                cout << "File info receiving failed from " << neighbourids[i] << endl;
                perror("Receive error: ");
            }
        }
        else {
            string msgtosend = "";
            for (auto it: filereqbyneighbours[i]) {
                if (searchword(it, filesindirectory, nooffilesindir)) {
                    msgtosend = msgtosend + to_string(clientid) + " ";
                }
                else {
                    string temp = "0";
                    for (int j = 0; j < noofneighbour; j++) {
                        if (j != i) {
                            if (searchword(it, filesatneighbours[j], filesatneighbours[j].size())) {
                                if (temp  == "0") {
                                    temp = to_string(neighbourids[j]);
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
                cout << "Semnt " << msgtosend << " to neighbour " << neighbourids[i] << endl;
            }
            if (recv(usingsocks[i], bufferp4, 1, 0) > 0) {
                cout << "File info successfully sent to " << neighbourids[i] << endl;
            }
            else {
                cout << "File info sending failed to " << neighbourids[i] << endl;
                perror("Send error: ");
            }
        }
    }

    cout << "Conclusion: " << endl;
    vector<int> newconnectionsneeded;
    for (int i = 0; i < nooffilestosearch; i++) {
        string depth = "0";
        if (wheretofind2[i] > 0 && in(wheretofind2[i], neighbourids)) {
            depth = "1";
        }
        else if (wheretofind2[i] > 0 && !in(wheretofind2[i], neighbourids)) {
            depth = "2";
            newconnectionsneeded.push_back(wheretofind2[i]);
        }
        cout << "Found " << filenamestosearch[i] << " at " << wheretofind2[i] << " at depth " << depth << endl;
    }
    if (newconnectionsneeded.size() > 0) {
        cout << "New connections needed : " << vectos(newconnectionsneeded) << endl;;
    }
    else {
        cout << "No new connections needed" << endl;
    }


    //Phase 5
    char bufferp5[1024];
    
    vector<vector<int> > neighboursofneighbours(noofneighbour);
    cout << "--------------------Phase 5----------------------" << endl;
    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                vector<int> nbrsofnbrs;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp5[j] == ' ') {
                        nbrsofnbrs.push_back(stoi(temp));
                        //cout << "Temp " << temp << " string" << endl;
                        temp = "";
                    }
                    else {
                        temp += bufferp5[j];
                    }
                }
                neighboursofneighbours[i] = nbrsofnbrs;
                temp = "5";
                send(usingsocks[i], temp.c_str(), 1, 0);
                cout << "Successfully received neighbour info from neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "Neighbour info receive failed from neighbour " << neighbourids[i] << endl;
                perror("Error");
            }
        }
        else {
            if (send(usingsocks[i], neighbours_as_str.c_str(), neighbours_as_str.length(), 0) > 0) {
                cout << "Succesfully sent neighbour info to neighbour " << neighbourids[i] << endl;
                recv(usingsocks[i], bufferp5, 1, 0);
            }
            else {
                cout << "Send neighbour info failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                vector<int> nbrsofnbrs;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp5[j] == ' ') {
                        nbrsofnbrs.push_back(stoi(temp));
                        temp = "";
                    }
                    else {
                        temp += bufferp5[j];
                    }
                }
                neighboursofneighbours[i] = nbrsofnbrs;
                temp = "5";
                send(usingsocks[i], temp.c_str(), 1, 0);
                cout << "Successfully received neighbour info from neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "Neighbour info receive failed from neighbour " << neighbourids[i] << endl;
                perror("Error");
            }
        }
        else {
            if (send(usingsocks[i], neighbours_as_str.c_str(), neighbours_as_str.length(), 0) > 0) {
                cout << "Succesfully sent neighbour info to neighbour " << neighbourids[i] << endl;
                recv(usingsocks[i], bufferp5, 1, 0);
            }
            else {
                cout << "Send neighbour info failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        cout << "Neighbours of neighbour " << neighbourids[i] << ": ";
        for (auto it: neighboursofneighbours[i]) {
            cout << it << " ";
        }
        cout << endl;
    }

    vector<vector<int> > connectionrequests(noofneighbour);
    for (auto it : newconnectionsneeded) {
        for (int i = 0; i < noofneighbour; i++) {
            if (in(it, neighboursofneighbours[i])) {
                connectionrequests[i].push_back(it);
            }
        }
    }
    for (int i = 0; i < noofneighbour; i++) {
        cout << "Connection request to neighbour " << neighbourids[i] << ": " << vectos(connectionrequests[i]) << endl;
    }


    int x = 0;
    for (auto it : connectionrequests) {
        for (auto it1 : it) {
            cout << "Need to connect to " << it1 << " through " << neighbourids[x] << endl; 
        }
        x++;
    }

    vector<vector<int> > nbrrequeststosend(noofneighbour);
    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvbytes > 0) {
                if (bufferp5[0] != ' ') {
                    string temp = "";
                    for (int j = 0; j < recvbytes; j++) {
                        if (bufferp5[j] == ' ') {
                            for (int k = 0; k < noofneighbour; k++) {
                                if (neighbourids[k] == stoi(temp)) {
                                    nbrrequeststosend[k].push_back(neighbourids[i]);
                                }
                            }
                            temp = "";
                        }
                        else {
                            temp += bufferp5[j];
                        }
                    }
                    temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
                cout << "Connection request info successfully received from neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "Connection request info receiving failed from neighbour " << neighbourids[i] << endl;
            }
        }
        else {
            string msgtosend = "";
            if (connectionrequests[i].size() == 0) {
                msgtosend = " ";
            }
            else {
                msgtosend = vectos(connectionrequests[i]);
                //msgtosend += " ";
            }
            if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) > 0) {
                cout << "Connection request info " << msgtosend << " successfully sent to neighbour " << neighbourids[i] << endl;
                if (msgtosend == " ") {
                    string temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
            else {
                cout << "Connection request info sending failed to neighbour" << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvbytes > 0) {
                if (bufferp5[0] != ' ') {
                    string temp = "";
                    for (int j = 0; j < recvbytes; j++) {
                        if (bufferp5[j] == ' ') {
                            for (int k = 0; k < noofneighbour; k++) {
                                if (neighbourids[k] == stoi(temp)) {
                                    nbrrequeststosend[k].push_back(neighbourids[i]);
                                }
                            }
                            temp = "";
                        }
                        else {
                            temp += bufferp5[j];
                        }
                    }
                    temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
                cout << "Connection request info successfully received from neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "Connection request info receiving failed from neighbour " << neighbourids[i] << endl;
            }
        }
        else {
            string msgtosend = "";
            if (connectionrequests[i].size() == 0) {
                msgtosend = " ";
            }
            else {
                msgtosend = vectos(connectionrequests[i]);
                //msgtosend += " ";
            }
            if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) > 0) {
                cout << "Connection request info " << msgtosend << " successfully sent to neighbour " << neighbourids[i] << endl;
                if (msgtosend == " ") {
                    string temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
            else {
                cout << "Connection request info sending failed to neighbour" << neighbourids[i] << endl;
            }
            
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvbytes > 0) {
                if (bufferp5[0] != ' ') {
                    string temp = "";
                    for (int j = 0; j < recvbytes; j++) {
                        if (bufferp5[j] == ' ') {
                            newconnectionsneeded.push_back(stoi(temp));
                            temp = "";
                        }
                        else {
                            temp += bufferp5[j];
                        }
                    }
                    temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
            else {
                cout << "Neighbour info receiving failed from neighbour " << neighbourids[i] << endl;
            }
        }
        else {
            string msgtosend = "";
            if (nbrrequeststosend[i].size() == 0) {
                msgtosend = " ";
            }
            else {
                msgtosend += vectos(nbrrequeststosend[i]);
                //msgtosend += " ";
            }
            if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) > 0) {
                cout << "Neighbour request info sent to neighbour " << neighbourids[i] << endl;
                if (msgtosend == " ") {
                    string temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
            else {
                cout << "Neighbour request info sending failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }  

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvbytes > 0) {
                if (bufferp5[0] != ' ') {
                    string temp = "";
                    for (int j = 0; j < recvbytes; j++) {
                        if (bufferp5[j] == ' ') {
                            newconnectionsneeded.push_back(stoi(temp));
                            temp = "";
                        }
                        else {
                            temp += bufferp5[j];
                        }
                    }
                    temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
            else {
                cout << "Neighbour info receiving failed from neighbour " << neighbourids[i] << endl;
            }
        }
        else {
            string msgtosend = "";
            if (nbrrequeststosend[i].size() == 0) {
                msgtosend = " ";
            }
            else {
                msgtosend += vectos(nbrrequeststosend[i]);
                //msgtosend += " ";
            }
            if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) > 0) {
                cout << "Neighbour request info sent to neighbour " << neighbourids[i] << endl;
                if (msgtosend == " ") {
                    string temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
            else {
                cout << "Neighbour request info sending failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }  

    vector<int> newconnectionsneededunique;

    for (auto it : newconnectionsneeded) {
        if (!in(it, newconnectionsneededunique)) {
            newconnectionsneededunique.push_back(it);
        }
    }
    if (newconnectionsneededunique.size() > 0) {
        cout << "New connections need to be established with :" << endl;
    }
    else {
        cout << "No new connections need to be established" << endl;
    }
    sort(newconnectionsneededunique.begin(), newconnectionsneededunique.end());
    for (auto it: newconnectionsneededunique) {
        cout << it << " ";
    }
    cout << endl;


    // vector<vector<int> > portsofnewconnections(newconnectionsneededunique.size());
    // for (int i = 0; i < noofneighbour; i++) {
    //     if (clientid < neighbourids[i]) {
    //         int recvbytes = recv(usingsocks[i], bufferp5, 1024, 0);
    //         if (recvbytes > 0) {
    //             if (bufferp5[0] != ' ') {
    //                 string temp = "";
    //                 vector<int> tempvec;
    //                 for (int j = 0; j < recvbytes; j++) {
    //                     if (bufferp5[j] == ' ') {
    //                         tempvec.push_back(stoi(temp));
    //                         temp = "";
    //                     }
    //                     else {
    //                         temp += bufferp5[j];
    //                     }
    //                 }
    //                 portsofnewconnections[i] = tempvec;
    //                 temp = "5";
    //                 send(usingsocks[i], temp.c_str(), 1, 0);
    //             }
    //             else {
    //                 recv(usingsocks[i], bufferp5, 1, 0);
    //             }
    //             cout << "Neighbour ports info successfully received from neighbour " << neighbourids[i] << endl;
    //         }
    //         else {
    //             cout << "Neighbour ports info receiving failed from neighbour " << neighbourids[i] << endl;
    //         }
    //     }
    //     else {
    //         string msgtosend = "";
    //         if (connectionrequests[i].size() == 0) {
    //             msgtosend = " ";
    //         }
    //         else {
    //             for (auto it : connectionrequests[i]) {
    //                 cout << it << "**********************" << endl;
    //                 for (int j = 0; j < noofneighbour; j++) {
    //                     if (neighbourids[j] == it) {
    //                             msgtosend += to_string(neighbourports[j]);
    //                             msgtosend += " ";
    //                     }
    //                 }
    //             }
    //             msgtosend += " ";
    //         }
    //         cout << msgtosend << "------------" << endl;
    //         if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) > 0) {
    //             cout << "Neighbour ports info " << msgtosend << " successfully sent to neighbour " << neighbourids[i] << endl;
    //             if (msgtosend == " ") {
    //                 string temp = "5";
    //                 send(usingsocks[i], temp.c_str(), 1, 0);
    //             }
    //             else {
    //                 recv(usingsocks[i], bufferp5, 1, 0);
    //             }
    //         }
    //         else {
    //             cout << "Neighbour info sending failed to neighbour" << neighbourids[i] << endl;
    //         }
    //     }
    // }

    vector<vector<int> > portsofneighboursneighbours(noofneighbour);
    for (int i = 0; i < noofneighbour; i++) {
        if (clientid < neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                vector<int> pnbrsofnbrs;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp5[j] == ' ') {
                        pnbrsofnbrs.push_back(stoi(temp));
                        //cout << "Temp " << temp << " string" << endl;
                        temp = "";
                    }
                    else {
                        temp += bufferp5[j];
                    }
                }
                portsofneighboursneighbours[i] = pnbrsofnbrs;
                temp = "5";
                send(usingsocks[i], temp.c_str(), 1, 0);
                cout << "Successfully received neighbour port info from neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "Neighbour port info receive failed from neighbour " << neighbourids[i] << endl;
                perror("Error");
            }
        }
        else {
            
            if (send(usingsocks[i], vectos(neighbourports).c_str(), vectos(neighbourports).length(), 0) > 0) {
                cout << "Succesfully sent neighbour port info " << vectos(neighbourports) << " to neighbour " << neighbourids[i] << endl;
                recv(usingsocks[i], bufferp5, 1, 0);
            }
            else {
                cout << "Send neighbour port info failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }

    for (int i = 0; i < noofneighbour; i++) {
        if (clientid > neighbourids[i]) {
            int recvdbytes = recv(usingsocks[i], bufferp5, 1024, 0);
            if (recvdbytes > 0) {
                string temp = "";
                vector<int> pnbrsofnbrs;
                for (int j = 0; j < recvdbytes; j++) {
                    if (bufferp5[j] == ' ') {
                        pnbrsofnbrs.push_back(stoi(temp));
                        //cout << "Temp " << temp << " string" << endl;
                        temp = "";
                    }
                    else {
                        temp += bufferp5[j];
                    }
                }
                portsofneighboursneighbours[i] = pnbrsofnbrs;
                temp = "5";
                send(usingsocks[i], temp.c_str(), 1, 0);
                cout << "Successfully received neighbour port info from neighbour " << neighbourids[i] << endl;
            }
            else {
                cout << "Neighbour port info receive failed from neighbour " << neighbourids[i] << endl;
                perror("Error");
            }
        }
        else {
            
            if (send(usingsocks[i], vectos(neighbourports).c_str(), vectos(neighbourports).length(), 0) > 0) {
                cout << "Succesfully sent neighbour port info " << vectos(neighbourports) << " to neighbour " << neighbourids[i] << endl;
                recv(usingsocks[i], bufferp5, 1, 0);
            }
            else {
                cout << "Send neighbour port info failed to neighbour " << neighbourids[i] << endl;
            }
        }
    }

    for (auto it : newconnectionsneededunique) {
        for (int i = 0; i < noofneighbour; i++) {
            for (int j = 0; j < neighboursofneighbours[i].size(); j++) {
                if (neighboursofneighbours[i][j] == it) {
                    cout << "Port of " << it << " is " << portsofneighboursneighbours[i][j] << endl;
                }
            }
        }
    }

 
    for (int i = 0; i < noofneighbour; i++) {
        close(usingsocks[i]);
    }

}