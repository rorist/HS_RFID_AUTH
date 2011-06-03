#include "mbed.h"
#include "EthernetNetIf.h"
#include "MySQLClient.h"
#include <iostream>
#include <fstream>
#include <string>

LocalFileSystem local("local");
DigitalOut led(LED1);
void getdbinfo(string dbserver, string dbname, string dbuser, string dbpass);

EthernetNetIf eth; 
MySQLClient sql;

string dbserver;
string dbname;
string dbuser;
string dbpass;

MySQLResult sqlLastResult;
void onMySQLResult(MySQLResult r)
{
  sqlLastResult = r;
}

int main()
{
  printf("Start\n");

  printf("Setting up...\n");
  EthernetErr ethErr = eth.setup();
  if(ethErr)
  {
    printf("Error %d in setup.\n", ethErr);
    return -1;
  }
  printf("Setup OK\n");
  
  // Db config
  printf("Get DB info from config file.\n");
  getdbinfo(&dbserver, &dbuser, &dbpass, &dbname);
  printf("DB info OK");
  
  Host host(IpAddr(), 3306, dbserver);
  
  //Connect
  sqlLastResult = sql.open(host, dbuser, dbpass, dbname, onMySQLResult);
  while(sqlLastResult == MYSQL_PROCESSING)
  {
    Net::poll();
  }
  if(sqlLastResult != MYSQL_OK)
  {
    printf("Error %d during connection\n", sqlLastResult);
  }else{
    printf("SQL connection OK\n");
  }
  
  //SQL Command
  //Make command
  char cmd[256] = {0};
  const char* msg="HOHO";
  sprintf(cmd, "INSERT INTO `base`.`table` (`col`) VALUES ('%s');", msg);
  
  //INSERT INTO DB
  string cmdStr = string(cmd); 
  sqlLastResult = sql.sql(cmdStr);
  while(sqlLastResult == MYSQL_PROCESSING)
  {
    Net::poll();
  }
  if(sqlLastResult != MYSQL_OK)
  {
    printf("Error %d during SQL Command\n", sqlLastResult);
  }
  
  sql.exit();
  
  while(1)
  {
  
  }
  
  return 0;
}

void getdbinfo(string dbserver, string dbname, string dbuser, string dbpass) {
    string line;
    ifstream ifs("/local/db.cnf");
    if (ifs.is_open())
    {   
        // Get values
        int linenb = 1;
        while (ifs.good())
        {
            getline(ifs, line);
            switch(linenb){
                case 1:
                    dbserver = line;
                    break;
                case 2:
                    dbuser = line;
                    break;
                case 3:
                    dbpass = line;
                    break;
                case 4:
                    dbname = line;
                    break;            
            }
            linenb++;
        }
        ifs.close();
        // Check values
        /*
        printf("server: %s\n", dbserver);
        printf("user: %s\n", dbuser);
        printf("pass: %s\n", dbpass);
        printf("name: %s\n", dbname);
        */
    } else {
        printf("file error\n");
        while(1){
            led = 1;
            wait(1);
            led = 0;
            wait(1);
        }
    }
}

