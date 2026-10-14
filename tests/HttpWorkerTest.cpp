#include <cassert>
#include <iostream>
#include <workers/http/HttpWorker.h>

int main()
{
	HttpWorker worker;

	HttpArguments args{
		"https://nonexistent.invalid"
	};

	HttpResult result = worker.execute(args);

	assert(result.success == false);
	assert(result.statusCode == 0);
	assert(!result.error.empty());
};