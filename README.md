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

The Value struct contains a type hint and the relevant data:

```c
typedef struct Value
{
    Hint hint;
    Type type;
} Value;
```

```Type``` is a union. It contains all possible .json types:
- null
- string
- integer
- float
- bool
- object
- array

```Hint``` is an enum type which indicates the correct type contained in the ```Type``` value.

```Object``` and ```Array``` are opaque types.

To retrieve data from an ```Object``` call the ```json_find()``` function and provide a key. 
If successful returns valid ```struct Value*``` else returns ```NULL```.

To retrieve data from an ```Array``` call the ```json_lookup()``` function and provide an index.
If successful returns valid ```struct Value*``` else returns ```NULL```.
