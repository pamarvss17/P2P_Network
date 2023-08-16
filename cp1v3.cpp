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

    int checker[noofneighbour] = {0};
    int checker2[noofneighbour] = {0};

    int position = next(nids,clientid,noofneighbour);
    cout << "Position " << position << endl; 
    //cout << position << endl;
    int usingsocksclient[noofneighbour], usingsocksserver[noofneighbour], valuesread[noofneighbour];
    //struct sockaddr_in serv_addr[noofneighbour-position];
    struct sockaddr_in serv_addr[noofneighbour];

    char buffer[1024] = {0};
    char buffer1[1024] = {0};

    int sockfd;
    int sockfd2;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)		// server side of socket
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
    sockfd2 = socket(AF_INET, SOCK_STREAM, 0);

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

    for (int i = 0; i < noofneighbour; i++)
    {
        usingsocksserver[i] = socket(AF_INET, SOCK_STREAM, 0);
        usingsocksclient[i] = socket(AF_INET, SOCK_STREAM, 0);
        serv_addr[i].sin_family = AF_INET;
	    serv_addr[i].sin_port = htons(neighbourports[i]);
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr)<=0)
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
                                usingsocksserver[j] = buf;
                                cout << "Accepted connection as server with " << neighbourids[j] << endl; 
                            }
                        }
                        privateids.push_back(buffer);
                    }  
                }
                else
                {
                    while ((connect(usingsocksclient[i], (struct sockaddr *)&serv_addr[i], sizeof(serv_addr[i])) < 0))
	                {
		                continue;
	                }
                    send(usingsocksclient[i] , num_char , 6 , 0 );
                    valuesread[i] = recv(usingsocksclient[i] , buffer, 1024,0);		// server side msg reading
                    privateids.push_back(buffer);
                    if (valuesread[i]==6)
                    {
                        checker[i]=1;
                        cout << "Connected to " << neighbourids[i] << " as client" << endl;
                    } 
                    
                }
            }
            
            
        }
        
    }

    while (check(checker2,noofneighbour))
    {
        for (int i = 0; i < noofneighbour; i++)
        {
            if(checker2[i]==0)
            {
                if (i>=position)
                {
                    int buf;
                    while (((buf = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0))
                    {
                        cout << "No" << endl;
                        continue;
                    }
                    send(buf ,  num_char, 6 , 0 );
                    valuesread[i] = recv( buf , buffer, 1024,0);
                    if (valuesread[i]==6)
                    {
                        int j = 0;
                        for (;j < noofneighbour; j++) {
                            if (neighbourids[j] == buffer[0]-'0') {
                                checker2[j] = 1;
                                usingsocksserver[j] = buf;
                                cout << "Accepted connection as server with " << neighbourids[j] << endl; 
                            }
                        }
                        privateids.push_back(buffer);
                    }  
                }
                else
                {
                    while ((connect(usingsocksclient[i], (struct sockaddr *)&serv_addr[i], sizeof(serv_addr[i])) < 0))
	                {
                        perror("Connect failed");
		                continue;
	                }
                    send(usingsocksclient[i] , num_char , 6 , 0 );
                    valuesread[i] = recv(usingsocksclient[i] , buffer, 1024,0);		// server side msg reading
                    privateids.push_back(buffer);
                    if (valuesread[i]==6)
                    {
                        checker2[i]=1;
                        cout << "Connected to " << neighbourids[i] << " as client" << endl;
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
        cout<< "Connected to "<<neighbourids[i]<<" with unique-ID "<<r<<" on port "<< neighbourports[i]<< " "  << endl;
    }
    // for (int i = 0; i < noofneighbour; i++) {
    //     close(usingsocks[i]);
    // }
}