#include "BodyReidentify.h"

#include "Sample.h"
#include "Utils.h"
#include "Timers.h"
#include "tdv/data/Context.h"

void BodyReidentify::convertDoubleToFloat(api::Context& data)
{
	api::Context templateContext = data["output_data"]["template"];
	std::vector<float> floatTemplate;

	for (api::Context value : templateContext)
	{
		floatTemplate.push_back(static_cast<float>(value.getDouble()));
	}

	templateContext.clear();

	*reinterpret_cast<tdv::data::Context*>(templateContext.getHandle()) = floatTemplate;

	data["output_data"]["template_size"] = static_cast<int64_t>(data["output_data"]["template_size"].getUnsignedLong());
}

BodyReidentify::BodyReidentify() :
	sdkPath(utility::getPathToCurrentModule()),
	service(api::Service::createService(sdkPath)),
	cudaThreshold(0),
	forceUseCuda(false)
{

}

void BodyReidentify::init(const framework::utility::JSONSettingsParser::ExecutorSettings& settings)
{
	cudaThreshold = settings.initParameters.getInt("cudaThreshold");
	forceUseCuda = settings.initParameters.getBool("forceUseCuda");
}

void BodyReidentify::doGet(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	response.addHeader("NeuralNetworksBackendVersion", std::format("v{}", version));

	try
	{
		const json::JSONParser& parser = request.getJSON();
		std::vector<Sample> samples;
		api::Context config = service.createContext();
		std::vector<std::pair<std::string, std::unique_ptr<api::ProcessingBlock>>> pipeline;
		std::vector<api::Context> preparedData;
		json::JSONBuilder responseBuilder(CP_UTF8);
		std::vector<json::utility::jsonObject> resultTime;

		samples.reserve(2);

		samples.emplace_back(parser.getObject("firstImage"), service);
		samples.emplace_back(parser.getObject("secondImage"), service);

		config["ONNXRuntime"]["library_path"] = sdkPath;

		config["unit_type"] = "HUMAN_BODY_DETECTOR";
		pipeline.emplace_back(config["unit_type"].getString(), std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "BODY_RE_IDENTIFICATION";
		pipeline.emplace_back(config["unit_type"].getString(), std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "MATCHER_MODULE";
		api::ProcessingBlock matcher = service.createProcessingBlock(config);

		for (const Sample& sample : samples)
		{
			api::Context data = sample.createDataContext();
			json::utility::jsonObject time;

			for (const auto& [unit_type, processingBlock] : pipeline)
			{
				double inferTime = 0.0;

				{
					utility::timers::AccumulatingTimer timer(inferTime);

					(*processingBlock)(data);
				}

				time.setDouble(unit_type, inferTime);

				if (!data["objects"].size())
				{
					break;
				}
			}

			preparedData.push_back(std::move(data));

			json::utility::appendArray(std::move(time), resultTime);
		}

		if (std::ranges::any_of(preparedData, [](const api::Context& context) { return context["objects"].size() != 1; }))
		{
			throw std::runtime_error("Images must contains only 1 body");
		}

		api::Context matcherData = service.createContext();
		api::Context verification = matcherData["verification"];
		api::Context verificationObjects = verification["objects"];
		
		BodyReidentify::convertDoubleToFloat(preparedData[0]);
		BodyReidentify::convertDoubleToFloat(preparedData[1]);

		verificationObjects.push_back(preparedData[0]["output_data"]);
		verificationObjects.push_back(preparedData[1]["output_data"]);

		matcher(matcherData);

		json::utility::jsonObject time;

		time.setArray("time", std::move(resultTime));
		time.setString("timeUnits", "ms");

		responseBuilder["distance"] = verification["result"]["distance"].getDouble();
		responseBuilder["verdict"] = verification["result"]["verdict"].getBool();
		responseBuilder["time"] = std::move(time);

		response.addBody(responseBuilder);
	}
	catch (const std::exception& e)
	{
		response.setResponseCode(web::responseCodes::internalServerError);

		response.addBody(e.what());
	}
}

void BodyReidentify::doPost(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::doHead(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::doPut(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::doDelete(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::doOptions(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::doTrace(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::destroy()
{

}
