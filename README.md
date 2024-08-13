# HOW TO RUN THE APPLICATION


STEP 1 : Initialize Database

```
g++ -o db/initialize_db db/initialize_db.cpp -lsqlite3
```
STEP 2: Connect to Database

```
./db/initialize_db
```

STEP 3: Compile the application code

```
g++ -o todo_app app.cpp -lsqlite3
```

STEP 4: Execute the app
```
./todo_app
```