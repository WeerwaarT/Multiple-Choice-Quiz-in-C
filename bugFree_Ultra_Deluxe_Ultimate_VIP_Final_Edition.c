//
// Created by WeerwaarT on 2020/8/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define START       1
#define LIST        2
#define ADD         3
#define REMOVE      4
#define EDIT        5
#define SAVE        6
#define LOAD        7
#define QUIT        8
#define CHOICE_MAX  20      // maximum number of choices of a question
#define LENGTH_MAX  200     // maximum size of one string
#define QUIZSIZE    1000    // maximum number of multiple choice questions in one quiz


typedef struct newMCQ
{
    char    *question;
    int     score;
    int     numberOfChoices;
    char    **choices;
    int     *isCorrect;
}MCQ;


MCQ     **mulChoiceQuiz;
int     indexOfTail = 0;
int     totalScore = 0;


_Noreturn void run(void);
void initialize(void);
void reinitialize(void);
void doOption(int);
void showMenu(void);
char *getString(char *);
int getInteger(char *);
int getCorrectness();
int getOption(void);
int quizIsEmpty(void);
int quizIsFull(void);
void startQuiz(void);
void shuffleQuestions(void);
void swapQuestion(int, int);
int testQuestion(int);
int getAnswers(int);
int getNumberOfAnswersChosen(int);
int getChoiceAnswer(int, int, int, int*);
int checkDuplicateOptions(int, int, const int*);
void shuffleChoices(int);
void swapChoice(int, int, int);
void addNewMCQ(void);
void removeMCQ(void);
void editMCQ(void);
void editOneMCQ(int);
int getIndexOfMCQToBeEdited(void);
void doEditOption(char, int);
void editQuestion(int);
void editScore(int);
void editChoice(int);
void deleteChoice(int);
void addChoice(int);
int checkIndexOfQuestionOutOfBound(int);
int checkIndexOfChoiceOutOfBound(int, int);
void saveFile(void);
void loadFile(void);
int readIntegerFromFile(FILE *);
char *readStringFromFile(FILE *);
void listAllQuestions(void);
void printOneQuestion(int);
void printQuestionNumber(int);
void printQuestion(int);
void printScore(int);
void printNumberOfChoices(int);
void printChoices(int);
MCQ *createQuestion(char *, int, int, char **, int *);


void initialize()
{
    mulChoiceQuiz = calloc(QUIZSIZE, sizeof(MCQ));
}

void reinitialize()
{
    int i = -1;
    while(mulChoiceQuiz[++i] != NULL) mulChoiceQuiz[i] = NULL;
    totalScore = 0;
    indexOfTail = 0;
}

_Noreturn void run()
{
    while (1)
    {
        showMenu();
        int option = getOption();
        doOption(option);
    }
}

void doOption(int option)
{
    switch (option)
    {
        case START:     startQuiz(); break;
        case LIST:      listAllQuestions(); break;
        case ADD:       addNewMCQ(); break;
        case REMOVE:    removeMCQ(); break;
        case EDIT:      editMCQ(); break;
        case SAVE:      saveFile(); break;
        case LOAD:      loadFile(); break;
        case QUIT:      exit(0);
        default:        printf("Invalid option, please try again.\n");
    }
}

char *getString(char *output)
{
    char buffer[LENGTH_MAX];
    printf("%s", output);
    fgets(buffer, LENGTH_MAX, stdin);
    size_t inputLength = strlen(buffer);
    char *input = calloc(inputLength, sizeof(char));
    strncpy(input, buffer, inputLength - 1);
    input[inputLength] = '\0';
    return input;
}

int getInteger(char *output)
{
    char    *invalidString;
    int     value = (int) strtol(getString(output), &invalidString, 10);
    if (*invalidString != '\0') printf("Invalid input: %s\n", invalidString);
    return (value < 0)?0:value;
}

int getCorrectness()
{
    int isCorrect = getInteger("Is this choice correct?"
                               "(Enter 0 if it is incorrect and enter 1 if it is correct)\n"
                               "Input: ");
    return (isCorrect > 0)?1:0;
}

