# simplesql
building a simple sql execution environment
Most of the code was written by my teacher. My code entirely in main.c

My code:
1. inputs a dtabase name and opens this database by calling the provided database_open()
2. traverses and outputs the database meta-data (schema) returned by database_open()
3. inputs a query from the user, calls the provided parser and analyzer functiosn to parse and analyze the query
4. if successful, traveses an outputs the AST returned by analyzer_build()
5. for table specified in query's "from" clause, outputs the first 5 lines from the table's data file
6. repeat steps 3-5 until user enters "$"
