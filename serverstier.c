# include <sys/socket.h>
# include <netdb.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <arpa/inet.h> //inet_addr

# define  SERVERPORT  9999
# define  LOCALHOST   "127.0.0.1"

# define MYMSGLEN   2048
#define MAXROOMS 1000 
struct Room
{
		int IPplayerActuels[20];
		int playerMax;
		int place;
		char Name[100];
};

struct Room rooms[MAXROOMS];
int numberRooms;

void addRoom(struct Room a)
{
   if ( numberRooms < MAXROOMS )
   {
      rooms[numberRooms] = a;
      numberRooms++;
   }
}
void delete_item(int item)
{
   if (numberRooms > 0 && item < numberRooms && item > -1)
   {
	  //free(&rooms[item]);
      numberRooms--;
      for (int i = item; i < numberRooms;i++)
      {
         rooms[i] =rooms[i + 1];
      }
      
   }
}

struct Room* createRoom(char * nom,int Ip,int playerMax)
{
	struct Room* res = malloc(sizeof(struct Room));
	if(res==NULL)
	{
		perror("malloc creation error");
	}
	res->IPplayerActuels[0]=Ip;
	strncpy(res->Name, nom, strlen(nom)+1);
	res->playerMax=playerMax;
	res->place=numberRooms;
	return res;
	}

void answer_client ( int sock )
{
	 struct  sockaddr_in  from ;
	socklen_t len;
	// Receive the length of the string.
	int code, length ;
	char string  [ MYMSGLEN ];	
	
	// Receive the string of characters.	
	
	while ( 1 )
	{
	  memset ( string, 0, MYMSGLEN );
	  
	  len = sizeof ( from ) ;
	  
	  length = recvfrom ( sock, string, MYMSGLEN, 0,( struct sockaddr * )  & from, & len ) ;
	
	  if ( length == -1 ) {
	    perror ( "recvfrom" ) ;
	    close ( sock ) ;
	    exit ( 1 ) ;
	  }
	string[length]='\0';
	  printf ( "Request received from: %s, with port number: %d \n" , inet_ntoa( from.sin_addr ), ntohs( from.sin_port ) ) ;
	fflush(stdout);
	  if ( length == 0 ) {
	    printf("Ignoring empty message received \n");
	    
	    continue ;
	   }
	   
	   printf ( "Received message: %s\n", string ) ;
	   
	   fflush(stdout);
	   len = sizeof ( from ) ;
	  // Check the string of characters.
	  if(strcmp(string,"list")==0)
	  {
		  //On envoie l'array
		  printf("envoi de l'array");
		  printf("taille de la liste %lu",sizeof(struct Room)*numberRooms);
		  for(int i=0;i<numberRooms;i++)
		  {
			code = sendto ( sock, &rooms[i], sizeof(struct Room), 0, ( struct sockaddr * ) & from, len ) ;
			if ( code == -1 ) {
				perror ( "sendto" ) ;
				close ( sock ) ;
				exit ( 1 ) ;
		  }
		  }
	  }
	  //si les 7 premier caractères sont bien connect
	  if(strncmp(string,"connect",7)==0)
	  {
		 
	  char result[20]="";
		  for(int i=7;i<strlen(string);i++)
		  {
			  result[i-7]=string[i];
		  }
		  int numbertoconnect=atoi(result);
		  printf("connection demandée à %d",numbertoconnect);
		  /* if(numbertoconnect<numberRooms)
		  {
			  code = sendto ( sock, &rooms[numbertoconnect].IPplayerActuels[0], length, 0, ( struct sockaddr * ) & from, len ) ;
				if ( code == -1 ) {
			perror ( "sendto" ) ;
			close ( sock ) ;
			exit ( 1 ) ;
			}
		  } */
		  //L'utilisateur a deja l'adresse IP du meneur de jeu
		  	
		  fflush(stdout);
	  }
	  if(strncmp(string,"create",6)==0)
	  {
		  printf("creation demandee");
		char numberplayer[2]="";
		numberplayer[0]=string[6];
		numberplayer[1]=string[7];
		int nombrejoueur=atoi(numberplayer);
		printf("nombre joueurs %d",nombrejoueur);
		char result[99]="";
		  for(int i=8;i<strlen(string);i++)
		  {
			  result[i-8]=string[i];
		  }
		    printf("creation de %s \n",result);
			printf("number ip %d \n",from.sin_addr.s_addr);
			struct Room* room=NULL;
			room=createRoom(result,from.sin_addr.s_addr,nombrejoueur);
			printf("create room passed");
		  addRoom(*room);
		  printf("%d",numberRooms);
		fflush(stdout);
		
	  }

	  
	  
	 // code = sendto ( sock, string, length, 0, ( struct sockaddr * ) & from, len ) ;
	  //if ( code == -1 ) {
	    //perror ( "sendto" ) ;
	    //close ( sock ) ;
	   // exit ( 1 ) ;
	  //}

	// Release ressources.
	
	}
	
	
}

int naming ( )
{
	// Socket creation.
	int sock ;
	sock = socket ( AF_INET, SOCK_DGRAM, 0 ) ;
	if ( sock == -1 ) {
		perror ( "socket" ) ;
		exit ( 1 ) ;
	}

	// Reuse the same port (useful when developing...).
	int code, enable = 1;
	code = setsockopt ( sock, SOL_SOCKET, SO_REUSEADDR, & enable, sizeof ( int ) ) ;
	if (code == -1 ) {
		perror ( "setsockopt" ) ;
		close ( sock ) ;
		exit ( 1 ) ;
	}

	// Initialisation of the sockaddr_in data structure.
	struct sockaddr_in addr ;
	memset ( & addr, 0, sizeof ( struct sockaddr_in ) ) ;
	addr . sin_family = AF_INET ;
	addr . sin_port = htons ( SERVERPORT ) ;
	addr . sin_addr . s_addr = inet_addr ( LOCALHOST ) ;

	// Name the socket.
	code = bind ( sock, ( struct sockaddr * ) & addr, sizeof ( struct sockaddr_in ) ) ;
	if ( code == -1 ) {
		perror ( "bind" ) ;
		close ( sock ) ;
		exit ( 1 ) ;
	}

	// Return the socket ID.
	return ( sock ) ;
}

int main ( int argc, char * argv [ ] )
{
	printf("TAILLE struct %lu, %lu", sizeof(struct Room),sizeof(char));
	// Name the socket.
	int sock ;
	
	sock = naming ( ) ;
	

	// Answering incoming requests.
	printf ( "Stier server ready to fire\n" ) ;
	fflush(stdout);
	answer_client ( sock ) ;
		     

	// This point in the program will never be reached.
	return ( 0 ) ;
}
