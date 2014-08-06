
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "qlist.h"
#include "mlist.h"

using namespace std;

struct Client //Struct to pass arguments to pool workers
{
 int client_sock_;
 string client_dir_;
};

int queue_size;
QList *queue; //List nodes with filepath-socket-mutex info | Used as the main task queue
MList *stats; //List nodes with socket-filenumber-mutex info | Used to hold the file number of each socket and for deleting socket mutexes
pthread_mutex_t dir_mutex; //2 threads cannot read directories together
pthread_mutex_t q_mutex; //2 threads cannot process task queue
pthread_mutex_t s_mutex; //2 threads cannot process stats list
pthread_cond_t empty_cond; //Empty queue condition variable
pthread_cond_t full_cond; //Full queue condition variable

void ListDirectoryTree(string dir,int sock,pthread_mutex_t *mut,QList *q) //Recursively list requested directory tree
{
 DIR *directory;
 directory=opendir(dir.c_str());
 if(directory==NULL)
        {
         cout<<"{opendir error}"<<endl;
         return;
        }
 struct dirent *de;
 de=readdir(directory);
 while(de!=NULL)
        {
         int status;
         struct stat statbuf;
         string path;
         path=dir+"/"+de->d_name;
         status=stat(path.c_str(),&statbuf);
         if(status==-1)
                {
                 cout<<"{stat error}"<<endl;
                 return;
                }
         if(!(S_ISDIR(statbuf.st_mode)))
                {
                 q->Insert(path,sock,mut);
                 pthread_mutex_lock(&s_mutex);
                 stats->IncreaseFiles(sock);
                 pthread_mutex_unlock(&s_mutex);
                }
         if(S_ISDIR(statbuf.st_mode) && strcmp(de->d_name,".")!=0 && strcmp(de->d_name,"..")!=0) //If it is a directory different from . and ..
                {
                 ListDirectoryTree(path,sock,mut,q);
                }
         de=readdir(directory);
        }
 closedir(directory);
 return;
}

void *ClientHandler(void *c) //Thread that inserts filepaths to the queue
{
 pthread_detach(pthread_self());
 int sock=((struct Client *)c)->client_sock_;
 string dir=((struct Client *)c)->client_dir_;
 delete (struct Client *)c;
 cout<<"[Thread "<<pthread_self()<<" processing request for directory "<<dir<<" and socket "<<sock<<"]"<<endl;
 pthread_mutex_t *c_mutex;
 c_mutex=new pthread_mutex_t; //Createe a mutex respective to the socket
 pthread_mutex_init(c_mutex,NULL);
 pthread_mutex_lock(&s_mutex);
 stats->Insert(sock,0,c_mutex); //Add new node to store mutex and file number
 pthread_mutex_unlock(&s_mutex);
 QList *q;
 q=new QList;
 pthread_mutex_lock(&dir_mutex);
 ListDirectoryTree(dir,sock,c_mutex,q); //Count files and add them to a temporary queue
 pthread_mutex_unlock(&dir_mutex);
 QListNode *temp=q->head_;
 while(temp!=NULL) //Produce
        {
         pthread_mutex_lock(&q_mutex);
         while(queue->size_>=queue_size) //As long as the queue is full
                {
                 pthread_cond_wait(&full_cond,&q_mutex);
                }
         queue->Insert(temp->path_,sock,c_mutex);
         cout<<"[Thread "<<pthread_self()<<" added file "<<temp->path_<<" for socket "<<sock<<" to the queue]"<<endl;
         pthread_mutex_unlock(&q_mutex);
         pthread_cond_broadcast(&empty_cond);
         temp=temp->next_;
        }
 delete q;
 pthread_exit(NULL);
}

void *PoolWorker(void *arg) //Thread that sents files to the clients
{
 pthread_detach(pthread_self());
 while(1) //Consume
        {
         string path;
         int c_sock;
         pthread_mutex_t *c_mutex;
         pthread_mutex_lock(&q_mutex);
         while(queue->size_<=0) //As long as the queue is empty
                {
                 pthread_cond_wait(&empty_cond,&q_mutex);
                }
         path=queue->head_->path_; //Get head data
         c_sock=queue->head_->client_sock_; //Get head data
         c_mutex=queue->head_->client_mutex_; //Get head data
         queue->DeleteHead(); //Remove task from queue
         pthread_mutex_unlock(&q_mutex);
         pthread_cond_broadcast(&full_cond);
         pthread_mutex_lock(c_mutex);
         struct stat file;
         stat(path.c_str(),&file);
         ostringstream ps,fs;
         ps<<getpagesize();
         fs<<file.st_size;
         string message;
         message=message+'@'+path+'#'+ps.str()+'$'+fs.str()+'%'; //Create metadata
         write(c_sock,message.c_str(),message.length()); //Send metadata
         char *buffer;
         buffer=new char[getpagesize()];
         ifstream in_file(path.c_str(),ifstream::binary);
         while(!(in_file.eof()))
                {
                 in_file.read(buffer,getpagesize()); //Read from file
                 if(in_file.gcount()!=0)
                        {
                         write(c_sock,buffer,in_file.gcount()); //Write file content to socket
                        }
                }
         delete[] buffer;
         pthread_mutex_unlock(c_mutex);
         pthread_mutex_lock(&s_mutex);
         cout<<"[Thread "<<pthread_self()<<" sent file "<<path<<" through socket "<<c_sock<<"]"<<endl;
         stats->DecreaseFiles(c_sock); //File sent
         MListNode *temp=stats->head_;
         while(temp!=NULL) //Find node containing stats about the respective socket
                {
                 if(temp->client_sock_==c_sock)
                        {
                         if(temp->client_files_==0) //If that was the last file to be sent
                                {
                                 char c='!';
                                 write(c_sock,&c,1); //Tell client there are no more files
                                 pthread_mutex_destroy(temp->client_mutex_);
                                 delete temp->client_mutex_;
                                 stats->Delete(temp);
                                 char conf;
                                 read(c_sock,&conf,1); //Make sure client has read everythng before closing the socket
                                 close(c_sock);
                                 cout<<"[Thread "<<pthread_self()<<" sent last file through socket "<<c_sock<<"]"<<endl;
                                 break;
                                }
                        }
                 temp=temp->next_;
                }
         pthread_mutex_unlock(&s_mutex);
        }
 pthread_exit(NULL);
}

