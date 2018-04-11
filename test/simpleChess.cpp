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

class Pawn: public SelectiveObject<RingObject<ModelObject>>
{
public:
	Pawn(int type, int i, int j):
		SelectiveObject<RingObject<ModelObject>>(getModel(type)), 
		player(type & 1), type(type & -2)
	{
		this->translate((j - 3.5) * detX, (3.5 - i) * detY, 0);
		this->rotate(0, 0, (!player ? -1 : 1) * ::glm::radians(90.0));
	}
	void move(int di, int dj)
	{
		this->translate(dj * detX, -di * detY, 0);
	}
	void onSelect() override
	{
		ring(true); fRingColor = ::glm::vec3(1, 0, 0);
	}
	void onHover() override
	{
		ring(true); fRingColor = selected() ? ::glm::vec3(1, 0, 0) : ::glm::vec3(0, 0, 1);
	}
	void onUnselect() override
	{
		ring(hovered()); if (hovered()) fRingColor = ::glm::vec3(0, 0, 1);
	}
	void onUnhover() override
	{
		ring(selected());
	}
private:
	static Model &getModel(int type)
	{
		static Model models[] = {
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
		return models[type];
	}
private:
	int player, type;
};

class Grid: public SelectiveObject<ModelObject>
{
public:
	Grid(int type, int i, int j):
		type(type)
	{
		translate((j - 3.5) * detX, (3.5 - i) * detY, 0);
	}
private:
	static Model &getModel(int type)
	{
		static Model models[] = {
			Model::load("chess/white-grid.obj"),
			Model::load("chess/black-grid.obj"),
		};
		return models[type];
	}
private:
	int type;
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

Scene::Ref<Pawn> board[8][8];

Window<4, 2> window("Simple Chess", false, 768, 768);

int Main(int argc, char **argv)
{
	Scene scene;
	scene.setScaleFactor(0.006);

	auto camera = scene.create<ViewPort>();//<FirstPersonCamera<>>();
	camera->setPosition(0, 2, 3);
	camera->setTarget(0, -0.5, -0.8);
	// CameraCarrier cc(camera);

	auto light = scene.create<PointLight>(vec3(0, 0, 2));
	light->setDiffuseIntensity(2.f);
	scene.setMajorLight(*light);

	auto initBoard = [&]() {
		for (int i = 0; i != 8; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				if (~initialize[i][j])
				{
					board[i][j] = scene.create<Pawn>(initialize[i][j], i, j);
				}
			}
		}
	};
	auto clearBoard = [&]() {
		for (int i = 0; i != 8; ++i)
		{
			for (int j = 0; j != 8; ++j)
			{
				scene.destroy(board[i][j]);
			}
		}
	};
	auto makeMove = [&](int i0, int j0, int i1, int j1) {
		if (board[i0][j0].isValid())
		{
			BEE_RAISE(Fatal, "Moving an empty grid.");
		}
		if (board[i1][j1].isValid())
		{
			scene.destroy(board[i1][j1]);
		}
		board[i0][j0]->move(i1 - i0, j1 - j0);
		board[i1][j1] = board[i0][j0];
		board[i0][j0].invalidate();
	};

	// scene.onMouseHover(
	// 	[](Object *object)
	// 	{
	// 		static SelectiveModelObject *hovered = nullptr;

	// 		if (auto obj = dynamic_cast<SelectiveModelObject*>(object))
	// 		{
	// 			obj->hover(true);
	// 			if (hovered != obj)
	// 			{
	// 				if (hovered)
	// 				{
	// 					hovered->hover(false);
	// 				}
	// 				hovered = obj;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			if (hovered)
	// 			{
	// 				hovered->hover(false);
	// 			}
	// 			hovered = nullptr;
	// 		}
	// 	}
	// );
	// scene.onMouseClick(
	// 	[](Object *object, int button)
	// 	{
	// 		static SelectiveModelObject *selected = nullptr;

	// 		if (auto obj = dynamic_cast<SelectiveModelObject*>(object))
	// 		{
	// 			if (button & 1)
	// 			{
	// 				obj->select(true);
	// 				if (selected != obj)
	// 				{
	// 					if (selected)
	// 					{
	// 						selected->select(false);
	// 					}
	// 					selected = obj;
	// 				}
	// 			}
	// 			else
	// 			{
	// 				obj->select(false);
	// 				if (selected)
	// 				{
	// 					selected->select(false);
	// 				}
	// 				selected = nullptr;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			if (selected)
	// 			{
	// 				selected->select(false);
	// 			}
	// 			selected = nullptr;
	// 		}
	// 	}
	// );

	initBoard();

	scene.create<ModelObject>("board/board.obj");
	window.dispatch<RenderEvent>(
		[&]() -> bool
		{
			scene.shadowPass();
			scene.cursorPass();
			scene.renderPass();
			return false;
		}
	);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	window.dispatchMessages();
	return 0;
}
