/*
 * Birzeit University
 * To: Dr. Majdi Mafarja
 * Ahmad Sawi
 * 1150007
 * Data Structures Project 1
 * */

/*
 * Summary:
 *  this application works as a registration system for Birzeit Universiry Computer Science department. it takes two files one for the courses
 *  that the department is offering and the other for the students and what they want to register. and it returns a report file that includes
 *  registration info the head of the department requiers.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STUDENTNAMELENGTH 20        //self explanatory macros. to be used for strings!
#define COURSENAMELENGTH 50
#define SCANNEDSTRINGMAXLENGTH 200
#define IDSIZE 9
#define CLOCKARRAYSIZE 9
#define PATHLENGTH   50

struct student{
    int id;
    char name[STUDENTNAMELENGTH];
    int coursesIndexes[5];
    int numberOfCoursesRegistered;
    struct student* next;
};

typedef struct student student_t;
typedef student_t *p2s;
typedef p2s List;
typedef p2s Position;


struct course{                                 //course struct to hold info for courses and a list of the students in thsat course
    char name[COURSENAMELENGTH], id[IDSIZE];   //a course is assumed to be 7 letters only "COMP" followed by 3 letters
    short maxStudents, year;
    double startTime, finishTime;
    List students;
};

typedef struct course course_t;

int fileLineCount(FILE*);                                              //counts how many lines are there in a file
void getFromFile(FILE*, char[][SCANNEDSTRINGMAXLENGTH]);               //gets everything in a file and stors it in an array of strings
void makeDot(char[]);                                                  //makes ':' a '.'
bool fillCourseArrayFromFile(course_t[], FILE*, int);                  //fills the array of courses with the courses info from file
void removeNonCOMPCourses(course_t[], int*);                           //removes courses that dont start with COMP from courses array
void removeSpace(char[]);                                              //removes ' '
void removeNewLine(char[]);                                            //removes '\n'
bool isCourseOffered(char[], course_t[], int);                         //checks if a course is one of the array of courses by its ID
int findAddressOfCourse(char[], course_t[], int);                      //returns the address of the course given the ID of
bool isAllowedToRegister(int, int, course_t[]);                        //checks if a student is allowed to register by year
int studentsInCourse(List);                                            //returns the number of students enrolled
int maxStudentsInCourse(int, course_t[]);                              //returns the maximum students allowed to register in a course
void initiateClockArray(bool[]);                                       //sets all clock array values to false
int getAddressForClock(double);                                        //returns a suitable address based on
void fillClockArray(bool[], double, double);                           //fills the addresses in the interval given with true
bool isNotConflict(bool[], int, course_t[]);                           //checks if a student has a conflict in his schedule
void fillStudentsLinkedListFromFile(course_t[], FILE*,int, int, List); //fills the students who want to register in a course in their lists
List newHead();                                                        //makes a new head for a list of students
Position newNode(char[], int);                                         //makes a new node for a list of students
bool isEmpty(List L);                                                  //checks if a list is empty
bool isLast(Position);                                                 //checks if a certian node is the last one in a list
void deleteList(List);                                                 //deletes the list from memory but keeps the head
bool insertFirst(List, char[], int);                                   //inserts a node to a list in the first spot
bool insertLast(List, char[], int);                                    //inserts a node to the end of the list
bool insertAfter(Position, Position);                                  //inserts a node after a special possition
bool insertSorted(List, char[STUDENTNAMELENGTH], int);                 //inserts a node based on a string value to its respective position
Position findStudent(List, int);                                       //finds and returns the position of a student from list
void initiateBuckets(List[]);                                          //puts null pointers at the head of the buckets
void emptyBuckets(List[]);                                             //empties and delets all buckets
List radixMerge(List[]);                                               //merges buckets into one list. to be used for radix sort
List radixSortedList(List, int);                                       //uses radix sort algorithm to sort a list
void sortCourses(course_t[], course_t[], int);                         //uses the bubble sort to arrange an array of courses
void courseStudentsReport(course_t[], FILE *, int);                    //reports to the report file the students registered in each course
bool studentScheduleReport(FILE*, course_t[], List, int);              //prints a students scedule to the reports file
List getAllStudentsSorted(List);                                       //groups all students into one linked list sorted by name
bool isNotAddedBefore(List, int);                                      //checks if a students has already been added to the previous list
void allStudentsReport(List , FILE*);                                  //reports all the students in all courses
void allCoursesReport(course_t[], FILE*, int);                         //reports all courses in the reports file
void fillClockWithIndex(int[], double, double, int);                   //special function for schedual printing espeically when dealing with lab courses. it fills an array with the index of the course in the place of the clock it will take up
int openMenu();                                                        //Menu and caller for all other functions

int main() {
    openMenu();
    return 0;
}

int fileLineCount(FILE *file){
    int count = 0;
    char ch;
    do{
        ch = (char) fgetc(file);
        if(ch == '\n')
            count++;
    } while (!feof(file));
    rewind(file);     //returns the cursor to the beginning of the file for future use
    return count+1;   //+1 because it starts from 0
}

void getFromFile(FILE *file, char ret[][SCANNEDSTRINGMAXLENGTH]){
    int i = 0;
    char buffer[SCANNEDSTRINGMAXLENGTH];
    while (fgets(buffer, SCANNEDSTRINGMAXLENGTH, file) != NULL)
        strcpy(ret[i++],buffer);
    fclose(file);
}

void makeDot(char s[]){
    int i = 0;
    for (i = 0; i < strlen(s); ++i) {
        if(s[i] == ':')
            s[i] = '.';
    }
}


bool fillCourseArrayFromFile(course_t coursesArray[], FILE *coursesFile, int numberOfCourses){

    char coursesStringsArray[numberOfCourses][SCANNEDSTRINGMAXLENGTH];
    getFromFile(coursesFile, coursesStringsArray);

    char data[6][COURSENAMELENGTH];
    int q = 0, j = 0;

    for (j = 0; j < numberOfCourses; j++) {

        char *token;
        token = strtok(coursesStringsArray[j], "#");
        q = 0;
        while(token != NULL){
            strcpy(data[q++], token);
            token = strtok(NULL, "#");
        }

        strcpy(coursesArray[j].name, data[0]);
        strcpy(coursesArray[j].id, data[1]);
        coursesArray[j].year = (short) atoi(data[2]);
        makeDot(data[3]);
        coursesArray[j].startTime = atof(data[3]);
        makeDot(data[4]);
        coursesArray[j].finishTime = atof(data[4]);
        coursesArray[j].maxStudents = (short) atoi(data[5]);
        coursesArray[j].students = (List) malloc(sizeof(student_t));
        if(coursesArray[j].students == NULL) {                         //checks if the malloc worked properly
            printf("OUT OF RAM! malloc related error");
            return false;
        }
        coursesArray[j].students->next = NULL;
    }
    return true;
}

void removeNonCOMPCourses(course_t ocrss[], int *numberOfCourses){
    int i = 0, k = 0;
    char substring[5] = "";
    for (i = 0; i < *numberOfCourses; i++){
        strncat(substring, ocrss[i].id, 4);
        if( strcmp(substring, "COMP") != 0 ){
            for (k = i; k < *numberOfCourses; k++){
                ocrss[k] = ocrss[k+1];
            }
            *numberOfCourses = *numberOfCourses - 1;
            i--;
        }

        strcpy(substring ,"");
    }
}

void removeSpace(char s[]){
    int i = 0, k = 0;
    for(i = 0; i < strlen(s); i++){
        if(s[i] == ' '){
            for(k = i; k < strlen(s); k++){  //shifting all characters so we have a tidy string when space is removed
                s[k] = s[k+1];
            }
            s[k-1] = '\0';   //setting last element to null so it declares the end of the list
            i--;
        }
    }
}

void removeNewLine(char s[]){   //same as removeSpace
    int i = 0, k = 0;
    for(i = 0; i < strlen(s); i++){
        if(s[i] == '\n'){
            for(k = i; k < strlen(s); k++){
                s[k] = s[k+1];
            }
            s[k-1] = '\0';
        }
    }
}

int findAddressOfCourse(char crs[], course_t ocrss[], int numberOfOfferedCourses){
    int i = 0;
    for(i = 0; i < numberOfOfferedCourses; i++){
        if( strcmp(ocrss[i].id, crs) == 0 )
            return i;
    }
    return -1;    //to identify a course not being found
}

bool isCourseOffered(char crs[], course_t ocrss[], int numberOfOfferedCourses){
    if(findAddressOfCourse(crs, ocrss, numberOfOfferedCourses) < 0)
        return false;
    else
        return true;
}

bool isAllowedToRegister(int stNumber, int addressOfcrs, course_t ocrss[]){
    if(addressOfcrs < 0)
        return false;
    else if(stNumber <= ((ocrss[addressOfcrs].year)*10000 + 9999) )  //checks the year to be bigger of equal to the one in course
        return true;

    else
        return false;
}

int studentsInCourse(List L){
    int counter = 0;
    Position p = L;
    while(p->next != NULL){
        counter++;
        p = p->next;
    }
    return counter;
}

int maxStudentsInCourse(int index, course_t ocrss[]){  //ocrss stands for Offered Courses
    return (ocrss[index].maxStudents);
}

void initiateClockArray(bool clk[CLOCKARRAYSIZE]){
    int i = 0;
    for(i = 0; i < CLOCKARRAYSIZE; i++)
        clk[i] = false;
}

/*
     * note: it has been assumed that there cant be a schedule with courses that have no breaks in between, like 11 - 12:30 & 12:30 - 2pm
     * key: index | time of day
     * 0 | 8 - 9
     * 1 | 9 - 10
     * 2 | 10 - 11
     * 3 | 11 - 12
     * 4 | 12 - 1pm
     * 5 | 1pm - 2pm
     * 6 | 2pm - 3pm
     * 7 | 3pm - 4pm
     * 8 | 4pm - 5pm
     * -1 if wrong time or time where there should be no classes
     * */
