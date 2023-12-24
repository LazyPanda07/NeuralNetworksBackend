#include "EmotionParser.h"

EmotionParser::EmotionParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject EmotionParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;
	const api::Context& emotions = object["emotions"];

	for (size_t i = 0; i < emotions.size(); i++)
	{
		result.setDouble(emotions[i]["emotion"].getString(), emotions[i]["confidence"].getDouble());
	}

	return result;
}
