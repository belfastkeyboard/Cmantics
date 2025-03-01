# Cmantics
## File Parsing for C

A simple interface for parsing JSON, INI, and CSV in C.


## Examples
### Read from JSON file
```c
JSON *json = create_json();

parse_json(json, "example.json");

ValueJSON *value = get_json(json);
ValueJSON *company = lookup_json(value, "company");
ValueJSON *name = lookup_json(company, "name");
ValueJSON *founded = lookup_json(company, "founded");
ValueJSON *isPublic = lookup_json(company, "isPublic");

printf("\nCompany: {\n\tname: %s,\n\tfounded: %d,\n\tisPublic: %s\n};\n",
       name->type.s,
       founded->type.i,
       (isPublic->type.b) ? "true" : "false");

destroy_json(&json);
```

### Read from INI file
```c
INI *ini = create_ini();

parse_ini(ini, "example.ini");

ValueINI *name = get_ini(ini, "company", "name");
ValueINI *founded = get_ini(ini, "company", "founded");
ValueINI *isPublic = get_ini(ini, "company", "isPublic");

printf("\nCompany: {\n\tname: %s,\n\tfounded: %d,\n\tisPublic: %s\n};\n",
       name->type.s,
       founded->type.i,
       isPublic->type.b.string);

destroy_ini(&ini);
```

### Read from CSV file
```c
CSV *csv = create_csv();

parse_csv(csv, "example.csv");

for (int r = 0; r < rows_csv(csv); r++)
{
    for (int c = 0; c < columns_csv(csv); c++)
    {
        const char *comma = (c < columns_csv(csv) - 1) ? "," : "\n";

        const ValueCSV *value = get_csv(csv, c, r);
        
        if (value.hint == CSV_STRING)
        {
            printf("%s%s", value.type.s, comma);
        }
        else if (value.hint == CSV_INT)
        {
            printf("%ld%s", value.type.i, comma);
        }
    }
    
    printf("\n");
}

destroy_csv(&csv);
```

## TYPES
Each parser is its own opaque type. 
They should be created and destroyed with their specific create and destroy functions.
Parsers user their own internal allocators and must be destroyed or memory will be leaked.

Parsers possess their own ```struct Value``` types.
These types represent an interface for accessing the possible types retrieved from a given file.
The type hint provides a way to resolve the actual type contained in the type union.

The below will demonstrate a generic implementation, common to all parsers.
This hypothetical implementation is for illustrative purposes only. 

#### Value
```c
typedef struct Value
{
    Hint hint;
    Type type;
} Hint;
```

#### Hint
```c
typedef enum Hint
{
    HINT_INT,
    HINT_FLOAT,
    HINT_BOOL,
    HINT_STRING,
    HINT_NULL
} Hint;
``` 

#### Type
```c
typedef union Type
{
    long        i;
    double      f;
    bool        b;
    const char *s;
    void       *n;
} Type;
``` 

The INI and JSON parsers can contain ```Array``` types and the JSON parser can contain an ```Object``` type. 
These types are opaque and the corresponding functions must be used to handle them.
