#include<stdio.h>
#include<string.h>
#include<libpq-fe.h>
#include<postgres.h>
#include<postgres_fe.h>

#include"create_conn.h"

PGconn *dbconnection;
PGresult *res;

int get_item_name_and_price_from_DB(int id, double* price, char * item_name)
{
  char query[1024]={0};
  int ret=0;
  sprintf(query, "SELECT itemname,price from b_inventory where itemid=%d;", id);
  ret=executeQuery(query,&res,1,0);
  if(ret==0)
  {
    sprintf(item_name,"%s",PQgetvalue(res, 0, 0));
    *price=atof(PQgetvalue(res, 0, 1));
  }
  return ret;
}

int decrementCountByItemId(int id)
{
  char query[2*1024]={0};
  int ret=0;
  sprintf(query, "UPDATE b_inventory SET available=available-1 where itemid=%d;", id);
  ret=executeQuery(query,&res,0,1);
  return ret;
}

int checkAvailablityById(int id, int *available_units)
{
  char query[2*1024]={0};
  int ret=0;
  sprintf(query, "SELECT available from b_inventory where itemid=%d;", id);
  ret = executeQuery(query,&res,1,0);
  if(ret==0)
    *available_units=atoi(PQgetvalue(res, 0, 0));
  return ret;
}

int fetchDetailsWithMobNo(long long mob_no, int *member_id, char *name, char *address, char *role)
{
  char query[2*1024]={0};
  int ret=0;
  sprintf(query, "SELECT memberid,membername,address,role from b_members where mobileno='%lld';", mob_no);
  ret = executeQuery(query,&res,1,0);
  if(ret < 0)
    printf("Entered Mobile no is not registered.\n");
  else
  {
    *member_id=atoi(PQgetvalue(res, 0, 0));
    sprintf(name,"%s",PQgetvalue(res, 0, 1));
    sprintf(address,"%s",PQgetvalue(res, 0, 2));
    sprintf(role,"%s",PQgetvalue(res, 0, 3));
  }
  return ret;
}

void main()
{
  char query[2*1024]={0};
  dbconnection = connect_with_psql();
  int id=0,available_units=0,mem_id=0,ret=0;
  double price=0;
  char item_name[100];
  char mem_name[100];
  char mem_role[100];
  char mem_address[1000];
  long long mob_no;
  printf("Enter the ID of product:");
  scanf("%d",&id);
  ret=get_item_name_and_price_from_DB(id, &price, item_name);
  if(!ret)
  {
    printf("Name of the item is: %s\n",item_name);
    printf("Price of item is: %.2lf\n",price);
  }
  
  ret=decrementCountByItemId(id);
  if(ret)
    printf("Decrement failed.");
  ret=checkAvailablityById(id, &available_units);
  if(!ret)
    printf("Available No of unit of the item is:  %d\n",available_units);
  
  printf("Enter the 10 digit mobile no of the member:");
  scanf("%lld",&mob_no);

  ret=fetchDetailsWithMobNo(mob_no, &mem_id, mem_name, mem_address, mem_role);
  if(!ret)
  {
    printf("ID of the Member is: %d\n",mem_id);
    printf("Name of the Member is: %s\n",mem_name);
    printf("Address of Member is: %s\n",mem_address);
    printf("Role of Member is: %s\n",mem_role);
  }
  /*sprintf(query,"Select * from b_inventory");
  res=executeQuery(query,0);
  int rows = PQntuples(res);
    
  for(i=0; i<rows; i++) 
  {
    printf("%s %s %s %s %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2), PQgetvalue(res, i, 2), PQgetvalue(res, i, 3));
  }*/
}
