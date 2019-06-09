#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
   

int main()
{      //prepare db
  MYSQL *conn = mysql_init(NULL);
	if(conn == NULL) {
		printf("fail to init connection.\n");
		mysql_close(conn);
		exit(1);
  }  
  //connect db
  if(mysql_real_connect(conn, "coen243.ccj2rgp4lzvc.us-west-1.rds.amazonaws.com", 
		"group_member", "coen243", "coen243_project", 0, NULL, 0) == NULL) {
		printf("fail to establish DB real connection.\n");
		mysql_close(conn);
		exit(1);
  }    
  
  if (mysql_query(conn, "SELECT max FROM user_input WHERE type='temperature'")) // select from DB
  {
  }
  
  MYSQL_RES *result = mysql_store_result(conn);
  
  if (result == NULL) // check if query is valid
  {
    printf("fail to retrieve from DB.\n");
		mysql_close(conn);
		exit(1);  }
      
  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  
  while ((row = mysql_fetch_row(result))) 
  { 
          float f = row[0] ? atof(row[0]) : 0.0f; //convert to float 
	  
         // print fetched row to float value
        printf("%2.1f\n", f);
        
	//check temp value using operands 
      if (f >= 28.0f){ 
      printf("turn fan or ac ON n");  //insert LED function here
    }  else
      printf("keep fan or ac OFF \n"); // keep LED at null or LOW
    }
      // Free results when completed, must do this otherwise will leak memory
      mysql_free_result(result);
      mysql_close(conn);
      exit (0);
    }
