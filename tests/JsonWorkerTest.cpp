#include <cassert>

#include <workers/json/JsonWorker.h>

int main()
{
	JsonWorker worker;

	assert(worker.name() == "json");
	assert(worker.description() == "Parse JSON and extract data by path");
};