int quizIsEmpty()
{
    return indexOfTail == 0;
}

int quizIsFull()
{
    return indexOfTail == QUIZSIZE;
}

int getOption()
{
    int option;
    option = getInteger("Input: ");
    return option;
}

void showMenu()
{
    printf("\nMultiple choice quiz\n");
    printf("%i. Start the multiple choice quiz(***Questions and choices will be displayed in random order***)\n", START);
    printf("%i. List all questions\n", LIST);
    printf("%i. Add a new question(***If you want to add a question to an existing file, load the file first***)\n", ADD);
    printf("%i. Remove a question\n", REMOVE);
    printf("%i. Edit a question\n", EDIT);
    printf("%i. Save question(s) to a file\n", SAVE);
    printf("%i. Load question(s) from a file\n", LOAD);
    printf("%i. Quit\n", QUIT);
    printf("(Default of any invalid input will be either '0'(zero) or the header number/character of your input)\n");
}

void startQuiz()
{
    if (quizIsEmpty())
    {
        printf("\nThe quiz is empty\n");
        return;
    }
    int score = 0;
    shuffleQuestions();
    for (int i = 0; i < indexOfTail; i++)
    {
        score += testQuestion(i);
    }
    if (totalScore == 0)
    {
        printf("You have finished the quiz, your score is: 0 / 0 (???.?%%)\n");
        return;
    }
    printf("\nYou have finished the quiz, your score is: %d / %d (%.1f%%)\n", score, totalScore,
           ((float) score / (float) totalScore) * 100);
}

void shuffleQuestions()
{
    int sizeOfQuiz = indexOfTail;
    for (int i = 0; i < indexOfTail; i++)
    {
        srandom(time(NULL));
        // srand(time((NULL)));
        int index = ((int) random()) % sizeOfQuiz + i;
        // int index = ((int) rand()) % sizeOfQuiz + i;
        sizeOfQuiz--;
        swapQuestion(index, i);
    }
}

void swapQuestion(int indexOfQuestionToBeSwapped, int indexOfDest)
{
    MCQ *temp = mulChoiceQuiz[indexOfDest];
    mulChoiceQuiz[indexOfDest] = mulChoiceQuiz[indexOfQuestionToBeSwapped];
    mulChoiceQuiz[indexOfQuestionToBeSwapped] = temp;
}

int testQuestion(int index)
{
    shuffleChoices(index);
    int numberOfCorrectChoices = 0;
    printf("\nQ: %s\n", mulChoiceQuiz[index]->question);
    for (int i = 0; i < mulChoiceQuiz[index]->numberOfChoices; i++)
    {
        printf("%d. %s\n", (i + 1), mulChoiceQuiz[index]->choices[i]);
        if (mulChoiceQuiz[index]->isCorrect[i] == 1)
        {
            numberOfCorrectChoices++;
        }
    }
    int numberOfCorrectChoicesChosen = getAnswers(index);
    return (numberOfCorrectChoicesChosen == numberOfCorrectChoices)
                ?mulChoiceQuiz[index]->score:0;
}

int getAnswers(int index)
{
    int numberOfAnswers = getNumberOfAnswersChosen(index);
    int numberOfCorrectChoicesChosen = 0;
    int *choicesChosen = calloc(numberOfAnswers, sizeof(int));
    for (int i = 0; i < numberOfAnswers; i++)
    {
        int choiceNumber = getChoiceAnswer(index, (i + 1), numberOfAnswers, choicesChosen);
        choicesChosen[i] = choiceNumber + 1;
        if (mulChoiceQuiz[index]->isCorrect[choiceNumber] == 1)
        {
            numberOfCorrectChoicesChosen++;
        }
    }
    return (numberOfAnswers == numberOfCorrectChoicesChosen)?numberOfCorrectChoicesChosen:-1;
}

