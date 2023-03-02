/*main.c*/

/*main.c*/

//
// Project 02: schema and AST output for SQL
//
// Emma Scally
// Northwestern University
// CS 211, Winter 2023
//

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h> // true, false


#include "database.h"
#include "parser.h"
#include "ast.h"
#include "analyzer.h"
#include "util.h"



/*print_schema is a required function. 
Takes in 1 argument: a database 
Traverses and outputs the the database "schema" returned by database_open(). 
Prints the name of the database, followed by the name of each table and all of the table's meta-data, including record size and each column. 
Also outputs the meta-data for each column, including column name, type (real, integer, or string), and index type (index, non-index, or unique index).
*/

void print_schema(struct Database *db) {
  
  int rows = db->numTables;
  //loops through rows and outputs meta-data
  for (int i = 0; i < rows; i++) { 
    printf("Table: %s\n", db->tables[i].name);
    printf("  Record size: %d\n", db->tables[i].recordSize);
    int cols = db->tables[i].numColumns;
    //loops through the columns of each row and outputs meta-data, checking 
    //for the column type and index type
    for (int j = 0; j < cols; j++) {
      printf("  Column: %s, ", db->tables[i].columns[j].name);
      if (db->tables[i].columns[j].colType == COL_TYPE_INT) {
        printf("int, ");
      } else if (db->tables[i].columns[j].colType == COL_TYPE_REAL) {
        printf("real, ");
      } else if (db->tables[i].columns[j].colType == COL_TYPE_STRING) {
        printf("string, ");
      }
      if (db->tables[i].columns[j].indexType == COL_NON_INDEXED) {
        printf("non-indexed\n");
      } else if (db->tables[i].columns[j].indexType == COL_INDEXED) {
        printf("indexed\n");
      } else if (db->tables[i].columns[j].indexType == COL_UNIQUE_INDEXED) {
        printf("unique indexed\n");
      }
    }
  }
  printf("**END OF DATABASE SCHEMA**\n");
}


/*another required function
function takes in 1 argument: a query
print_ast traverses an abstract syntax tree (AST) and outputs information, such as the query table name, column names, column function types, and looks for clauses like join, where, order by, limit, and into. It also identifies and outputs expression operators (<, >, <=, etc.) and literal types (string, int, or real literal). 
*/

void print_ast(struct QUERY *myQuery) {
  
  printf("**QUERY AST**\n");
  struct SELECT *sel = myQuery->q.select;
  printf("Table: %s\n", sel->table);
  struct COLUMN *c = sel->columns;
  //if column is not null, check to see if the col. has a function like max, min, etc.
  //Then, go to the next column. loop ends when column equals null
  while (c != NULL) {
    if (c->function==MAX_FUNCTION){
      printf("Select column: MAX(%s.%s) \n", c->table, c->name);
    }
    else if (c->function==MIN_FUNCTION){
      printf("Select column: MIN(%s.%s) \n", c->table, c->name);
    }
    else if (c->function==SUM_FUNCTION){
      printf("Select column: SUM(%s.%s) \n", c->table, c->name);
    }
    else if (c->function==AVG_FUNCTION){
      printf("Select column: AVG(%s.%s) \n", c->table, c->name);
    }
    else if (c->function==COUNT_FUNCTION){
      printf("Select column: COUNT(%s.%s) \n", c->table, c->name);
    }
    else{
      printf("Select column: %s.%s \n", c->table, c->name);
    }
    //this is the statement to get the next column
    c = c->next;
  }
  
  //after getting all the columns, this checks for the 5 different kids of clauses: join, where,
  //orderby, limit, and into. 
  if (sel->join != NULL) {
    printf("Join %s On %s.%s = %s.%s\n", sel->join->table, sel->join->left->table, sel->join->left->name,
           sel->join->right->table, sel->join->right->name);
  } else {
    printf("Join (NULL)\n");
  }
  if (sel->where != NULL) {
    printf("Where %s.%s ", sel->where->expr->column->table,
           sel->where->expr->column->name);
    if (sel->where->expr->operator== EXPR_LT) {
      printf("< ");
    } 
    else if (sel->where->expr->operator== EXPR_LTE) {
      printf("<= ");
    } 
    else if (sel->where->expr->operator== EXPR_GT) {
      printf("> ");
    } 
    else if (sel->where->expr->operator== EXPR_GTE) {
      printf(">= ");
    } 
    else if (sel->where->expr->operator== EXPR_EQUAL) {
      printf("= ");
    } 
    else if (sel->where->expr->operator== EXPR_NOT_EQUAL) {
      printf("<> ");
    } 
    else if (sel->where->expr->operator== EXPR_LIKE) {
      printf("like ");
    }
    if (sel->where->expr->litType == INTEGER_LITERAL) { //using atoi to convert string into integer
      printf("%d\n", atoi(sel->where->expr->value));
    } 
    else if (sel->where->expr->litType == REAL_LITERAL) { 
      printf("%d\n", atoi(sel->where->expr->value));
    } 
    else if (sel->where->expr->litType == STRING_LITERAL) {
      //if expression contains single quotes, make into double
      if (strchr(sel->where->expr->value, '\'') != NULL) { 
        printf("\"%s\"\n", sel->where->expr->value);
      }
      else{
        //else, always print with single quotes
        printf("\'%s\'\n", sel->where->expr->value);
      }
    }

  } else {
    printf("Where (NULL)\n");
  }
  if (sel->orderby != NULL) {
    printf("Order By ");
    //if orderby is not null, check to see if the col.
    //has a function like max, min, etc, just like we did at the beginning.
    if (sel->orderby->column->function==MAX_FUNCTION){
      printf("MAX(%s.%s) %s\n", sel->orderby->column->table, 
        sel->orderby->column->name, 
        sel->orderby->ascending ? "ASC" : "DESC");
    }
    else if (sel->orderby->column->function==MIN_FUNCTION){
      printf("MIN(%s.%s) %s\n", sel->orderby->column->table, 
        sel->orderby->column->name, 
        sel->orderby->ascending ? "ASC" : "DESC");
    }
    else if (sel->orderby->column->function==SUM_FUNCTION){
      printf("SUM(%s.%s) %s\n", sel->orderby->column->table, 
        sel->orderby->column->name, 
        sel->orderby->ascending ? "ASC" : "DESC");
    }
    else if (sel->orderby->column->function==AVG_FUNCTION){
      printf("AVG(%s.%s) %s\n", sel->orderby->column->table, 
        sel->orderby->column->name, 
        sel->orderby->ascending ? "ASC" : "DESC");
    }
    else if (sel->orderby->column->function==COUNT_FUNCTION){
      printf("COUNT(%s.%s) %s\n", sel->orderby->column->table, 
        sel->orderby->column->name, 
        sel->orderby->ascending ? "ASC" : "DESC");
    }
    else{
      printf("%s.%s %s\n", sel->orderby->column->table, sel->orderby->column->name, sel->orderby->ascending ? "ASC" : "DESC");
    }
    
  } else {
    printf("Order By (NULL)\n");
  }
  if (sel->limit != NULL) {
    printf("Limit %d\n", sel->limit->N);
  } else {
    printf("Limit (NULL)\n");
  }
  if (sel->into != NULL) {
    printf("Into %s\n", sel->into->table);
  } else {
    printf("Into (NULL)\n");
  }
}


