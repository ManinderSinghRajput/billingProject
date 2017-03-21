#include<stdio.h>
#include<string.h>
#include<libpq-fe.h>
#include<postgres.h>
#include<postgres_fe.h>


char res_freed='0';
long long int start_time, end_time, query_exec_time;
/* database variables */
const char *conninfo;
PGconn *dbconnection;
/* to store result */
PGresult *res;

//get postgresql version
void getPsqlVersion()
{
  int lib_ver = PQlibVersion();
  printf("  You are using %d version of postgresql.\n", lib_ver);
}

//calculate time in seconds
long long int getTimeOfDayInSec()
{
  struct timeval want_time;
  long long int timestamp;

  gettimeofday(&want_time, NULL);
  timestamp = want_time.tv_sec;
  return timestamp;
}

/* Mode:
 * 1 : For clearing result structure
 * 2 : For closing conn and clearing result structure
 * 3 : For exiting program after closing conn and clearing result structure
 */

void cleanup(PGconn *conn, PGresult *res,int mode) 
{   
    char exit_flag = '0'; 
    
    switch(mode)
    {
      case 3:
        exit_flag='1';
      case 2:
      if(PQstatus(conn) == CONNECTION_OK)
        PQfinish(conn);
      case 1:    
      if(res && res_freed=='0')
      {
        res_freed='1';
        PQclear(res);
      }
    }

    if(exit_flag  == '1')
      exit(1);
}

PGconn* connect_with_psql()
{
  conninfo = "dbname=sidedb user=maninder";
  /* make a connection to the database */
  dbconnection = PQconnectdb(conninfo);
  /* Check to see that the backend connection was successfully made */
  if(PQstatus(dbconnection) != CONNECTION_OK)
  {
    printf("  Connection to database failed: %s\n", PQerrorMessage(dbconnection));
    
    cleanup(dbconnection, NULL,3);
  }
  else
  {
    printf("  Connection to database is successful.\n");
  }
  
  return(dbconnection);
}

int executeQuery(char *query,PGresult **exec_res, int mode, int clear_result)
{
  int ret=0;
  //We can also use following query to get number of connections
  //select sum(numbackends) from pg_stat_database;
  *exec_res = PQexec(dbconnection, query);
  res_freed=0;

  if(PQresultStatus(*exec_res) == PGRES_BAD_RESPONSE || PQresultStatus(*exec_res) == PGRES_FATAL_ERROR || PQresultStatus(*exec_res) == PGRES_NONFATAL_ERROR)
  {
    fprintf(stderr, "  Query execution Failed:\n  %s\n", PQerrorMessage(dbconnection));
    ret=-1;
    cleanup(dbconnection, *exec_res, 1);
  }
  else if (PQresultStatus(*exec_res) == PGRES_TUPLES_OK)
  {
    if(!(PQntuples(res)))
    {
      fprintf(stderr, "  No data retreived\n  %s\n", PQerrorMessage(dbconnection));
      ret=-1;
      cleanup(dbconnection, *exec_res, 1);
    }
  }

  if(clear_result)
    cleanup(dbconnection, *exec_res, 1);

  return ret;
}

void addTables()
{
  int ret=0;
  char query[2*1024];

  //Making query
  sprintf(query,"DROP TABLE IF EXISTS B_Inventory");
  ret|=executeQuery(query,&res,0,1);

  sprintf(query,"CREATE TABLE B_Inventory(ItemId INTEGER PRIMARY KEY,ItemName VARCHAR(100), Price float8, Available INT,Seller VARCHAR(100))");
  ret|=executeQuery(query,&res,0,1);
 
  sprintf(query,"DROP TABLE IF EXISTS B_Members");
  ret|=executeQuery(query,&res,0,1);

  sprintf(query,"CREATE TABLE B_Members(MemberId INTEGER PRIMARY KEY,MemberName VARCHAR(100), MobileNo VARCHAR(11),"\
                "Address VARCHAR(1000),Role VARCHAR(10))");
  ret|=executeQuery(query,&res,0,1);
 
  sprintf(query,"DROP TABLE IF EXISTS B_Admin");
  ret|=executeQuery(query,&res,0,1);

  sprintf(query,"CREATE TABLE B_Admin(MemberId INTEGER PRIMARY KEY,Password VARCHAR(500))");
  ret|=executeQuery(query,&res,0,1);
  
  if(ret != 0)
    exit(-1);
}

/*void main(int argc, char* argv[])
{
  
  char query[1024]={0};
  getPsqlVersion();

  dbconnection=connect_with_psql();

  //addTables(); 

  cleanup(dbconnection, res, 2);
}*/
