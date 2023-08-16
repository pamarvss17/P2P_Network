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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>

#include <openssl/md5.h>

using namespace std;

#define MD5_DIGEST_LENGTH 16
#define PROT_READ 0x1
#define RDONLY 00
#define MAP_SHARED 0x01


unsigned char result[MD5_DIGEST_LENGTH];
unsigned char *MD5(const unsigned char *d, unsigned long n, unsigned char *md);

// Print the MD5 sum as hex-digits.
void print_md5_sum(unsigned char* md) 
{
    int i;
    for(i=0; i <MD5_DIGEST_LENGTH; i++) 
    {
            printf("%02x",md[i]);
    }
}

// Get the size of the file by its file descriptor
unsigned long get_size_by_fd(int fd) 
{
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) 
    {
        cout<< "Stat buff error"<<endl;
    }
    return statbuf.st_size;
}

void printmd5(char const* filepath)
{
    int file_descript;
    unsigned long file_size;
    char* file_buffer;
    //printf("using file:\t%s\n", filepath);
    file_descript = open(filepath, RDONLY);
    if(file_descript < 0)
    {
        cout<< "File descript error"<<endl;
    }

    file_size = get_size_by_fd(file_descript);
    //printf("file size:\t%lu\n", file_size);

    file_buffer = (char*)mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
    unsigned char* filebuffer = (unsigned char*) file_buffer;
    MD5(filebuffer, file_size, result);
    munmap(file_buffer, file_size); 

    print_md5_sum(result);
}



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

    vector<int> neighbouruniqueids(noofneighbour);

    for (auto& it : filesindirectory) 
    {
        cout << it << endl;
    }

    for (int i = 0; i < noofneighbour; i++)
    {
        vector<string> r = returnwords(privateids[i]);
        neighbouruniqueids[i] = stoi(r[1]);
        cout<< "Connected to "<<neighbourids[i]<<" with unique-ID "<<r[1]<<" on port "<< neighbourports[i] << endl;
    }

    string neighbourinfo_as_str = "";
    for (int i = 0; i < noofneighbour; i++) {
        neighbourinfo_as_str = neighbourinfo_as_str + to_string(neighbourids[i]) + " " + to_string(neighbourports[i]) + " " + to_string(neighbouruniqueids[i]) + " ";

    }

    //Phase 2
    
    vector<string> replyfromneighbours;

    for (int i = 0; i < noofneighbour; i++) {
        replyfromneighbours.push_back("");
    }

    string query = "1 ";
    for (auto& it : filenamestosearch){
        query += it;
        query += " ";
    }
    char const* query_as_c_str = query.c_str();

    char bufferp2[1024] = {0};


    vector<vector<string> > filereqbyneighbours(noofneighbour);
    int phase = 0;

    for (int iterate = 0; iterate < 2; iterate++) { 
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
                int sentbytes = send(usingsocks[i], query_as_c_str, query.length(), 0);
                int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
                if (bytesreceived > 0) {
                    string temp = "";
                    for (int j = 2; j < bytesreceived; j++) {
                        temp += bufferp2[j];
                    }
                    replyfromneighbours[i] = temp;
                }
                string temp = "5";
                sentbytes = send(usingsocks[i], temp.c_str(), 1, 0);
            }
            else if (((clientid > neighbourids[i]) && (phase == 0)) || ((clientid < neighbourids[i]) && (phase == 1))) {
                int bytesreceived = recv(usingsocks[i], bufferp2, 1024, 0);
                if (bytesreceived > 0) {
                    string temp = "";
                    vector<string> wordstosearch;
                    int noofwords = 0;
                    string result = "2 ";
                    for (int j = 2; j < bytesreceived; j++) {
                        if (bufferp2[j] == ' ') {
                            wordstosearch.push_back(temp);
                            temp = "";
                            noofwords++;
                        }
                        else {
                            temp += bufferp2[j];
                        }
                    }
                    filereqbyneighbours[i] = wordstosearch;
                    for (int j = 0; j < noofwords; j++) {
                        if (searchword(wordstosearch[j], filesindirectory, nooffilesindir)) {
                            result += "1";
                        }
                        else {
                            result += "0";
                        }
                    }
                    int sentbytes = send(usingsocks[i], result.c_str(), noofwords+2, 0);
                }
                int br = recv(usingsocks[i], bufferp2, 1, 0);
            }
        }
        phase = (phase + 1)%2;
    }

    int wheretofindinit[nooffilestosearch] = {0};

    for (int i = 0; i < noofneighbour; i++) {
        for (int j = 0; j < nooffilestosearch; j++) {
            if (replyfromneighbours[i][j] == '1') {
                //cout << "Found " << filenamestosearch[j] << " at " << neighbourids[i] << endl;
                if (wheretofindinit[j] == 0) {
                    wheretofindinit[j] = neighbouruniqueids[i];
                }
                else {
                    wheretofindinit[j] = min(wheretofindinit[j], neighbouruniqueids[i]);
                }
            }
        }
    }

    for (int i = 0; i < nooffilestosearch; i++) {
        if (wheretofindinit[i] > 0) {
            cout << "Found " << filenamestosearch[i] << " at " << wheretofindinit[i] << " with MD5 0 at depth 1" << endl; 
        }
        else {
            cout << "Found " << filenamestosearch[i] << " at 0 with MD5 0 at depth 0" << endl; 
        }
    }

    //Phase 3
    string downloaddirstr = "sample-data2/files/client" + to_string(clientid)+ "/Downloaded";
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
            else {
                if (replyfromneighbours[j][i] == '1') {
                    if (neighbouruniqueids[wheretofind[i]] > neighbouruniqueids[j]) {
                        wheretofind[i] = j;
                    }
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
                temp += " ";
                dummy.push_back(filenamestosearch[j]);
                count++;
            }
        }
        temp = to_string(count) + temp.substr(1);
        filestoask.push_back(temp);
        filestoask2.push_back(dummy);
        nooffilestoask[i] = count;
    }

    char bufferp3[1024] = {0};
    char bufferp3read[1024] = {0};
    for (int iterate = 0; iterate < 2; iterate++) {
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid > neighbourids[i]) && (phase == 0)) || ((clientid < neighbourids[i]) && (phase == 1))) {
                int sentbytes = send(usingsocks[i], filestoask[i].c_str(), filestoask[i].length(), 0);
                int cnt = nooffilestoask[i];
                int sizesoffiles[cnt] = {0};
                int filereceiving = 0;
                if (filestoask[i][0] != '0') {
                    while (filereceiving < cnt) {
                        if (sizesoffiles[filereceiving] == 0) {
                            int recvsize = recv(usingsocks[i], bufferp3, 1024, 0);
                            if (recvsize > 0) {
                                string temp = "";
                                for (int j = 0; j < recvsize; j++) {
                                    temp += bufferp3[j];
                                } 
                                send(usingsocks[i], to_string(filereceiving).c_str(), to_string(filereceiving).length(), 0);
                                int remainingdata = stoi(temp);
                                string filepath  = "sample-data2/files/client" + to_string(clientid) + "/Downloaded/" + filestoask2[i][filereceiving];
                                FILE* filepathdesc = fopen(filepath.c_str(), "w");
                                int rb = 0;
                                while (remainingdata > 0) {
                                    int bytesrecvd = recv(usingsocks[i], bufferp3, 1024, 0);
                                    if (bytesrecvd > 0) {
                                        remainingdata -= bytesrecvd;
                                        rb += bytesrecvd;
                                        fwrite(bufferp3, sizeof(char), bytesrecvd, filepathdesc);
                                    }
                                }
                                fclose(filepathdesc);
                                temp = "5";
                                send(usingsocks[i], temp.c_str(), 1 , 0);
                                sizesoffiles[filereceiving] = stoi(temp);
                            }
                        }
                        filereceiving++;
                    }
                    string ack = "6";
                    send(usingsocks[i], ack.c_str(), 1, 0);
                }
                else {
                    char bufferp3dup[1];
                    recv(usingsocks[i], bufferp3dup, 1, 0);
                }
            }
            else if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
                int filerequestmsg = recv(usingsocks[i], bufferp3, 1024, 0);
                if (filerequestmsg > 0) {
                    if (bufferp3[0] != '0') {
                        int x = 0;
                        string temp = "";
                        while (bufferp3[x] != ' ') {
                            temp += bufferp3[x];
                            x++;
                        }
                        x++;
                        int y = 0;
                        int nooffilestosend = stoi(temp);
                        temp = "";
                        while (y < nooffilestosend) {
                            while (bufferp3[x] != ' ') {
                                temp += bufferp3[x];
                                x++;
                            }
                            x++;
                            temp = "sample-data2/files/client" + to_string(clientid) + "/" + temp;
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
                                while (remainingdata > 0) {
                                    fread(bufferp3read, sizeof(char), min(1024, remainingdata), fp);
                                    if ((sentbytes = send(usingsocks[i], bufferp3read, min(1024, remainingdata), 0)) > 0) {
                                        remainingdata -= sentbytes;
                                    }
                                    else {
                                        perror("Send failed");
                                    }
                                }
                                fclose(fp);
                                char bufferp3dup[1];
                                recv(usingsocks[i], bufferp3dup, 1, 0);
                            }
                            else {
                                cout << "Didn't receive approval to send file" << endl;
                            }
                            temp = "";
                            y++;
                        }
                        recv(usingsocks[i], bufferp3, 1, 0);
                    }
                    else {
                        string temp = "5";
                        send(usingsocks[i], temp.c_str(), 1, 0);
                    }
                }
            }
        }
        phase = (phase + 1)%2;
    }

    for (int i = 0; i < nooffilestosearch; i++)
    {
        if (wheretofindinit[i] == 0)
        {
            cout<< "Found " << filenamestosearch[i] << " at 0 with MD5 0 at depth 0" << endl;
        }
        else
        {
            string path = "sample-data2/files/client"+ to_string(clientid) + "/Downloaded/" + filenamestosearch[i];
            char const *pathoffile = path.c_str();
            cout<< "Found " << filenamestosearch[i] << " at "<< wheretofindinit[i]<< " with MD5 "; printmd5(pathoffile);
            cout << " at depth 1" << endl;
        }
        
    }

    //Phase 4
    char bufferp4[1024];    
    vector<vector<int> > neighboursofneighbours(noofneighbour);
    vector<vector<int> > portsofneighboursneighbours(noofneighbour);
    vector<vector<int> > uniqueidsofneighboursneighbours(noofneighbour);
    
    for (int iterator = 0; iterator < 2; iterator++) {
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
                int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
                if (recvdbytes > 0) {
                    string temp = "";
                    vector<int> nbrsofnbrs;
                    vector<int> portsofnbrsnbrs;
                    vector<int> uniqueidsofnbrsnbrs;
                    int cnt = 0;
                    for (int j = 0; j < recvdbytes; j++) {
                        if (bufferp4[j] == ' ') {
                            if (cnt == 0) {
                                nbrsofnbrs.push_back(stoi(temp));
                                cnt++;
                            }
                            else if (cnt == 1) {
                                portsofnbrsnbrs.push_back(stoi(temp));
                                cnt++;
                            }
                            else if (cnt == 2) {
                                uniqueidsofnbrsnbrs.push_back(stoi(temp));
                                cnt = 0;
                            }
                            temp = "";
                        }
                        else {
                            temp += bufferp4[j];
                        }
                    }
                    neighboursofneighbours[i] = nbrsofnbrs;
                    portsofneighboursneighbours[i] = portsofnbrsnbrs;
                    uniqueidsofneighboursneighbours[i] = uniqueidsofnbrsnbrs;
                    temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
                else {
                    perror("Error");
                }
            }
            else if (((clientid < neighbourids[i]) && (phase == 1)) || ((clientid > neighbourids[i]) && (phase == 0))) {
                if (send(usingsocks[i], neighbourinfo_as_str.c_str(), neighbourinfo_as_str.length(), 0) > 0) {
                    recv(usingsocks[i], bufferp4, 1, 0);
                }
            }
        }
        phase = (phase + 1)%2;
    }

    vector<vector<string> > filesatneighbours(noofneighbour);

    //Sending and receiving file info from neighbours
    for (int iterator = 0; iterator < 2; iterator++) {
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
                int recvdbytes = recv(usingsocks[i], bufferp4, 1024, 0);
                if (recvdbytes > 0) {
                    string temp = "";
                    vector<string> filesatnbr;
                    for (int j = 0; j < recvdbytes; j++) {
                        if (bufferp4[j] == ' ') {
                            filesatnbr.push_back(temp);
                            temp = "";
                        }
                        else {
                            temp += bufferp4[j];
                        }
                    }
                    filesatneighbours[i] = filesatnbr;
                }
                string temp = "5";
                send(usingsocks[i], temp.c_str(), 1, 0);
            }
            else if (((clientid > neighbourids[i]) && (phase == 0)) || ((clientid < neighbourids[i]) && (phase == 1))) {
                send(usingsocks[i], filesindir_as_str.c_str(), filesindir_as_str.length(), 0);
                recv(usingsocks[i], bufferp4, 1, 0);
            }
        }
        phase = (phase + 1)%2;
    }

    int wheretofind2[nooffilestosearch] = {0};

    for (int iterator = 0; iterator < 2; iterator++) {
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
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
                                    if (!in(wheretofind2[count], neighbouruniqueids)) {
                                        if (in(stoi(temp), neighbouruniqueids)) {
                                            wheretofind2[count] = stoi(temp);
                                        }
                                        else {
                                            wheretofind2[count] = min(wheretofind2[count], stoi(temp));
                                        }
                                    }
                                    else {
                                        if (in(stoi(temp), neighbouruniqueids)) {
                                            wheretofind2[count] = min(wheretofind2[count], stoi(temp));
                                        }
                                    }
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
                }
                else {
                    perror("Receive error: ");
                }
            }
            else if (((clientid > neighbourids[i]) && (phase == 0)) || ((clientid < neighbourids[i]) && (phase == 1))) {
                string msgtosend = "";
                for (auto it: filereqbyneighbours[i]) {
                    if (searchword(it, filesindirectory, nooffilesindir)) {
                        msgtosend = msgtosend + to_string(clientuniqueid) + " ";
                    }
                    else {
                        string temp = "0";
                        for (int j = 0; j < noofneighbour; j++) {
                            if (j != i) {
                                if (searchword(it, filesatneighbours[j], filesatneighbours[j].size())) {
                                    if (temp  == "0") {
                                        temp = to_string(neighbouruniqueids[j]);
                                    }
                                    else {
                                        temp = min(temp, to_string(neighbouruniqueids[j]));
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
                recv(usingsocks[i], bufferp4, 1, 0);
            }
        }
        phase = (phase + 1)%2;
    }

    int wheretofind3[nooffilestosearch] = {0};
    for (int i = 0; i < nooffilestosearch; i++) {
        if (wheretofind2[i] > 0) {
            for (int j = 0; j < noofneighbour; j++) {
                if (neighbouruniqueids[j] == wheretofind2[i]) {
                    wheretofind3[i] = neighbourids[j];
                }
                for (int k = 0; k < neighboursofneighbours[j].size(); k++) {
                    if (uniqueidsofneighboursneighbours[j][k] == wheretofind2[i]) {
                        wheretofind3[i] = neighboursofneighbours[j][k];
                    }
                }
            }
        }
    }
    
    vector<int> newconnectionsneeded;
    for (int i = 0; i < nooffilestosearch; i++) {
        string depth = "0";
        if (wheretofind3[i] > 0 && in(wheretofind3[i], neighbourids)) {
            depth = "1";
        }
        else if (wheretofind3[i] > 0 && !in(wheretofind3[i], neighbourids)) {
            depth = "2";
            newconnectionsneeded.push_back(wheretofind3[i]);
        }
        cout << "Found " << filenamestosearch[i] << " at " << wheretofind2[i] << " with MD5 0 at depth " << depth << endl;
    }

    //Phase 5
    char bufferp5[1024];

    vector<int> newconnectionsneededunique;

    for (auto it : newconnectionsneeded) {
        if (!in(it, newconnectionsneededunique)) {
            newconnectionsneededunique.push_back(it);
        }
    }

    vector<vector<int> > connectionrequests(noofneighbour);
    int newconnectionreqto[newconnectionsneededunique.size()] = {0};
    for (int i = 0; i < newconnectionsneededunique.size(); i++) {
        for (int j = 0; j < noofneighbour; j++) {
            if (in(newconnectionsneededunique[i], neighboursofneighbours[j])) {
                if (newconnectionreqto[i] == 0) {
                    newconnectionreqto[i] = neighbourids[j];
                    connectionrequests[j].push_back(newconnectionsneededunique[i]);
                }
            }
        }
    }

    vector<vector<int> > nbrrequeststosend(noofneighbour);
    for (int iterator = 0; iterator < 2; iterator++) {
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
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
                        string temp = "5";
                        send(usingsocks[i], temp.c_str(), 1, 0);
                    }
                }
            }
            else if (((clientid > neighbourids[i]) && (phase == 0)) || ((clientid < neighbourids[i]) && (phase == 1))) {
                string msgtosend;
                if (connectionrequests[i].size() == 0) {
                    msgtosend = " ";
                }
                else {
                    msgtosend = vectos(connectionrequests[i]);
                }
                int sentbytes = send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0);
                recv(usingsocks[i], bufferp5, 1, 0);
            }
        }
        phase = (phase + 1)%2;
    }

    for (int iterator = 0; iterator < 2; iterator++) {
        for (int i = 0; i < noofneighbour; i++) {
            if (((clientid < neighbourids[i]) && (phase == 0)) || ((clientid > neighbourids[i]) && (phase == 1))) {
                int recvbytes = recv(usingsocks[i], bufferp5, 1024, 0);
                if (recvbytes > 0) {
                    if (bufferp5[0] != ' ') {
                        string temp = "";
                        for (int j = 0; j < recvbytes; j++) {
                            if (bufferp5[j] == ' ') {
                                newconnectionsneededunique.push_back(stoi(temp));
                                temp = "";
                            }
                            else {
                                temp += bufferp5[j];
                            }
                        }
                    }
                    string temp = "5";
                    send(usingsocks[i], temp.c_str(), 1, 0);
                }
            }
            else if (((clientid > neighbourids[i]) && (phase == 0)) || ((clientid < neighbourids[i]) && (phase == 1))) {
                string msgtosend = "";
                if (nbrrequeststosend[i].size() == 0) {
                    msgtosend = " ";
                }
                else {
                    msgtosend += vectos(nbrrequeststosend[i]);
                }
                if (send(usingsocks[i], msgtosend.c_str(), msgtosend.length(), 0) > 0) {
                    recv(usingsocks[i], bufferp5, 1, 0);
                }
            }
        }  
        phase = (phase + 1)%2;
    }

    vector<int> newconnectionsneededfinal;
    for (auto it: newconnectionsneededunique) {
        if (!in(it, newconnectionsneededfinal)) {
            newconnectionsneededfinal.push_back(it);
        }
    }
    sort(newconnectionsneededfinal.begin(), newconnectionsneededfinal.end());

    int noofnewconnectionsneeded = newconnectionsneededfinal.size();
    vector<int> portsofnewconnections(noofnewconnectionsneeded);

    for (int i = 0; i < noofnewconnectionsneeded; i++){
        for (int j = 0; j < noofneighbour; j++) {
            for (int k = 0; k < neighboursofneighbours[j].size(); k++) {
                if (neighboursofneighbours[j][k] == newconnectionsneededfinal[i]) {
                    portsofnewconnections[i] = portsofneighboursneighbours[j][k];
                }
            }
        }
    }

    int newnids[noofnewconnectionsneeded];
    for (int i = 0; i < noofnewconnectionsneeded; i++) {
        newnids[i] = newconnectionsneededfinal[i];
    }

    cout << "New connections needed " << vectos(newconnectionsneededfinal) << endl;
    cout << "Ports of new connections needed " << vectos(portsofnewconnections) << endl;

    int newpos = next(newnids, clientid, noofnewconnectionsneeded);
    int newusingsocks[noofnewconnectionsneeded];
    struct sockaddr_in new_serv_addr[noofnewconnectionsneeded-newpos];

    for (int i = newpos; i < noofnewconnectionsneeded; i++) {
        newusingsocks[i] = socket(AF_INET ,SOCK_STREAM, 0);
        new_serv_addr[i-newpos].sin_family = AF_INET;
        new_serv_addr[i-newpos].sin_port = htons(portsofnewconnections[i]);
    }

    int newchecker[noofnewconnectionsneeded] = {0};
    int newvaluesread[noofnewconnectionsneeded] = {0};
    char newbufferp5[1024] = {0};

    struct sockaddr_in address_new;

	address_new.sin_family = AF_INET;
	address_new.sin_addr.s_addr = INADDR_ANY;
	address_new.sin_port = htons(clientportno);

    for (int i = 0; i < noofnewconnectionsneeded; i++) {
        if (clientid < newconnectionsneededfinal[i]) {
            while (connect(newusingsocks[i], (struct sockaddr *)&new_serv_addr[i-newpos], sizeof(new_serv_addr[i-newpos]) ) <= 0) {
                perror("Connect error");
                continue;
            }
            cout << "Connected with " << newconnectionsneededfinal[i] << endl;
            send(newusingsocks[i], to_string(clientid).c_str(), to_string(clientid).length(), 0);
            int recvbytes = recv(newusingsocks[i], bufferp5, 1024, 0);
        }
        else {
            int buf;
            while ((buf = accept(sockfd, (struct sockaddr *)&address_new, (socklen_t*)&addrlen))<=0) {
                perror("Accept error");
                continue;
            }
            cout << "Accepted connection with " << newconnectionsneededfinal[i] << endl;
            int recvbytes = recv(buf, newbufferp5, 1024, 0);
            if (recvbytes > 0) {
                for (int j = 0; j < noofnewconnectionsneeded; j++) {
                    if (newconnectionsneededfinal[j] == newbufferp5[0]-'0') {
                        newusingsocks[j] = buf;
                    }
                }
            }
            send(buf, to_string(clientid).c_str(), to_string(clientid).length(), 0);
        }
    }
    

    vector<string> filestoasknew(noofnewconnectionsneeded);
    vector<vector<string> > filestoask2new(noofnewconnectionsneeded);
    int nooffilestoasknew[noofnewconnectionsneeded] = {0};
    for (int i = 0; i < noofnewconnectionsneeded; i++) {
        string temp = "0 ";
        int count = 0;
        vector<string> dummy;
        for (int j = 0; j < nooffilestosearch; j++) {
            if (wheretofind3[j] == newconnectionsneededfinal[i]) {
                count++;
                temp += filenamestosearch[j];
                temp += " ";
                dummy.push_back(filenamestosearch[j]);
            }
        }
        temp = to_string(count) + temp.substr(1);
        filestoasknew[i] = temp;
        filestoask2new[i] = dummy;
        nooffilestoasknew[i] = count;

    }

    char bufferp5read[1024] = {0};
    for (int iterator = 0; iterator < 2; iterator++ ) {
        for (int i = 0; i < noofnewconnectionsneeded; i++) {
            if (((clientid > newconnectionsneededfinal[i]) && (phase == 0)) || ((clientid < newconnectionsneededfinal[i]) && (phase == 1))) {
                int sentbytes = send(newusingsocks[i], filestoasknew[i].c_str(), filestoasknew[i].length(), 0);
                int cnt = nooffilestoasknew[i];
                int sizesoffiles[cnt] = {0};
                int filereceiving = 0;
                if (filestoasknew[i][0] != '0') {
                    while (filereceiving < cnt) {
                        if (sizesoffiles[filereceiving] == 0) {
                            int recvsize = recv(newusingsocks[i], bufferp5, 1024, 0);
                            if (recvsize > 0) {
                                string temp = "";
                                for (int j = 0; j < recvsize; j++) {
                                    temp += bufferp5[j];
                                }
                                send(newusingsocks[i], to_string(filereceiving).c_str(), to_string(filereceiving).length(), 0);
                                int remainingdata = stoi(temp);
                                string filepath  = "sample-data2/files/client" + to_string(clientid) + "/Downloaded/" + filestoask2new[i][filereceiving];
                                FILE* filepathdesc = fopen(filepath.c_str(), "w");
                                int rb = 0;
                                while (remainingdata > 0) {
                                    int bytesrecvd = recv(newusingsocks[i], bufferp5, 1024, 0);
                                    if (bytesrecvd > 0) {
                                        remainingdata -= bytesrecvd;
                                        rb += bytesrecvd;
                                        fwrite(bufferp5, sizeof(char), bytesrecvd, filepathdesc);
                                    }
                                }
                                fclose(filepathdesc);
                                temp = "5";
                                send(newusingsocks[i], temp.c_str(), 1 , 0);
                                sizesoffiles[filereceiving] = stoi(temp);
                            }
                        }
                        filereceiving++;
                    }
                    string ack = "6";
                    send(newusingsocks[i], ack.c_str(), 1, 0);
                }
                else {
                    char bufferp5dup[1];
                    recv(newusingsocks[i], bufferp5dup, 1, 0);
                }
            }
            else if (((clientid < newconnectionsneededfinal[i]) && (phase == 0)) || ((clientid > newconnectionsneededfinal[i]) && (phase == 1))) {
                int filerequestmsg = recv(newusingsocks[i], bufferp5, 1024, 0);
                if (filerequestmsg > 0) {
                    if (bufferp5[0] != '0') {
                        int x = 0;
                        string temp = "";
                        while (bufferp5[x] != ' ') {
                            temp += bufferp5[x];
                            x++;
                        }
                        x++;
                        int y = 0;
                        int nooffilestosend = stoi(temp);
                        temp = "";
                        while (y < nooffilestosend) {
                            while (bufferp5[x] != ' ') {
                                temp += bufferp5[x];
                                x++;
                            }
                            x++;
                            temp = "sample-data2/files/client" + to_string(clientid) + "/" + temp;
                            FILE* fp = fopen(temp.c_str(), "rb");
                            fseek(fp, 0L, SEEK_END);
                            int fsize = ftell(fp);
                            fclose(fp);
                            fp = fopen(temp.c_str(), "rb");
                            string msg = to_string(fsize);
                            send(newusingsocks[i], msg.c_str(), msg.length(), 0);
                            int filetosend = recv(newusingsocks[i], bufferp5, 1024, 0);
                            int remainingdata = fsize;
                            if (filetosend > 0) {
                                int sentbytes = 0;
                                off_t offset = 0;
                                while (remainingdata > 0) {
                                    fread(bufferp5read, sizeof(char), min(1024, remainingdata), fp);
                                    if ((sentbytes = send(newusingsocks[i], bufferp5read, min(1024, remainingdata), 0)) > 0) {
                                        remainingdata -= sentbytes; 
                                    }
                                    else {
                                        perror("Send failed");
                                    }
                                }
                                fclose(fp);
                                char bufferp5dup[1];
                                recv(usingsocks[i], bufferp5dup, 1, 0);
                            }
                            temp = "";
                            y++;
                        }
                        recv(newusingsocks[i], bufferp5, 1, 0);
                    }
                    else {
                        string temp = "5";
                        send(newusingsocks[i], temp.c_str(), 1, 0);
                    }
                }
            }
        }
        phase = (phase + 1)%2;
    }

    for (int i = 0; i < nooffilestosearch; i++) {
        string depth = "0";
        if (wheretofind2[i] > 0 && in(wheretofind2[i], neighbouruniqueids)) {
            depth = "1";
        }
        else if (wheretofind2[i] > 0 && !in(wheretofind2[i], neighbouruniqueids)) {
            depth = "2";
        }
        string path = "sample-data2/files/client"+ to_string(clientid) + "/Downloaded/" + filenamestosearch[i];
        char const *pathoffile = path.c_str();
        cout << "Found " << filenamestosearch[i] << " at " << wheretofind2[i] << " with MD5 "; printmd5(pathoffile); cout << " at depth " << depth << endl;
    }

    for (int i = 0; i < noofneighbour; i++) {
        close(usingsocks[i]);
    }

}