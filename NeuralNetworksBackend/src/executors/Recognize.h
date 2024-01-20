#pragma once

#include <Executors/BaseHeavyOperationStatefulExecutor.h>

#include "api/Service.h"

class Recognize : public framework::BaseHeavyOperationStatefulExecutor
{
private:
	std::string sdkPath;
	api::Service service;
	int64_t cudaThreshold;
	bool forceUseCuda;

public:
	Recognize();

	void init(const framework::utility::JSONSettingsParser::ExecutorSettings& settings) override;

	void doGet(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doPost(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doHead(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doPut(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doDelete(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doOptions(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void doTrace(framework::HTTPRequest& request, framework::HTTPResponse& response) override;

	void destroy() override;

	~Recognize() = default;
};
