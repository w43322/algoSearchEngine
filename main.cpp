#include "./include/searchengine/searchengine.h"
#include <ctime>

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
    /* UNIX-style OS. ------------------------------------------- */
    const char* const cls = "clear";
#else
    const char* const cls = "cls";
#endif

const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

cppjieba::Jieba jieba(DICT_PATH,
                HMM_PATH,
                USER_DICT_PATH,
                IDF_PATH,
                STOP_WORD_PATH);
SearchEngine se_cn("./cn.csv", jieba), se_en("./news.csv");

void function2();
void function1();
void PauseAndTip();
void welcome();
void show_menu();
void show_menu2();
void show_menu1();
void ClearBuffer();
int get_a_int();
void function2_1();
void function2_2();
void function1_1();
void function1_2();

std::vector<std::string> GetWords(bool fuzzy = false)
{
    std::string text, space_delimiter = " ";
    std::vector<std::string> words{};

    if (fuzzy)
        std::cout << " - Please input a phrase in Chinese, characters ONLY\n";
    else
        std::cout << " - Please input words, seperated by spaces\n";
    
    getchar();
    getline(std::cin, text);

    if (fuzzy)
        jieba.CutForSearch(text, words, true);
    else
    {
        text += " ";
        size_t pos = 0;
        while ((pos = text.find(space_delimiter)) != std::string::npos)
        {
            words.push_back(text.substr(0, pos));
            text.erase(0, pos + space_delimiter.length());
        }
    }
    return words;
}

void function2_1()// 包含以下全部关键词
{
	puts(" - Contains all the following keywords in English");
    se_en.GetResultAllWords(GetWords());
}
void function2_2()// 包含以下任意关键词
{
	puts(" - Contains any of the following keywords in English");
    se_en.GetResultAnyWord(GetWords());
}

void function1_1()// 包含以下全部关键词
{
	puts(" - Contains all the following keywords in Chinese");
    se_cn.GetResultAllWords(GetWords());
}
void function1_2()// 包含以下任意关键词
{
	puts(" - Contains any of the following keywords in Chinese");
    se_cn.GetResultAnyWord(GetWords());
}
void function1_3()// 模糊搜索
{
	puts(" - Fuzzy search in Chinese");
    se_cn.GetResultAllWords(GetWords(true));
}

void function2() //英文搜索
{

	int input2 = 0;

	show_menu2();

	while (1)
	{
		std::cout << "Please select a choice:   ";
		input2 = get_a_int();

		if (input2 < 0 || input2 > 2)
		{
			puts("Please input a number from 0 ~ 2");
			continue;
		}

		if (0 == input2)
		{
			break;
		}

		switch (input2)
		{
		case 1:
			system(cls);
			function2_1();
			PauseAndTip();
			system(cls);
			show_menu2();
			break;
		case 2:
			system(cls);
			function2_2();
			PauseAndTip();
			system(cls);
			show_menu2();
			break;
		default:
			break;
		}
	}
}
void function1() //中文搜索
{
	int input1 = 0;

	show_menu1();

	while (1)
	{
		std::cout << "Please select a choice:   ";
		input1 = get_a_int();

		if (input1 < 0 || input1 > 3)
		{
			puts("Please input a number from 0 ~ 3");
			continue;
		}

		if (0 == input1)
		{
			break;
		}

		switch (input1)
		{
		case 1:
			system(cls);
			function1_1();
			PauseAndTip();
			system(cls);
			show_menu1();
			break;
		case 2:
			system(cls);
			function1_2();
			PauseAndTip();
			system(cls);
			show_menu1();
			break;
        case 3:
			system(cls);
			function1_3();
			PauseAndTip();
			system(cls);
			show_menu1();
			break;
		default:
			break;
		}
	}
}
void PauseAndTip()
{
	#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
        /* UNIX-style OS. ------------------------------------------- */
        system("read -p ""Press_enter_to_continue""");
    #else
        puts("Please input \"ENTER\" key to continue!");
        system("pause");
    #endif
}

void welcome()
{
	std::cout << "==========================================\n";
	std::cout << "    Welcome to Algorithm course design    \n";
	std::cout << "==========================================\n";
}

void show_menu()
{
	puts("======================================");
	puts(" Here is the choice     ");
	puts(" 1. Search for Chinese");
	puts(" 2. Search for English");
	puts(" 0. quit");
}

void show_menu1()
{
	puts("======================================");
	puts(" Here is the choice     ");
	puts(" 1. Contains all the following keywords in Chinese");
	puts(" 2. Contains any of the following keywords in Chinese");
    puts(" 3. Fuzzy Search in Chinese");
	puts(" 0. return");
}
void show_menu2()
{
	puts("======================================");
	puts(" Here is the choice     ");
	puts(" 1. Contains all the following keywords in English");
	puts(" 2. Contains any of the following keywords in English");
	puts(" 0. return");
}
void ClearBuffer()
{
	char ch;
	while ('\n' != (ch = getchar()))
	{
		std::cout << "The dirty data is:  ";
		putchar(ch);
		std::cout << "\n";
	}
}

int get_a_int()
{
	int input;

	while (1 != scanf("%d", &input))
	{
		puts("That is not a number. Please input a number again...");
		ClearBuffer();
	}

	return input;
}

int main(void)
{
	int input = 0;

	welcome();
	PauseAndTip();
	system(cls);
	show_menu();

	while (1)
	{
		std::cout << "Please select a choice:   ";
		input = get_a_int();

		if (input < 0 || input > 2)
		{
			puts("Please input a number from 0 ~ 2");
			continue;
		}

		if (0 == input)
		{
			break;
		}

		switch (input)
		{
		case 1:
			system(cls);
			function1();
			//PauseAndTip();
			system(cls);
			show_menu();
			break;
		case 2:
			system(cls);
			function2();
			//PauseAndTip();
			system(cls);
			show_menu();
			break;
		default:
			break;
		}
	}

	puts("这是东北大学2022算法课设作业，感谢李老师的悉心指导，组长：王艺霖，组员：魏思源，组员：徐文栋");
	puts("Good Bye!"); 
	//system("pause");

	return 0;
}
