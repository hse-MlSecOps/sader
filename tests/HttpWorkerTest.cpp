#include <cassert>

#include <workers/http/HttpWorker.h>

int main()
{
	HttpWorker worker;

	auto validResult = worker.execute({ "https://test.com" });
	assert(validResult.success);


	auto emptyUrlResult = worker.execute({ "" });
	assert(!emptyUrlResult.success);
	assert(emptyUrlResult.error == "URL is required");

	auto invalidProtocolResult = worker.execute({ "ftp://test.com" });
	assert(!invalidProtocolResult.success);
	assert(invalidProtocolResult.error == "URL must use http or https scheme");
};