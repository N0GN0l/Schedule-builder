#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct{
    char string[100];
} String;

typedef struct{
    String time;
    String location;
    String dates;
    String type;
} Section;


typedef struct{
    String overarching_name;
    Section lecture;
    Section special;
    bool has_non_lecture_section;
} classes;

void list_filler(bool type, FILE *f_in, classes* list, int top,char buffer[], int searching_type)
{
    int i = 0;
    char* id_for_time = "win0divMTG_SCHED$";
    char* id_for_location = "win0divMTG_LOC$";
    char* id_for_dates = "win0divMTG_DATES$";
    char temp_string[100];
    char ch = ' ';
    // int offset;
    char* id;
    
    switch (searching_type) {
        case 1:
        id = id_for_time;
        // offset = 79;
        break;
        case 2:
        id = id_for_dates;
        // offset = 79;
        break;
        case 3:
            id = id_for_location;
            // offset = 75;
            break;
        }

    if(strstr(buffer, id) != NULL)
    {
        while(ch != '"')
        {
            fseek(f_in, -2, SEEK_CUR);
            ch = fgetc(f_in);
        }
        ch = fgetc(f_in);
        while(ch != '<')
        {
            ch = fgetc(f_in);
            if(ch == '<') break;
            temp_string[i] = ch;
            i++;
        }
        temp_string[i] = '\0';
        i=0;
        ch = ' ';
        char* token;
        if(type)
        {
            switch (searching_type) {
                case 1:
                    strcpy(list[top].lecture.time.string, temp_string);
                    break;
                case 2:
                    strcpy(list[top].lecture.dates.string, temp_string);
                    break;
                case 3:
                    char *target = strstr(temp_string, "WESTCAMPUS");
                    if (target != NULL) {
                        memmove(target, target + 10, strlen(target + 10) + 1);
                    }    
                    strcpy(list[top].lecture.location.string, temp_string);
                    break;
            }
        }
        else {
            switch (searching_type) {
                case 1:
                    strcpy(list[top].special.time.string, temp_string);
                    break;
                case 2:
                    strcpy(list[top].special.dates.string, temp_string);
                    break;
                case 3:
                    char *target = strstr(temp_string, "WESTCAMPUS");
                    if (target != NULL) {
                        memmove(target, target + 10, strlen(target + 10) + 1);
                    }
                    strcpy(list[top].special.location.string, temp_string);
                    break;
            }
        }
    }
}
void class_finder(FILE *f_in, classes* list)
{
    int top = 0;
    
    char buffer[1024];
    char ch;
    char temp_string[50];
    
    

    char* id_for_class = "<tbody><tr><td class=\"PAGROUPDIVIDER\" align=\"left\">";//this is the sequence that dictates when a class is being displayed
    char* id_for_type = "win0divMTG_COMP$";
    
    
    bool type = false;

    while(fgets(buffer, sizeof(buffer), f_in) != NULL)
    {
        int i = 0;
        if(strstr(buffer, id_for_class) != NULL)
        {
            while(ch != '"')
            {
                fseek(f_in, -2, SEEK_CUR);
                ch = fgetc(f_in);
            }
            ch = fgetc(f_in);
            while(ch != '<')
            {
                ch = fgetc(f_in);
                if(ch == '<') break;
                temp_string[i] = ch;
                i++;
            }
            temp_string[i] = '\0';
            i = 0;
            ch = ' ';
            strcpy(list[top].overarching_name.string, temp_string);
            
            memset(temp_string, 0, sizeof(temp_string));//reset temp string



            while((fgets(buffer, sizeof(buffer), f_in) != NULL )&& (strstr(buffer, "<td height=\"10\" colspan=\"2\"></td>") == NULL))
            {
                // printf("==searching for information==");
                if(strstr(buffer, id_for_type) != NULL)
                {
                    // printf("%s", buffer);
                    while(ch != '"')
                    {
                        fseek(f_in, -2, SEEK_CUR);
                        ch = fgetc(f_in);
                    }
                    ch = fgetc(f_in);
                    while(ch != '<')
                    {
                        ch = fgetc(f_in);
                        if(ch == '<') break;
                        temp_string[i] = ch;
                        i++;
                    }
                    temp_string[i] = '\0';
                    i=0;
                    ch = ' ';
                    // printf("``````````%s```````````\n", temp_string);
                    if(strcmp(temp_string,"Lecture") == 0)
                    {
                        type = true;//the type of the class is a lecture
                        // printf("const char *restrict  _Nonnull  _Nonnull format, ..."); testing to see if the code is detecting "Lecture"
                    }
                    else
                    {
                        list[top].has_non_lecture_section = true;
                        strcpy(list[top].special.type.string, temp_string);
                        type = false;
                    }
                    
                    // printf("%d", list[top].has_non_lecture_section);
                }i=0;
                memset(temp_string, 0, sizeof(temp_string));
                if(strstr(buffer, "win0divDERIVED_REGFRM1_DESCR20$") != NULL) break;
                // printf("%d\n", type);
                list_filler(type, f_in, list, top, buffer,1);
                list_filler(type, f_in, list, top, buffer,3);
                list_filler(type, f_in, list, top, buffer,2);
                
            }
            top++;
        }
    }
}

