#include "Recognize.h"

#include "Sample.h"
#include "Utils.h"

Recognize::Recognize() :
	sdkPath(utility::getPathToCurrentModule()),
	service(api::Service::createService(sdkPath)),
	cudaThreshold(0),
	forceUseCuda(false)
{

}

void Recognize::init(const framework::utility::JSONSettingsParser::ExecutorSettings& settings)
{
	cudaThreshold = settings.initParameters.getInt("cudaThreshold");
	forceUseCuda = settings.initParameters.getBool("forceUseCuda");
}

void Recognize::doGet(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	try
	{
		const json::JSONParser& parser = request.getJSON();
		std::vector<Sample> samples;
		api::Context config = service.createContext();
		std::vector<std::unique_ptr<api::ProcessingBlock>> pipeline;
		std::vector<api::Context> preparedData;
		json::JSONBuilder responseBuilder(CP_UTF8);

		samples.reserve(2);

		samples.emplace_back(parser.getObject("firstImage"), service);
		samples.emplace_back(parser.getObject("secondImage"), service);

		config["ONNXRuntime"]["library_path"] = sdkPath;

		config["unit_type"] = "FACE_DETECTOR";
		pipeline.emplace_back(std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "FITTER";
		pipeline.emplace_back(std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "FACE_RECOGNIZER";
		pipeline.emplace_back(std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "MATCHER_MODULE";
		api::ProcessingBlock matcher = service.createProcessingBlock(config);

		for (const Sample& sample : samples)
		{
			api::Context data = sample.createDataContext();

			for (const std::unique_ptr<api::ProcessingBlock>& processingBlock : pipeline)
			{
				(*processingBlock)(data);
			}

			preparedData.push_back(std::move(data));
		}

		if (std::ranges::any_of(preparedData, [](const api::Context& context) { return context["objects"].size() > 1; }))
		{
			throw std::runtime_error("Images must contains only 1 face");
		}

		api::Context matcherData = service.createContext();
		api::Context verification = matcherData["verification"];
		api::Context verificationObjects = verification["objects"];

		verificationObjects.push_back(preparedData[0]["objects"][0]);
		verificationObjects.push_back(preparedData[1]["objects"][0]);

		matcher(matcherData);

		responseBuilder["distance"] = verification["result"]["distance"].getDouble();
		responseBuilder["verdict"] = verification["result"]["verdict"].getBool();

		response.addBody(responseBuilder);
	}
	catch (const std::exception& e)
	{
		response.setResponseCode(web::responseCodes::internalServerError);

		response.addBody(e.what());
	}
}

void Recognize::doPost(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	try
	{
		const json::JSONParser& parser = request.getJSON();
		std::vector<Sample> samples;
		std::vector<json::utility::jsonObject> images = json::utility::JSONArrayWrapper(parser.getArray("images")).getAsObjectArray();
		api::Context config = service.createContext();
		std::vector<std::unique_ptr<api::ProcessingBlock>> pipeline;
		std::vector<api::Context> preparedData;
		json::JSONBuilder responseBuilder(CP_UTF8);
		std::vector<json::utility::jsonObject> result;

		samples.reserve(images.size() + 1);

		samples.emplace_back(parser.getObject("referenceImage"), service);

		for (const json::utility::jsonObject& image : images)
		{
			samples.emplace_back(image, service);
		}

		config["ONNXRuntime"]["library_path"] = sdkPath;
		config["use_cuda"] = images.size() >= static_cast<size_t>(cudaThreshold) &&
			(forceUseCuda || (parser.contains("useCuda", json::utility::variantTypeEnum::jBool) ? parser.getBool("useCuda") : false));

		config["unit_type"] = "FACE_DETECTOR";
		pipeline.emplace_back(std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "FITTER";
		pipeline.emplace_back(std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "FACE_RECOGNIZER";
		pipeline.emplace_back(std::make_unique<api::ProcessingBlock>(service.createProcessingBlock(config)));

		config["unit_type"] = "MATCHER_MODULE";
		api::ProcessingBlock matcher = service.createProcessingBlock(config);

		for (const Sample& sample : samples)
		{
			api::Context data = sample.createDataContext();

			for (const std::unique_ptr<api::ProcessingBlock>& processingBlock : pipeline)
			{
				(*processingBlock)(data);
			}

			preparedData.push_back(std::move(data));

			if (preparedData.front()["objects"].size() > 1)
			{
				throw std::runtime_error("More than 1 face in reference image");
			}
		}

		for (size_t i = 1; i < preparedData.size(); i++)
		{
			const api::Context& reference = preparedData.front();
			const api::Context& current = preparedData[i];
			api::Context matcherData = service.createContext();
			api::Context verification = matcherData["verification"];
			api::Context verificationObjects = verification["objects"];
			std::vector<json::utility::jsonObject> recognitionResult;
			std::unique_ptr<BaseParser> faceDetectorParser = std::move
			(
				BaseParser::createParsers
				(
					{ "FACE_DETECTOR" },
					static_cast<int>(current["image"]["shape"][1].getLong()),
					static_cast<int>(current["image"]["shape"][0].getLong())
				).begin()->second
			);

			for (const api::Context& object : current["objects"])
			{
				verificationObjects.clear();

				verificationObjects.push_back(reference["objects"][0]);
				verificationObjects.push_back(object);

				matcher(matcherData);

				json::utility::jsonObject recognitionPerFace;

				recognitionPerFace.setObject
				(
					"boundingBox",
					faceDetectorParser->parse(object)
				);

				recognitionPerFace.setDouble("distance", verification["result"]["distance"].getDouble());
				recognitionPerFace.setBool("verdict", verification["result"]["verdict"].getBool());

				json::utility::appendArray(std::move(recognitionPerFace), recognitionResult);
			}

			json::utility::appendArray(std::move(recognitionResult), result);
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

void Recognize::doHead(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Recognize::doPut(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Recognize::doDelete(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Recognize::doOptions(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Recognize::doTrace(framework::HTTPRequest& request, framework::HTTPResponse& response)
{
	throw framework::exceptions::NotImplementedException();
}

void Recognize::destroy()
{

}
