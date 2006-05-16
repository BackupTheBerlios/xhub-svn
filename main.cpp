#include "main.h"

using namespace std;

/*************************
        CONSTRUCTOR
**************************/

int send_to_socket(int id, string txt);
int send_to_all(string txt);
int loop(int id, char buffer[BUFSIZE]);
int new_connect(int id);
int close_connect(int id);
int supports(int id, char buffer[BUFSIZE]);
int validate_nick(int id, char buffer[BUFSIZE]);
int myinfo(int id, char buffer[BUFSIZE]);
int getnicklist(int id, char buffer[BUFSIZE]);
int loginok(int id);
int chat(int id, char buffer[BUFSIZE]);
int connecttome(int id, char buffer[BUFSIZE]);
int search(char buffer[BUFSIZE]);
int privatemessage(int id, char buffer[BUFSIZE]);
int set_default_vars();
int load_config();
int load_reglist();
int load_banlist();
int load_message_file();
int load_files();
string load_file(char filename[80]);
int botinfo(int id, char buffer[BUFSIZE]);
int massmessage(char msg[2048]);
int commands(int id, char buffer[BUFSIZE]);
int log(int id, int mode, string buffer);
int antiraw(int id, char buffer[BUFSIZE]);
int kick(int id, char buffer[BUFSIZE]);
int opchat(int id, char buffer[BUFSIZE]);
int opforcemove(int id, char buffer[BUFSIZE]);


/*************************
	FUNCTIONS
**************************/