int getNumberOfAnswersChosen(int index)
{
    int number;
    while (1)
    {
        number = getInteger("\nHow many choices do you want to choose:(You cannot choose any option twice)"
                   "\nNumber = ");
        if (number <= mulChoiceQuiz[index]->numberOfChoices)
        {
            return number;
        }
        printf("\nInvalid input, please try again\n");
    }
}

int getChoiceAnswer(int index, int number, int size, int *choicesChosen)
{
    int choiceNumber;
    while (1)
    {
        printf("\nChoice %d: ", number);
        choiceNumber = getInteger("");
        if ((choiceNumber > 0)
        && (choiceNumber <= mulChoiceQuiz[index]->numberOfChoices)
        && checkDuplicateOptions(choiceNumber, size, choicesChosen))
        {
            return choiceNumber - 1;
        }
        printf("\nInvalid input, please try again\n");
    }
}

int checkDuplicateOptions(int indexOfChoice, int size, const int *choicesChosen)
{
    for (int i = 0; i < size; i++)
    {
        if (choicesChosen[i] == indexOfChoice) return 0;
    }
    return 1;
}

void shuffleChoices(int index)
{
    int numberOfChoices = mulChoiceQuiz[index]->numberOfChoices;
    for (int i = 0; i < numberOfChoices; i++)
    {
        srandom(time(NULL));
        // srand(time(NULL));
        int indexOfChoice = ((int) random()) % numberOfChoices + i;
        // int indexOfChoice = ((int) rand()) % numberOfChoices + i;
        numberOfChoices--;
        swapChoice(index, indexOfChoice, i);
    }
}

void swapChoice(int indexOfQuestion, int indexOfChoiceToBeSwapped, int indexOfDest)
{
    char *temp = mulChoiceQuiz[indexOfQuestion]->choices[indexOfDest];
    int isCorrect = mulChoiceQuiz[indexOfQuestion]->isCorrect[indexOfDest];

    mulChoiceQuiz[indexOfQuestion]->choices[indexOfDest] =
            mulChoiceQuiz[indexOfQuestion]->choices[indexOfChoiceToBeSwapped];
    mulChoiceQuiz[indexOfQuestion]->isCorrect[indexOfDest] =
            mulChoiceQuiz[indexOfQuestion]->isCorrect[indexOfChoiceToBeSwapped];

    mulChoiceQuiz[indexOfQuestion]->choices[indexOfChoiceToBeSwapped] =
            temp;
    mulChoiceQuiz[indexOfQuestion]->isCorrect[indexOfChoiceToBeSwapped] =
            isCorrect;
}

void listAllQuestions()
{
    if (quizIsEmpty())
    {
        printf("\nThe quiz is empty\n");
        return;
    }
    int index = 0;
    printf("\nTotal number of questions: %d\n", indexOfTail);
    printf("Total score of questions:  %d\n", totalScore);
    while (index < indexOfTail)
    {
        printOneQuestion(index);
        index++;
    }
}

void printOneQuestion(int index)
{
    printQuestionNumber(index);
    printQuestion(index);
    printScore(index);
    printNumberOfChoices(index);
    printChoices(index);
}

void printQuestionNumber(int index)
{
    printf("\nQ%d\n", (index + 1));
}

void printQuestion(int index)
{
    printf("Question: %s\n", mulChoiceQuiz[index]->question);
}

void printScore(int index)
{
    printf("Score: %d\n", mulChoiceQuiz[index]->score);
}

void printNumberOfChoices(int index)
{
    printf("Number of Choices: %d\n", mulChoiceQuiz[index]->numberOfChoices);
}

void printChoices(int index)
{
    for (int i = 0; i < mulChoiceQuiz[index]->numberOfChoices; i++)
    {
        printf("Choice %d: %s\n", (i + 1), mulChoiceQuiz[index]->choices[i]);
        if (mulChoiceQuiz[index]->isCorrect[i])
        {
            printf("It is correct\n");
        }
        else
        {
            printf("It is incorrect\n");
        }
    }
}

