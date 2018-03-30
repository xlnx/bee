#include "bee.h"

using namespace bee;
using namespace bee::gl;
using namespace std;
using namespace glm;

constexpr auto modelWhite = 0x00;
constexpr auto modelBlack = 0x01;
constexpr auto modelPawn = 0x00;
constexpr auto modelRook = 0x02;
constexpr auto modelKnight = 0x04;
constexpr auto modelBishop = 0x06;
constexpr auto modelQueen = 0x08;
constexpr auto modelKing = 0x0A;

constexpr float detX = 0.38;
constexpr float detY = 0.38;

class Pawn
{
public:
	Pawn(ModelObject &object, int type, int i, int j):
		object(object), player(type & 1), type(type & -2)
	{
		object.translate((j - 3.5) * detX, (3.5 - i) * detY, 0);
	}
	void move(int di, int dj)
	{
		object.translate(dj * detX, -di * detY, 0);
	}
private:
	ModelObject &object;
	int player, type;
};

int initialize[8][8] = {
	modelRook|modelWhite, modelKnight|modelWhite, 
		modelBishop|modelWhite, modelQueen|modelWhite, 
			modelKing|modelWhite, modelBishop|modelWhite,
				modelKnight|modelWhite, modelRook|modelWhite,
	modelPawn|modelWhite, modelPawn|modelWhite, 
		modelPawn|modelWhite, modelPawn|modelWhite, 
			modelPawn|modelWhite, modelPawn|modelWhite, 
				modelPawn|modelWhite, modelPawn|modelWhite, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, 
	modelPawn|modelBlack, modelPawn|modelBlack, 
		modelPawn|modelBlack, modelPawn|modelBlack, 
			modelPawn|modelBlack, modelPawn|modelBlack, 
				modelPawn|modelBlack, modelPawn|modelBlack, 
	modelRook|modelBlack, modelKnight|modelBlack, 
		modelBishop|modelBlack, modelKing|modelBlack, 
			modelQueen|modelBlack, modelBishop|modelBlack,
				modelKnight|modelBlack, modelRook|modelBlack,
};

Pawn *board[8][8];

Window<4, 2> window("Simple Chess", false, 768, 768);

int Main(int argc, char **argv)
{
	Scene scene;

	Model models[] = {
		Model::load("chess/white-pawn.obj"),
		Model::load("chess/black-pawn.obj"),
		Model::load("chess/white-rook.obj"),
		Model::load("chess/black-rook.obj"),
		Model::load("chess/white-knight.obj"),
		Model::load("chess/black-knight.obj"),
		Model::load("chess/white-bishop.obj"),
		Model::load("chess/black-bishop.obj"),
		Model::load("chess/white-queen.obj"),
		Model::load("chess/black-queen.obj"),
		Model::load("chess/white-king.obj"),
		Model::load("chess/black-king.obj"),
	};
	auto createObject = [&](const string &name) -> ModelObject& {
		auto &object = scene.createObject<ModelObject>(name);
		object.scale(0.006);
		return object;
	};
	auto createPawn = [&](int model) -> ModelObject& {
		auto &object = scene.createObject<ModelObject>(models[model]);
		object.scale(0.006);
		return object;
	};
	auto initBoard = [&]() {
		for (int i = 0; i != 8; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				if (~initialize[i][j])
				{
					board[i][j] = new Pawn(
						createPawn(initialize[i][j]), 
						initialize[i][j],
						i, j
					);
				}
			}
		}
	};
	auto clearBoard = [&]() {
		for (int i = 0; i != 8; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				delete board[i][j];
			}
		}
	};
	auto makeMove = [&](int i0, int j0, int i1, int j1) {
		if (!board[i0][j0])
		{
			BEE_RAISE(Fatal, "Moving an empty grid.");
		}
		if (board[i1][j1])
		{
			delete board[i1][j1];
			board[i1][j1] = nullptr;
		}
		board[i0][j0]->move(i1 - i0, j1 - j0);
		board[i1][j1] = board[i0][j0];
		board[i0][j0] = nullptr;
	};

	auto render = [&]() {
		scene.shadowPass();
		scene.cursorPass();
		scene.renderPass();
	};

	auto &camera = scene.createCamera<FirstPersonCamera<>>();//ViewPort>();
	camera.setPosition(0, 3, 3);
	camera.setTarget(0, -0.5, -0.5);
	CameraCarrier cc(camera);

	auto &light = scene.createController<PointLight>(vec3(0, 0, 2));
	light.setDiffuseIntensity(2.f);
	scene.setMajorLight(light);

	initBoard();

	createObject("board/board.obj");
	window.dispatch<RenderEvent>([&]() -> bool{
		render();
		return false;
	});
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	window.dispatchMessages();
	return 0;
}
