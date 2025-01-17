# JSON Parsing For C

Simple interface for using the JSON file format in C.

## EXAMPLES
### READ FROM FILE
```c
json_open();

JSON *json = json_read("json_example.json");

json = json_find(json,
                 "courses");

json = json_lookup(courses,
                   0);

JSON *course_id = json_find(course,
                            "course_id");

JSON *course_name = json_find(course,
                              "course_name");

JSON *credits = json_find(course,
                          "credits");

JSON *grade = json_find(course,
                        "grade");

printf("\nCourse: {\n\tcourse_id: %s,\n\tcourse_name: %s,\n\tcredits: %d,\n\tgrade: %s\n};\n",
       course_id->type.s,
       course_name->type.s,
       credits->type.i,
       grade->type.s);

json_close();
```

### WRITE TO FILE
```c
json_open();

JSON *value = json_open("json_example.json");

json_write("json_copy.json",
           value);

json_close();
```

### MODIFY FILE
```c
json_open();

JSON *obj = json_read("example.json");


JSON *employees = json_find(obj,
                            "employees");

JSON *employee = json_make_object();

int id = 9545;
JSON *employee_id = json_make_value(HINT_INT,
                                    &id);

json_push_object(employee->type.o,
                 "id",
                 employee_id);

JSON *employee_name = json_make_value(HINT_STRING,
                                      "Paul O'Brien");

json_push_object(employee->type.o,
                 "name",
                 employee_name);

json_push_array(employees->type.a,
                employee);

json_write("example.json",
           obj);

json_close();
```

## TYPES
The ```JSON``` struct contains a type hint and the relevant data:

```c
typedef struct JSON
{
    Hint hint;
    Type type;
} JSON;
```

```Hint``` is an enum type which indicates the correct type contained in the ```Type``` value.

```Type``` is a union. It contains all possible JSON types represented with C types:
```c
// json type: null
void *null = NULL;

// json type: string
char *string = "string";

// json type: integer
int number_int = 0;

// json type: float
float number_float = 0.5f;

// json type: bool
bool true_or_false = false;

// json type: object
Object *obj;

// json type: array
Array *arr;
```

```Object``` and ```Array``` are opaque types.

#### Object
To retrieve data from an ```Object``` call the ```json_find()``` function and provide a key. 
If successful returns valid ```JSON*``` else returns ```NULL```.

Insert an element into an object with ```json_push_object()``` and provide a key and value.
If the key already exists the value will be overwritten, otherwise a new key-value pair will be inserted.

Remove an element from an object with ```json_pop_object()``` and provide a key.
If the key exists, the element will be removed. 
If the key does not exist, the object will not be modified.

#### Array
To retrieve data from an ```Array``` call the ```json_lookup()``` function and provide an index.
If successful returns valid ```JSON*``` else returns ```NULL```.

Insert an element into an array with ```json_push_array()``` and provide a value.
The value will be inserted at the back of the array.

Remove an element from an array with ```json_pop_object()``` and provide an index.
If the index is valid, the element will be removed. 
If the index is not valid, the object will not be modified.
