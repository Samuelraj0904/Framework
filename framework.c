#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DATAFILE "framework.dat"
#define FIELDSFILE "Fields.config"
#define MENUFILE "Menu.config"
#define MESSAGEFILE "CustomizedUserMessages.config"
#define MAXFIELDLENGTH 20
#define MAXLINELENGTH 100

FILE *fpRecord;
int countOfFields;
char **fieldNames;
char *recordId;

int getFieldsCount();
char **getFieldsArray();
char *getRecordId();
void showMenu();
void addRecord();
void showAllRecords();
void updateRecord(char *recordId);
void deleteRecord(char *recordId);
void findRecord(char *recordId);
long int getRecordPosition(char *recordId);
void printRecord(long int recordPosition);
bool update(long int recordPosition, char *updateFieldValue);
void printRecordUpdateStatus(bool isRecordUpdated, char *operation);
void printRecordIdNotFoundMessage(char *recordId);
char* printCustomizedUserMessage(int messageNumber);

void main()
{

    countOfFields = getFieldsCount();
    fieldNames = getFieldsArray();
    do
    {
        showMenu();
    } while (1);
}

int getFieldsCount()
{
    char buffer[MAXLINELENGTH];
    int counter = 0;
    FILE *fpFields = fopen(FIELDSFILE, "r");
    while (fgets(buffer, sizeof(buffer), fpFields))
    {
        counter++;
    }
    fclose(fpFields);
    return counter;
}

char **getFieldsArray()
{
    countOfFields = getFieldsCount();
    char *fieldNames = (char *)malloc(sizeof(char *) * countOfFields);
    char buffer[MAXLINELENGTH];
    int counter = 0;
    FILE *fpFields = fopen(FIELDSFILE, "r");
    while (fgets(buffer, sizeof(buffer), fpFields))
    {
        buffer[strlen(buffer) - 1] = '\0';
        fieldNames[counter] = malloc(MAXFIELDLENGTH * sizeof(char));
        strcpy(fieldNames[counter], buffer);

        counter++;
    }
    fclose(fpFields);
    return fieldNames;
}

void showMenu()
{
    int option;
    FILE *fpMenu = fopen(MENUFILE, "r");
    fseek(fpMenu, 0, SEEK_END);
    char menu[ftell(fpMenu)];
    // printf("%ld, %ld\n", sizeof(menu), ftell(fpMenu));
    rewind(fpMenu);
    fread(menu, 1, sizeof(menu), fpMenu);
    printf("\n");
    // fputs(menu, stdout);
    // puts(menu);
    fwrite(menu, 1, sizeof(menu) - 8, stdout);
    fclose(fpMenu);
    printf("\nPlease enter your option: ");
    scanf("%d", &option);
    switch (option)
    {
    case 1:
        addRecord();
        break;
    case 2:
        showAllRecords();
        break;
    case 3:
        updateRecord(getRecordId());
        break;
    case 4:
        deleteRecord(getRecordId());
        break;
    case 5:
        findRecord(getRecordId());
        break;
    case 0:
        exit(0);
    default:
        printf("\n\tPlease enter a valid choice!\n");
    }
}

void addRecord()
{
    int counter;
    char *fieldValue = (char *)malloc(MAXFIELDLENGTH);
    fpRecord = fopen(DATAFILE, "a");
    for (counter = 0; counter < countOfFields; counter++)
    {
        if (counter == 0)
        {
            strcpy(fieldValue, "true");
        }
        else
        {
            printf("Enter %s: ", fieldNames[counter]);
            scanf("%s", fieldValue);
        }
        // printf("fieldValue:%s\n", fieldValue);
        fwrite(fieldValue, MAXFIELDLENGTH, 1, fpRecord);
    }
    fclose(fpRecord);
    printf("\t%s\n", printCustomizedUserMessage(1));
}

void showAllRecords()
{
    char *fieldValue = (char *)malloc(MAXFIELDLENGTH);
    int counter = 0;
    int countOfRecords = 0;
    fpRecord = fopen(DATAFILE, "r");
    printf("\t%s\n", printCustomizedUserMessage(2));
    while (fread(fieldValue, MAXFIELDLENGTH, 1, fpRecord))
    {
        if (counter == countOfFields)
        {
            counter = 0;
            printf("\t-------------------------------\n");
            // printf("\n");
        }
        if (!strcmp(fieldValue, "false"))
        {
            fseek(fpRecord, (countOfFields - 1) * MAXFIELDLENGTH, SEEK_CUR);
            continue;
        }
        if (counter != 0)
        {
            printf("\t%s: %s\n", fieldNames[counter], fieldValue);
        }
        if (counter == 0)
        {
            countOfRecords++;
        }
        counter++;
    }
    printf("\n\t%d records found!\n", countOfRecords);
    fclose(fpRecord);
}

