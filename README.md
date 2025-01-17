# JSON Parsing For C

Simple interface for using the JSON file format in C.

## EXAMPLES
### READ FROM FILE
```c
JSON *json = json_open("json_example.json");

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
JSON *value = json_open("json_example.json");

json_write("json_copy.json",
           value);

json_close();
```

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
Array *array;
```

```Object``` and ```Array``` are opaque types.

To retrieve data from an ```Object``` call the ```json_find()``` function and provide a key. 
If successful returns valid ```JSON*``` else returns ```NULL```.

To retrieve data from an ```Array``` call the ```json_lookup()``` function and provide an index.
If successful returns valid ```JSON*``` else returns ```NULL```.

When writing ```JSON*``` to file with ```json_write()``` note that ```JSON*``` must be of type ```Object*```.