int getAddressForClock(double time){
    if(time >= 8.0 && time < 9.0)
        return 0;
    else if(time >= 9.0 && time < 10.0)
        return 1;
    else if(time >= 10.0 && time < 11.0)
        return 2;
    else if(time >= 11.0 && time < 12.0)
        return 3;
    else if(time >= 12.0 && time < 13.0)
        return 4;
    else if(time >= 13.0 && time < 14.0)
        return 5;
    else if(time >= 14.0 && time < 15.0)
        return 6;
    else if(time >= 15.0 && time < 16.0)
        return 7;
    else if(time >= 16.0 && time < 17.0)
        return 8;
    else
        return -1;
}

void fillClockArray(bool clk[], double start, double finish){
    int stAddress = getAddressForClock(start), fnAddress = getAddressForClock(finish), i = 0;
    for(i = stAddress; i <= fnAddress; i++){
        clk[i] = true;
    }
}

bool isNotConflict(bool clk[CLOCKARRAYSIZE], int addressOfcrs, course_t ocrss[]){
    //return true;

    if(addressOfcrs < 0)
        return false;

    int i = 0;
    double start = ocrss[addressOfcrs].startTime;
    int stAddress = getAddressForClock(start);

    double finish = ocrss[addressOfcrs].finishTime;
    int fnAddress = getAddressForClock(finish);

    if(start >= finish)
        return false;

    for(i = stAddress; i <= fnAddress; i++){   //checks every element it needs to be empty so it makes sure there will be no conflicts
        if(clk[i] == true)
            return false;
    }

    fillClockArray(clk, start, finish);
    return true;
}

