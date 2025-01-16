# JSON Parsing For C

Simple interface for reading from .json files.

Example:

```c
Value *json = json_open("json_example.json");

Value *courses = json_find(json,
                           "courses");

Value *course = json_lookup(courses,
                            0);

Value *course_id = json_find(course,
                             "course_id");

Value *course_name = json_find(course,
                               "course_name");

Value *credits = json_find(course,
                           "credits");

Value *grade = json_find(course,
                         "grade");

printf("\nCourse: {\n\tcourse_id: %s,\n\tcourse_name: %s,\n\tcredits: %d,\n\tgrade: %s\n};\n",
       course_id->type.s,
       course_name->type.s,
       credits->type.i,
       grade->type.s);

json_close();
```

The ```Value``` struct contains a type hint and the relevant data:

```c
typedef struct Value
{
    Hint hint;
    Type type;
} Value;
```

```Type``` is a union. It contains all possible .json types represented with C types:
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

```Hint``` is an enum type which indicates the correct type contained in the ```Type``` value.

```Object``` and ```Array``` are opaque types.

To retrieve data from an ```Object``` call the ```json_find()``` function and provide a key. 
If successful returns valid ```Value*``` else returns ```NULL```.

To retrieve data from an ```Array``` call the ```json_lookup()``` function and provide an index.
If successful returns valid ```Value*``` else returns ```NULL```.
