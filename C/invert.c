#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Doc
{
	int doc_key;
	char *original_title;
	char *original_abstract;	
};

//Function declarations
int check (char *line, const char *needle, int number);

int main()
{
char id_temp[10];
char file_name[25];
const char id_needle[4] = ".I ";
const char title_needle[4] = ".T\n";
const char abstract_needle[4] = ".W\n";
const char publication_needle[4] = ".B\n";
const char author_needle[4] = ".A\n";
const char number_needle[4] = ".N\n";
const char ex_needle[4] = ".X\n";
FILE *cacm, *dictionary, *postings;

//Creates/erases and writes files dictionary.txt and postings_lists.txt
dictionary = fopen("dictionary.txt", "w");
postings = fopen("postings_lists.txt", "w");

printf("Please enter the file name of the document collection:\n");
scanf("%s", file_name);

cacm = fopen(file_name, "r");

if (cacm != NULL) //If the file pointer cacm points to a file
{
	int t_start = 0;
	int t_stop_w = 0;
	int t_stop_b = 0;
	int w_start = 0;
	int w_stop = 0;
	int i_id = 0;
	int i_doc_ot = 0;
	int i_doc_oa = 0;
	int numberdocs = 0;
    char line[90];
	
	//Reads the file line by line to figure out how many different documents are in the collection
	while (fgets (line, sizeof line, cacm) != NULL)
	{
		if (strstr(line, id_needle) != NULL)
			numberdocs++;
	}

	//Points to the beginning of the file
	rewind(cacm);	

	struct Doc *docs_up = (struct Doc*)malloc(sizeof(struct Doc)*numberdocs);
	struct Doc *docs_temp = (struct Doc*)malloc(sizeof(struct Doc)*numberdocs);
	
	//Reads the file line by line
    while (fgets (line, sizeof line, cacm) != NULL)
    {
		
		//Grabs the document ID
		if (strstr(line, id_needle) != NULL)
		{
			//docs_up[i_id] = (struct Doc*)malloc(sizeof(struct Doc));
			docs_up[i_id].doc_key = i_id + 1;
			i_id++;
		}
		
		//Grabs the title
		//Some files do not have an abstract so the ending expression is either .W or .B. These two lines are put here to omit .W or .B.
		t_stop_w = check(line, abstract_needle, t_stop_w);
		t_stop_b = check(line, publication_needle, t_stop_b);
		if (t_start == 1) //If the line begins with expression .T
		{
			if ((t_stop_w == 0) && (t_stop_b == 0)) //If the line has not found expression .W or .B
			{
				if (docs_up[i_doc_ot].original_title != NULL)
				{
					docs_up[i_doc_ot].original_title = (char*)realloc(docs_up[i_doc_ot].original_title, (sizeof(char)*(1 + strlen(docs_up[i_doc_ot].original_title) + strlen(line))));
					docs_up[i_doc_ot].original_title = strcat(docs_up[i_doc_ot].original_title, line);
				}
				else
				{
					docs_up[i_doc_ot].original_title = (char*)malloc(sizeof(char)*(1 + strlen(line)));
					strcpy(docs_up[i_doc_ot].original_title, line);
				}
			}
			else //Else found expression .W or .B and resets
			{
				t_start = 0;
				t_stop_w = 0;
				t_stop_b = 0;
				i_doc_ot++;
			}
		}
		else //Else line with expression .T hasn't been found
		{
			t_start = 0;
			t_stop_w = 0;
			t_stop_b = 0;
		}
		//Check if the beginning expression has been found. This is placed here to omit .T.
		t_start = check(line, title_needle, t_start);		
				
  		//Grabs the abstract
		//Checks if the ending expression has been found. This is placed here to omit .B, .A, .N, .X, or .I.
		w_stop = check(line, publication_needle, w_stop);
		if (w_start == 1) //If the line begins with expression .W
		{
			if (w_stop == 0) //If the line has not found ending expression
			{
 				if (docs_temp[i_doc_oa].original_abstract != NULL)
				{
					docs_temp[i_doc_oa].original_abstract = (char*)realloc(docs_temp[i_doc_oa].original_abstract, (sizeof(char)*(1 + strlen(docs_temp[i_doc_oa].original_abstract) + strlen(line))));
					docs_temp[i_doc_oa].original_abstract = strcat(docs_temp[i_doc_oa].original_abstract, line);
				}
				else
				{
					docs_temp[i_doc_oa].original_abstract = (char*)malloc(sizeof(char)*(1 + strlen(line)));
					strcpy(docs_temp[i_doc_oa].original_abstract, line);
				}
				
			}
			else //Else found ending expression and resets
			{
				w_start = 0;
				w_stop = 0;
				
				docs_up[i_id - 1].original_abstract = (char*)malloc(sizeof(char)*(1 + strlen(docs_temp[i_doc_oa].original_abstract)));
				strcpy(docs_up[i_id - 1].original_abstract, docs_temp[i_doc_oa].original_abstract);
				docs_temp[i_doc_oa].original_abstract = NULL;
				
				//The following prints correctly
				//fprintf(postings, "docs_up[%i].original_abstract:\n%s\n", i_id - 1, docs_up[i_id - 1].original_abstract);
				
				i_doc_oa++;
			}
		}
		else //Else line with expression .W hasn't been found
		{
			docs_temp[i_doc_oa].original_abstract = NULL;

			w_start = 0;
			w_stop = 0;
		}

		//Check if the beginning expression has been found. This is placed here to omit .W.
		w_start = check(line, abstract_needle, w_start);
		
	}
	
	printf("\n");
	
    fclose(cacm);

/*
	//Use for troubleshooting
   	for (int i = 0; i < numberdocs; i++)
	{
		printf("\ndocs_up[%i].original_title:\n%sdocs_up[%i].original_abstract:\n%s\n\n", docs_up[i].doc_key, docs_up[i].original_title, docs_up[i].doc_key, docs_up[i].original_abstract); 
	}
*/

}
else //Else close the program if the file pointer cacm does not point to a file
{
	printf("File does not exist.\nNow exiting...\n");
	exit(0);
}


return 0;

}

/*
* Texts are in between two expressions.
* This function returns the integer 1 if the line is the beginning expression.
*/
int check (char *line, const char *needle, int number)
{
	if (strstr(line, needle) != NULL )
		number = 1;
	
	return number;
}