int main(int argc,char **argv)
{
 bool p_flag,s_flag,q_flag;
 int port,thread_pool_size;
 p_flag=false;
 s_flag=false;
 q_flag=false;
 if(argc!=7) //Start argument checking
        {
         cout<<"Invalid arguments."<<endl;
         return 0;
        }
 for(int i=0;i<3;i++)
        {
         if(strcmp(argv[2*i+1],"-p")==0)
                {
                 p_flag=true;
                 port=atoi(argv[2*i+2]);
                }
         else if(strcmp(argv[2*i+1],"-s")==0)
                {
                 s_flag=true;
                 thread_pool_size=atoi(argv[2*i+2]);
                }
         else if(strcmp(argv[2*i+1],"-q")==0)
                {
                 q_flag=true;
                 queue_size=atoi(argv[2*i+2]);
                }
        }
 if(p_flag==false || s_flag==false || q_flag==false)
        {
         cout<<"Invalid arguments."<<endl;
         return 0;
        } //Arguments are OK
 int listen_sock,serve_sock;
 struct sockaddr_in server,client;
 socklen_t clientlen;
 struct hostent *he;
 listen_sock=socket(PF_INET,SOCK_STREAM,0); //Initializing listening socket
 if(listen_sock<0)
        {
         cout<<"{socket error}"<<endl;
         return 0;
        }
 int option=1;
 if(setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,(char *)&option,sizeof(option))<0)
        {
         cout<<"{setsockopt error}"<<endl;
         return 0;
        }
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=htonl(INADDR_ANY);
 server.sin_port=htons(port);
 if(bind(listen_sock,(struct sockaddr *)&server,sizeof(server))<0)
        {
         cout<<"{bind error}"<<endl;
         return 0;
        }
 if(listen(listen_sock,32)<0)
        {
         cout<<"{listen error}"<<endl;
         return 0;
        } //Listening socket established
 queue=new QList();
 stats=new MList();
 pthread_mutex_init(&dir_mutex,NULL);
 pthread_mutex_init(&q_mutex,NULL);
 pthread_mutex_init(&s_mutex,NULL);
 pthread_cond_init(&empty_cond,NULL);
 pthread_cond_init(&full_cond,NULL);
 pthread_t *pool;
 pool=new pthread_t[thread_pool_size];
 for(int i=0;i<thread_pool_size;i++) //Create thread pool
        {
         pthread_create(pool+i,NULL,PoolWorker,NULL);
        }
 cout<<"[READY]"<<endl;
 while(1)
        {
         serve_sock=accept(listen_sock,(struct sockaddr *)&client,&clientlen); //Accept a connection
         if(serve_sock<0)
                {
                 cout<<"{accept error}"<<endl;
                 continue;
                }
         char c='!';
         char buffer[4096];
         read(serve_sock,buffer,4096);
         if(strcmp(buffer,"EXIT")==0) //If this is an exit request
                {
                 cout<<"[Terminating...]"<<endl;
                 write(serve_sock,&c,1);
                 break;
                }
         cout<<"[Assigning request for directory "<<buffer<<" to socket "<<serve_sock<<"]"<<endl;
         string s(buffer);
         struct Client *cl;
         cl=new Client;
         cl->client_sock_=serve_sock;
         cl->client_dir_=s;
         pthread_t *t;
         t=new pthread_t;
         pthread_create(t,NULL,ClientHandler,(void *)cl); //Create thread to handle request
        }
 delete pool;
 delete queue;
 delete stats;
 pthread_mutex_destroy(&dir_mutex);
 pthread_mutex_destroy(&q_mutex);
 pthread_mutex_destroy(&s_mutex);
 pthread_cond_destroy(&empty_cond);
 pthread_cond_destroy(&full_cond);
 close(listen_sock);
 exit(0);
}