void updateRecord(char *recordId)
{
    long int recordPosition = getRecordPosition(recordId);
    if (recordPosition != -1)
    {
        printRecord(recordPosition);
        int counter, updateOption;
        char *updateFieldValue = (char *)malloc(MAXFIELDLENGTH);
        for (counter = 2; counter < countOfFields; counter++)
        {
            printf("\t%d: %s\n", counter - 1, fieldNames[counter]);
        }
        printf("\n%s", printCustomizedUserMessage(6));
        scanf("%d", &updateOption);
        if (updateOption > counter - 2 || updateOption < 0)
        {
            printf("\tplease enter valid option!\n");
        }
        else
        {
            recordPosition += (updateOption + 1) * MAXFIELDLENGTH;
            printf("Enter %s: ", fieldNames[updateOption + 1]);
            scanf("%s", updateFieldValue);
            bool isRecordUpdated = update(recordPosition, updateFieldValue);
            char updateMessage[MAXLINELENGTH];
            strcpy(updateMessage, fieldNames[updateOption+1]);
            // strcat(updateMessage, updateFieldValue);
            strcat(updateMessage, printCustomizedUserMessage(3));
            printRecordUpdateStatus(isRecordUpdated, updateMessage);
        }
    }
    else
    {
        printRecordIdNotFoundMessage(recordId);
    }
}

void deleteRecord(char *recordId)
{
    long int recordPosition = getRecordPosition(recordId);
    if (recordPosition != -1)
    {
        printRecord(recordPosition);
        char *updateFieldValue = (char *)malloc(MAXFIELDLENGTH);
        strcpy(updateFieldValue, "false");
        bool isRecordUpdated = update(recordPosition, updateFieldValue);
        printRecordUpdateStatus(isRecordUpdated, printCustomizedUserMessage(4));
    }
    else
    {
        printRecordIdNotFoundMessage(recordId);
    }
}

bool update(long int recordPosition, char *updateFieldValue)
{
    bool isRecordUpdated = false;
    fpRecord = fopen(DATAFILE, "r+");
    fseek(fpRecord, recordPosition, SEEK_SET);
    if (fwrite(updateFieldValue, MAXFIELDLENGTH, 1, fpRecord))
    {
        isRecordUpdated = true;
    }
    fclose(fpRecord);
    return isRecordUpdated;
}

void findRecord(char *recordId)
{
    long int recordPosition = getRecordPosition(recordId);
    if (recordPosition != -1)
    {
        printRecord(recordPosition);
    }
    else
    {
        printRecordIdNotFoundMessage(recordId);
    }
}

char *getRecordId()
{
    recordId = (char *)malloc(MAXFIELDLENGTH);
    printf("Enter %s: ", fieldNames[1]);
    scanf("%s", recordId);
    return recordId;
}

void printRecord(long int recordPosition)
{
    char *fieldValue = (char *)malloc(MAXFIELDLENGTH);
    int counter = 1;
    printf("\n");
    fpRecord = fopen(DATAFILE, "r");
    fseek(fpRecord, recordPosition + MAXFIELDLENGTH, SEEK_SET);
    while (fread(fieldValue, MAXFIELDLENGTH, 1, fpRecord))
    {
        if (counter == countOfFields)
        {
            break;
        }
        printf("\t%s: %s\n", fieldNames[counter], fieldValue);
        counter++;
    }
    printf("\n");
}

long int getRecordPosition(char *recordId)
{
    long int recordPosition = -1;
    char fieldValue[MAXFIELDLENGTH];
    fpRecord = fopen(DATAFILE, "r");
    while (fread(fieldValue, sizeof(fieldValue), 1, fpRecord))
    {
        if (!strcmp(fieldValue, "true"))
        {
            fread(fieldValue, sizeof(fieldValue), 1, fpRecord);
            fseek(fpRecord, (long int)-sizeof(fieldValue), SEEK_CUR);
            if (!strcmp(fieldValue, recordId))
            {
                recordPosition = ftell(fpRecord) - (MAXFIELDLENGTH);
                break;
            }
        }
        fseek(fpRecord, (countOfFields - 1) * MAXFIELDLENGTH, SEEK_CUR);
    }
    fclose(fpRecord);
    return recordPosition;
}

void printRecordUpdateStatus(bool isRecordUpdated, char *updateMessage)
{
    if (isRecordUpdated)
    {
        printf("\t%s\n", updateMessage);
    }
    else
    {
        printf("\tError in Writing into file!\n");
    }
}

void printRecordIdNotFoundMessage(char *recordId)
{
    printf("\n\t%s%s%s\n", fieldNames[1], recordId, printCustomizedUserMessage(5));
}

char* printCustomizedUserMessage(int messageNumber)
{
    char buffer = (char)malloc(MAXLINELENGTH);
    int counter = 1;
    FILE *fpRecordMessage = fopen(MESSAGEFILE, "r");
    while(fgets(buffer, MAXLINELENGTH, fpRecordMessage) != NULL)
    {
        if(counter == messageNumber)
        {
            break;
        }
        counter++;
    }
    fclose(fpRecordMessage);
    buffer[strlen(buffer) - 1] = '\0';
    return buffer;
}