void addNewMCQ()
{
    if (quizIsFull())
    {
        printf("\nThe quiz has reached its maximum size\n");
        return;
    }
    char    *question = getString("Question: ");
    int     score = getInteger("Score: ");
    int     numberOfChoices = getInteger("Number of Choices(Up to 20): ");
    numberOfChoices = (numberOfChoices>CHOICE_MAX)?CHOICE_MAX:numberOfChoices;
    char    **choices = calloc(numberOfChoices, sizeof(char *));
    int     *isCorrect = calloc(numberOfChoices, sizeof(int));
    for (int i = 0; i < numberOfChoices; i++)
    {
        printf("Choice %d: ", (i + 1));
        choices[i] = getString("");
        isCorrect[i] = getCorrectness();
    }
    mulChoiceQuiz[indexOfTail++] = createQuestion(question, score, numberOfChoices,
                                                  choices, isCorrect);
    totalScore += score;
}

void removeMCQ()
{
    if (quizIsEmpty())
    {
        printf("\nThe quiz is empty\n");
        return;
    }
    listAllQuestions();
    int indexOfMCQToBeRemoved = getInteger("Which question do you want to remove?\n"
                                           "Enter the question number: ");
    if (checkIndexOfQuestionOutOfBound(--indexOfMCQToBeRemoved))
    {
        return;
    }
    totalScore -= mulChoiceQuiz[indexOfMCQToBeRemoved]->score;
    mulChoiceQuiz[indexOfMCQToBeRemoved] = mulChoiceQuiz[--indexOfTail];
    mulChoiceQuiz[indexOfTail] = NULL;
}

void editMCQ()
{
    if (quizIsEmpty())
    {
        printf("\nThe quiz is empty\n");
        return;
    }
    while (1)
    {
        listAllQuestions();
        int indexOfMCQToBeEdited = getIndexOfMCQToBeEdited();
        if (indexOfMCQToBeEdited == -1)
        {
            break;
        }
        editOneMCQ(indexOfMCQToBeEdited);
    }
}

int getIndexOfMCQToBeEdited()
{
    int indexOfMCQToBeEdited = getInteger("Which question do you want to edit?\n"
                                          "(Enter '0' if you want to stop editing)\n"
                                          "Enter the question number: ");
    if (checkIndexOfQuestionOutOfBound(--indexOfMCQToBeEdited))
    {
        return -1;
    }
    return indexOfMCQToBeEdited;
}

void editOneMCQ(int indexOfMCQToBeEdited)
{
    while (1)
    {
        printOneQuestion(indexOfMCQToBeEdited);
        char option = getString("What do you want to edit?\n"
                                "(Enter 'q' to edit question, 's' to edit score, 'c' to edit choice, "
                                "'d' to delete choice, 'a' to add choice and 't' to stop editing"
                                " THIS question)\n"
                                "Input: ")[0];
        if (option == 't' || option == '\0')
        {
            return;
        }
        else
        {
            doEditOption(option, indexOfMCQToBeEdited);
        }
    }
}

void doEditOption(char option, int indexOfMCQToBeEdited)
{
    switch (option)
    {
        case 'q': editQuestion(indexOfMCQToBeEdited); break; // 'q': question
        case 's': editScore(indexOfMCQToBeEdited); break; // 's': score
        case 'c': editChoice(indexOfMCQToBeEdited); break; // 'c': edit choice
        case 'd': deleteChoice(indexOfMCQToBeEdited); break; // 'd': delete choice
        case 'a': addChoice(indexOfMCQToBeEdited); break; // 'a': add choice
        default: printf("\nInvalid option\n"); break;
    }
}

void editQuestion(int indexOfMCQToBeEdited)
{
    printf("\nQuestion(current): %s\n", mulChoiceQuiz[indexOfMCQToBeEdited]->question);
    char *question = getString("Question(new): ");
    mulChoiceQuiz[indexOfMCQToBeEdited]->question = question;
}

