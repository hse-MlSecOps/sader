#include <cassert>
#include <iostream>    
#include <curl/curl.h> 
#include <workers/http/HttpWorker.h>

int main()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	HttpWorker worker;

	//auto validResult = worker.execute({ "https://test.com" });
	auto validResult = worker.execute(HttpArguments{ "https://test.com" });
	assert(validResult.success);


	//auto emptyUrlResult = worker.execute({ "" });
	auto emptyUrlResult = worker.execute(HttpArguments{ "" });
	assert(!emptyUrlResult.success);
	assert(emptyUrlResult.error == "URL is required");

	auto invalidProtocolResult = worker.execute(HttpArguments{ "ftp://test.com" });
	//auto invalidProtocolResult = worker.execute({ "ftp://test.com" });
	assert(!invalidProtocolResult.success);
	assert(invalidProtocolResult.error == "URL must use http or https scheme");

	// Успешный тестовый запрос (метод DELETE)
	auto deleteSuccessResult = worker.execute(HttpArguments{ "https://typicode.com", "DELETE" });
	assert(deleteSuccessResult.success);
	assert(deleteSuccessResult.statusCode == 200);
	assert(!deleteSuccessResult.body.empty()); 

	// Тест на удаление несуществующего ресурса (метод DELETE)
	auto delete404Result = worker.execute(HttpArguments{ "https://typicode.com", "DELETE" });
	assert(!delete404Result.success);
	assert(delete404Result.statusCode == 404);

	curl_global_cleanup();

};