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
                    cout<<en->d_name<<endl; //printing the file name
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

    // cout<<clientid<<"  "<< clientportno<<endl;

    string tmp = to_string(clientuniqueid);
    tmp = to_string(clientid) + " " + tmp;
    char const *num_char = tmp.c_str();

    int checker[noofneighbour] = {0};

    int position = next(nids,clientid,noofneighbour);
    // int reqsocks[noofneighbour-position], newacceptsocks[position];
    // int reqrecieved[position] = {0}, reqvalread[noofneighbour-position] = {0};
    int usingsocks[noofneighbour], valuesread[noofneighbour];
    // int servervalread[position], recvvalueread[noofneighbour-position];
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
    
    

    while (check(checker,noofneighbour))
    {
        for (int i = 0; i < noofneighbour; i++)
        {
            if(checker[i]==0)
            {
                if (i<position)
                {
                    // if(checker[i]==0)
                    // {
                        // cout<<"Inside requests accepting loop for "<<neighbourids[i]<<endl;
                        if (!((usingsocks[i] = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0))
                        {
                            // if((valuesread[i] = recv( usingsocks[i] , buffer, 1024,0))<=0)		// server side msg reading
                            // {
                            //     close(usingsocks[i]);
                            //     continue;
                            // }
                            // else if (valuesread[i]==6)
                            // {
                            //     checker[i]=1;
                            //     printf("%s\n",buffer );
                            //     send(usingsocks[i] ,  num_char, 6 , 0 );
                            // }    
                            // cout<<"Inside requests accepting loop, Not inside checker but connection eshtablished, id value is "<<neighbourids[i]<<endl;
                            send(usingsocks[i] ,  num_char, 6 , 0 );
                            valuesread[i] = recv( usingsocks[i] , buffer, 1024,0);
                            if (valuesread[i]==6)
                            {
                                checker[i]=1;
                                printf("%s\n",buffer );
                                //cout<<"Inside checker, id value is "<< neighbourids[i] <<endl;
                            }    
                        }
                        //cout << usingsocks[i] << errno << " -accept- " << neighbourids[i] << endl;
                    //}
                }
                else
                {
                    // if (checker[i]==0)
                    // {
                        //cout<<"Inside requests sending loop for "<<neighbourids[i]<<endl;
                        // if ((usingsocks[i] = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	                    // {   
                        //     continue;
	                    // }
                        // serv_addr[i-position].sin_family = AF_INET;
	                    // serv_addr[i-position].sin_port = htons(neighbourports[i]);

                        // if(inet_pton(AF_INET, "127.0.0.1", &serv_addr[i-position].sin_addr)<=0)
	                    // {
		                //     printf("\nInvalid address/ Address not supported \n");
	                    // }

                        //cout << usingsocks[i] << endl;
                        if (!(connect(usingsocks[i], (struct sockaddr *)&serv_addr[i-position], sizeof(serv_addr[i-position])) < 0))
	                    {
		                    //printf("\nConnection Failed \n");
        	    	        //return -1;
                            // int noofbytessent = send(usingsocks[i] , num_char , 6 , 0 );
                            // if (noofbytessent == 6)
                            // {
                                //cout<<"Inside requests sending, loopNot inside checker but connection eshtablished, id value is "<<neighbourids[i]<<endl;
                                send(usingsocks[i] , num_char , 6 , 0 );
                                valuesread[i] = recv(usingsocks[i] , buffer, 1024,0);		// server side msg reading
	                            printf("%s\n",buffer );
                                if (valuesread[i]==6)
                                {
                                    checker[i]=1;
                                    //cout<<"Inside checker, id value is "<<neighbourids[i]<<endl;
                                }

                            // } 

	                    }
                   // }


                }
            }
            
            
        }
        
    }
    

    // if(position != 0)
    // {
    //     if (listen(sockfd, 5) < 0)		// checking for listen error
	//     {
	// 	    perror("listen");
	// 	    exit(EXIT_FAILURE);
	//     }

    //     // while (check(reqrecieved, position))
    //     // {
            
    //     // }
    //     for (int i = 0; i < position; i++)
    //     {
    //         if ((newacceptsocks[i] = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)	
	//         {
	// 	        perror("accept");
	// 	        exit(EXIT_FAILURE);
	//         }
    //         servervalread[i] = recv( newacceptsocks[i] , buffer, 1024,0);		// server side msg reading
	//         printf("%s\n",buffer );
    //         send(newacceptsocks[i] ,  num_char, 6 , 0 );
    //     } 
    // }

    // if((noofneighbour-position)!=0)
    // {
    //     for (int i = 0; i < noofneighbour-position; i++)
    //     {
    //         if ((reqsocks[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	//         {
	// 	        printf("\n Socket creation error \n");
	// 	        return -1;
	//         }
    //         serv_addr[i].sin_family = AF_INET;
	//         serv_addr[i].sin_port = htons(neighbourports[i+position]);

    //         if(inet_pton(AF_INET, "127.0.0.1", &serv_addr[i].sin_addr)<=0)
	//         {
	// 	        printf("\nInvalid address/ Address not supported \n");
	// 	        return -1;
	//         }

    //         if (connect(reqsocks[i], (struct sockaddr *)&serv_addr[i], sizeof(serv_addr[i])) < 0)
	//         {
	// 	        printf("\nConnection Failed \n");
    //     		return -1;
	//         }

    //         send(reqsocks[i] , num_char , 6 , 0 );
	//         // cout<<"hello msg sent to neighbour"<<neighbourids[i]<<" from client "<< clientid<< endl;
    //         recvvalueread[i] = recv( reqsocks[i] , buffer, 1024,0);		// server side msg reading
	//         printf("%s\n",buffer );
    //     }
        
    // }
    
}