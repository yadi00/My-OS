/**********************************************************************************************************************
Created By:Yadiel R. Hernandez
Created Date:10/30/15
**********************************************************************************************************************/

//#include <queue>
#include <string>
#include <iomanip>
#include "YadiOS.h"



using namespace std;

int main()
{
	Yadi_OS os1;

	//Executes the Sysgen
	os1.sysgen();
	//Goes into running state
	os1.running();

	return 0;
}
