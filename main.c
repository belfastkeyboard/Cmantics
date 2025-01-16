#include "json.h"

#define PATH "../example.json"


// TODO:
//  stuff for writing to .json


int main(void)
{
    Value *json = json_open(PATH);

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

    return 0;
}