void fillStudentsLinkedListFromFile(course_t coursesArray[], FILE *studentsFile, int numberOfCourses, int numberOfStudents, List miniStudentList){

    char studentsStringArray[numberOfStudents][SCANNEDSTRINGMAXLENGTH];
    getFromFile(studentsFile, studentsStringArray);

    int numberOfCoursesRegistered = 0;
    int j = 0;
    bool clocks[CLOCKARRAYSIZE];
    initiateClockArray(clocks);

    Position p = miniStudentList;   //to store a list of students and the addresses of their courses for easier access when making reports

    for(j = 0; j < numberOfStudents; j++){
        numberOfCoursesRegistered = 0;
        char *token2;
        char nameTemp[STUDENTNAMELENGTH];
        int idTemp;

        //miniStudentList[j].numberOfCoursesRegistered = 0;

        token2 = strtok(studentsStringArray[j], "#");
        strcpy(nameTemp, token2);
        //strcpy(miniStudentList[j].name, nameTemp);

        //puts(nameTemp);

        token2 = strtok(NULL, "#");
        idTemp = atoi(token2);   //atoi converts from string to integer
        //miniStudentList[j].id = idTemp;

        insertLast(miniStudentList, nameTemp, idTemp);
        //insertSorted(miniStudentList, nameTemp, idTemp);
        //p = miniStudentList;

        Position t = p->next;
        t->numberOfCoursesRegistered = 0;

        token2 = strtok(NULL, "#");
        while(token2 != NULL){
            removeSpace(token2);
            removeNewLine(token2);
            //puts(token2);
            int indexOfAddress = findAddressOfCourse(token2,coursesArray, numberOfCourses);

            if(numberOfCoursesRegistered < 5 && isCourseOffered(token2, coursesArray, numberOfCourses) && isAllowedToRegister(idTemp, indexOfAddress, coursesArray) && (studentsInCourse(coursesArray[indexOfAddress].students) < maxStudentsInCourse(indexOfAddress, coursesArray)) && isNotConflict(clocks, indexOfAddress, coursesArray)){
                insertSorted(coursesArray[indexOfAddress].students, nameTemp, idTemp);
                //puts("*********");    //for testing and analysis
                //miniStudentList[j].coursesIndexes[numberOfCoursesRegistered] = indexOfAddress;
                t->coursesIndexes[numberOfCoursesRegistered] = indexOfAddress;

                numberOfCoursesRegistered++;
                //miniStudentList[j].numberOfCoursesRegistered = numberOfCoursesRegistered;
                t->numberOfCoursesRegistered = numberOfCoursesRegistered;

                if(numberOfCoursesRegistered == 5)
                    break;
            }
            token2 = strtok(NULL, "#");

        }
        initiateClockArray(clocks);   //re-empties the clock array so it can deal with the next student
        p = p->next;
    }
}