void editScore(int indexOfMCQToBeEdited)
{
    printf("\nScore(current): %d\n", mulChoiceQuiz[indexOfMCQToBeEdited]->score);
    int score = getInteger("Score(new): ");
    totalScore -= mulChoiceQuiz[indexOfMCQToBeEdited]->score - score;
    mulChoiceQuiz[indexOfMCQToBeEdited]->score = score;
}

void editChoice(int indexOfMCQToBeEdited)
{
    printf("\nCurrent choices:\n");
    printChoices(indexOfMCQToBeEdited);
    int indexOfChoiceToBeEdited = getInteger("Which choice do you want to edit?\n"
                                             "Input: ") - 1;
    if (checkIndexOfChoiceOutOfBound(indexOfChoiceToBeEdited, indexOfMCQToBeEdited))
    {
        return;
    }
    char *choice = getString("Choice(new): ");
    int isCorrect = getCorrectness();
    mulChoiceQuiz[indexOfMCQToBeEdited]->choices[indexOfChoiceToBeEdited] = choice;
    mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect[indexOfChoiceToBeEdited] = isCorrect;
}

void deleteChoice(int indexOfMCQToBeEdited)
{
    printf("\nCurrent choices:\n");
    printChoices(indexOfMCQToBeEdited);
    int indexOfChoiceToBeDeleted = getInteger("Which choice do you want to delete?\nInput: ") - 1;
    if (checkIndexOfChoiceOutOfBound(indexOfChoiceToBeDeleted, indexOfMCQToBeEdited))
    {
        return;
    }
    int numberOfChoices = --mulChoiceQuiz[indexOfMCQToBeEdited]->numberOfChoices;
    printNumberOfChoices(indexOfMCQToBeEdited);

    mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect[indexOfChoiceToBeDeleted] =
            mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect[numberOfChoices - 1];

    mulChoiceQuiz[indexOfMCQToBeEdited]->choices[indexOfChoiceToBeDeleted] =
            mulChoiceQuiz[indexOfMCQToBeEdited]->choices[numberOfChoices - 1];

    mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect =
            realloc(mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect, sizeof(int) * numberOfChoices);

    mulChoiceQuiz[indexOfMCQToBeEdited]->choices =
            realloc(mulChoiceQuiz[indexOfMCQToBeEdited]->choices, sizeof(char *) * numberOfChoices);
}

void addChoice(int indexOfMCQToBeEdited)
{
    printf("\nCurrent choices:\n");
    printChoices(indexOfMCQToBeEdited);
    if (mulChoiceQuiz[indexOfMCQToBeEdited]->numberOfChoices == 20)
    {
        printf("\nmaximum number of options is reached (20 / 20)\n");
        return;
    }
    char *choice = getString("Choice(new): ");
    int isCorrect = getCorrectness();
    mulChoiceQuiz[indexOfMCQToBeEdited]->numberOfChoices += 1;
    int numberOfChoices = mulChoiceQuiz[indexOfMCQToBeEdited]->numberOfChoices;
    printNumberOfChoices(indexOfMCQToBeEdited);
    mulChoiceQuiz[indexOfMCQToBeEdited]->choices = (char **) realloc(mulChoiceQuiz[indexOfMCQToBeEdited]
            ->choices, numberOfChoices * sizeof(char *));
    mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect = (int *) realloc(mulChoiceQuiz[indexOfMCQToBeEdited]
            ->isCorrect, numberOfChoices * sizeof(int));
    mulChoiceQuiz[indexOfMCQToBeEdited]->choices[numberOfChoices - 1] = choice;
    mulChoiceQuiz[indexOfMCQToBeEdited]->isCorrect[numberOfChoices - 1] = isCorrect;
}

int checkIndexOfQuestionOutOfBound(int index)
{
    if ((index < 0) || (index > (indexOfTail - 1)))
    {
        printf("\nIndex out of bound\n");
        return 1;
    }
    return 0;
}

