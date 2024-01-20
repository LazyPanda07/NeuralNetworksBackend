#include "BodyReidentify.h"

#include "Sample.h"
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
	sdkPath(pathToCurrentModule),
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
	throw framework::exceptions::NotImplementedException();
}

void BodyReidentify::doPost(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	response.addHeader("NeuralNetworksBackendVersion", std::format("v{}", version));

	try
	{
		const json::JSONParser& parser = request.getJSON();
		std::vector<json::utility::jsonObject> images = json::utility::JSONArrayWrapper(parser.getArray("images")).getAsObjectArray();
		std::vector<Sample> samples;
		api::Context config = service.createContext();
		std::vector<std::pair<std::string, std::unique_ptr<api::ProcessingBlock>>> pipeline;
		std::vector<api::Context> preparedData;
		json::JSONBuilder responseBuilder(CP_UTF8);
		std::vector<json::utility::jsonObject> resultTime;
		std::vector<json::utility::jsonObject> result;
		bool allowMultipleBodies = parser.contains("allowMultipleBodies", json::utility::variantTypeEnum::jBool) ?
			parser.getBool("allowMultipleBodies") :
			false;
		double verifyThreshold = parser.contains("verifyThreshold", json::utility::variantTypeEnum::jDouble) ?
			parser.getDouble("verifyThreshold") :
			100.0;
		
		samples.reserve(images.size() + 1);

		samples.emplace_back(parser.getObject("referenceImage"), service);

		for (const json::utility::jsonObject& image : images)
		{
			samples.emplace_back(image, service);
		}

		config["ONNXRuntime"]["library_path"] = sdkPath;
		config["use_cuda"] = images.size() >= static_cast<size_t>(cudaThreshold) &&
			(forceUseCuda || (parser.contains("useCuda", json::utility::variantTypeEnum::jBool) ? parser.getBool("useCuda") : false));

		config["unit_type"] = "HUMAN_BODY_DETECTOR";
		pipeline.emplace_back(config["unit_type"].getString(), std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "BODY_RE_IDENTIFICATION";
		pipeline.emplace_back(config["unit_type"].getString(), std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "MATCHER_MODULE";
		config["threshold"] = verifyThreshold;
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

				if (!allowMultipleBodies && data["objects"].size() > 1)
				{
					throw std::runtime_error("All images must contains only 1 body");
				}
				else if (!data["objects"].size())
				{
					throw std::runtime_error("Can't detect body in image");
				}
			}

			BodyReidentify::convertDoubleToFloat(data);

			preparedData.push_back(std::move(data));

			json::utility::appendArray(std::move(time), resultTime);
		}

		api::Context matcherData = service.createContext();
		api::Context verification = matcherData["verification"];
		api::Context verificationObjects = verification["objects"];

		for (size_t i = 1; i < preparedData.size(); i++)
		{
			verificationObjects.clear();

			verificationObjects.push_back(preparedData.front()["output_data"]);
			verificationObjects.push_back(preparedData[i]["output_data"]);

			matcher(matcherData);

			json::utility::jsonObject verificationObject;

			verificationObject.setDouble("distance", verification["result"]["distance"].getDouble());
			verificationObject.setBool("verdict", verification["result"]["verdict"].getBool());

			json::utility::appendArray(std::move(verificationObject), result);
		}

		json::utility::jsonObject time;

		time.setArray("time", std::move(resultTime));
		time.setString("timeUnits", "ms");

		responseBuilder["result"] = std::move(result);
		responseBuilder["time"] = std::move(time);

		response.addBody(responseBuilder);
	}
	catch (const std::exception& e)
	{
		response.setResponseCode(web::responseCodes::internalServerError);

		response.addBody(e.what());
	}
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

DECLARE_EXECUTOR(BodyReidentify)