List newHead(){
    List head = (List) malloc(sizeof(student_t));
    if(head == NULL){
        printf("OUT OF RAM! malloc realted error.\n");
        return NULL;
    }
    head->next = NULL;
    return head;
}

bool isEmpty(List L) {
    return (L->next == NULL);
}

bool isLast(Position p){
    return (p->next == NULL);
}

void deleteList(List L){
    Position p, temp;
    p = L->next;
    while(p != NULL){
        temp = p->next;
        free(p);           //so it frees every node from memory
        p = temp;
    }
    L->next = NULL;
}

Position newNode(char name[], int id){
    Position new;
    new = (Position) malloc(sizeof(student_t));
    if(new == NULL){
        printf("OUT OF RAM! malloc realted error.\n");
        return NULL;
    }
    new->id = id;
    strcpy(new->name, name);
    new->next = NULL;
    return new;
}

bool insertFirst(List L, char name[STUDENTNAMELENGTH], int id){
    Position temp, p = L;
    temp = (student_t*) malloc(sizeof(student_t));
    if(temp == NULL){
        printf("OUT OF RAM! malloc realted error.\n");
        return false;
    }
    temp->id = id;
    strcpy(temp->name, name);
    temp->next = p->next;
    p->next = temp;
    return true;
}

bool insertLast(List L, char name[], int id){
    Position p = L, new = newNode(name, id);
    if(new == NULL)
        return false;
    while(!isLast(p))
        p = p->next;
    p->next = new;
    new->next = NULL;
    return true;
}

bool insertAfter(Position p, Position toAdd){
    toAdd->next = p->next;
    p->next = toAdd;
    return true;
}

bool insertSorted(List L, char name[STUDENTNAMELENGTH], int id){
    if(isEmpty(L)){
        insertFirst(L, name, id);
        return true;
    }

    Position p = L, toAdd;
    toAdd = (Position) malloc(sizeof(student_t));
    if(toAdd == NULL) {
        printf("OUT OF RAM! malloc related error.\n");
        return false;
    }

    toAdd->id = id;
    strcpy(toAdd->name, name);
    while(p->next != NULL && ( strcmp(p->next->name, name) < 0 ))         //puts names starting from the smallest(A.B...) at first
        p = p->next;
    insertAfter(p,toAdd);
    return true;
}

Position findStudent(List L, int id){
    Position p = L->next;
    while(p != NULL && p->id != id){
        p = p->next;
    }
    return p;
}

void initiateBuckets(List buckets[]){
    int i = 0;
    for (i = 0; i < 10; i++) {
        buckets[i] = newHead();
    }
}

void emptyBuckets(List buckets[]){
    int i = 0;
    for(i = 0; i < 10; i++){
        deleteList(buckets[i]);
    }
}

List radixMerge(List buckets[]){
    List head = newHead();
    int i = 0;

    for (i = 0; i < 10; i++) {
        Position p = buckets[i]->next;

        while(p != NULL){
            insertLast(head, p->name, p->id);
            p = p->next;
        }

    }

    emptyBuckets(buckets);                  //empties buckets for next use
    return head;
}

