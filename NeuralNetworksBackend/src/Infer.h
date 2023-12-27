#pragma once

#include <Executors/BaseHeavyOperationStatefulExecutor.h>

#include "api/Service.h"

class Infer : public framework::BaseHeavyOperationStatefulExecutor
{
private:
	std::string sdkPath;
	int64_t cudaThreshold;
	bool forceUseCuda;

public:
	Infer();

	void init(const framework::utility::JSONSettingsParser::ExecutorSettings& settings) override;

	void doGet(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doPost(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doHead(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doPut(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doDelete(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doOptions(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doTrace(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void destroy() override;

	~Infer() = default;
};

EXECUTOR_CREATION_FUNCTION(Infer)
