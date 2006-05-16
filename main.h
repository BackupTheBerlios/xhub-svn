#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <errno.h>
#include <fcntl.h>


#define HUB_VERSION_NAME "xHub Linux"
#define __CURR_DATE_TIME__ "15.5.2006 16:04 UTC"
#define VERSION "0.2.5.6"
#define BUFSIZE 25240
#define TIMEOUT 180000
#define CAPACITY_INCREMENT 30000

#define MAX_NICK_LENGTH    75
#define MAX_MyINFO_LENGTH  400
#define MAX_DESC_LENGTH    128
#define MAX_TAG_LENGTH     85
#define MAX_EMAIL_LENGTH   75 
#define MAX_SOCKETS 	   30000
#define MAX_CONN_LENGTH    50 
#define MAX_NICK_LEN       100              	/* Maximum length of nickname */
#define MAX_HOST_LEN       121             	/* Maximum length of hostname */
#define MAX_VERSION_LEN    30              	/* Maximum length of version name */
#define MAX_MESS_SIZE      650             	/* Maximum size of a received message */
#define MAX_HUB_NAME       100              	/* Maximum length of hub name, 25 from win version */
#define MAX_HUB_DESC       150             	/* Maximum length of hub description */
#define MAX_PASS_LEN 	   50              	/* Maximum length of  pass */
#define MAX_BUF_SIZE       1000000         	/* Maximum length of users buf */
#define MAX_MASS_LEN	   2047			/* Maximum length of mass message */
#define MAX_REGS	   500
#define MAX_BANS 	   500

int mainSocket;
sockaddr_in sockName, clientInfo;
std::vector<int> sockets;
pollfd *ufds = NULL, *temp;
std::vector<int>::iterator i;
extern int errno;

/* Global variables */

int    listening_port;         			/* Port on which we listen for connections */
int    hub_users;
int    max_users;
int    max_sockets;
int    min_share;      				/* Minimum share for clients */
int    searchspam_time;
int    hh,mm,ss;	
int    spamid;
int    edition;
int    max_desc_len;
int    public_count;

long int traffic;
long int currtrafficUp;
long int currtrafficDown;

char   hub_description[MAX_HUB_DESC+1]; 	/* The description of hub that is uploaded to public hublist */
char   hub_redirect[MAX_HOST_LEN+1];		/* Redirect hub address */
char   hub_owner[MAX_NICK_LEN+1];       	/* Nickname of owner */ 
char   public_hub_host[MAX_HOST_LEN+1]; 	/* This is the hostname to upload hub description to */
char   min_version[MAX_VERSION_LEN+1];  	/* Minimum client verison to allow users to the hub. */
char   hub_hostname[MAX_HOST_LEN+1];    	/* This is the hostname that is uploaded to the public hublist */
char   redirect_host[MAX_HOST_LEN+1];   	/* Host to redirect users to if hub is full */
char   hub_full_mess[200];
char   hub_botname[MAX_NICK_LEN+1];             /* Nickname of bot */ 
char   hub_opchatname[MAX_NICK_LEN+1];          /* Nickname of opchat */ 
char   default_pass[MAX_PASS_LEN+1];
char   pinger_name[MAX_NICK_LEN+1];		/* Nickname of pinger */

std::string hub_name;				/* Name of the hub. */
std::string hub_motd;				/* Hub MOTD */
std::string hub_rules;				/* Hub Rules */
std::string hub_faq;				/* Hub FAQ */
std::string hub_banner;				/* Hub Banner */
std::string spambuf;				/* Deflood buffer */
std::string hub_help;				/* Hub Help */
std::string hub_help_op;			/* Hub Help for OPs*/	
std::string hub_help_admin;			/* Hub Help for ADMIN*/
std::string StartTime;				/* Hub starttime */

bool   search_enable;				/* command $Search */
bool   syslog_enable;				/* Logging communication */
bool   motd_enable;				/* MOTD send */
bool   usercommand_enable;			/* UserCommand send */
bool   quit;					/* Quit variable */

/*************************
	  STRUCT
**************************/

// Funktor pro odesilani textu.
struct SendText : public std::unary_function<int, void>
{
  std::string Text;
  int operator() (int socket)
  {
    if(int size = send(socket, Text.c_str(), Text.size(), 0) == -1)
	return 0;	
  }
};

typedef struct
{
	char nick[MAX_NICK_LENGTH];
	std::string desc;
	std::string conn;
	std::string TAG;
	std::string email;
	std::string MyINFO;
	std::string IPaddress;
	long int share;
	int LoginProc;
}USER;
USER User[MAX_SOCKETS];

typedef struct
{
	int    myinfo;
	int    validatenick;
	int    search;
	int    getnicklist;
	int    privatemessage;
	int    connecttome;
	int    revconnecttome;
}NUM;
NUM num;
NUM currlength;

typedef struct
{
	std::string nick;         /* Nickname of user */
}BANLIST;
BANLIST Ban[MAX_BANS];
int bannumber;

typedef struct
{
	std::string nick;         /* Nickname of user */
	std::string pass;         /* Password of user */
	int Class;
}REGLIST;
REGLIST Reg[MAX_REGS];
int number;

typedef struct
{
	bool uREGon;
	bool uVIPon;
	bool uOPon;
	bool uSOPon;
	bool uADMINon;

	std::string uREG;
	std::string uVIP;
	std::string uOP;
	std::string uSOP;
	std::string uADMIN;
}WMSG;
WMSG WelcomeMsg;
