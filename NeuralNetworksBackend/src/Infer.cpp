#include "Infer.h"

#include <future>

#include "Timers.h"

static std::vector<uint8_t> decodeBase64(std::string_view base64EncodedData)
{
	static constexpr uint8_t lookup[] =
	{
		62,  255, 62,  255, 63,  52,  53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
		255, 0,   255, 255, 255, 255, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10,  11,  12,  13,  14,  15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		255, 255, 255, 255, 63,  255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
		36,  37,  38,  39,  40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50, 51
	};

	static_assert(sizeof(lookup) == 'z' - '+' + 1);

	std::vector<uint8_t> result;
	int val = 0;
	int valb = -8;

	for (uint8_t c : base64EncodedData)
	{
		if (c < '+' || c > 'z')
		{
			break;
		}
			
		c -= '+';

		if (lookup[c] >= 64)
		{
			break;
		}
			
		val = (val << 6) + lookup[c];

		valb += 6;

		if (valb >= 0)
		{
			result.push_back((val >> valb) & 0xFF);

			valb -= 8;
		}
	}

	return result;
}

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
		std::future<std::vector<uint8_t>> bytesFuture = std::async(std::launch::async, decodeBase64, parser.getString("data"));
		api::Service service = api::Service::createService(sdkPath);
		int64_t width = parser.getInt("width");
		int64_t height = parser.getInt("height");
		api::Context config = service.createContext();
		std::vector<json::utility::jsonObject> result;
		json::JSONBuilder responseBuilder(CP_UTF8);
		double inferTime = 0.0;

		config["unit_type"] = "FACE_DETECTOR";
		config["ONNXRuntime"]["library_path"] = sdkPath;
		config["use_cuda"] = forceUseCuda || (parser.contains("useCuda", json::utility::variantTypeEnum::jBool) ? parser.getBool("useCuda") : false);

		api::ProcessingBlock detector = service.createProcessingBlock(config);

		api::Context data = service.createContext();
		api::Context image = data["image"];
		api::Context shape = image["shape"];

		shape.push_back(height);
		shape.push_back(width);
		shape.push_back(parser.getInt("channels"));

		image["format"] = "NDARRAY";
		image["dtype"] = parser.getString("depthType");

		std::vector<uint8_t> bytes(bytesFuture.get());

		image["blob"].setDataPtr(bytes.data());

		{
			utility::timers::AccumulatingTimer timer(inferTime, utility::timers::OutputTimeType::milliseconds);

			detector(data);
		}

		for (const api::Context& object : data["objects"])
		{
			json::utility::jsonObject jsonObject;
			json::utility::jsonObject jsonBbox;
			json::utility::jsonObject topLeft;
			json::utility::jsonObject bottomRight;
			api::Context bbox = object["bbox"];

			topLeft.setInt("x", static_cast<int64_t>(bbox[0].getDouble() * width));
			topLeft.setInt("y", static_cast<int64_t>(bbox[1].getDouble() * height));

			bottomRight.setInt("x", static_cast<int64_t>(bbox[2].getDouble() * width));
			bottomRight.setInt("y", static_cast<int64_t>(bbox[3].getDouble() * height));

			jsonBbox.setObject("topLeft", std::move(topLeft));
			jsonBbox.setObject("bottomRight", std::move(bottomRight));

			jsonObject.setObject("boundingBox", std::move(jsonBbox));

			json::utility::appendArray(std::move(jsonObject), result);
		}

		json::utility::jsonObject infer;

		infer.setDouble("time", inferTime);
		infer.setString("timeUnits", "ms");

		responseBuilder["result"] = std::move(result);
		responseBuilder["infer"] = std::move(infer);
		responseBuilder["additionalInformation"] = json::utility::toUTF8JSON("Ну ты и пес канеш", 1251);

		response.addBody(responseBuilder);
	}
	catch (const std::exception& e)
	{
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