int init_socket()
{
  if(listening_port == 0)
  {
     std::cerr << "**ERROR** Bad port number" << std::endl;
     exit(EXIT_FAILURE);
     return -1;
  }

  if ((mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
  {
     std::cerr << "**ERROR** I cant create main socket" << std::endl;
     exit(EXIT_FAILURE);
     return -1;
  }

  // Nastavíme soket do neblokovacího re?imu
  int oldFlag = fcntl(mainSocket, F_GETFL, 0);
  if (fcntl(mainSocket, F_SETFL, oldFlag | O_NONBLOCK) == -1)
  {
    cerr << "**ERROR** Problem with set socket mode" << endl;
    return -1;
  }

  sockName.sin_family = AF_INET;
  sockName.sin_port = htons(listening_port); 
  sockName.sin_addr.s_addr = INADDR_ANY;
  if (bind(mainSocket, (sockaddr *)&sockName, sizeof(sockName)) == -1)
  {
     std::cerr << "**ERROR** Port of this server is used/deny" << std::endl;
     exit(EXIT_FAILURE);
     return -1;
  }
  if (listen(mainSocket, 256) == -1)
  {
    std::cerr << "**ERROR** I cant create front" << std::endl;
    exit(EXIT_FAILURE);
    return -1;
  }
  return 1;
}

int send_to_socket(int id, string txt)
{
     int size;
     if ((size = send(id, txt.c_str(), txt.size(), 0)) == -1)
     {
       cerr << "**ERROR** Problem with send any data (send_to_socket)" << endl;
       return 0;
     }
     else
     {
	log(id, 1, txt);
     }

     return 1;
}

int send_to_all(string txt)
{
	if(txt.size() > 0)
	{  
		//for_each(sockets.begin(), sockets.end(), t);
		for(int y = 1; y < (sockets.size()+1); y++)
		{
			if(!send_to_socket(ufds[y].fd, txt))
			{
				cerr << "**WARNING** send_to_all >> " << User[ufds[y].fd].nick << " >> " << ufds[y].fd << endl;
				//return 0;
			}
		}

		log(0, 1, txt);
	}
	else
	  return 0;
	
	return 1;
}


/* Set default variables, used if config does not exist or is bad */
int set_default_vars()
{  
   hub_name = "Unnamed hub";
   hub_users = 0;
   min_share = 0;
   max_users = 1000;
   max_desc_len = 100;
   listening_port = 4111;
   edition = 0;
   syslog_enable = false;
   motd_enable = true;
   usercommand_enable = false;
   search_enable = true;

   sprintf(public_hub_host, "dreamland.gotdns.org");
   sprintf(hub_hostname, "127.0.0.1");
   sprintf(hub_description, "xHub");
   sprintf(hub_full_mess, "Sorry, this hub is full at the moment");
   sprintf(hub_opchatname, "OP-Chat");
   sprintf(default_pass, "PASSWORD");
   sprintf(redirect_host, "127.0.0.1");
   sprintf(pinger_name, "{HubListPinger}");

   return 1;
}

/*************************
	  FILES
**************************/

/*int load_config()
{
	FILE *ptr;
	char s[200];

	ptr=fopen(".xHub/config","r");
	if(ptr!=NULL)
	{
		do
		{
			fgets(s,80,ptr);
			if(strcmp(s,"[hubname]\n"))
			{
				fgets(s,100,ptr);
				char hname[128] = "";
				memcpy (hname,s,strlen(s)-1);
				hub_name = hname;
			}
			if(strcmp(s,"[description]\n")==0)
			{
				fgets(s,128,ptr);
				char hdesc[128];
				memcpy (hdesc,s,strlen(s)-1);
				sprintf(hub_description, hdesc);
			}
			if(strcmp(s,"[address]\n")==0)
			{
				fgets(s,122,ptr);
				memcpy (hub_hostname,s,strlen(s)-1);
			}
			if(strcmp(s,"[hub_port]\n") == 0)
			{
				fgets(s,40,ptr);
				listening_port=atoi(s);
			}
			if(strcmp(s,"[hub_maxusers]\n")==0)
			{
				fgets(s,40,ptr);
				max_users=atoi(s);
			}
			if(strcmp(s,"[edition]\n")==0)
			{
				fgets(s,40,ptr);
				edition=atoi(s);
			}
			if(strcmp(s,"[log]\n")==0)
			{
				fgets(s,40,ptr);
				syslog_enable=atoi(s);
			}
			if(strcmp(s,"[motd]\n")==0)
			{
				fgets(s,40,ptr);
				motd_enable=atoi(s);
			}
			if(strcmp(s,"[usercommand]\n")==0)
			{
				fgets(s,40,ptr);
				usercommand_enable=atoi(s);
			}
			if(strcmp(s,"[redir_address]\n")==0)
			{
				char redir[122];
				fgets(s,122,ptr);
				memcpy (redir,s,strlen(s)-1);
				sprintf(redirect_host, redir);
			}
			if(strcmp(s,"[hublist]\n")==0)
			{
				fgets(s,122,ptr);
				memcpy (public_hub_host,s,strlen(s)-1);
			}
            		if(strcmp(s,"[bot_name] ")==0)
			{
				fgets(s,51,ptr);
				memcpy (hub_botname,s,strlen(s)-1);
			}
            		if(strcmp(s,"[owner_name]\n")==0)
			{
				fgets(s,51,ptr);
				memcpy (hub_owner,s,strlen(s)-1);
			}

		}while(!feof(ptr));
		fclose(ptr);
	}
	else
	{
        	std::cerr << "**WARNING** Config file not found" << std::endl;
		return 0;
	}
}*/

int load_config()
{	
	char minshare[30];

	ifstream fConfig(".xHub/config");
	
		fConfig >> hub_name;
		fConfig >> hub_description;
		fConfig >> listening_port;
		fConfig >> hub_hostname;
		fConfig >> max_users;
		fConfig >> minshare;
		fConfig >> syslog_enable;
		fConfig >> usercommand_enable;
		fConfig >> motd_enable;
		fConfig >> search_enable;
		fConfig >> edition;
		fConfig >> public_hub_host;
		fConfig >> pinger_name;
		fConfig >> redirect_host;
		fConfig >> hub_botname;
		fConfig >> hub_owner;

	fConfig.close();

	min_share = atoi(minshare)*1024*1024;

	return 1;
}

string load_file(char filename[128])
{
	char s[1000];

	string text;

	FILE *ptr;
	ptr=fopen(filename,"r");
	if(ptr!=NULL)
	{
		do
		{
			fgets(s,1000,ptr);
			text += s;

		}while(!feof(ptr));

		fclose(ptr);
	}
	return text;

}

int load_message_file()
{
	FILE *ptr;
	char s[40];

	ptr=fopen(".xHub/message","r");
	if(ptr!=NULL)
	{
		do
		{
			fgets(s,40,ptr);
			if(strcmp(s,"[REG]\n")==0)
			{
				fgets(s,40,ptr);
				WelcomeMsg.uREGon=atoi(s);
			}
			if(strcmp(s,"[VIP]\n")==0)
			{
				fgets(s,40,ptr);
				WelcomeMsg.uVIPon=atoi(s);
			}
			if(strcmp(s,"[OP]\n")==0)
			{
				fgets(s,40,ptr);
				WelcomeMsg.uOPon=atoi(s);
			}
			if(strcmp(s,"[MASTER]\n")==0)
			{
				fgets(s,40,ptr);
				WelcomeMsg.uSOPon=atoi(s);
			}
            		if(strcmp(s,"[ADMIN]\n")==0)
			{
				fgets(s,40,ptr);
				WelcomeMsg.uADMINon=atoi(s);
			}

		}while(!feof(ptr));
		fclose(ptr);
	}

	return 1;
}

int load_files()
{
	hub_motd       	  = load_file(".xHub//motd");
	hub_help       	  = load_file(".xHub//help");
	hub_help_op    	  = load_file(".xHub//help_op");
	hub_help_admin 	  = load_file(".xHub//help_admin");
	hub_rules         = load_file(".xHub//rules");
	hub_faq           = load_file(".xHub//faq");
	hub_banner        = load_file(".xHub//banner");

        WelcomeMsg.uREG   = load_file(".xHub//messages//reg");
        WelcomeMsg.uVIP   = load_file(".xHub//messages//vip");
        WelcomeMsg.uOP    = load_file(".xHub//messages//op");
        WelcomeMsg.uSOP   = load_file(".xHub//messages//master");
        WelcomeMsg.uADMIN = load_file(".xHub//messages//admin");

	load_config();
	load_reglist();
	load_banlist();
	load_message_file();

	return 1;
}

void readstr(FILE *f,char *string)
{
	do
	{
		fgets(string, 255, f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

int load_reglist()
{
	FILE *filein;
	char oneline[255];
	int regnum;
	char Name[40],Pass[40];
	int Class;

	filein = fopen(".xHub/reglist", "rt");

	readstr(filein,oneline);
	sscanf(oneline, "regnum %d\n", &regnum);
	number = regnum;

	for (int loop = 0; loop < regnum; loop++)
	{

		readstr(filein,oneline);
		sscanf(oneline, "%s %s %d", &Name, &Pass, &Class);
		
		Reg[loop].nick = Name;
		Reg[loop].pass = Pass;
		Reg[loop].Class = Class;

	}

	fclose(filein);

	return 1;
}

int load_banlist()
{
	FILE *filein;
	char oneline[255];
	int bannum;
	char Name[51];

	filein = fopen(".xHub/banlist", "rt");

	readstr(filein,oneline);
	sscanf(oneline, "bannum %d\n", &bannum);
	bannumber = bannum;

	for (int loop = 0; loop < bannum; loop++)
	{
		readstr(filein,oneline);
		sscanf(oneline, "%s", &Name);
		
		Ban[loop].nick = Name;
	}

	fclose(filein);

	return 1;
}

/*************************
	   MAIN
**************************/

int main(int argc, char *argv[])
{
  unsigned int count = 0, maxAlloc = 0, changed, p;

  set_default_vars();
  load_files();
  init_socket();

  // Get a Time (APP run at)	
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  StartTime = asctime (timeinfo);

  cout << "Hub StartTime: " << StartTime << "---------------------------------------" << endl << endl;
	
  // Start of Log
  if(syslog_enable)
  {
	string str;
	str += "\n---------------------------------------------\n";

	char runtime[150];
	sprintf(runtime, "%s", asctime (timeinfo));
	str += runtime;

	str += "---------------------------------------------";

	FILE *flog;
	flog=fopen(".xHub//log","a");					
	fputs (str.c_str(), flog);
	fclose(flog);
  }

  do
  {
    if (count != sockets.size() + 1)
    {
      count = sockets.size() + 1;
      public_count = count;
      if (maxAlloc < sockets.size() + 1)
      {
	if(maxAlloc > (sockets.size() - 1 + CAPACITY_INCREMENT))
	{
		cerr << "**ERROR** CAPACITY_INCREMENT is overstep" << endl;
		exit(EXIT_FAILURE);
		return -1;
	}

	maxAlloc = sockets.size() + 1 +  CAPACITY_INCREMENT;

	temp = ufds;
	if ((ufds = (pollfd *)realloc(ufds, maxAlloc)) == NULL)
	{
	  // Zavru hlavni soket
	  close(mainSocket);
	  // Vsechny ostatni sokety take uzavru.
	  for_each(sockets.begin(), sockets.end(), close);
	  free(temp);
	  cerr << "**ERROR** Problem with memory" << endl;
	  exit(EXIT_FAILURE);
	  return -1;
	}
      }
      ufds[0].fd = mainSocket;
      ufds[0].events = POLLIN;
      ufds[0].revents = 0;
      for(i = sockets.begin(), p = 1; i != sockets.end(); i++, p++)
      {
	ufds[p].fd = *i;
	ufds[p].events = POLLIN | POLLPRI;
	ufds[p].revents = 0;
      }
    }
    if ((changed = poll(ufds, count, TIMEOUT)) < 0)
    {
      // Zavru hlavni soket
      close(mainSocket);
      // Vsechny ostatni sokety take uzavru.
      for_each(sockets.begin(), sockets.end(), close);
      free(ufds);
      cerr << "**ERROR** Problem with poll" << endl;
      exit(EXIT_FAILURE);
      return -1;
    }
    // Je nejaka zmena na "hlavnim" soketu?
    if (ufds[0].revents & POLLIN != 0)
    {
      socklen_t addrlen = sizeof(clientInfo);
      // Prijimam nove spojeni
      int client = accept(mainSocket, (sockaddr*)&clientInfo, &addrlen);
      sockets.push_back(client);

      if(client > 0 && new_connect(client))
      {
      	cout << "New connection >> " << inet_ntoa((in_addr)clientInfo.sin_addr) << " >> " << client <<  endl;
	User[client].IPaddress = inet_ntoa((in_addr)clientInfo.sin_addr);
	hub_users ++;
      }
      else
	cerr << "New connection is failed >> " << inet_ntoa((in_addr)clientInfo.sin_addr) << " >> " << client <<  endl;
    }
    // Projdu vsechna spojeni a zkontroluji, jestli na nich nedoslo k zmene
    for(p = 1; p < count; p++)
    {
      if (ufds[p].revents & (POLLIN | POLLPRI) != 0)
      {
	char buffer[BUFSIZE];
	int lenght;
	if ((lenght = recv(ufds[p].fd, (void *)buffer, BUFSIZE - 256, 0)) <= 0)
	{
	  // Prijato 0 bytu - druha strana uzavrela spojeni
	  close(ufds[p].fd);
	  sockets.erase(find(sockets.begin(), sockets.end(), ufds[p].fd));
	  close_connect(ufds[p].fd); 
	  // Ukoncim cyklus. Neni dobre pokracovat v prochazeni kontejneru, kdyz jsem vymazal prvek.
	  break;
	}
	else
	{
	  buffer[lenght] = 0;
	  // Nastavim text pro odeslani.
	  if ((ufds[p].revents & POLLPRI) != 0)
	  {
	    // Urgentni data
	  }
	  else
	  {
	    loop(ufds[p].fd, buffer);
	  }

	}
	ufds[p].revents = 0;
      }
    }
  } while (changed > 0);
  // Zavru hlavni soket
  close(mainSocket);
  // Vsechny ostatni sokety take uzavru.
  for_each(sockets.begin(), sockets.end(), close);
  cout << "**WARNING** Hub go down (shutdown)";
  exit(1);
}

/*************************
	  LOOP
**************************/

int loop(int id, char buffer[BUFSIZE])
{

	// Loop of socket communication
	//supports(id, buffer);		// EXTENDEDPROTOCOL
	cout << buffer << endl;
	
	if(antiraw(id, buffer) == 0){
	chat(id, buffer);
	commands(id, buffer);
	validate_nick(id, buffer);
	getnicklist(id, buffer);
	myinfo(id, buffer);
	privatemessage(id, buffer);
	opchat(id, buffer);
	connecttome(id, buffer);
	search(buffer);
	opforcemove(id, buffer);
	botinfo(id, buffer);
	kick(id, buffer);}
	log(id, 0, (string)buffer);

	return 1;
}

/*************************
       SOCKET WORK
**************************/

int new_connect(int id)
{
	string str;
	str = "$Lock Linux Pk=xhub|";
	str += "<";str += hub_botname;str += "> ";str += "Hub is running version ";
	str += VERSION;if(edition != 0){str += "BE ";}str += " (";str += __CURR_DATE_TIME__;str += ") of ";
	str += HUB_VERSION_NAME; str += " produced by TomSoft|";
	

	if(send_to_socket(id, str))
	{
	  User[id].LoginProc = 1;
	}
	else
	{
	  cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
	  return 0;	
	}
}

int close_connect(int id)
{
	if(strlen(User[id].nick) > 1)
	{
		string str   ("$Quit ");
		str += User[id].nick;
		str += "|";
	
		if(!send_to_all(str))
		{
			cerr << "**ERROR** Problem with send data (close_connect) >> " << User[id].nick << " >> " << id << endl;
			return 0;	
		}

		User[id].nick[1] = '\0';

		hub_users --;
	}
}

int supports(int id, char buffer[BUFSIZE])
{
	// $Supports (EXTENDEDPROTOCOL)
	if(User[id].LoginProc == 1)
	{
		char TextSupports[] = "$Supports";
		char *tobuf;
		char *tobuf_new;
		if ((tobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
		{ 	
			printf("**ERROR** Low memory (supports)");
			return 0;
		}
		tobuf_new = tobuf;
		tobuf = strstr(buffer,TextSupports);	

		if(tobuf)
		{	char a[15],b[20];
			sscanf(tobuf, "%s %s", (char*)&a, (char*)&b);
			
			if(strlen(a) > 1 && strlen(b) > 1)
			{
			
				if(send_to_socket(id, "$Supports QuickList|"))
				{
				  User[id].LoginProc == 2;
				}
				else
				{
				  cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
				  return 0;	
				}	
			}
			else
				User[id].LoginProc == 1;
		}

		free(tobuf_new);
		
	}

	return 1;
}

int validate_nick(int id, char buffer[BUFSIZE])
{
	// $Supports
	if(User[id].LoginProc == 1)
	{
		char TextVN[] = "$ValidateNick";
		char *tobuf;
		char *tobuf_new;
		if ((tobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
		{ 	
			printf("**ERROR** Low memory (validate_nick)");
			return 0;
		}
		tobuf_new = tobuf;
		tobuf = strstr(buffer,TextVN);	

		if(tobuf != NULL)
		{	
			//Calculate stats
			num.validatenick ++;
			currlength.validatenick += strlen(tobuf);

			char nick[128];
			if(strlen(tobuf) < (15 + MAX_NICK_LENGTH))
				memcpy(nick, tobuf+14, strlen(tobuf)-15);		// Remove char | and $ValidateNick
			nick[strlen(tobuf)-15] = '\0';

			string pingernck = nick;


			if(strlen(nick) > 1 && strlen(nick) < MAX_NICK_LENGTH)
			{
				if(hub_users <= max_users)
				{	
					int log = 0,log2 = 0;


					for(int p = 1; p < public_count; p++)
					{
						log2 ++;
						if(pingernck != User[ufds[p].fd].nick)
							log ++;
					}

					if(log == log2)
					{
						string str("$Hello ");
						str += nick;
						str += "|";
			
						if(send_to_socket(id, str))
						{
							User[id].LoginProc = 2;
							//return 1;
						}
						else
						{
							cerr << "**ERROR** Problem with send data (validate_nick) >> " << nick << " >> " << id << endl;
							return 0;	
						}
					}
					else
					{
						string str;
						str = "$ValidateDenide|$ForceMove ";
						str += redirect_host;str += "|";

						if(!send_to_socket(id, str))
							return 0;

						User[id].nick[1] = '\0';
					}
				}
				else
				{	
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += hub_full_mess;str += "|$ForceMove ";
					str += redirect_host;str += "|";
				
					if(!send_to_socket(id, str))
						return 0;
				}
				
			}
			else
				User[id].LoginProc = 1;
		}

		free(tobuf_new);
		

	}

		// $MyPass
		char TextMyPass[] = "$MyPass";
		char v1[100],v2[250];
		char *tobuf2;
		char *tobuf2_new;
		if ((tobuf2 = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
		{ 	
			printf("**ERROR** Low memory (MyPass)");
			return 0;
		}
		tobuf2_new = tobuf2;
		tobuf2 = strstr(buffer,TextMyPass);
	
		if (tobuf2)
		{	//sprintf (v2, "");
			sscanf(tobuf2,"%s %s", &v1, &v2);
			string sv1, sv2;
			sv1 = v1;sv2 = v2;
	
			for (int loop = 0; loop < number; loop++)
			{
				string pass;
				pass = Reg[loop].pass;
				pass += "|";
				if(sv1 == "$MyPass")
				{
					if(Reg[loop].nick == User[id].nick && sv2 == pass)
					{	
						//User[id].LoginProc = 3;
						string ok;
						ok += "<";ok += hub_botname;ok += "> ";
						ok += "Login OK|";
						
						send_to_socket(id, ok);
					}
					if(Reg[loop].nick == User[id].nick && sv2 != pass)
					{
						string str;
						str = "$BadPass|$ForceMove ";
						str += redirect_host;str += "|";
						send_to_socket(id, str);
					}
				}
				//if(Reg[loop].nick != User[id].nick)
				//{
				//	string Hello("$Hello ");
				//	Hello += User[id].nick;Hello += "|";
				//
				//	send_to_socket(id,Hello);			
				//}
	
			}
		}

		free(tobuf2_new);

	return 1;
}

int myinfo(int id, char buffer[BUFSIZE])
{

	if(User[id].LoginProc == 2)
	{
		char TextMyINFO[] = "$MyINFO";
		char *tobuf;
		char *tobuf_new;
		if ((tobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
		{ 	
			printf("**ERROR** Low memory (myinfo)");
			return 0;
		}
		tobuf_new = tobuf;
		tobuf = strstr(buffer,TextMyINFO);	

		if(tobuf != NULL)
		{
			char c1[150], c2[150], newnick[356];
			string s2 = c2;
			sscanf(tobuf, "%s %s %s", &c1, &c2, &newnick);

			cout << "User -> Nick " << newnick << ", Socket " << id << " => sent $MyINFO" << endl;
	
			if(s2 == "$ALL" && strlen(newnick) > 1 && strlen(newnick) < MAX_NICK_LENGTH)
			{
				string strx;
				strx += "$ForceMove ";strx += redirect_host;strx += "|";

				if(!send_to_socket(id, strx))
					return 0;

				return 0;
			}

			strcpy(User[id].nick, newnick);

			// Disconnect banned user
			for (int loop = 0; loop < bannumber; loop++)
			{
				if(Ban[loop].nick == User[id].nick || Ban[loop].nick == User[id].IPaddress)
				{
					string banmsg;
					banmsg += "<";banmsg += hub_botname;banmsg += "> ";
					banmsg += "Your nick/IP address is banned !|$ForceMove ";
					banmsg += redirect_host;banmsg += "|";

					if(!send_to_socket(id, banmsg))
					{
						cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
						send_to_socket(id, banmsg);	
					}	
				}
			}

			for (int loop = 0; loop < number; loop++)
			{	
				string reg;
				reg = Reg[loop].nick;	
				
				if(strlen(User[id].nick) > 0 && strlen(User[id].nick) < MAX_NICK_LENGTH-1)
				{
					string my;
					my = User[id].nick;

					if(reg == my)
					{  
						string pass;
						pass += "<";pass += hub_botname;pass += "> Your nick is registered, please type password ... (Default: 'PASSWORD')|$GetPass|";
		
						if(!send_to_socket(id, pass))
						{
							cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
							send_to_socket(id, pass);	
						}					
					}
				}
			}


			/* Bandwidth Edition */
			/*User[id].MyINFO = "$MyINFO $ALL ";
			User[id].MyINFO += User[id].nick;
			User[id].MyINFO += " $ $?$?$0$|";*/
	
			/* Normal Edition */
	
			//DESC & TAG

		if(edition == 0 || edition == 1)
		{
			char str[650] = "";

			if(strlen(tobuf) < 400)
				strcpy(str, tobuf);
			else
			{
				string str2;
				str2 +="<";str2 += hub_botname;str2 += "> Your MyINFO was wrong length !|$ForceMove ";str2 += redirect_host;str2 += "|";
				
				if(!send_to_socket(id, str2))
				{
					cerr << "**ERROR** Problem with send data (myinfo_wronglength) >> " << User[id].nick << " >> " << id << endl;
					return 0;
				}
			
				//close_connect(id);
				//close(id);

				printf("**WARNING** $MyINFO has bad length");
				return 0;
			}
	
			//Calculate stats
			num.myinfo ++;
			currlength.myinfo += strlen(str);

			char * pch2;
			char * pch2_new;
			if ((pch2 = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
			{ 	
				printf("**ERROR** Low memory (myinfo_DESC & TAG)");
				return 0;
			}
			pch2_new = pch2;
			int tagstart = 0;
	
			pch2=strchr(str,'<');
	
			int t = 0;
			while ( pch2!=NULL && t == 0)
			{
				t = 1;
				tagstart = pch2-str+1;
	
				pch2=strchr(pch2+1,'<');
			}
			free(pch2_new);
	
			// MyINFO
			
			char * pch;
			char * pch_new;
			if ((pch = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
			{ 	
				printf("**ERROR** Low memory (myinfo_MyINFO)");
				return 0;
			}
			pch_new = pch;
			int n = 0;
			
			int sharestart = 0;
			int shareend = 0;
			int descstart = 0;
			int descend = 0;
			int emailstart = 0;
			int emailend = 0;
			int connstart = 0;
			int connend = 0;
	
			pch=strchr(str,'$');
	
			while ( pch!=NULL && n <= 7 )
			{
				n ++;
	
				if(n == 2)
					descstart = pch-str+1;
				if(n == 3)
					descend = pch-str+1;
				if(n == 4)
					connstart = pch-str+1;
				if(n == 5)
					connend = pch-str+1;
				if(n == 5)
					emailstart = pch-str+1;
				if(n == 6)
					emailend = pch-str+1;
				if(n == 6)
					sharestart = pch-str+1;
				if(n == 7)
					shareend = pch-str+1;
	
				pch=strchr(pch+1,'$');
			}

			free(pch_new);

			char share[150] = "";
			char desctag[250] = "";
			char email[100] = "";
			char conn[100] = "";
			char desc[250] = "";
			char tag[150] = "";


			if(sharestart > 0 && shareend > 0 && (shareend-sharestart-1) < 70 && sharestart < (shareend-1))
				memcpy (share,str+sharestart,shareend-sharestart-1);		// Share size

		if(edition != 1)
		{		
			if(emailstart > 0 && emailend > 0 && (emailend-emailstart-1) < 70 && emailstart < (emailend-1))
				memcpy (email,str+emailstart,emailend-emailstart-1);		// E-Mail
	
			if(connstart > 0 && connend > 0 && (connend-connstart-1) < 70 && connstart < (connend-1))
				memcpy (conn,str+connstart,connend-connstart-1);		// Connection type
			
			if(tagstart > 0 && descend > 0 && (descend-tagstart) < 70 && tagstart < (descend+1))
				memcpy (tag,str+tagstart-1,descend-tagstart);			// TAG
	

			if(tagstart > 0)
			{
				if(descend > 0 && descstart < (tagstart-1))
					memcpy (desc,str+descstart,tagstart-descstart-1);		// Description
			}
			else
			{
				if(descend > 0 && descstart > 0 && descstart < (descend-1))
					memcpy (desc,str+descstart,descend-descstart-1);		// Description
			}
		}

			
	
		if(edition == 0)
		{	
			char myinfo[610];

			sprintf(myinfo, "$MyINFO $%s$ $%s$%s$%s$|", &desc, &conn, &email, &share);

			// To user
			if(strlen(myinfo) < MAX_MyINFO_LENGTH)
				User[id].MyINFO = myinfo;
		}
		else
		{
			User[id].MyINFO = "$MyINFO $ALL ";
			User[id].MyINFO += User[id].nick;
			User[id].MyINFO += " $ $?$?$";
			User[id].MyINFO += share;
			User[id].MyINFO += "$|";
		}

			if(strlen(share) < 50)
				User[id].share = atoi(share);	// prevedeme share do ciselne hodnoty int
			
			if(strlen(desc) < MAX_DESC_LENGTH)
				User[id].desc = desc;

			if(strlen(tag) < MAX_TAG_LENGTH)
				User[id].TAG = tag;

			if(strlen(conn) < MAX_CONN_LENGTH)
				User[id].conn = conn;

			if(strlen(email) < MAX_EMAIL_LENGTH)
				User[id].email = email;

		}
		if(edition != 0 && edition != 1)
		{
			User[id].MyINFO = "$MyINFO $ALL ";
			User[id].MyINFO += User[id].nick;
			User[id].MyINFO += " $ $?$?$0$|";
		}

			// Anti-MLDC
			string antimldc("ALL ");
			       antimldc += User[id].nick;
			       antimldc += " ";
			       antimldc += User[id].desc;

			string mldcstr("ALL ");
			       mldcstr += User[id].nick;
			       mldcstr += " mldc client";

			string mldcstr2("ALL ");
			       mldcstr2 += User[id].nick;
			       mldcstr2 += " MLDonkey mldc client";
			      
			if(antimldc == mldcstr || antimldc == mldcstr2)
			{	
				//close_connect(id);
				//close(id);
				string strc;
				strc += "$ForceMove ";strc += redirect_host;strc += "|";

				if(!send_to_socket(id, strc))
					return 0;
			}
			else
			{
				if(send_to_all(User[id].MyINFO))
				{
					User[id].LoginProc = 3;
					loginok(id);
					//return 2;
				}
				else
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					return 0;	
				}
	
				for (int loop = 0; loop < number; loop++)
				{	
					string reg;
					reg = Reg[loop].nick;
	
					string my;
					my = User[id].nick;	
							
					if(reg == my && Reg[loop].Class > 2)
					{
						string op("$OpList ");
						op += my;
						op += "$$|";
	
						if(!send_to_all(op))
						{
							cerr << "**ERROR** Problem with send data >> " << endl;
							send_to_all(op);	
						}					
					}
				}
			}
			
		}

		//tobuf = tobuf_new;
		free(tobuf_new);
		
		
	}

	return 1;
}

int getnicklist(int id, char buffer[BUFSIZE])
{
	char TextGetNickList[] = "$GetNickList";
	char *tobuf;
	if ((tobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (getnicklist)");
		return 0;
	}
	char *tobuf_new;
	tobuf_new = tobuf;
	tobuf = strstr(buffer,TextGetNickList);

	
	if (tobuf != NULL)
	{	

		//Calculate stats
		num.getnicklist ++;
		currlength.getnicklist += strlen(tobuf);

		// User list
		/*string NickList("$NickList ");
		for(std::vector<int>::iterator y = sockets.begin(); y != sockets.end(); y++)
		{
			
			NickList += User[*y].nick;
			NickList += "$$";


			if(y == sockets.end()-1)
			{
				NickList += "|";
				if(!send_to_socket(id, NickList))
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					send_to_socket(id, NickList);	
				}				
			}
			
		}*/
	

		// Bot + Op-Chat MyINFO
		string bot("$MyINFO $ALL ");
		bot += hub_botname;
		bot += " Hub-Security$ $BOT $$0$|";
		bot += "$MyINFO $ALL ";
		bot += hub_opchatname;
		bot += " ChatRoom $ $BOT $$0$|";

		if(!send_to_socket(id, bot))
		{
			cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
			send_to_socket(id, bot);
		}		
		
		// Users MyINFO
		for(int c = 1; c < public_count; c++)
		{
			if(ufds[c].fd != id)
			{
				string str;
				str += User[ufds[c].fd].MyINFO;
			
				if(!send_to_socket(id, str))
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					send_to_socket(id, str);
				}
			}

		}

		// $OpList
		string op("$OpList ");
		op += hub_botname;
		op += "$$";
		op += hub_opchatname;
		op += "$$";

		for (int loop = 0; loop < number; loop++)
		{	
			string reg;
			reg = Reg[loop].nick;

			if(Reg[loop].Class > 2)
			{
				for(int o = 1; o < public_count; o++)
				{
					string in;
					in = User[ufds[o].fd].nick;
						
					if(reg == in)
					{
						op += in;
						op += "$$";
					}
				}
			}
		}				

		op += "|";
		if(!send_to_socket(id, op))
		{
			cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
			send_to_socket(id, op);
		}
	}

	
	free(tobuf_new);


	return 1;
}

int loginok(int id)
{
	if(User[id].LoginProc == 3)
	{
		User[id].LoginProc = 4;

		string str;
		str = "$HubName ";str += hub_name;str += " - ";str += hub_description;str += "|";
	
		if(!send_to_socket(id, str))
		{
			cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
			send_to_socket(id, str);
		}	

		if(motd_enable)
		{
			str  = "<MOTD>\n";
			str += hub_motd;
			str += "|";
			
			if(!send_to_socket(id, str))
			{
				cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
				User[id].LoginProc = 3;
				loginok(id);
				return 0;
			}
		}

		if(usercommand_enable)
		{
			str  = "|$UserCommand 1 3 ";str += hub_botname;str += "\\? Help$<%[mynick]> +help&#124;|";
			str += "|$UserCommand 1 3 ";str += hub_botname;str += "\\? Rules$<%[mynick]> +rules&#124;|";
			str += "|$UserCommand 1 3 ";str += hub_botname;str += "\\? Motd$<%[mynick]> +motd&#124;|";
			str += "|$UserCommand 1 3 ";str += hub_botname;str += "\\? FAQ$<%[mynick]> +faq&#124;|";
			
			if(!send_to_socket(id, str))
			{
				cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
				User[id].LoginProc = 3;
				loginok(id);
				return 0;
			}
		}

	}
	
}

int chat(int id, char buffer[BUFSIZE])
{
	if(User[id].LoginProc >= 3)
	{
		if(strlen(buffer) < MAX_MESS_SIZE)
		{
			string buf = buffer;
			
			char TextCmd[] = "> +";
			char *Cmdtobuf;
			if ((Cmdtobuf = (char*)malloc(strlen(buffer)*sizeof(char)))==NULL)
			{ 	
				printf("**ERROR** Low memory (TextCmd)");
				return 0;
			}
			char *Cmdtobuf_new;
			Cmdtobuf_new = Cmdtobuf;
			Cmdtobuf = strstr(buffer,TextCmd);
			
			if(!Cmdtobuf)
			{

				char TextDolar[] = "$";
				char *Dolartobuf;
				char *Dolartobuf_new;
				if ((Dolartobuf = (char*)malloc(strlen(buffer)*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory (Dolartobuf)");
					return 0;
				}
				Dolartobuf_new = Dolartobuf;
				Dolartobuf = strstr(buffer,TextDolar);
			
				char TextChar[] = "<";
				char *Chartobuf;
				char *Chartobuf_new;
				if ((Chartobuf = (char*)malloc(strlen(buffer)*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory (Chartobuf)");
					return 0;
				}
				Chartobuf_new = Chartobuf;
				Chartobuf = strstr(buffer,TextChar);


				if(!Dolartobuf && Chartobuf && strlen(User[id].nick) > 1)
				{
					// Deflood
					string sbuf;
					sbuf = buffer;
					char flood[1024];
					char oldflood[1024];

					memcpy(flood, buffer+strlen(User[id].nick)+3, strlen(buffer)-strlen(User[id].nick)-3);
					string sflood,soldflood; 
					if(sflood == soldflood)
						spamid ++;
					else
						spamid = 0;
	
					if(sbuf == spambuf)
						spamid ++;
					else
						spamid = 0;
					
					if(spamid == 0)
					{
						// Send to all (chat)
						if(!send_to_all(buf))
							return 0;
					}
				
					if(spamid == 4)
					{	
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "User ";str += User[id].nick;str += " sent a same text (FLOOD) -> ";
						str += sbuf;str += "|";
						send_to_all(str);
					}

					spambuf = sbuf;
					soldflood = sflood;	
				}

				free(Dolartobuf_new);
				free(Chartobuf_new);
					

				
				
			}
			else	// User typed any command
			{

				// Deflood
				string sbuf;
				sbuf = buffer;
	
				if(sbuf == spambuf)
					spamid ++;
				else
					spamid = 0;
					
				if(spamid == 0)
				{
					if(strlen(Cmdtobuf) < 256)
					{
						// Command structure
						char cnick[64],ccmd1[256];
						sscanf(buffer, "%s %s", &cnick, &ccmd1);
		
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "User ";str += User[id].nick;str += " sent a command: ";
						str += ccmd1;str += "|";
		
						if(!send_to_all(str))
							return 0;
					}	
				}
				
				if(spamid == 2)
				{	
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "User ";str += User[id].nick;str += " sent a same text (SPAM) -> ";
					str += sbuf;str += "|";
					send_to_all(str);
				}

				spambuf = sbuf;	
			}

			free(Cmdtobuf_new);
			
		}
		else
		{

			if(strlen(buffer) > 0)
			{
				string str;
				str +="<";str += hub_botname;str += "> Message was wrong length ";
				char lmsg[50];sprintf(lmsg, "(%d/%d)|",(strlen(buffer)-strlen(User[id].nick)-4), MAX_MESS_SIZE);str += lmsg;
				
				if(!send_to_socket(id, str))
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					return 0;
				}
			}	
		}
	}

	return 1;
}

int connecttome(int id, char buffer[BUFSIZE])
{

	char TextCTM[] = "$ConnectToMe";
	char *tobuf;
	if ((tobuf = (char*)malloc((strlen(buffer)+1)*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (connecttome)");
		return 0;
	}
	char *tobuf_new;
	tobuf_new = tobuf;
	tobuf = strstr(buffer,TextCTM);

	if(tobuf != NULL)
	{
		//Calculate stats
		num.connecttome ++;
		currlength.connecttome += strlen(tobuf);

		char a[150],b[250],c[250];

		sscanf(tobuf, "%s %s %s", &a, &b, &c);
		
		if(strlen(b) > 1 && strlen(b) < MAX_NICK_LENGTH)
		{
			string nick = b;
			string str = tobuf;
			
			int is = 0;
			for(int y = 1; y < public_count; y++)
			{
				string nck;
				nck = User[ufds[y].fd].nick;
				if(nck == nick)
					is = ufds[y].fd;	
			}
			if(is != 0)
			{
				if(!send_to_socket(is, str))
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					return 0;
				}
			}
		}
	}	

	char TextRCTM[] = "$RevConnectToMe";
	char *tobuf2;
	if ((tobuf2 = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (RevConnectToMe)");
		return 0;
	}
	char *tobuf2_new;
	tobuf2_new = tobuf2;
	tobuf2 = strstr(buffer,TextRCTM);

	if(tobuf2 != NULL)
	{	
		//Calculate stats
		num.revconnecttome ++;
		currlength.revconnecttome += strlen(tobuf2);
		
		char a[550],b[550],c[550];

		sscanf(tobuf2, "%s %s %s", &a, &b, &c);

		if(strlen(c) > 1 && strlen(c) < MAX_NICK_LENGTH)
		{
			char cnck[350];
			string nick;

			if(strlen(c) > 1 && strlen(c) < MAX_NICK_LENGTH)
				memcpy(cnck, c, strlen(c)-1);

			nick = cnck;
			string str;
			str = tobuf2;
			
			int is = 0;
			string nck;
			
			for(int y = 1; y < public_count; y++)
			{
				nck = User[ufds[y].fd].nick;

				if(nck == nick)
					is = ufds[y].fd;	
			}
		
			if(is != 0)
			{
				if(!send_to_socket(is, str))
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					return 0;
				}
			}
		}
	}

	free(tobuf_new);
	free(tobuf2_new);

	return 1;
}

int search(char buffer[BUFSIZE])
{
	if(search_enable)
	{
		char TextSearch[] = "$Search";
		char *tobuf;
		char *tobuf_new;
		if ((tobuf = (char*)malloc((strlen(buffer)+1)*sizeof(char)))==NULL)
		{ 	
			printf("**ERROR** Low memory (search)");
			return 0;
		}	
	 	tobuf_new = tobuf;
		tobuf = strstr(buffer,TextSearch);
	
		if(tobuf != NULL)
		{	
			//Calculate stats
			num.search ++;
			currlength.search += strlen(tobuf);

			string buf;
			buf = tobuf;
	
			if(!send_to_all(buf))
			{
				cerr << "**ERROR** Problem with send data (search)" << endl;
				return 0;
			}
				
				
		}

		free(tobuf_new);
	}
	
	return 1;
}

int privatemessage(int id, char buffer[BUFSIZE])
{
	char PMto[] = "$To:";
	char cPM1[80],cPM2[150],cPM3[80],cPM4[80],cPM5[80],cPM6[80];
	string PM1,PM2,PM3,PM4,PM5,PM6;

	char *PMtobuf;
	char *PMtobuf_new;
	if ((PMtobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (privatemessage)");
		return 0;
	}
	PMtobuf_new = PMtobuf;
	PMtobuf = strstr(buffer,PMto);

	if (PMtobuf != NULL)
	{
		//Calculate stats
		num.privatemessage ++;
		currlength.privatemessage += strlen(PMtobuf);

		sscanf(PMtobuf, "%s %s %s %s %s %s", &cPM1, &cPM2, &cPM3, &cPM4, &cPM5, &cPM6);

		if(strlen(cPM2) > 1 && strlen(cPM2) < MAX_NICK_LENGTH)
		{
			PM2 = cPM2;
	
			int is = 0;
			for(int y = 1; y < public_count; y++)
			{
				if (User[ufds[y].fd].nick == PM2)
					is = ufds[y].fd;
				
			}
			
			string str;
			str = PMtobuf;
	
			if(is != 0)
			{
				if(!send_to_socket(is, str))
				{
					cerr << "**ERROR** Problem with send data >> " << User[id].nick << " >> " << id << endl;
					return 0;
				}
			}

		}

	}

	free(PMtobuf_new);

	return 1;
}

int botinfo(int id, char buffer[BUFSIZE])
{

	char TextBI[] = "$BotINFO";
	char *TextBItobuf;
	char *TextBItobuf_new;
	if ((TextBItobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (botinfo)");
		return 0;
	}
	TextBItobuf_new = TextBItobuf;
	TextBItobuf = strstr(buffer,TextBI);

	if(TextBItobuf != NULL)
	{
		/*$HubINFO <hub name>$<hub address:port>$<hub description>$<max users>$<min share in bytes>$<min slots>$<max hubs>$<hub type>$<hubowner login>|
		*/
		string response;
		response += "$HubINFO ";response += hub_name;response += "$"; 
		if(listening_port == 411)
			response += hub_hostname;
		else
		{
			response += hub_hostname;
			char sport[100];sprintf(sport, ":%d", listening_port);response += sport;
		}
		response += "$";response += hub_description;response += "$";
		char susers[100];sprintf(susers, "%d", max_users);response += susers;
		response += "$0$0$0$xHub$";response += hub_owner;response += "|";

		if(!send_to_socket(id, response))
		{
			cerr << "**ERROR** Problem with send data" << endl;
			return 0;
		}

		string str;
		str += "<";str += hub_botname;str += "> ";
		str += "Hub Pinger -> ";str += TextBItobuf;

		for(int y = 1; y < public_count; y++)
		{

			for (int loop = 0; loop < number; loop++)
			{	
				if(Reg[loop].nick == User[ufds[y].fd].nick && Reg[loop].Class >= 3)
					if(!send_to_socket(ufds[y].fd, str))
						return 0;
			}
		}

		/*if(!send_to_all(str))
		{
			cerr << "**ERROR** Problem with send data" << endl;
			return 0;
		}*/

		

	}

	free(TextBItobuf_new);

	return 1;
}

int massmessage(char msg[2048])
{
	for(int m = 1; m < public_count; m++)
	{
		string snd;
		snd = "$To: ";
		snd += User[ufds[m].fd].nick;
		snd += " From: ";
		snd += hub_botname;
		snd += " $<";
		snd += hub_botname;
		snd += "> ";
		snd += msg;
		snd += "|";

		if(!send_to_socket(ufds[m].fd, snd))
		{
			cerr << "**ERROR** Problem with send data >> " << User[ufds[m].fd].nick << " >> " << ufds[m].fd << endl;
			return 0;
		}
	}


	return 1;
}


/*************************
         COMMANDS
**************************/

int commands(int id, char buffer[BUFSIZE])
{
	// Command structure
	char cnick[200],ccmd1[200],ccmd2[200],ccmd3[200];		// CREATE
	cnick[1] = 0;ccmd1[1] = 0;ccmd2[1] = 0;ccmd3[1] = 0;		// NULL
	
	sscanf(buffer, "%s %s %s %s", &cnick, &ccmd1, &ccmd2, &ccmd3);	// GET
	string Nick(""),Command(""),Params(""),Params2("");

	if(strlen(cnick) < 1 || strlen(cnick) > MAX_NICK_LENGTH)
		return 0;

	// P?etypujeme char do stringu
	Nick = cnick;
	Command = ccmd1;
	Params = ccmd2;
	Params2 = ccmd3;

	/* Commands for guests */
	
	// +help
	if(Command == "+help|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";

		int is = 0;
		for(int y = 1; y < public_count; y++)
		{	string nck("<");
			nck +=  User[ufds[y].fd].nick;
			nck += ">";

			if (nck == Nick)
				is = ufds[y].fd;
		}


		int x = 0;
		for (int loop = 0; loop < number; loop++)
		{	
			string nck("<");
			nck +=  Reg[loop].nick;
			nck += ">";
			
			if(nck == Nick)
			{
				x ++;
				if(Reg[loop].Class == 1 || Reg[loop].Class == 2)
					str += hub_help;

				if(Reg[loop].Class >= 3 && Reg[loop].Class < 5)
					str += hub_help_op;

				if(Reg[loop].Class >= 5)
					str += hub_help_admin;
			}
		}

		if(x == 0)			// Unregistered user
			str += hub_help;		

		str += "|";

		send_to_socket(id, str);
	}

	// +motd
	if(Command == "+motd|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";
		str += hub_motd;str += "|";

		send_to_socket(id, str);
	}

	// +rules
	if(Command == "+rules|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";
		str += hub_rules;str += "|";

		send_to_socket(id, str);
	}

	// +faq
	if(Command == "+faq|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";
		str += hub_faq;str += "|";

		send_to_socket(id, str);
	}

	// +credits
	if(Command == "+credits|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";
		str += "\n__________________________________________\n\n    Developer: Tomá? 'ZeXx86' J?drzejek\n     E-Mail: Admin@Infern0.tk\n__________________________________________";str += "|";

		send_to_socket(id, str);
	}

	// +hubinfo
	if(Command == "+hubinfo|")
	{
		string str;
		str += "<";str += hub_botname;str += ">\n------------------------------------------------------------\nHub Info\n------------------------------------------------------------\n";

		str += "Hub Name: ";str += hub_name;
		str += "\nHub Description: ";str += hub_description;
		str += "\nHub Version: ";str += VERSION;
		str += "\nHub StartTime: ";str += StartTime;
		//str += "Hub UpTime: ";str += UpTime(apptime);
		str += "Hub owner: ";str += hub_owner;
		str += "\nHub hostname: ";char hostport[128];sprintf(hostport, "%s:%d", hub_hostname, listening_port);str += hostport;
		str += "\nHub redirect: ";str += redirect_host;
		str += "\n------------------------------------------------------------";
		str += "\nSearch: ";if(search_enable == 0){str += "disabled";}if(search_enable != 0){str += "enabled";}
		str += "\nCur./Max. users: ";char curmaxusers[80];sprintf(curmaxusers, "%d / %d", hub_users, max_users); str += curmaxusers;
		str += "\nMin. share: ";char minshare[10];sprintf(minshare, "%dMB", min_share);str += minshare;
		str += "\nTotal traffic: ";char ctraffic[50];
		sprintf(ctraffic, "%0.2fkB -> %0.2fMB", (float)traffic/1024, (float)traffic/(1024*1024));
		str += ctraffic;
		str += "\nActual traffic: ";char ctraffic2[100];sprintf(ctraffic2, "Up: %d / Down: %dB", currtrafficUp, currtrafficDown);str += ctraffic2;
		str += "\nMain buffer: ";char cbuf[100];sprintf(cbuf, "%d / %d", strlen(buffer), BUFSIZE);str += cbuf;
		str += "\n------------------------------------------------------------";
		str += "\nMyINFO commands: ";char myinfobuf[100];sprintf(myinfobuf, "%dx (avg length: %0.2fB)", num.myinfo, (float)currlength.myinfo/(float)num.myinfo);if(currlength.myinfo != 0 && num.myinfo != 0){str += myinfobuf;}else{str += "0x";}

		str += "\nValidateNick commands: ";char vnbuf[100];sprintf(vnbuf, "%dx (avg length: %0.2fB)", num.validatenick, (float)currlength.validatenick/(float)num.validatenick);if(currlength.validatenick != 0 && num.validatenick != 0){str += vnbuf;}else{str += "0x";}

		str += "\nSearch commands: ";char srbuf[100];sprintf(srbuf, "%dx (avg length: %0.2fB)", num.search, (float)currlength.search/(float)num.search);if(currlength.search != 0 && num.search != 0){str += srbuf;}else{str += "0x";}

		str += "\nGetNickList commands: ";char gnlbuf[100];sprintf(gnlbuf, "%dx (avg length: %0.2fB)", num.getnicklist, (float)currlength.getnicklist/(float)num.getnicklist);str += gnlbuf;

		str += "\nPrivateMessage commands: ";char pmbuf[100];sprintf(pmbuf, "%dx (avg length: %0.2fB)", num.privatemessage, (float)currlength.privatemessage/(float)num.privatemessage);if(currlength.privatemessage != 0 && num.privatemessage != 0){str += pmbuf;}else{str += "0x";}

		str += "\nConnectToMe commands: ";char ctmbuf[100];sprintf(ctmbuf, "%dx (avg length: %0.2fB)", num.connecttome, (float)currlength.connecttome/(float)num.connecttome);if(currlength.connecttome != 0 && num.connecttome != 0){str += ctmbuf;}else{str += "0x";}

		str += "\nRevConnectToMe commands: ";char rctmbuf[100];sprintf(rctmbuf, "%dx (avg length: %0.2fB)", num.revconnecttome, (float)currlength.revconnecttome/(float)num.revconnecttome);if(currlength.revconnecttome != 0 && num.revconnecttome != 0){str += rctmbuf;}else{str += "0x";}

		str += "\n------------------------------------------------------------|";
		if(!send_to_socket(id, str))
			return 0;
	}

	// +reglist
	if(Command == "+reglist|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";
		str += "\nTable with registered users\n\nNick / Class\n______________________\n\n";

		for (int loop = 0; loop < number; loop++)
		{	
			str += Reg[loop].nick ; 
			str += " / ";
			char regclass[5];
			sprintf(regclass, "%d", Reg[loop].Class);
			str += regclass;
			str += "\n";
		}

		str += "______________________|";

		if(send_to_socket(id, str))
			return 0;
	}

	// +reglist
	if(Command == "+banlist|")
	{
		string str;
		str += "<";str += hub_botname;str += "> ";
		str += "\nTable with banned users\n______________________\n\n";

		for (int loop = 0; loop < bannumber; loop++)
		{	
			str += Ban[loop].nick; 
			str += "\n";
		}

		str += "______________________|";

		if(send_to_socket(id, str))
			return 0;
	}

	if(Command == "+myinfo|")
	{
		int is = 0;
		for(int y = 1; y < public_count; y++)
		{	string nck("<");
			nck +=  User[ufds[y].fd].nick;
			nck += ">";

			if (nck == Nick)
				is = ufds[y].fd;

		}

		if(is != 0)
		{
			string str;
			str += "<";str += hub_botname;str += "> ";
			str += "Information of user ";str += User[is].nick;
			str += "\nClass: ";	
			char regclass[10] = "0";
			for (int loop = 0; loop < number; loop++)
			{	
				string nck("<");
				nck +=  Reg[loop].nick;
				nck += ">";
	
				if(nck == Nick)
				{
					sprintf(regclass, "%d", Reg[loop].Class);
				}
			}
				
			str += regclass;

			if(User[is].desc.size() > 0)
				str += "\nDescription: ";str += User[is].desc;
			if(User[is].conn.size() > 0)
				str += "\nConn. type: ";str += User[is].conn;
			if(User[is].email.size() > 0)
				str += "\nE-Mail: ";str += User[is].email;
			if(User[is].share > 0)
				str += "\nShare: ";char share[30];sprintf(share, "%d", User[is].share);str += share;
			if(User[is].TAG.size() > 0)
				str += "\nTAG: ";str += User[is].TAG;
			str += "\nIP Address: ";str += User[is].IPaddress;
			str += "|";
					
			if(!send_to_socket(id, str))
				return 0;
		}
	}

	// +passwd <pass>
	if(Command == "+passwd")
	{
		if(strlen(ccmd2) > 2 && strlen(ccmd2) < MAX_PASS_LEN+1)
		{
			load_reglist();	// Load old reglist
		
			string regdata;
			char inttostring[60];
			sprintf(inttostring, "regnum %d\n", number); 
			regdata = inttostring;

			char newpass[256];

			memcpy(newpass, ccmd2, strlen(ccmd2)-1);
			newpass[strlen(ccmd2)-1] = '\0';


			int y = 0;
			for (int loop = 0; loop < number; loop++)
			{
				if(Reg[loop].nick != User[id].nick)
				{
					regdata += Reg[loop].nick;regdata += " ";
					regdata += Reg[loop].pass;regdata += " ";
					char classtostring[10];sprintf(classtostring, "%d", Reg[loop].Class); 
					regdata += classtostring;regdata += "\n";
				}
				if(Reg[loop].nick == User[id].nick && y == 0)
				{
					y = 1;
					regdata += Reg[loop].nick;regdata += " ";
					regdata += newpass;regdata += " ";
					char classtostring[10];sprintf(classtostring, "%d", Reg[loop].Class); 
					regdata += classtostring;regdata += "\n";					
				}
			}					
			
			// Save to file "reglist"
			FILE *freglist;
			freglist=fopen(".xHub//reglist","w");					
				fputs (regdata.c_str(), freglist);
			fclose(freglist);		

			// Send info text to PM
			string str;
			str = "$To: ";
			str += User[id].nick;
			str += " From: ";
			str += hub_botname;
			str += " $<";
			str += hub_botname;
			str += "> ";
			str += "Password was changed on '";str += newpass;str += "'|";

			send_to_socket(id, str);

			load_reglist();	// Reload new reglist
		}
		else
		{
			string str;
			str += "<";str += hub_botname;str += "> ";
			str += "Invalid params!|";
			send_to_socket(id, str);				
		}
	}

	// +regme
	if(Command == "+regme|")
	{
		int x = 0;
		for (int loop = 0; loop < number; loop++)
		{
			if(Reg[loop].nick == User[id].nick)
				x ++;	
		}

		if(x == 0)
		{
			for(int y = 1; y < public_count; y++)
			{
				for (int loop2 = 0; loop2 < number; loop2++)
				{
					if(Reg[loop2].nick == User[ufds[y].fd].nick && Reg[loop2].Class >= 3)	// Je nas clovek regly a ma tridu vyzsi nez 2?
					{
						string snd;
						snd = "$To: ";
						snd += User[ufds[y].fd].nick;
						snd += " From: ";
						snd += hub_opchatname;
						snd += " $<";
						snd += hub_opchatname;
						snd += "> ";
						snd += "User '";
						snd += User[id].nick;
						snd += "' (";
						snd += User[id].IPaddress;
						snd += ") want to registration.|";

						if(!send_to_socket(ufds[y].fd,snd))
							return 0;
					}
				}
			}
		}
	}


	/* Commands for OPs */

	for (int loop = 0; loop < number; loop++)
	{
		if(Reg[loop].nick == User[id].nick && Reg[loop].Class >= 3)
		{
			// +mass
			if(Command == "+mass")
			{
				char TextM[] = "> +mass";
				char *TextMtobuf;
				if ((TextMtobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory (+mass)");
					return 0;
				}
				TextMtobuf = strstr(buffer,TextM);
				
				if(TextMtobuf && strlen(TextMtobuf) < MAX_MASS_LEN)
				{
					char text[3072];
					memcpy(text, TextMtobuf+8,strlen(TextMtobuf)-1); 
					massmessage(text);
				}
			}

			// +banner
			if(Command == "+banner|")
			{
				string str;
				str += "<";str += hub_botname;str += "> ";
				str += hub_banner;str += "|";

				send_to_all(str);
			}

			// +reload
			if(Command == "+reload|")
			{
				load_files();

				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "OK";str += "|";

				send_to_socket(id, str);
			}

			// +hubname
			if(Command == "+hubname|")
			{
				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "Hub name is: ";str += hub_name;str += "|";

				send_to_socket(id, str);
			}

			// +hubname <Name>
			if(Command == "+hubname")
			{
				string HubName	 ("$HubName ");
				char name[256];

				char TextM[] = "> +hubname";
				char *TextMtobuf;
				if ((TextMtobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory");
					return 0;
				}
				TextMtobuf = strstr(buffer,TextM);
				
				if(TextMtobuf && strlen(TextMtobuf) < MAX_HUB_NAME-1)
				{

					char str1[strlen(TextMtobuf)+1];
					strcpy(str1, TextMtobuf);
					memcpy(name, str1+11, strlen(str1)-12);
					name[strlen(str1)-12] = '\0';					

					HubName += name;
					HubName += " - ";
					HubName += hub_description;
					HubName += "|";
	
					if(!send_to_all(HubName))
						return 0;
	
					hub_name = name;
	
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Hub name is changed on: ";str += name;str += "|";
	
					if(!send_to_socket(id, str))
						return 0;
				}
				else
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Wrong length of hubname|";
	
					if(!send_to_socket(id, str))
						return 0;
				}

			}

			// +hubdesc <Description>
			if(Command == "+hubdesc")
			{
				string HubName	 ("$HubName ");
				char desc[MAX_HUB_NAME];

				char TextM[] = "> +hubdesc";
				char *TextMtobuf;
				if ((TextMtobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory (+hubdesc)");
					return 0;
				}
				TextMtobuf = strstr(buffer,TextM);
				
				if(TextMtobuf && strlen(TextMtobuf) < MAX_HUB_NAME-1)
				{
					char str1[strlen(TextMtobuf)+1];
					strcpy(str1, TextMtobuf);
					memcpy(desc, str1+11, strlen(str1)-12);
					desc[strlen(str1)-12] = '\0';

					HubName += hub_name;
					HubName += " - ";
					HubName += desc;
					HubName += "|";
	
					if(!send_to_socket(id, HubName))
						return 0;
	
					strcpy(hub_description, desc);
	
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Hub description is changed on: ";str += desc;str += "|";
	
					if(!send_to_socket(id, str))
						return 0;
				}
				else
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Wrong length of hub description|";
	
					if(!send_to_socket(id, str))
						return 0;
				}
			}

			if(Command == "+getinfo")
			{
				int is = 0;
				for(int y = 1; y < public_count; y++) 
				{	string nck;
					nck =  User[ufds[y].fd].nick;
					nck += "|";

					if (nck == Params)
						is = ufds[y].fd;

				}

				if(is != 0)	
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Information of user ";str += User[is].nick;
					str += "\nClass: ";	
					char regclass[10] = "0";
					for (int loop = 0; loop < number; loop++)
					{	
						string nck;
						nck =  Reg[loop].nick;
						nck += "|";
	
						if(nck == Params)
						{
							sprintf(regclass, "%d", Reg[loop].Class);
						}
					}
					
					str += regclass;
					if(User[is].desc.size() > 0)
						str += "\nDescription: ";str += User[is].desc;
					if(User[is].conn.size() > 0)
						str += "\nConn. type: ";str += User[is].conn;
					if(User[is].email.size() > 0)
						str += "\nE-Mail: ";str += User[is].email;
					if(User[is].share > 0)
						str += "\nShare: ";char share[30];sprintf(share, "%d", User[is].share);str += share;
					if(User[is].TAG.size() > 0)
						str += "\nTAG: ";str += User[is].TAG;
					str += "\nIP Address: ";str += User[is].IPaddress;
					str += "|";
							
					if(!send_to_socket(id, str))
						return 0;

				}
				else
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "User is offline|";
							
					if(!send_to_socket(id, str))
						return 0;
				}
			}

			// +redirect
			if(Command == "+redirect|")
			{
				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "Example: +redirect <Nick> <Address>";str += "|";

				if(!send_to_socket(id, str))
					return 0;
			}

			// +redirect <who> <where>
			if(Command == "+redirect")
			{
				int is;

				string Redir ("$ForceMove ");
					   Redir += Params2;
					   Redir += "|";

				if (Params != "ALL" && Params != "all" && Params != "All" && Params != "aLL")
				{ 
					for(int y = 1; y < public_count; y++)
					{
												
						if (User[ufds[y].fd].nick == Params)
								is = ufds[y].fd;	

						if(Params2.size() > 0)
						{
							if(!send_to_socket(is, Redir))
								return 0;
						}

					}
				}
				else
				{
					if(Params2.size() > 0)
						send_to_all(Redir);
				}

				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "OK";str += "|";

				send_to_socket(id, str);
			}

			// +kick <who>
			if(Command == "+kick")
			{

				for(int y = 1; y < public_count; y++)
				{
					string findnick;
					findnick = User[ufds[y].fd].nick;
					findnick += "|";

					if (findnick == Params && ufds[y].fd != id)
					{	
						/*close_connect(ufds[y].fd);
						close(ufds[y].fd);*/
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "You was kicked by ";str += User[id].nick;str += "!|$ForceMove ";str += redirect_host;str += "|";

						if(!send_to_socket(ufds[y].fd, str))
							return 0;
					}
				}

				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "OK";str += "|";

				if(!send_to_socket(id, str))
					return 0;

			}

			// +hubmaxusers
			if(Command == "+hubmaxusers|")
			{
				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "The number of max. connected users is: ";char maxusers[20];sprintf(maxusers, "%d", max_users); str += maxusers;str += "|";

				if(!send_to_socket(id, str))
					return 0;
			}

			// +hubmaxusers <number>
			if(Command == "+hubmaxusers")
			{
				max_users = atoi(Params.c_str());

				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "Max. Users is changed on: ";char maxusers[20];sprintf(maxusers, "%d", max_users); str += maxusers;str += "|";

				if(!send_to_socket(id, str))
					return 0;
			}

			// +search <number>
			if(Command == "+search")
			{
				char newvalue[128];
				memcpy(newvalue, ccmd2, strlen(ccmd2) -1);
				newvalue[strlen(ccmd2)-1] = '\0';
				search_enable = atoi(newvalue);

				string str;
				str += "<";str += hub_botname;str += "> ";

				if(search_enable == 0)
				{
					str += "Search is disabled|";
					search_enable = 0;
				}
				if(search_enable != 0)
				{
					str += "Search is enabled|";
					search_enable = 1;
				}

				if(!send_to_socket(id, str))
					return 0;
			}

			// +hublist
			/*if(Command == "+hublist|")
			{
				upload_to_hublist();

				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "Hub will be added to hublist: ";str += public_hub_host;str += "|";

				send_to_socket(id, str);
			}*/

			// +regnew <nick> <class>
			if(Command == "+regnew")
			{
				char bannick[256];
				memcpy(bannick, ccmd2, strlen(ccmd2) -1);
				string nck;
				nck = bannick;

				//string vnck;
				//vnck += "<";vnck += User[id].nick;vnck += ">";

				int is = 0;//,is2 = 0;
				for(int y = 1; y < public_count; y++)
				{
					if(User[ufds[y].fd].nick == nck)
						is = ufds[y].fd;

					//if(Nick == vnck)
						//is2 = ufds[y].fd;
				}

				//if (is2 != id)
					//return 0;

				// Validate typed class
				if(Params2 != "1|" && Params2 != "2|" && Params2 != "3|" && Params2 != "4|" && Params2 != "5|")
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Invalid class!|";

					if(!send_to_socket(id, str))
						return 0;

					return 0;
				}

					if(strlen(ccmd3) > 0 && strlen(ccmd2) > 1)
					{

						load_reglist();	// Load old reglist

						if(number >= MAX_REGS-1)
						{
							cerr << "**ERROR** #define MAX_REGS is overflowed !";
							return 0;
						}
			
						string regdata;
						int newregnum = number +1;
						char inttostring[50];
						sprintf(inttostring, "regnum %d\n", newregnum); 
						regdata += inttostring;
						int in = 0, classok = 0;

						char regclass[15];
						memcpy(regclass, ccmd3, strlen(ccmd3) -1);
						regclass[strlen(ccmd3)-1] = '\0';


						for (int loop = 0; loop < number; loop++)
						{
							regdata += Reg[loop].nick;regdata += " ";
							regdata += Reg[loop].pass;regdata += " ";
							char classtostring[10];sprintf(classtostring, "%d", Reg[loop].Class); 
							regdata += classtostring;regdata += "\n";

							if(Reg[loop].nick == Params)
								in ++;
	
							string nck("<");
							nck +=  Reg[loop].nick;
							nck += ">";
				
							if(nck == Nick && Reg[loop].Class >= atoi(regclass))
								classok ++;
							
						}
						if(classok == 0)
						{
							string str;
							str += "<";str += hub_botname;str += "> ";
							str += "You havent permission for regging class ";
							str += regclass;str += " !|";

							if(!send_to_socket(id, str))
								return 0;

							return 0;
						}
						if(in != 0)
						{
							string str;
							str += "<";str += hub_botname;str += "> ";
							str += "User ";str += Params;str += " is already regged !|";

							if(!send_to_socket(id, str))
								return 0;

							return 0;
						}


						regdata += Params;regdata += " PASSWORD ";
						regdata += regclass;	
					
						// Save to file "reglist"
						FILE *freglist;
						freglist=fopen(".xHub//reglist","w");					
							fputs (regdata.c_str(), freglist);
						fclose(freglist);		

						// Send info text to mainchat
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "User ";str += Params;str += " is regged by ";str += User[id].nick;str += "|";
						
						int ia = 0;
						for(int x = 1; x < public_count; x++)
						{
							if(User[ufds[x].fd].nick == Params)
								ia = ufds[x].fd;					
						}						
			
						if(ia != 0)
						{
							if(!send_to_all(str))
								return 0;

							str = "$To: ";
							str += User[ia].nick;
							str += " From: ";
							str += hub_botname;
							str += " $<";
							str += hub_botname;
							str += "> ";
							str += "You is regged by ";
							str += User[id].nick;
							str += " with class ";
							str += regclass;
							str += ", your password is 'PASSWORD' now.|";

							if(!send_to_socket(ia, str))
								return 0;

							str = "$To: ";
							str += User[ia].nick;
							str += " From: ";
							str += hub_botname;
							str += " $<";
							str += hub_botname;
							str += "> ";
							str += "Type '+passwd <newpassword>' on mainchat for change..|";
	
							if(!send_to_socket(ia, str))
								return 0;
						}

						load_reglist();	// Reload new reglist
					
					}
					else
					{
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "Invalid params!|";

						if(!send_to_socket(id, str))
							return 0;
					}
				//}
			}

			// +regdel <nick>
			if(Command == "+regdel")
			{
				if(strlen(ccmd2) > 2)
				{
					load_reglist();	// Load old reglist
		
					string regdata;
					int newregnum = number -1;
					char inttostring[50];
					sprintf(inttostring, "regnum %d\n", newregnum); 
					regdata += inttostring;

					char delnick[100];
					memcpy(delnick, ccmd2, strlen(ccmd2) -1);

					//memcpy(delnick, Texttobuf+9,strlen(Texttobuf)-10); 
					delnick[strlen(ccmd2)-1] = '\0';

					int n = 0,classok = -1,myclass = 0,unregclass = 0;

					for (int loop2 = 0; loop2 < number; loop2++)
					{				
						if(Reg[loop2].nick == User[id].nick)
							myclass = Reg[loop2].Class;
					}

					for (int loop = 0; loop < number; loop++)
					{
						if(Reg[loop].nick != delnick)
						{
							regdata += Reg[loop].nick;regdata += " ";
							regdata += Reg[loop].pass;regdata += " ";
							char classtostring[10];sprintf(classtostring, "%d", Reg[loop].Class); 
							regdata += classtostring;regdata += "\n";
						}
						else
						    n ++;
	
						string regnck;
						regnck = Reg[loop].nick;
						regnck += "|";
						if(regnck == Params && Reg[loop].Class > myclass)
						{
							classok = 1;
							unregclass = Reg[loop].Class;
						}
							
					}
					if(n == 0)
					{
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "User ";str += delnick;str += " is'nt regged !|";
	
						if(!send_to_socket(id, str))
							return 0;

						return 0;
					}
					if(classok == 1)
					{
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "You havent permission for unregging class ";
						char cuc[5];sprintf(cuc, "%d", unregclass);str += cuc;str += " !|";

						if(!send_to_socket(id, str))
							return 0;

						return 0;
					}

					// Save to file "reglist"
					FILE *freglist;
					freglist=fopen(".xHub//reglist","w");					
						fputs (regdata.c_str(), freglist);
					fclose(freglist);		

					// Send info text to mainchat
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "User ";str += delnick;str += " is deleted from reglist by ";str += User[id].nick;str += "|";

					send_to_all(str);

					load_reglist();	// Reload new reglist
				}
				else
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Invalid params!|";

					if(!send_to_socket(id, str))
						return 0;				
				}
			}

			// +permban <nick>
			if(Command == "+permban")
			{
				char bannick[128];

				char Text[] = "+permban";
				char *Texttobuf;
				if ((Texttobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory (+permban)");
					return 0;
				}
				Texttobuf = strstr(buffer,Text);
				
				if(Texttobuf)
				{
					
					memcpy(bannick, Texttobuf+9,strlen(Texttobuf)-10); 
					bannick[strlen(Texttobuf)-10] = '\0';
							
					//memcpy(bannick, ccmd2, strlen(ccmd2) -1);
					string nck;
					nck = bannick;
					int is;
					for(int y = 1; y < public_count; y++)
					{
						if(User[ufds[y].fd].nick == nck)
							is = ufds[y].fd;					
					}
	
					//if(User[id].Class >= User[is].Class)
					//{
						if(strlen(ccmd2) > 1)
						{
	
							load_banlist();	// Load old reglist
				
							string bandata;
							int newbannum = bannumber +1;
							char inttostring[50];
							sprintf(inttostring, "bannum %d\n", newbannum); 
							bandata += inttostring;
	
							for (int loop = 0; loop < bannumber; loop++)
							{
								bandata += Ban[loop].nick;
								bandata += "\n";
							}
	
							char bannick[51];
							memcpy(bannick, Texttobuf+9,strlen(Texttobuf)-10); 
							bannick[strlen(Texttobuf)-10] = '\0';
							bandata += bannick;
							
					
							// Save to file "reglist"
							FILE *fbanlist;
							fbanlist=fopen(".xHub//banlist","w");					
								fputs (bandata.c_str(), fbanlist);
							fclose(fbanlist);		
	
							// Send info text to mainchat
							string str;
							str += "<";str += hub_botname;str += "> ";
							str += "User ";str += bannick;str += " is banned by ";str += User[id].nick;str += "|";
	
							send_to_all(str);
	
	
							string str2;
							str2 += "|$ForceMove ";str2 += redirect_host;str2 += "|";
	
							if(!send_to_socket(is, str2))
							{
								string notin;
								notin += "<";notin += hub_botname;notin += "> ";
								notin += "User ";notin += bannick;notin += " is'nt connected|";
	
								if(!send_to_socket(id, notin))
									return 0;
							}
	
							load_banlist();	// Reload new reglist
						
						}
						else
						{
							string str;
							str += "<";str += hub_botname;str += "> ";
							str += "Invalid params!|";
	
							if(!send_to_socket(id, str))
								return 0;				
						}
				}
			}

			// +unban <nick>
			if(Command == "+unban")
			{
				if(strlen(ccmd2) > 1)
				{
					load_banlist();	// Load old banlist
		
					string bandata;
					int newbannum = bannumber -1;
					char inttostring[50];
					sprintf(inttostring, "bannum %d\n", newbannum); 
					bandata += inttostring;

					char delnick[100];
					memcpy(delnick, ccmd2, strlen(ccmd2) -1);

					int n = 0;

					for (int loop = 0; loop < bannumber; loop++)
					{
						if(Ban[loop].nick != delnick)
						{
							bandata += Ban[loop].nick;
							bandata += "\n";
			
						}
						else
							n++;
					}	

					if(n == 0)
					{
						string str;
						str += "<";str += hub_botname;str += "> ";
						str += "User ";str += delnick;str += " is'nt banned|";
	
						if(!send_to_socket(id, str))
							return 0;

						return 0;
					}
			
					// Save to file "banlist"
					FILE *fbanlist;
					fbanlist=fopen(".xHub//banlist","w");					
						fputs (bandata.c_str(), fbanlist);
					fclose(fbanlist);		

					// Send info text to mainchat
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "User ";str += delnick;str += " is deleted from banlist by ";str += User[id].nick;str += "|";

					send_to_all(str);

					load_banlist();	// Reload new reglist
				}
				else
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "Invalid params!|";

					if(!send_to_socket(id, str))
						return 0;				
				}
			}

			/* Commands for Admins */
			if(Reg[loop].Class >= 5)
			{

				// +shutdown
				if(Command == "+shutdown|")
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "*** Hub go down ***";str += "|";

					send_to_all(str);

					// Zavru hlavni soket
					close(mainSocket);

					// Vsechny ostatni sokety take uzavru.
					for_each(sockets.begin(), sockets.end(), close);

					
					return -1;
					//sleep(100);

					exit(1);
				}

				// +restart
				if(Command == "+restart|")
				{
					string str;
					str += "<";str += hub_botname;str += "> ";
					str += "*** Hub will be restarted now ***";str += "|";

					send_to_all(str);

					// Zavru hlavni soket
					close(mainSocket);

					// Vsechny ostatni sokety take uzavru.
					for_each(sockets.begin(), sockets.end(), close);

					exit(1);
		
				}

			}
	
		}
	}

	return 1;
}

int log(int id, int mode, string buffer)
{
	// Calculate Traffic
	if(id != 0)	
		traffic += buffer.size();
	else
		traffic += (hub_users*buffer.size());

	if(mode == 0)
		currtrafficDown = buffer.size();
	else
		currtrafficUp = buffer.size();

	// Logging to file
	if(syslog_enable)
	{
		string str("");

		str += "\n";

		if(id == 0)
			str += "All";
		else
			str += User[id].nick;
		
		if(mode == 0)
			str += " >> ";
		else
			str += " << ";

		str += buffer;

		FILE *flog;
		flog=fopen(".xHub//log","a");					
			fputs (str.c_str(), flog);
		fclose(flog);

		return 1;
	}
	else
		return 0;
}

int antiraw(int id, char buffer[BUFSIZE])
{
	// Anti-RAW

	int is = 0;
	string nb;
	nb = "<"; nb += User[id].nick; nb += ">";

	// Command structure
	char cnick[512],ccmd1[1024];
	sscanf(buffer, "%s %s", &cnick, &ccmd1);
	string Nick,Command;

	// Pretypujeme char do stringu
	Nick = cnick;
	Command = ccmd1;

	if(nb != Nick) // Porovnam jmeno v mainchatu se jmenem socketu
	{
		for(int iraw = 1; iraw < public_count; iraw++) 
		{	
			string nb2;
			nb2 = "<"; nb2 += User[ufds[iraw].fd].nick; nb2 += ">";
			
			if(Nick == nb2)
			{	
				is ++;
				string str;
				str += "<";str += hub_botname;str += "> ";
				str += "User ";
				if(strlen(User[id].nick) > 1)
					str += User[id].nick;
				else
					str += "Unknown";
				str += " type as ";
				str += User[ufds[iraw].fd].nick;
				str += "|";

				if(!send_to_all(str))
					return 0;

				str = "$ForceMove ";str += redirect_host;str += "|";
				
				if(!send_to_socket(id, str))
					return 0;

				close(id);
			}
			//else
				//is = 0;
		}
	}
	else
		is = 0;

	return is;
}

int kick(int id, char buffer[BUFSIZE])
{

	char TextKick[] = "kicking";
	char *TextKicktobuf;
	char *TextKicktobuf_new;
	if ((TextKicktobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (kick)");
		return 0;
	}
	TextKicktobuf_new = TextKicktobuf;
	TextKicktobuf = strstr(buffer,TextKick);

	if(TextKicktobuf != NULL)
	{	
		
		// Command structure
		char cnick[51],ccmd1[80];
		sscanf(buffer, "%s %s", &cnick, &ccmd1);
		string Nick,Command;

		// Retype char to string
		Nick = cnick;
		Command = ccmd1;

		for(int y = 1; y < public_count; y++)
		{

			if (User[ufds[y].fd].nick == Command)
			{
				for (int loop = 0; loop < number; loop++)
				{
					if(Reg[loop].nick == User[id].nick && Reg[loop].Class >= 3)
					{
						string str;
						str =  "$ForceMove ";
						str += redirect_host;
						str += "|";
	
						if(!send_to_socket(ufds[y].fd, str))
							return 0;
					}
				}
			}
			
		}		

	}

	free(TextKicktobuf_new);	

	return 1;
}

int opchat(int id, char buffer[BUFSIZE])
{

	for (int loop = 0; loop < number; loop++)
	{
		if(Reg[loop].nick == User[id].nick && Reg[loop].Class >= 3)	// Je nas clovek regly a ma tridu vyzsi nez 2?
		{
			char PMto[] = "$To:";
			char cPM1[80],cPM2[80],cPM3[80],cPM4[80],cPM5[80],cPM6[80];
			string PM1,PM2,PM3,PM4,PM5,PM6;

			char *PMtobuf;
			char *PMtobuf_new;
			if ((PMtobuf = (char*)malloc(BUFSIZE*sizeof(char)))==NULL)
			{ 	
				printf("**ERROR** Low memory (opchat)");
				return 0;
			}
			PMtobuf_new = PMtobuf;
			PMtobuf = strstr(buffer,PMto);

			if (PMtobuf != NULL)
			{

				sscanf(PMtobuf, "%s %s %s %s %s %s", &cPM1, &cPM2, &cPM3, &cPM4, &cPM5, &cPM6);

				PM1 = cPM1;PM2 = cPM2;PM3 = cPM3;PM4 = cPM4;PM5 = cPM5;PM6 = cPM6;

				string opchat;
				opchat = hub_opchatname;
				if (PM2 == opchat)
				{
					char PMmsg[] = "$<";
					char *PMmsgtobuf;
					char *PMmsgtobuf_new;
					if ((PMmsgtobuf = (char*)malloc(2048*sizeof(char)))==NULL)
					{ 	
						printf("**ERROR** Low memory (opchat_PMmsg)");
						return 0;
					}
					PMmsgtobuf_new = PMmsgtobuf;
					PMmsgtobuf = strstr(PMtobuf,PMmsg);
							
					if(PMmsgtobuf != NULL)
					{
						for(int y = 1; y < public_count; y++)
						{
							for (int loop2 = 0; loop2 < number; loop2++)
							{
								if(Reg[loop2].nick == User[ufds[y].fd].nick && Reg[loop2].Class >= 3 && User[ufds[y].fd].nick != User[id].nick)	// Je nas clovek regly a ma tridu vyzsi nez 2?
								{
									string snd;
									snd = "$To: ";
									snd += User[ufds[y].fd].nick;
									snd += " From: ";
									snd += opchat;
									snd += " ";
									snd += PMmsgtobuf;

									if(!send_to_socket(ufds[y].fd,snd))
										return 0;
								}
							}
						}
					}
					free(PMmsgtobuf_new);
				}
			}
			free(PMtobuf_new);	
		}
	}

	return 1;
}

int opforcemove(int id, char buffer[BUFSIZE])
{


	char Text[] = "$OpForceMove";
	char *Texttobuf;
	char *Texttobuf_new;
	if ((Texttobuf = (char*)malloc((strlen(buffer)+1)*sizeof(char)))==NULL)
	{ 	
		printf("**ERROR** Low memory (opforcemove)");
		return 0;
	}
	Texttobuf_new = Texttobuf;
	Texttobuf = strstr(buffer,Text);

	if(Texttobuf != NULL && strlen(Texttobuf) < 480)
	{
		for (int loop = 0; loop < number; loop++)
		{
			if(Reg[loop].nick == User[id].nick && Reg[loop].Class >= 3)	// Je nas clovek regly a ma tridu vyzsi nez 2?
			{
				char str[(strlen(buffer)+1)];
				
				strcpy(str, Texttobuf);
				str[strlen(Texttobuf)-1] = '\0';


				char * pch;
				char * pch_new;
				if ((pch = (char*)malloc((strlen(buffer)+1)*sizeof(char)))==NULL)
				{ 	
					printf("**ERROR** Low memory (opforcemove_pch)");
					return 0;
				}
				pch_new = pch;
				int n = 0;
				
				int who = 0;
				int where = 0;
				int msg = 0;

		
				pch=strchr(str,'$');
		
				while ( pch!=NULL && n <= 4 )
				{
					n ++;
		
					if(n == 2)
						who = pch-str+1;
					if(n == 3)
						where = pch-str+1;
					if(n == 4)
						msg = pch-str+1;

					pch=strchr(pch+1,'$');
				}
		
				char cwho[128] = "";
				char cwhere[128] = "";
				char cmsg[256] = "";
		
				if(who > 0 && where > 0 && who < (where-1) && (where-who) < 128)
					memcpy (cwho,str+who+4,where-who-5);

				if(where > 0 && msg > 0 && where < (msg-1) && (msg-where) < 128)
					memcpy (cwhere,str+where+6,msg-where-7);

				if(msg > 0 && msg < (strlen(str)-1) && (strlen(str)-msg) < 256)
					memcpy (cmsg,str+msg+4,strlen(str)-msg-4);

			  	//cout << "OpForceMove: " << cwho << " " << cwhere << " " << cmsg << endl;


				string nick;
				nick = cwho;

				for(int y = 1; y < public_count; y++)
				{
					if (User[ufds[y].fd].nick == nick)
					{
						string str;
						str = "$To: ";
						str += nick;
						str += " From: ";
						str += hub_botname;
						str += " $<";
						str += hub_botname;
						str += "> ";
						str += cmsg;
						str += "|";

						str +=  "$ForceMove ";
						str += cwhere;
						str += "|";
			
						if(!send_to_socket(ufds[y].fd, str))
							return 0;
					}
				}

				free(pch_new);	
			}
		}
	}

	free(Texttobuf_new);
	
	return 1;
}

