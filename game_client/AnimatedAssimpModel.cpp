////////////////////////////////////////
// AnimatedAssimpModel.cpp
////////////////////////////////////////

#include "AnimatedAssimpModel.h"

AnimatedAssimpModel::AnimatedAssimpModel(string const& path) : AssimpModel(path)
{
	startTime = chrono::system_clock::now();
}

// draws the model, and thus all its meshes
void AnimatedAssimpModel::draw(uint shader)
{
	// calculate the animated transformation for this frame
	chrono::duration<double> elapsed_seconds = chrono::system_clock::now() - startTime;
	float runningTime = elapsed_seconds.count();

	updateBoneTransform(runningTime);

	// Render
	AssimpModel::draw(shader);
}


/* Animation related functions */
/* The interpolation assumes all starting and ending keyframes are at the edge of the timerange */

void AnimatedAssimpModel::updateBoneTransform(float TimeInSeconds)
{
	float TicksPerSecond = (float)(m_aiScene->mAnimations[0]->mTicksPerSecond != 0
		? m_aiScene->mAnimations[0]->mTicksPerSecond
		: 50.0f); // the last value is the default ticks/sec
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_aiScene->mAnimations[0]->mDuration);

	traverseNodeHeirarchy(AnimationTime, m_aiScene->mRootNode, glm::mat4(1));
}

void AnimatedAssimpModel::traverseNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = m_aiScene->mAnimations[0];

	glm::mat4 NodeTransformation = convertToGlmMat(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate transformations
		aiVector3D Scaling;
		calcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM = glm::scale(glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		aiQuaternion RotationQ;
		calcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 RotationM = convertToGlmMat(aiMatrix4x4(RotationQ.GetMatrix()));

		aiVector3D Translation;
		calcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) {
		uint BoneIndex = boneMap[NodeName];
		glm::mat4 m_GlobalInverseTransform = convertToGlmMat(m_aiScene->mRootNode->mTransformation.Inverse()); // TODO;  Replace the passed in argument with this
		bones[BoneIndex].finalTransformation = m_GlobalInverseTransform * GlobalTransformation * bones[BoneIndex].boneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		traverseNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

const aiNodeAnim* AnimatedAssimpModel::findNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName)
			return pNodeAnim;
	}
	return NULL;
}


void AnimatedAssimpModel::calcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	// TODO: the std::max is a temp fix to avoid cases when the first keyframe is > 0
	float Factor = std::max(0.0f, (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime);
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void AnimatedAssimpModel::calcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	// TODO: the std::max is a temp fix to avoid cases when the first keyframe is > 0
	float Factor = std::max(0.0f, (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime);
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void AnimatedAssimpModel::calcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	// TODO: the std::max is a temp fix to avoid cases when the first keyframe is > 0
	float Factor = std::max(0.0f, (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime);
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

uint AnimatedAssimpModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	assert(0);
	return 0;
}


uint AnimatedAssimpModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	assert(0);
	return 0;
}


uint AnimatedAssimpModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}

	assert(0);
	return 0;
}
