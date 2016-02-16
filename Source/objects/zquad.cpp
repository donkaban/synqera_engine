#include "TheEngine/objects/zquad.h"
#include "TheEngine/engine.h"


the::zquad::zquad(float zPosition):zPosition(zPosition)
{
	addSurface(std::move(the::surface({}, "billboard")));

	update();

	setMaterial(the::material::get("def::base"));
	setTexture(0, the::texture::get("def::base"));
}

void the::zquad::update()
{
	the::node::update();
	setTag(aux::format("the::zquad::%.2f", zPosition));

	unbind();

	auto engine = aux::engine->get();
	auto camera = engine-> getScene()->getCamera();

	float cameraRelativeZ = zPosition - camera->getWorldPosition().z;
	float h = tan(the::math::DEG2RAD *  camera->fov() * 0.5) * cameraRelativeZ;
	float w = h * camera->aspect();

	std::vector<Vertex> &vertexes = getVertices();
	vertexes.clear();

	addVertex(
	{
	   {{ w, h, zPosition}, {0, 0}, {0, 0, 1}},
	   {{-w, h, zPosition}, {1, 0}, {0, 0, 1}},
	   {{-w,-h, zPosition}, {1, 1}, {0, 0, 1}},
	   {{ w,-h, zPosition}, {0, 1}, {0, 0, 1}},
	});

	auto &s = getSurface(0);
	std::vector<uint16_t> &indecies = s.getIndecies();
	indecies.clear();
	s.add({0, 1, 2, 2, 3, 0});

	computeTBN();
	bind();

	prjViewMatrix = camera->getPVMatrix();
}


void the::zquad::load(const std::string &filename)
{}

void the::zquad::setZPosition(float zPos)
{
	zPosition = zPos;
	update();
}

void the::zquad::onRender(render::state &state)
{
	if(aux::engine->get()->getScene()->getCamera()->getPVMatrix()!=prjViewMatrix)
	{
		update();
	}

	the::object::onRender(state);
	for (auto & s : surfaces)
	{
		s.setDefaultParameters(state);
		s.setUserParameters(state);
		s.draw();
	}
}

void the::zquad::serialize(pugi::xml_node &node)
{
	logger::error("call not implemented method zquad::serialize()");
}

void the::zquad::deserialize(const pugi::xml_node &node)
{
	logger::error("call not implemented method zquad::deserialize()");
}

void the::zquad::log()
{
	logger::system("zquad '%s'; order: %d; blend: %s", getTag().c_str(),getRenderOrder(),getBlend()? "true" : "false");
    the::object::log();
    for(const auto &child : childs) child->log();
     
}
