#include "Infer.h"

#include <future>
#include <queue>

#include "Timers.h"
#include "Sample.h"

struct ProcessingBlockPriority
{
	constexpr bool operator()(const std::string& left, const std::string& right) const
	{
		auto getPriority = [this](const std::string& name) -> int
			{
				return name == "FACE_DETECTOR" ?
					2 : name == "HUMAN_BODY_DETECTOR" ?
					2 : name == "FITTER" ?
					1 : 0;
			};

		return getPriority(left) < getPriority(right);
	}
};

Infer::Infer() :
	forceUseCuda(false)
{

}

void Infer::init(const framework::utility::JSONSettingsParser::ExecutorSettings& settings)
{
	sdkPath = settings.initParameters.getString("sdkPath");
	forceUseCuda = settings.initParameters.getBool("forceUseCuda");
}

void Infer::doGet(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Infer::doPost(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	try
	{
		const json::JSONParser& parser = request.getJSON();
		std::vector<json::utility::jsonObject> images = json::utility::JSONArrayWrapper(parser.getArray("images")).getAsObjectArray();
		api::Service service = api::Service::createService(sdkPath);
		json::JSONBuilder responseBuilder(CP_UTF8);
		std::priority_queue<std::string, std::vector<std::string>, ProcessingBlockPriority> unitTypes;
		std::vector<json::utility::jsonObject> result;
		std::unordered_map<std::string, std::unique_ptr<api::ProcessingBlock>> pipeline;
		std::vector<Sample> samples;
		std::vector<std::string> jsonUnitTypes(json::utility::JSONArrayWrapper(parser.getArray("unitTypes")).getAsStringArray());
		api::Context config = service.createContext();

		for (const std::string& unitType : jsonUnitTypes)
		{
			unitTypes.push(unitType);
		}

		config["ONNXRuntime"]["library_path"] = sdkPath;
		config["use_cuda"] = images.size() > 4 && (forceUseCuda || (parser.contains("useCuda", json::utility::variantTypeEnum::jBool) ? parser.getBool("useCuda") : false));

		while (unitTypes.size())
		{
			const std::string& unitType = unitTypes.top();

			config["unit_type"] = unitType;

			pipeline[unitType] = std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config));

			unitTypes.pop();
		}

		samples.reserve(images.size());

		for (const json::utility::jsonObject& image : images)
		{
			samples.emplace_back(image, service);
		}

		for (const Sample& sample : samples)
		{
			std::unordered_map<std::string, std::unique_ptr<BaseParser>> parsers = sample.createParsers(jsonUnitTypes);
			api::Context data = sample.createDataContext();
			json::utility::jsonObject sampleObject;
			std::vector<json::utility::jsonObject> sampleData;
			json::utility::jsonObject infer;
			json::utility::jsonObject time;

			for (auto& [unitType, processingBlock] : pipeline)
			{
				double inferTime = 0.0;
				
				{
					utility::timers::AccumulatingTimer timer(inferTime);

					(*processingBlock)(data);
				}

				time.setDouble(unitType, inferTime);
			}

			for (const api::Context& object : data["objects"])
			{
				json::utility::jsonObject jsonObject;

				for (const auto& [unitType, parser] : parsers)
				{
					jsonObject.setObject
					(
						unitType == "FACE_DETECTOR" || unitType == "HUMAN_BODY_DETECTOR" ? "boundingBox" : unitType,
						parser->parse(object)
					);
				}

				json::utility::appendArray(std::move(jsonObject), sampleData);
			}

			infer.setObject("time", std::move(time));
			infer.setString("timeUnits", "ms");

			sampleObject.setObject("infer", std::move(infer));
			sampleObject.setArray("data", std::move(sampleData));

			json::utility::appendArray(std::move(sampleObject), result);
		}

		responseBuilder["result"] = std::move(result);

		response.addBody(responseBuilder);
	}
	catch (const std::exception& e)
	{
		response.setResponseCode(web::responseCodes::internalServerError);

		response.addBody(e.what());
	}
}

void Infer::doHead(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Infer::doPut(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Infer::doDelete(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Infer::doOptions(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Infer::doTrace(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Infer::destroy()
{

}