List radixSortedList(List L, int maxLength){
    List sorted = newHead();

    if(isEmpty(L)){
        return sorted;
    }

    Position p = L->next;

    //printf("******\n*********\n*********\n");

    List buckets[10];
    initiateBuckets(buckets);


    int i = 0, mod = 10, div = 1;
    for(i = 0; i < maxLength; i++){

        while(p != NULL){
            int number = (p->id)%mod;
            number = number / div;
            insertLast(buckets[number], p->name, p->id);
            p = p->next;
        }

//        for (int j = 0; j < 10; ++j) {       //for testing and analysis. i encourage you to uncomment it and see how radix sort does its magic!
//            printf("bucket %d: ",j);
//            printList(buckets[j]);
//        }
//        printf("RunDOne\n");

        deleteList(sorted);
        sorted = radixMerge(buckets);
        p = sorted->next;

        mod*=10;
        div*=10;

    }

    return sorted;
}

void sortCourses(course_t sorted[], course_t ocrss[], int numberOfCourses){
    int i = 0, j = 0, q = 0;
    course_t temp;
    for (i = 0; i < numberOfCourses; i++){
        sorted[i] = ocrss[i];
    }

    for (j = 0; j < numberOfCourses-1; j++){         //uses bubble sort because radix sort is inefficient for small arrays/lists
        for (q = j; q < numberOfCourses; q++){
            if(sorted[j].year > sorted[q].year){
                temp = sorted[q];
                sorted[q] = sorted[j];
                sorted[j] = temp;
            }
        }
    }

}

void courseStudentsReport(course_t ocrss[], FILE *out, int numberOfCourses) {
    int q = 0;
    for(q = 0; q < numberOfCourses; q++) {

        List substituteStudentList = radixSortedList(ocrss[q].students, 7);
        Position p = substituteStudentList->next;

        fprintf(out, "STUDENTS TABLE FOR %s COURSE: \n", ocrss[q].name);
        fprintf(out, "------------------------------------------------------\n");

        if (isEmpty(substituteStudentList)) {
            fprintf(out, "No Students Have Registered is This Course!\n");
            fprintf(out, "-----------------------------------------------------\n\n");

        } else {
            fprintf(out, "ID \t|\t Name\n");
            fprintf(out, "------------------------------------------------------\n");


            while (p != NULL) {
                fprintf(out, "%d |\t %s\n", p->id, p->name);
                p = p->next;
            }

            fprintf(out, "------------------------------------------------------\n\n");
        }
    }
}

List getAllStudentsSorted(List L){
    List allStudents = newHead();
    Position p = L->next;
    while (p != NULL){
        if(isNotAddedBefore(allStudents, p->id)){
            insertSorted(allStudents, p->name, p->id);
        }
        p = p->next;
    }
    return allStudents;
}

bool isNotAddedBefore(List allStudents, int id){
    Position p = allStudents->next;
    while (p != NULL){
        if(p->id == id){
            return false;
        }
        p = p->next;
    }
    return true;
}

void allStudentsReport(List L, FILE *out){
    List allStudents = getAllStudentsSorted(L);
    Position p = allStudents->next;

    fprintf(out, "ALL STUDENTS IN ALL COURSES\n");
    fprintf(out, "---------------------------------\n");

    if (isEmpty(L)) {
        fprintf(out, "No Students Have Registered This semester!\n");
        fprintf(out, "--------------------------------\n\n");
    } else {
        fprintf(out, "ID \t|\t Name\n");
        fprintf(out, "---------------------------------\n");
        while (p != NULL){
            fprintf(out, "%d |\t%s\n", p->id, p->name);
            p = p->next;
        }
        fprintf(out, "---------------------------------\n\n");
    }
    deleteList(allStudents);
}

void allCoursesReport(course_t courses[], FILE *out, int numberOfCourses){

    course_t sorted[numberOfCourses];
    sortCourses(sorted, courses, numberOfCourses);

    fprintf(out, "ALL COURSES\n");
    fprintf(out, "------------------------------------------------------------------\n");
    fprintf(out, "ID \t    | Name\t\t\t   |\t Students Enrolled\n");
    fprintf(out, "------------------------------------------------------------------\n");
    int  i = 0;
    for (i = 0; i < numberOfCourses; i++){
        fprintf(out, "%s     | %-28s |\t %d\n", sorted[i].id, sorted[i].name, studentsInCourse(sorted[i].students));
    }
    fprintf(out, "------------------------------------------------------------------\n\n");
}