int checkIndexOfChoiceOutOfBound(int indexOfChoiceToBeEdited, int indexOfMCQToBeEdited)
{
    if (indexOfChoiceToBeEdited < 0 ||
        indexOfChoiceToBeEdited >= mulChoiceQuiz[indexOfMCQToBeEdited]->numberOfChoices)
    {
        printf("\nIndex out of bound\n");
        return 1;
    }
    return 0;
}

void saveFile()
{
    char *fileName = getString("\nSave multiple choice questions to a file\n"
                               "Enter the file name: ");
    FILE *file = fopen(fileName, "w+");
    int index = 0;
    while (index < indexOfTail)
    {
        int num = mulChoiceQuiz[index]->numberOfChoices;
        fprintf(file, "%s\n", mulChoiceQuiz[index]->question);
        fprintf(file, "%i\n", mulChoiceQuiz[index]->score);
        fprintf(file, "%i\n", num);
        for(int i = 0; i < num; i++)
        {
            fprintf(file, "%s\n", mulChoiceQuiz[index]->choices[i]);
            fprintf(file, "%i\n", mulChoiceQuiz[index]->isCorrect[i]);
        }
        index++;
    }
    fclose(file);
}

void loadFile()
{
    char    *fileName = getString("\nLoad multiple choice questions from a file\n"
                               "Enter the file name: ");
    FILE    *file = fopen(fileName, "r");

    if(file == NULL)
    {
        printf("\nFile does not exist - no question loaded.\n");
        return;
    }

    reinitialize();

    int index = 0;
    while(index < QUIZSIZE)
    {
        char    *question;
        int     score;
        int     numberOfChoices;
        int     *isCorrect;
        char    **choices;

        // get question
        question = readStringFromFile(file);
        if(question == NULL)
        {
            break;
        }

        score = readIntegerFromFile(file);
        numberOfChoices = readIntegerFromFile(file);
        int numberOfChoicesExceedMAX = (numberOfChoices > CHOICE_MAX) ? (numberOfChoices - CHOICE_MAX) : 0;
        numberOfChoices = (numberOfChoices>CHOICE_MAX)?CHOICE_MAX:numberOfChoices;

        isCorrect = calloc(sizeof(int), numberOfChoices);
        choices = calloc(sizeof(char *), numberOfChoices);

        // read choices
        for(int i = 0; i < numberOfChoices; i++)
        {
            choices[i] = readStringFromFile(file);
            isCorrect[i] = (readIntegerFromFile(file) > 0)?1:0;
        }
        for (int i = 0; i < numberOfChoicesExceedMAX; i++)
        {
            readStringFromFile(file);
            readIntegerFromFile(file);
        }
        mulChoiceQuiz[index] = createQuestion(question, score, numberOfChoices,
                                              choices, isCorrect);
        totalScore += score;
        index++;
    }
    indexOfTail = index;
    if (!quizIsFull())
    {
        mulChoiceQuiz[indexOfTail] = NULL;
    }
    fclose(file);
}

int readIntegerFromFile(FILE *file)
{
    char buffer[LENGTH_MAX];
    return (int) strtol(fgets(buffer, LENGTH_MAX - 1, file), NULL, 10);
}

char *readStringFromFile(FILE *file)
{
    char buffer[LENGTH_MAX];
    if (fgets(buffer, LENGTH_MAX - 1, file) == NULL)
    {
        return NULL;
    }
    size_t length = strlen(buffer);
    char *question = calloc(length, sizeof(char));
    strncpy(question, buffer, length - 1);
    question[length] = '\0';
    return question;
}

MCQ *createQuestion(char *question, int score, int numberOfChoices, char **choices
                        , int *isCorrect)
{
    MCQ *aMultipleChoiceQuestion = calloc(1, sizeof(MCQ));
    aMultipleChoiceQuestion->question = question;
    aMultipleChoiceQuestion->score = score;
    aMultipleChoiceQuestion->numberOfChoices = numberOfChoices;
    aMultipleChoiceQuestion->choices = choices;
    aMultipleChoiceQuestion->isCorrect = isCorrect;
    return aMultipleChoiceQuestion;
}

int main(void)
{
    initialize();
    run();
}