/*another required function
execute_query takes in 2 arguments: database and query
builds a filename in the format "Database/table.data", then checks to see if this file can be opened
if opening the file is successful, allocates buffer for the filename (using a factor of DATABASE_MAX_ID_LENGTH)
by dynamically allocating the neccessary memory based on the database table's record size
If this is a success, read from the file and output the first 5 lines
*/

void execute_query(struct Database *myDatabase, struct QUERY *query){
  
  printf("**END OF QUERY AST**\n");
  char filename[DATABASE_MAX_ID_LENGTH*2 + 10]; 
  strcpy(filename, myDatabase->name);
  strcat(filename,"/");
  int index = 0;
  struct SELECT *select = query->q.select;
  struct COLUMN *col = select->columns;

  for (int i = 0; i<DATABASE_MAX_ID_LENGTH; i++){
    if (strcasecmp(myDatabase->tables[i].name, select->table)==0){ 
      //compares query table name and database table name. if the names are a match, add 
      //to the filename 
      strcat(filename, myDatabase->tables[i].name);
      strcat(filename, ".data");
      index = i;
      break;
    }  
  }
  
  FILE *openfile = fopen(filename, "r");
  if (openfile == NULL) {
    printf("**ERROR: unable to open '%s' .\n", filename);
  }
  char* buffer = (char*) malloc(sizeof(char)*myDatabase->tables[index].recordSize+3); 
  if (buffer == NULL){
    panic ("out of memory");
  }
  for (int j = 0; j<5;j++){   //loop will run 5 times to print 5 lines
    fgets(buffer, myDatabase->tables[index].recordSize+3, openfile);  //gets the next line
    printf("%s", buffer);
  }
}



/*Prompts the user to input a database, then sees of this database can be opened
If successful, calls function print-schema to output database schema
Then prompts user for a query - if the query is not eof, use analyzer_build to build an abstract syntax tree
Then call print_ast() and execute_query() to output AST and first 5 lines of a file (created in execute_query), which contain database table's file.
  */

int main() {

  //allocates space: max database name length + string null terminator
  char myDatabase[DATABASE_MAX_ID_LENGTH + 1]; 
  printf("Database? ");
  scanf("%s", myDatabase);
  struct Database *input = database_open(myDatabase);
  if (input == NULL) {
    printf("**Error: unable to open database '%s'\n", myDatabase);
    exit(-1);
  } 
  else {
    printf("**DATABASE SCHEMA**\n");
    printf("Database: %s\n", myDatabase);
    print_schema(input);
  }
  parser_init();
  while (true) {
    printf("query? "); 
    struct TokenQueue *myParse = parser_parse(stdin); //uses scanner to input query from user
   
    if (myParse == NULL) {  //if query input is null, check if input was eof   
      bool iseof = parser_eof();      
      if (iseof) {
        break;
      } else if (!iseof) { //if not eof, then get another query
        continue;
      }
    }
   
    else {
      struct QUERY *myQuery = analyzer_build(input, myParse); 
      if (myQuery == NULL){ //if the query is null, get another query
        continue;
      }
      else{
        print_ast(myQuery);     
        execute_query(input, myQuery);
      }
    }
  }
  database_close(input);    
  return 0;
}


