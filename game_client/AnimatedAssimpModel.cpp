////////////////////////////////////////
// AnimatedAssimpModel.cpp
////////////////////////////////////////

#include "AnimatedAssimpModel.h"

AnimatedAssimpModel::AnimatedAssimpModel(string const& path, uint shader) : AssimpModel()
{
	importScene(path, shader);

	glm::mat4 globalInverseTransform = convertToGlmMat(m_aiScene->mRootNode->mTransformation.Inverse());
	loadModelByNodeTraversal(m_aiScene->mRootNode, globalInverseTransform);

	// Check if there's too many bones
	assert(bones.size() <= 100); // which is the MAX_BONES, set in the vertex shader 

	for (int i = 0; i < m_aiScene->mNumAnimations; i++) {
		animatedNodeMapList.push_back(new std::unordered_map<std::string, aiNodeAnim*>());
		for (int j = 0; j < m_aiScene->mAnimations[i]->mNumChannels; j++) {
			aiNodeAnim* curAnimNode = m_aiScene->mAnimations[i]->mChannels[j];
			std::string name = std::string(curAnimNode->mNodeName.C_Str());
			(*(animatedNodeMapList[i]))[name] = curAnimNode;
		}
	}

	// fin the root bone
	// teh root bone is direct child of the scene's root node hte top of the bone structure
	setRootBone();

	startTime = chrono::system_clock::now();
}

AnimatedAssimpModel::~AnimatedAssimpModel()
{
	for (std::unordered_map<std::string, aiNodeAnim*>* map : animatedNodeMapList) {
		delete map;
	}
}

// populates the bone vectors which stores the transformation data for each bone, and
// populates the bone reference vectors for the mesh
void AnimatedAssimpModel::loadBoneData(const aiMesh* mesh, vector<BoneReferenceData>& boneReferences) {
	for (uint i = 0; i < mesh->mNumBones; i++) {
		uint boneIndex = 0;
		string boneName(mesh->mBones[i]->mName.data);

		if (boneMap.find(boneName) == boneMap.end()) {
			// Allocate an index for a new bone
			boneIndex = bones.size();
			Bone bone;
			bones.push_back(bone);
			bones[boneIndex].boneOffset = convertToGlmMat(mesh->mBones[i]->mOffsetMatrix);
			boneMap[boneName] = boneIndex;
		}
		else {
			boneIndex = boneMap[boneName];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			uint vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			boneReferences[vertexID].addData(boneIndex, weight);
		}
	}
}

// draws the model, and thus all its meshes
void AnimatedAssimpModel::draw(SceneNode& node, const glm::mat4& viewProjMtx)
{
	glUseProgram(shader);

	for (uint i = 0; i < bones.size(); i++) {
		// set the uniform
		string str = "gBones[" + to_string(i) + "]";
		const char* uniformName = &(str)[0];
		glm::mat4 boneTransform = boneSceneNodeMap[i]->transform * bones[i].boneOffset;
		glUniformMatrix4fv(glGetUniformLocation(shader, uniformName), 1, false, (float*)&(boneTransform));
	}

	const glm::mat4 model = glm::mat4(1.0); // because the global transform is applied to the bone transform already
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&viewProjMtx);

	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->draw(shader);

	glUseProgram(0);
}

// this funciton needs to load the bone data from the Scene nodes
void AnimatedAssimpModel::update(SceneNode* node)
{
	if (node->animationId >= m_aiScene->mNumAnimations) {
		std::cerr << "Scene node " << node->name
			<< " does not have animation ID " << node->animationId
			<< " (has only " << m_aiScene->mNumAnimations << " animations)" << std::endl;
		return;
	}

	// update bone transform for the specific animation of this node/object, and apply it to node->transform
	updateBoneTransform(node);
}

/* Animation related functions */
/* The interpolation assumes all starting and ending keyframes are at the edge of the timerange */

void AnimatedAssimpModel::updateBoneTransform(SceneNode* node)
{
	auto anim = m_aiScene->mAnimations[node->animationId];

	// gets tick time
	float ticksPerSecond = (float)(
		anim->mTicksPerSecond != 0
		? anim->mTicksPerSecond
		: 25.0f // the last value is the default ticks/sec
	);
	float timeInTicks = node->animPlayedTime * ticksPerSecond;
	float animationTime = fmod(timeInTicks, (float)anim->mDuration);

	// handle looping vs no-looping
	if (!node->loopAnimation) { // not a looping anim
		if (node->playedOneAnimCycle) {
			//set to near the last frame
			animationTime = (float)anim->mDuration - 0.0001; // can be redundant as SceneNode::updateAnim() will stop update given the same condition
		}
		else if (timeInTicks / ((float)anim->mDuration) >= 1.0) {
			node->playedOneAnimCycle = true;
			//set to the near the last frame
			animationTime = (float)anim->mDuration - 0.0001;
		}
	}

	// set this line to just use the root bone node rather than root bone
	calcAnimByNodeTraversal(node->animationId, animationTime, rootBone, convertToGlmMat(m_aiScene->mRootNode->mTransformation));
}

