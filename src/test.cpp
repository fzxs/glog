
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#include "rule.h"


using namespace std;
using namespace glog;

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
# if defined __GNUC__ && __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif


#define TOTAL_SIZE 10000*50


int test()
{
	int result = 0;
	result = RuleController::getInstance()->open("../config.xml", "asr");
	if (result)
	{
		printf("rule.open() failed .\n");
		return -1;
	}

	time_t t_start = 0;
	time_t t_end = 0;

	time(&t_start);

	for (int i = 0; i < TOTAL_SIZE; i++)
	{
		RuleController::getInstance()->outputLog(__FILE__, __LINE__, __func__, 20, "%s-------------------%d ", "hello world.", i);
	}
	
	time(&t_end);

	printf("need time[%ld]\n", t_end - t_start);


	return 0;
}

int main()
{
	test();
	printf("-ok-\n");
	getchar();

	return 0;
}



