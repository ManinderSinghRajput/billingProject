#include"create_conn.c"

extern void cleanup(PGconn *conn, PGresult *res,int mode);
extern PGconn* connect_with_psql();
extern int executeQuery(char *query,PGresult** res, int mode, int clear_result);