void CSV_creation(FILE* f_out, classes* list, bool lecture, int i)
{
    String dates[2];
    String times[2];
    char* token;
    char* days_of_week;

    if(!lecture)
    {
        token = strtok(list[i].overarching_name.string, "-");
        // token = strtok(NULL, "-"); // Subsequent calls pass NULL
        // memmove(token, token+1, strlen(token)+1);
        strcpy(list[i].overarching_name.string, token);
    }

    memset(dates, 0, sizeof(dates));
    memset(times, 0, sizeof(times));
    // ======================= Split the dates =======================
    if(lecture)
    {
        token = strtok(list[i].lecture.dates.string, " -"); 
    }
    else{
        token = strtok(list[i].special.dates.string, " -"); 
    }
    int j = 0;
    while (token != NULL && j < 2) 
    {
        strcpy(dates[j].string, token);
        j++;
        token = strtok(NULL, "-"); // Subsequent calls pass NULL
    }
    
    //=================get rid of empty spaces before the date====================
    if (dates[1].string[0] == ' ') {
        memmove(dates[1].string, dates[1].string + 1, strlen(dates[1].string));
    }
        
    // ====================== Split the time data ========================
    // Get the first token
    if(lecture)
    {
        token = strtok(list[i].lecture.time.string, " ");
    }
    else {
        token = strtok(list[i].special.time.string, " ");
    }
    days_of_week = token;

    int k = 0;
    while ((token = strtok(NULL, " -")) != NULL && k < 2)
    {
        int temp = 0;
        // printf("%s\n", token);
        strcpy(times[k].string, token);
        char* ptr = times[k].string;
        while(*ptr != '\0' && *ptr != 'M')
        {
            temp++;
            ptr++;
        }
        memmove(&times[k].string[temp], &times[k].string[temp-1], strlen(times[k].string)-temp+1);
        times[k].string[temp-1]=' ';
        k++;
    }
    
    //=============calculate the number of days inbetween the starting and end dates
    //This is used to repeatedly make events for the same class

    int integer_version_of_mm = (dates[0].string[0] - '0') * 10 + (dates[0].string[1] - '0') - 1;
    int integer_version_of_dd = (dates[0].string[3] - '0') * 10 + (dates[0].string[4] - '0');
    
    // printf("dd: %d, mm: %d\n", integer_version_of_dd, integer_version_of_mm);
    struct tm date1 = {0};
    date1.tm_mon = integer_version_of_mm;
    date1.tm_mday = integer_version_of_dd;
    date1.tm_year = 2026-1900;
    
    
    // printf("date2:%s\n",dates[1].string );
    
    
    
    integer_version_of_mm = (dates[1].string[0] - '0') * 10 + (dates[1].string[1] - '0') - 1;
    integer_version_of_dd = (dates[1].string[3] - '0') * 10 + (dates[1].string[4] - '0');
    struct tm date2 = {0};
    date2.tm_mon = integer_version_of_mm;
    date2.tm_mday = integer_version_of_dd;
    date2.tm_year = 2026-1900;
    
    time_t t1 = mktime(&date1);
    time_t t2 = mktime(&date2);
    
    if (t1 == (time_t)-1 || t2 == (time_t)-1) {
        printf("Error converting date.\n");
        exit(1);
    }
    
    double seconds = difftime(t2, t1);
    
    double days = seconds / 86400.0;
    
    
    integer_version_of_mm = (dates[0].string[0] - '0') * 10 + (dates[0].string[1] - '0') - 1;
    integer_version_of_dd = (dates[0].string[3] - '0') * 10 + (dates[0].string[4] - '0');
    
    char temp_day_of_week[3];
    int current_date = 0;
    char date_to_print_out[20];
    switch(strlen(days_of_week)){
        case 6:
            j = 3;
            break;
        case 4:
            j = 2;
            break;
        case 2:
            j = 1;
            break;
    }
    struct tm temp_date;


    temp_date = date1;
    while(j > 0)
    {
        temp_day_of_week[0] = days_of_week[0];
        temp_day_of_week[1] = days_of_week[1];
        temp_day_of_week[2] = '\0';
        if(strcmp("Tu", temp_day_of_week) == 0) date1.tm_mday++;
        else if(strcmp("We", temp_day_of_week) == 0) date1.tm_mday += 2;
        else if(strcmp("Th", temp_day_of_week) == 0) date1.tm_mday += 3;
        else if(strcmp("Fr", temp_day_of_week) == 0) date1.tm_mday += 4;    
        memmove(days_of_week, days_of_week+2, strlen(days_of_week));
        current_date = 0;
        while(current_date <= days)
        {
            strftime(date_to_print_out, sizeof(date_to_print_out), "%m/%d/%Y", &date1);
            if(lecture)
            {
                fprintf(f_out, "%s,", list[i].overarching_name.string);
            }
            else {
                fprintf(f_out, "%s", list[i].overarching_name.string);
                fprintf(f_out, "%s, ", list[i].special.type.string);
            }
            fprintf(f_out,"%s,", date_to_print_out);//start date
            fprintf(f_out, "%s,", times[0].string);//start time
            fprintf(f_out,"%s,", date_to_print_out);//end date
            fprintf(f_out, "%s,", times[1].string);//end time
            // fprintf(f_out, "FALSE,testing,");
            if(lecture)
            {
                fprintf(f_out, "%s", list[i].lecture.location.string);
            }
            else{
                fprintf(f_out, "%s", list[i].special.location.string);
            }
            date1.tm_mday += 7;
            mktime(&date1);
            
            current_date+=7;
            fputc('\n', f_out);
        }

        j--;
        date1 = temp_date;
    }
}
int main(void)
{
    int class_amount;
    printf("How many classes do you have, do NOT include labs and recitations (enter as an integer number, e.g: 1-2): ");
    scanf("%d", &class_amount);
    // Get the current calendar time
    time_t raw_time = time(NULL);

    // Convert to local time structure
    struct tm *time_info = localtime(&raw_time);

    // tm_year stores years since 1900, so we add 1900
    int current_year = time_info->tm_year + 1900;


    FILE* f_in, *f_out;
    classes* list = malloc(sizeof(classes) * 10);
    if((f_in = fopen("input.txt", "r")) == NULL)
    {
        perror("Couild not open html file");
        exit(1);
    }
    if((f_out = fopen("calender.csv", "w+")) == NULL)
    {
        perror("Could not create csv file");
        exit(1);
    }
    
    class_finder(f_in, list);

    //================================Print statements to show the raw data that is stored=========================================
    for(int i = 0; i < class_amount; i++)
    {
        printf("\n%s\n", list[i].overarching_name.string);
        printf("%s\n", list[i].lecture.dates.string);
        printf("%s\n", list[i].lecture.location.string);
        printf("%s\n", list[i].lecture.time.string);
        if(list[i].has_non_lecture_section)
        {
            printf("===%s===:\n", list[i].special.type.string);
            printf("%s\n", list[i].special.dates.string);
            printf("%s\n", list[i].special.location.string);
            printf("%s\n", list[i].special.time.string);
        }
    }


    //============= start outputting to the CSV file =================
    fprintf(f_out, "Subject,Start Date,Start Time,End Date,End Time,Location\n");
    for(int i = 0; i < class_amount; i++)
    {
        CSV_creation(f_out, list, true,i);
        if(list[i].has_non_lecture_section)
        {
            CSV_creation(f_out, list, false, i);
        }
    }

    fclose(f_in);
    fclose(f_out);
    free(list);
}