SceneNode* AnimatedAssimpModel::createSceneNodes(uint objectId)
{
	SceneNode * newNode = new SceneNode(this, string("modelRoot"), objectId);
	newNode->loadAnimData(m_aiScene->mNumAnimations, 0); // TODO: to be removed if updating animation on the server side
	newNode->addChild(createSceneNodesRec(objectId, rootBone));
	return newNode;
}

SceneNode* AnimatedAssimpModel::createSceneNodesRec(uint objectId, aiNode* curNode)
{
	SceneNode* newNode = new SceneNode(NULL, string(curNode->mName.C_Str()), objectId);

	// add to the map that maps the bone index to the scene node that simulates the bone
	if (boneMap.find(newNode->getName()) != boneMap.end()) {
		boneSceneNodeMap[boneMap[newNode->getName()]] = newNode;
	}

	// create scene nodes for children recursively
	for (int i = 0; i < curNode->mNumChildren; i++) {
		newNode->addChild(createSceneNodesRec(objectId, curNode->mChildren[i]));
	}
	return newNode;
}

void AnimatedAssimpModel::calcAnimByNodeTraversal(int animId, float AnimationTime, const aiNode* pNode, const glm::mat4& parentTransform)
{
	string NodeName(pNode->mName.data);

	glm::mat4 nodeTransformation = convertToGlmMat(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = findAnimNode(animId, NodeName);

	if (pNodeAnim) {
		// Interpolate transformations
		aiVector3D Scaling;
		calcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 scalingM = glm::scale(glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		aiQuaternion RotationQ;
		calcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		glm::mat4 rotationM = convertToGlmMat(aiMatrix4x4(RotationQ.GetMatrix()));

		aiVector3D Translation;
		calcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 translationM = glm::translate(glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		nodeTransformation = translationM * rotationM * scalingM;
	}

	glm::mat4 boneLocalTransformation = parentTransform * nodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) {
		uint BoneIndex = boneMap[NodeName];

		// we pass in local transform because this will be passed into node.transform
		// and SceneNode::update() will apply the global transform to the node.transform
		bones[BoneIndex].finalTransformation = boneLocalTransformation;
		boneSceneNodeMap[BoneIndex]->updated = true;
		boneSceneNodeMap[BoneIndex]->transform = boneLocalTransformation;

		// reset boneLocalTransformation, which will be passed as parentTransform, so that make the transform local to each bone
		boneLocalTransformation = glm::mat4(1);
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		calcAnimByNodeTraversal(animId, AnimationTime, pNode->mChildren[i], boneLocalTransformation);
	}
}

const aiNodeAnim* AnimatedAssimpModel::findAnimNode(const int animId, const string NodeName)
{
	if ((animatedNodeMapList[animId])->count(NodeName) > 0) {
		return (*(animatedNodeMapList[animId]))[NodeName];
	}
	return NULL;
}


void AnimatedAssimpModel::calcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = findAnimPosition(AnimationTime, pNodeAnim);
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

	uint RotationIndex = findAnimRotation(AnimationTime, pNodeAnim);
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

// this finds the root bone in the bone hierarcy ideally this coudl be simplfied
// if all root bones were name root bone in the model this would become much easier
void AnimatedAssimpModel::setRootBone()
{
	std::map<string, uint>::iterator bones;
	for (bones = boneMap.begin(); bones != boneMap.end(); bones++) {
		for (int i = 0; i < m_aiScene->mRootNode->mNumChildren; i++) {
			// find() is used instead of == because the importer might add extra layers named "root_bone_name$Extra_transform_name"
			if (std::string(m_aiScene->mRootNode->mChildren[i]->mName.C_Str()) == bones->first) {
				rootBone = m_aiScene->mRootNode->mChildren[i];
			}
		}
	}
}

void AnimatedAssimpModel::calcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = findAnimScaling(AnimationTime, pNodeAnim);
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

uint AnimatedAssimpModel::findAnimPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	assert(0);
	return 0;
}


uint AnimatedAssimpModel::findAnimRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	assert(0);
	return 0;
}


uint AnimatedAssimpModel::findAnimScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}

	assert(0);
	return 0;
}
