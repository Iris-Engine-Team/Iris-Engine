#include <gtest/gtest.h>
#include "SECS/Scene.h"
#include "SECS/IrisObject.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/IComponent.h"
#include "Core/Tools/Log.h"

class SECSTest : public ::testing::Test
{
public:
	static int Argc;
	static char** Args;

public:
	SECSTest() {}
	virtual ~SECSTest() override {}

	static void SetUpTestCase() {}
	static void TearDownTestCase() {}

	virtual void SetUp() override {}
	virtual void TearDown() override {}

	virtual void TestBody() override {}
};
/*
using namespace Iris::Engine;
TEST_F(SECSTest, TestSceneRoot1Object)
{
	IrisObject::SetGlobalID();

	Scene s;
	
	IrisObject* o1 = new IrisObject();
	s.AddObject(o1);

	DEBUG_LOG(o1->PrintIrisObject().c_str());
}

TEST_F(SECSTest, TestSceneSetParent)
{
	IrisObject::SetGlobalID();

	Scene s;

	IrisObject* o1 = new IrisObject();
	s.AddObject(o1);

	ITransform* t2 = new ITransform(Vector3(3.f, 0.f, 0.f));
	IrisObject* o2 = new IrisObject(t2);
	s.AddObject(o2);

	DEBUG_LOG(o2->PrintIrisObject().c_str());

	o2->GetTransform()->SetParent(o1);

	DEBUG_LOG(o2->PrintIrisObject().c_str());
}

TEST_F(SECSTest, TestSceneSetParentPosition)
{
	IrisObject::SetGlobalID();

	Scene s;

	ITransform* t1 = new ITransform(Vector3(3.f, 0.f, 0.f));
	IrisObject* o1 = new IrisObject(t1);
	s.AddObject(o1);

	ITransform* t2 = new ITransform(Vector3(0.f, 0.f, 0.f));
	IrisObject* o2 = new IrisObject(t2);
	s.AddObject(o2);

	DEBUG_LOG(o1->PrintIrisObject().c_str());
	DEBUG_LOG(o2->PrintIrisObject().c_str());

	o2->GetTransform()->SetParent(o1);

	DEBUG_LOG(o2->PrintIrisObject().c_str());
}

TEST_F(SECSTest, TestSceneSetParentPositionScale)
{
	IrisObject::SetGlobalID();

	Scene s;

	ITransform* t1 = new ITransform(Vector3(3.f, 0.f, 0.f), Quaternion(1.f, 0.f, 0.f, 0.f), Vector3(0.5f));
	IrisObject* o1 = new IrisObject(t1);
	s.AddObject(o1);

	ITransform* t2 = new ITransform(Vector3(0.f, 0.f, 0.f), Quaternion(1.f, 0.f, 0.f, 0.f), Vector3(2.f));
	IrisObject* o2 = new IrisObject(t2);
	s.AddObject(o2);

	DEBUG_LOG(o1->PrintIrisObject().c_str());
	DEBUG_LOG(o2->PrintIrisObject().c_str());

	o2->GetTransform()->SetParent(o1);

	DEBUG_LOG(o2->PrintIrisObject().c_str());
}

TEST_F(SECSTest, TestSceneSetParentPositionRotation)
{
	IrisObject::SetGlobalID();

	Scene s;

	ITransform* t1 = new ITransform(Vector3(3.f, 0.f, 0.f), Quaternion(0.f,45.f*Deg2Rad,0.f), Vector3(1.f));
	IrisObject* o1 = new IrisObject(t1);
	s.AddObject(o1);

	ITransform* t2 = new ITransform(Vector3(0.f), Quaternion(1.f, 0.f, 0.f, 0.f), Vector3(1.f));
	IrisObject* o2 = new IrisObject(t2);
	s.AddObject(o2);

	DEBUG_LOG(o1->PrintIrisObject().c_str());
	DEBUG_LOG(o2->PrintIrisObject().c_str());

	o2->GetTransform()->SetParent(o1);

	DEBUG_LOG(o2->PrintIrisObject().c_str());
	Matrix4 trs = o2->GetTransform()->GetGlobalTRS();
	o2->GetTransform()->GetGlobalPosition().PrintInfo();
}

*/