void fillClockWithIndex(int courseIndex[], double sttime, double fntime, int index){
    int start = getAddressForClock(sttime);
    int finish = getAddressForClock(fntime);
    int i = 0;
    for (i = start; i <= finish; i++){
        courseIndex[i] = index;
    }
}

bool studentScheduleReport(FILE *out, course_t ocrss[], List L, int id){
    Position p = findStudent(L, id);
    int i = 0;

    if(p == NULL){
        printf("Student Not Enrolled into the system! re-check the number and try again:\n");
        return false;
    }

    fprintf(out, "Schedule For: %s\n", p->name);
    fprintf(out, "------------------------------------------------------------------------------\n");
    fprintf(out, "Hour:| 8     | 9     | 10    | 11    | 12    | 1pm   | 2pm   | 3pm   | 4pm   |\n");
    fprintf(out, "------------------------------------------------------------------------------\n");
    bool times[9];
    initiateClockArray(times);
    int courseIndex[9];


    for (i = 0; i < p->numberOfCoursesRegistered; i++){
        int index = p->coursesIndexes[i];
        fillClockArray(times, ocrss[index].startTime, ocrss[index].finishTime);
        //int where = getAddressForClock(ocrss[index].startTime);
        //courseIndex[where] = index;
        fillClockWithIndex(courseIndex, ocrss[index].startTime, ocrss[index].finishTime, index);
    }

    fprintf(out, "     |");

    for (i = 0; i < 9; i++){
        if(times[i]){
            fprintf(out, "%s|", ocrss[courseIndex[i]].id);    //a course is assumed to be 7 letters only "COMP" followed by 3 letters
        }else{
            fprintf(out, "       |");                         //so 7 spaces is whats used
        }
    }

    fprintf(out, "\n------------------------------------------------------------------------------\n");
    return true;
}

int openMenu(){

    FILE *coursesFile, *studentsFile, *reportFile;

    char cfile[PATHLENGTH], sfile[PATHLENGTH], rfile[PATHLENGTH];

    printf("Wellcome to the student registration gate!\nPlease enter the complete path for the file that has the courses:");
    scanf("%s", cfile);
    printf("Great! now enter the path for the students file:");
    scanf(" %s", sfile);
    printf("And where do you want your output file to be?");
    scanf("%s", rfile);

    coursesFile = fopen(cfile,"r");
    studentsFile = fopen(sfile, "r");
    reportFile = fopen(rfile,"w");

//    coursesFile = fopen("/Users/AhmadSawi/Desktop/Data Structures/Project1/Courses.txt","r");
//    studentsFile = fopen("/Users/AhmadSawi/Desktop/Data Structures/Project1/Students.txt", "r");
//    reportFile = fopen("/Users/AhmadSawi/Desktop/Data Structures/Project1/Reports.txt","w");


    int numberOfCourses = fileLineCount(coursesFile);    //to house the array size to be used a lot afterwards
    int numberOfStudents = fileLineCount(studentsFile);

    course_t coursesArray[numberOfCourses];

    fillCourseArrayFromFile(coursesArray, coursesFile, numberOfCourses);

    removeNonCOMPCourses(coursesArray, &numberOfCourses);

    List miniStudentList = (List) malloc(sizeof(student_t));    //the List that has the students and their courses for easy access and shorter run times
    miniStudentList->next = NULL;

    fillStudentsLinkedListFromFile(coursesArray, studentsFile, numberOfCourses, numberOfStudents, miniStudentList);

    courseStudentsReport(coursesArray, reportFile, numberOfCourses);

    allStudentsReport(miniStudentList, reportFile);
    allCoursesReport(coursesArray, reportFile, numberOfCourses);

    int stid;
    printf("Enter the ID of the student you want to have his scedule at the end of the Report file:\n");

    scanf("%d", &stid);
    bool check = studentScheduleReport(reportFile, coursesArray, miniStudentList, stid);

    while(!check){    //to make sure number entered goes back to a student
        scanf("%d", &stid);
        check = studentScheduleReport(reportFile, coursesArray, miniStudentList, stid);
    }

    //printf("You're all done, go check the %s file for output", rfile);
    printf("You're all done, go check the Reports.txt file for output");

    fclose(reportFile);            //closing this file only because all input files were closed in the function that gets data from them
    return 0;
}