#include "PoseParser.h"

static const std::vector<std::pair<std::string, std::string>> boneMap =
{
	{ "right_ankle", "right_knee" },
	{ "right_knee", "right_hip" },
	{ "left_hip", "right_hip" },
	{ "left_shoulder", "left_hip" },
	{ "right_shoulder", "right_hip" },
	{ "left_shoulder", "right_shoulder" },
	{ "left_shoulder", "left_elbow" },
	{ "right_shoulder", "right_elbow" },
	{ "left_elbow", "left_wrist" },
	{ "right_elbow", "right_wrist" },
	{ "left_eye", "right_eye" },
	{ "nose", "left_eye" },
	{ "left_knee", "left_hip" },
	{ "right_ear", "right_shoulder" },
	{ "left_ear", "left_shoulder" },
	{ "right_eye", "right_ear" },
	{ "left_eye", "left_ear" },
	{ "nose", "right_eye" },
	{ "left_ankle", "left_knee" }
};

PoseParser::PoseParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject PoseParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;
	std::vector<json::utility::jsonObject> points;
	const api::Context& keypoints = object["keypoints"];

	for (const auto& [key1, key2] : boneMap)
	{
		json::utility::jsonObject first;
		json::utility::jsonObject second;

		int64_t x1 = static_cast<int64_t>(keypoints[key1]["proj"][0].getDouble() * imageWidth);
		int64_t y1 = static_cast<int64_t>(keypoints[key1]["proj"][1].getDouble() * imageHeight);
		int64_t x2 = static_cast<int64_t>(keypoints[key2]["proj"][0].getDouble() * imageWidth);
		int64_t y2 = static_cast<int64_t>(keypoints[key2]["proj"][1].getDouble() * imageHeight);

		first.setInt("x", x1);
		first.setInt("y", y1);

		second.setInt("x", x2);
		second.setInt("y", y2);

		result.setObject(key1, std::move(first));
		result.setObject(key2, std::move(second));
	}

	for (auto it = keypoints.begin(); it != keypoints.end(); ++it)
	{
		api::Context keypoint = (*it)["proj"];
		json::utility::jsonObject point;

		point.setInt("x", static_cast<int64_t>(keypoint[0].getDouble() * imageWidth));
		point.setInt("y", static_cast<int64_t>(keypoint[1].getDouble() * imageHeight));

		json::utility::appendArray(std::move(point), points);
	}

	result.setArray("keypoints", std::move(points));

	return result;
}
