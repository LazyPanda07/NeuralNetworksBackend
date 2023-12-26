#include "Sample.h"

#include "ThreadPool.h"
#include "api/Service.h"

void Sample::decodeBase64(const std::string& base64EncodedData)
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
			imageData.push_back((val >> valb) & 0xFF);

			valb -= 8;
		}
	}
}

Sample::Sample(const json::utility::jsonObject& image, api::Service& service) :
	imageDataProcessing(std::async(std::launch::async, &Sample::decodeBase64, this, std::ref(image.getString("data")))),
	service(service),
	depthType(image.getString("depthType")),
	width(image.getInt("width")),
	height(image.getInt("height")),
	channels(image.getInt("channels"))
{

}

api::Context Sample::createDataContext() const
{
	api::Context data = service.createContext();
	api::Context image = data["image"];
	api::Context shape = image["shape"];

	shape.push_back(height);
	shape.push_back(width);
	shape.push_back(channels);

	image["format"] = "NDARRAY";
	image["dtype"] = depthType;

	imageDataProcessing.wait();

	image["blob"].setDataPtr(const_cast<uint8_t*>(imageData.data()));

	return data;
}

std::unordered_map<std::string, std::unique_ptr<BaseParser>> Sample::createParsers(const std::vector<std::string>& unitTypes) const
{
	return BaseParser::createParsers(unitTypes, static_cast<int>(width), static_cast<int>(height));
}
