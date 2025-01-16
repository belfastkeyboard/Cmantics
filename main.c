#include "json.h"

#define PATH "../example.json"


// TODO:
//  stuff for writing to .json


#include <time.h>

int main(void)
{
    clock_t start = clock();

    Value *json = json_open(PATH);

    clock_t end = clock();

    Value *settings = json_find(json,
                                "settings");

    Value *backup = json_find(settings,
                              "backup");

    Value *lastBackup = json_find(backup,
                                 "lastBackup");

//    Value *course_name = json_find(course,
//                                   "course_name");

//    Value *credits = json_find(course,
//                               "credits");

//    Value *grade = json_find(course,
//                             "grade");

//    printf("\nCourse: {\n\tcourse_id: %s,\n\tcourse_name: %s,\n\tcredits: %d,\n\tgrade: %s\n};\n",
//           course_id->type.s,
//           course_name->type.s,
//           credits->type.i,
//           grade->type.s);

    printf("Last backup: %s.\n",
           lastBackup->type.s);

    json_close();

    double elapsed = (double )(end - start) / CLOCKS_PER_SEC;

    printf("\nTime elapsed: %f.\n",
           elapsed);

    return 0;
}
